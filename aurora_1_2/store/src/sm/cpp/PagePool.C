
#include <sm/include/PagePool.H>
#include <LockHolder.H>

#include <stdlib.h>
#include <malloc.h>
#include <sys/mman.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>

using namespace std;

PagePool::PagePool(unsigned int page_size,
                   unsigned int capacity,
                   bool locked) :
    _page_size(page_size),
    _capacity(capacity),
    _alloced_pages(0),
    _block(0),
    _block_size(page_size * capacity),
    _locked(locked),
    _free_list(0)
{
    int sys_pagesize = getpagesize();

    if (_block_size % sys_pagesize) {
        // Pad up to a full page
        _block_size += sys_pagesize - _block_size % sys_pagesize;
    }

    _block = memalign(sys_pagesize, _block_size);
    if (!_block) {
        throw SmTooFullException(__FILE__, __LINE__,
                                 "Unable to allocate memory block");
    }

    if (_locked) {
        if (mlock(_block, _block_size)) {
            free(_block);
            throw SmException(__FILE__, __LINE__,
                              "mlock: " + string(strerror(errno)));
        }
    }

    // Set up afree list.  _free_list points to the beginning of the
    // block; each block points to the block after it
    _free_list = (FreeNode*)_block;

    FreeNode *node = (FreeNode*)_block;

    for (unsigned int i = 0; i < capacity - 1; ++i) {
        node->next = (FreeNode*)((char*)node + _page_size);
        node = node->next;
    }
    node->next = 0;
}

PagePool::~PagePool() {
    if (_block) {
        if (_locked)
            munlock(_block, _block_size);
        ::free(_block);
    }
}

void *PagePool::alloc()
{
    LockHolder hold(_mutex);

    if (!_free_list)
        throw SmTooFullException(__FILE__, __LINE__, "All blocks in storage manager in use");

    FreeNode *node = _free_list;
    _free_list = _free_list->next;
    ++_alloced_pages;
    return node;
}

void PagePool::free(void *page)
{
    LockHolder hold(_mutex);

    // Make sure the address is within the block
    assert(page >= _block);
    assert(page < (char*)_block + _capacity * _page_size);

    // Make sure the address is page-aligned
    assert(((char*)page - (char*)_block) % _page_size == 0);

    // Add to free list
    ((FreeNode*)page)->next = _free_list;
    _free_list = (FreeNode*)page;
    --_alloced_pages;
}

void PagePool::dump() const
{
    cerr << "PagePool {\n"
         << "   _page_size = " << _page_size << "\n"
         << "   _capacity = " << _capacity << "\n"
         << "   _alloced_pages = " << _alloced_pages << "\n"
         << "   _block = " << _block << "\n"
         << "   _free = {";

    bool first = true;
    for (FreeNode *node = _free_list; node; node = node->next) {
        if (first) {
            first = false;
        } else {
            cerr << ",";
        }
        cerr << node;
    }
    cerr << "}\n"
         << "}"
         << endl;
}

void PagePool::dumpNoFreeList() const
{
    cerr << "PagePool {\n"
         << "   _page_size = " << _page_size << "\n"
         << "   _capacity = " << _capacity << "\n"
         << "   _alloced_pages = " << _alloced_pages << "\n"
         << "   _block = " << _block << "\n}" << endl;
    
}
