
#include <sm/include/TupleQueue.H>
#include <LockHolder.H>

#include <stdlib.h>
#include <malloc.h>
#include <sys/mman.h>
#include <errno.h>
#include <iostream>

using namespace std;

TupleQueue::TupleQueue(PagePool &pool,
                       unsigned int tuple_size) :
    _pool(pool), _tuple_size(tuple_size),
    _enq_count(0), _deq_count(0)
{
    _tuples_per_page = (pool.page_size() - sizeof(PageHdr)) / _tuple_size;
    assert(_tuples_per_page > 0);

    _enq_page = _deq_page = (PageHdr*)_pool.alloc();
    _enq_page->next = 0;

    _enq_location = _deq_location = (char *)_enq_page + sizeof(PageHdr);
}

TupleQueue::~TupleQueue()
{
    PageHdr *next;

    for (PageHdr *page = _deq_page; page; page = next) {
        // Have to set next now (before free); free clobbers page data
        next = page->next;
        _pool.free(page);
    }
}

void TupleQueue::dump() const
{
    cerr << "TupleQueue {\n"
         << "    _tuple_size = " << _tuple_size << "\n"
         << "    _tuples_per_page = " << _tuples_per_page << "\n"
         << "    _enq_page = " << _enq_page << "\n"
         << "    _enq_location = " << _enq_location << "\n"
         << "    _deq_page = " << _deq_page << "\n"
         << "    _deq_location = " << _deq_location << "\n"
         << "    size() = " << size() << "\n"
         << "    pages = {\n";

    for (PageHdr *page = _deq_page; page; page = page->next) {
        cerr << "        " << (void*)page << " = \"";
        const char *data = (const char *)page;
        for (unsigned int i = 0; i < _pool.page_size(); ++i) {
            cerr << ((data[i] >= ' ' && data[i] <= '~') ? data[i] : '.');
        }
        cerr << "\"\n";
    }
    cerr << "    }\n"
         << "}"
         << endl;
}
