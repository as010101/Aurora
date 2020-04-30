#ifndef BUFFERCACHE_DEFS_H
#define BUFFERCACHE_DEFS_H

//==============================================================================

// Note: The number assignments matter, as framelock types form a hierarchy
// of strength, which a small amount of code makes use of...
typedef enum 
  {
    // No lock at all is held.
    FRAMELOCK_NONE = 1, 

    // The holder of the lock has read-only access to the page buffer. The owner
    // can rely on the page buffer remaining in memory at its current location,
    // and its contents not changing.
    FRAMELOCK_READONLY = 2, 

    // Like FRAMELOCK_READONLY, except that the holder of this lock has the
    // exclusive right to modify the page's contents.
    FRAMELOCK_READWRITE = 3
  } FRAME_LOCK_TYPE;

//==============================================================================

typedef enum 
  {
    // There is no I/O operation presently active regarding this 
    // frame.
    FRAMEIO_IDLE, 

    // This frame's buffer is currently being loaded from disk.
    FRAMEIO_LOADING, 

    // This frame's buffer is currently being saved to disk.
    FRAMEIO_SAVING
  } FRAME_IO_STATE;

//==============================================================================

typedef enum 
  {
    // The buffer isn't presently associated with any page, so its content
    // should be seen as meaningless. Note that the frame using this buffer
    // might be associated with a page, it's just that this buffer hasn't
    // yet held that page's content.
    BUFFERSTATE_INIT,
    
    // The buffer accurately reflects what's on disk, or the page is newly
    // allocated so that a difference from what's on disk doesn't matter.
    BUFFERSTATE_CLEAN,                    
    
    // The buffer has been modified by the user-side code since it was last
    // loaded from disk, and the buffer hasn't yet been written back to disk.
    BUFFERSTATE_DIRTIED_BY_USER,

    // The buffer is dirty because a worker thread is in the process of
    // populating it from the disk.
    BUFFERSTATE_DIRTIED_BY_WORKERTHREADS
  } FRAME_BUFFER_STATE;


//==============================================================================

#endif
