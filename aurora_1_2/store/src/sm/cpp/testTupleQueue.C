
#include <sm/include/PagePool.H>
#include <sm/include/TupleQueue.H>

#include <unistd.h>
#include <iostream>
#include <semaphore.h>

using namespace std;

const int PAGE_COUNT = 16384;
const int PAGE_SIZE = 4096;
const int TUPLE_SIZE = 36;

// Never get more than about half full.
const int MAX_TUPLES_IN_QUEUE = (PAGE_SIZE / TUPLE_SIZE / 2) * PAGE_COUNT;

TupleQueue *q;

// run_enq_thread and run_deq_thread each decrement the semaphore
// by 1; the semaphore starts at 2 so they can run simultaneously.
//
// The "dump" thread grabs 2, so that it can't run simultaneously with
// the enq or deq threads.
sem_t sem;

bool done = false;

void grab() {
    int ret = sem_wait(&sem);
    if (ret < 0) {
        perror("sem_wait");
        exit(1);
    }
}

void post() {
    int ret = sem_post(&sem);
    if (ret < 0) {
        perror("sem_post");
        exit(1);
    }
}

void *run_enq_thread(void *) {
    cout << "In enq thread" << endl;

    // Each tuple is just a bunch of bytes, all the same; first tuple
    // is all 'A's, then all 'B's, etc., and so forth through 'Z'.
    unsigned int enq_count = 0;

    TupleQueue::EnqIterator enq = q->enq_iterator();

    while (!done) {
        grab();
        // Write between 1 and 1000 tuples right now.
        int to_write = 1 + rand() % 999;
        cout << "Writing " << to_write << endl;
        for (int i = 0; q->size() < MAX_TUPLES_IN_QUEUE && i < to_write; ++i) {
            memset(enq.tuple(), 'A' + enq_count % 26, TUPLE_SIZE);
            ++enq;
            ++enq_count;
        }
        post();

        // Yield processor
        sched_yield();
    }

    return 0;
}

void check_tuple(const void *data, int count) {
    char tdata = 'A' + count % 26;

    for (int i = 0; i < TUPLE_SIZE; ++i)
        assert( ((const char*)data)[i] == tdata );
}

void *run_deq_thread(void *) {
    cout << "In deq thread" << endl;

    TupleQueue::DeqIterator deq = q->deq_iterator();

    int deq_count = 0;

    while (!done) {
        grab();
        // 25% of the time, read everything in the queue
        unsigned int to_read;

        if (rand() % 4 == 0)
            to_read = UINT_MAX;
        else
            to_read = q->size() * 3 / 4;

        int count = 0;
        while (deq.avail() && to_read--) {
            check_tuple(deq.tuple(), deq_count);
            //            cout << "Read \"" << *(const char *)deq.tuple() << "\"" << endl;
            ++deq;
            ++deq_count;
            ++count;
        }
        post();

        cout << "Read " << count << "; " << q->size() << " left" << endl;

        // Yield processor
        sched_yield();
    }

    return 0;
}

int main() {
    try {
        PagePool pool(PAGE_SIZE, PAGE_COUNT, false);
        q = new TupleQueue(pool, TUPLE_SIZE);

        pthread_t enq_thread, deq_thread;

        int ret = sem_init(&sem, 0, 2);
        if (ret < 0) { perror("sem_init"); return 1; }

        ret = pthread_create(&enq_thread, 0, &run_enq_thread, 0);
        if (ret < 0) { perror("pthread_create"); return 1; }

        ret = pthread_create(&deq_thread, 0, &run_deq_thread, 0);
        if (ret < 0) { perror("pthread_create"); return 1; }

        for (int i = 0; i < 120; ++i) {
            sleep(1);
            grab();
            grab();
            cout << "Queue has " << q->size() << " elements" << endl;
            post();
            post();
        }

        done = true;

        void *thread_ret;
        pthread_join(enq_thread, &thread_ret);
        pthread_join(deq_thread, &thread_ret);

        delete q;
    } catch (SmException& e) {
        cerr << "Uhoh: " << e.what() << endl;
    }
}
