
#ifndef TUPLEQUEUE_H
#define TUPLEQUEUE_H

#include "PagePool.H"

/// Thread-safe tuple queue.
class TupleQueue {
  public:
    /// Constructor.  The pool's page size must be at least big enough
    /// to hold a tuple plus a small amount of accounting data.
    TupleQueue(PagePool& pool, unsigned int tuple_size);

    /// Destructor.
    ~TupleQueue();

    friend class EnqIterator {
      public:
        /// Dummy constructor.
        EnqIterator() : _q(0) {}

	/// Returns a pointer to a tuple-sized chunk of memory
	/// to which data may be written.
	void *tuple() const {
            assert(_q);
            return _q->_enq_location;
        }

	/// Indicates that the tuple to which this iterator points has
	/// been populated.
	EnqIterator& operator ++ () {
            assert(_q);

            void *next_enq_location = (char*)_q->_enq_location + _q->_tuple_size;
            if ((char*)next_enq_location + _q->_tuple_size > (char*)_q->_enq_page + _q->_pool.page_size()) {
                // Allocate new page.  Note that the DeqIterator
                // cannot ever advance to the new page until we set
                // _enq_location (at the very end of this method)

                PageHdr *new_page = (PageHdr*)_q->_pool.alloc();
                new_page->next = 0;

                _q->_enq_page->next = new_page;
                _q->_enq_page = new_page;

                next_enq_location = (char*)new_page + sizeof(PageHdr);
            }

            _q->_enq_location = next_enq_location;
            ++(_q->_enq_count);
            return *this;
        }

      private:
        explicit EnqIterator(TupleQueue &q) : _q(&q) {}

	/// Prohibit postfix increment; you can't advance the iterator
	/// until you've populated the tuple.
	EnqIterator operator ++ (int);

        TupleQueue *_q;

        friend class TupleQueue;
    };

    friend class DeqIterator {
      public:
        /// Dummy constructor.
        DeqIterator() : _q(0) {}

	/// Returns true iff there is a valid tuple to dequeue.
	bool avail() const {
            assert(_q);

            return _q->_deq_location != _q->_enq_location;
        }

	/// Returns a pointer to a tuple to be consumed.  Valid
	/// only if avail().
	const void *tuple() const {
            assert(avail());
            return _q->_deq_location;
        }

	/// Consumes a tuple.
	DeqIterator& operator ++ () {
            assert(avail());

            void *next_deq_location = (char*)_q->_deq_location + _q->_tuple_size;
            if ((char*)next_deq_location + _q->_tuple_size > (char*)_q->_deq_page + _q->_pool.page_size()) {
                // Advance to next page; can free this page
                PageHdr *next_page = _q->_deq_page->next;
                _q->_pool.free(_q->_deq_page);
                _q->_deq_page = next_page;

                next_deq_location = (char*)next_page + sizeof(PageHdr);
            }

            _q->_deq_location = next_deq_location;
            ++(_q->_deq_count);
            return *this;
        }

      private:
        explicit DeqIterator(TupleQueue &q) : _q(&q) {}

	/// Prohibit postfix increment.
	DeqIterator operator ++ (int);

        TupleQueue *_q;

        friend class TupleQueue;
    };

    friend class SeekIterator {
	public:
        /// Dummy constructor.
        SeekIterator() : _q(0) {}
		
		/// Returns true iff there is a valid tuple to dequeue.
		// in SeekIterator this would not work right. Not sure if I can remove
		// this function.
		bool avail() const {
            assert(_q);

            return true; //_q->_deq_location != _q->_enq_location;
        }
		
		/// Returns a pointer to a tuple to be read.  Valid
		/// only if avail(). Do not check avail.
		const void *tuple() const {
			//assert(avail());
            return _seek_location;
        }
		
		/// Reads a tuple.
		SeekIterator& operator ++ () {
            //assert(avail());
			
            void *next_seek_location = (char*)_seek_location + _q->_tuple_size;
            if ((char*)next_seek_location + _q->_tuple_size > (char*)_seek_page + _q->_pool.page_size()) {
                // Advance to next page; can free this page
                PageHdr *next_page = NULL;//_q->_deq_page->next;
                //_q->_pool.free(_q->_deq_page);
                //_q->_deq_page = next_page;
				_seek_page = ((PageHdr *)_seek_page)->next;

                next_seek_location = (char*)_seek_page + sizeof(PageHdr);
            }

            _seek_location = next_seek_location;
            //++(_q->_deq_count);
            return *this;
        }

      private:
        explicit SeekIterator(TupleQueue &q) : _q(&q) 
		{ 
			_seek_location = _q->_deq_location; 
			_seek_page = _q->_deq_page; 
		}

		/// Prohibit postfix increment.
		SeekIterator operator ++ (int);
		
        TupleQueue *_q;

		// this is where our seek iterator is pointing
		void *_seek_location;
		/// The page we're set to dequeue data from
		void *_seek_page;

        friend class TupleQueue;
    };


    EnqIterator enq_iterator() { return EnqIterator(*this); }
    DeqIterator deq_iterator() { return DeqIterator(*this); }
    SeekIterator seek_iterator() { return SeekIterator(*this); }

    void dump() const;

    int size() const { return _enq_count - _deq_count; }

private:
    struct PageHdr {
        PageHdr *next;
    };

    PagePool &_pool;
    unsigned int _tuple_size;
    unsigned int _tuples_per_page;

    unsigned int _enq_count;
    unsigned int _deq_count;

    /// The page we're set to enqueue data on
    PageHdr *_enq_page;

    /// The location we're set to enqueue data on.  We can always
    /// enqueue data here
    void *_enq_location;

    /// The page we're set to dequeue data from
    PageHdr *_deq_page;

    /// The location we're set to dequeue data from.  We can always
    /// dequeue unless _deq_page_index == _enq_page_index
    void *_deq_location;
};

#endif
