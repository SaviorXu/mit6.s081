// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"
#define HASHSIZE 13
// struct {
//   struct spinlock lock;
//   struct buf buf[NBUF];

//   // Linked list of all buffers, through prev/next.
//   // Sorted by how recently the buffer was used.
//   // head.next is most recent, head.prev is least.
//   struct buf head;
// } bcache;

struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  struct buf buckets[HASHSIZE];
  struct spinlock hashlocks[HASHSIZE];
} bcache;

int getIndex(int blockno)
{
  return blockno%HASHSIZE;
}
// void
// binit(void)
// {
//   struct buf *b;

//   initlock(&bcache.lock, "bcache");
  

//   // Create linked list of buffers
//   bcache.head.prev = &bcache.head;
//   bcache.head.next = &bcache.head;
//   for(b = bcache.buf; b < bcache.buf+NBUF; b++){
//     b->next = bcache.head.next;
//     b->prev = &bcache.head;
//     initsleeplock(&b->lock, "buffer");
//     bcache.head.next->prev = b;
//     bcache.head.next = b;
//   }
// }

//savior
void
binit(void)
{
  //printf("binit\n");
  initlock(&bcache.lock, "bcache");
  
  //savior
  for(int i=0;i<HASHSIZE;i++)
  {
    initlock(&bcache.hashlocks[i],"bcache");
  }
  for(int i=0;i<HASHSIZE;i++)
  {
    bcache.buckets[i].prev=&bcache.buckets[i];
    bcache.buckets[i].next=&bcache.buckets[i];
  }
  for(int i=0;i<NBUF;i++)
  {
    int index=getIndex(bcache.buf[i].blockno);
    bcache.buf[i].next=bcache.buckets[index].next;
    bcache.buf[i].prev=&bcache.buckets[index];
    bcache.buf[i].buf_ticks=ticks;
    initsleeplock(&bcache.buf[i].lock,"buffer");
    bcache.buckets[index].next->prev=&bcache.buf[i];
    bcache.buckets[index].next=&bcache.buf[i];
  }
  //printf("binit finish\n");
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
// static struct buf*
// bget(uint dev, uint blockno)
// {
//   struct buf *b;

//   acquire(&bcache.lock);

//   // Is the block already cached?
//   for(b = bcache.head.next; b != &bcache.head; b = b->next){
//     if(b->dev == dev && b->blockno == blockno){
//       b->refcnt++;
//       release(&bcache.lock);
//       acquiresleep(&b->lock);
//       return b;
//     }
//   }

//   // Not cached.
//   // Recycle the least recently used (LRU) unused buffer.
//   for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
//     if(b->refcnt == 0) {
//       b->dev = dev;
//       b->blockno = blockno;
//       b->valid = 0;
//       b->refcnt = 1;
//       release(&bcache.lock);
//       acquiresleep(&b->lock);
//       return b;
//     }
//   }
//   panic("bget: no buffers");
// }


//savior
static struct buf*
bget(uint dev, uint blockno)
{ 

  //printf("acquire bcache lock\n");
  // Is the block already cached?
    struct buf *b;
    int index=getIndex(blockno);
    acquire(&bcache.hashlocks[index]);
    //printf("acquire hashlocks\n");
    for(b=bcache.buckets[index].next;b!=&bcache.buckets[index];b=b->next)
    {
      if(b->dev==dev&&b->blockno==blockno)
      {
        b->refcnt++;
        //b->buf_ticks=ticks;
        release(&bcache.hashlocks[index]);
        acquiresleep(&b->lock);
        //printf("bget finish1\n");
        return b;
      }
    }
    release(&bcache.hashlocks[index]);


  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  int flag=-1;
  uint min = __UINT32_MAX__;
  struct buf *min_buf=bcache.buckets[0].next;
  acquire(&bcache.lock);
  for(b=bcache.buckets[index].next;b!=&bcache.buckets[index];b=b->next)
  {
    if(b->dev==dev&&b->blockno==blockno)
    {
      acquire(&bcache.hashlocks[index]);
      b->refcnt++;
      //b->buf_ticks=ticks;
      release(&bcache.hashlocks[index]);
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  for(int i=0;i<HASHSIZE;i++)
  {
    if(i!=index)
    {
      acquire(&bcache.hashlocks[i]);
      for(b=bcache.buckets[i].next;b!=&bcache.buckets[i];b=b->next)
      {
        if(b->refcnt==0&&b->buf_ticks<min)
        {
          min_buf=b;
          min=b->buf_ticks;
          flag=i;
        }
      }
      release(&bcache.hashlocks[i]);
    }
  }
  if(flag!=-1)
  {
    acquire(&bcache.hashlocks[flag]);
    min_buf->next->prev=min_buf->prev;
    min_buf->prev->next=min_buf->next;
    release(&bcache.hashlocks[flag]);
    acquire(&bcache.hashlocks[index]);
    min_buf->next=bcache.buckets[index].next;
    min_buf->prev=&bcache.buckets[index];
    bcache.buckets[index].next->prev=min_buf;
    bcache.buckets[index].next=min_buf;
    min_buf->dev=dev;
    min_buf->blockno=blockno;
    min_buf->valid=0;
    min_buf->refcnt=1;
    min_buf->buf_ticks=ticks;
    release(&bcache.hashlocks[index]);
    acquiresleep(&min_buf->lock);
    release(&bcache.lock);
    return min_buf;
  }
  release(&bcache.lock);
  panic("bget:no buffers");
}


// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
// void
// brelse(struct buf *b)
// {
//   if(!holdingsleep(&b->lock))
//     panic("brelse");

//   releasesleep(&b->lock);

//   acquire(&bcache.lock);
//   b->refcnt--;
//   if (b->refcnt == 0) {
//     // no one is waiting for it.
//     b->next->prev = b->prev;
//     b->prev->next = b->next;
//     b->next = bcache.head.next;
//     b->prev = &bcache.head;
//     bcache.head.next->prev = b;
//     bcache.head.next = b;
//   }
  
//   release(&bcache.lock);
// }


//savior
void
brelse(struct buf *b)
{
  //printf("brelse start\n");
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int index=getIndex(b->blockno);

  acquire(&bcache.hashlocks[index]);
  b->refcnt--;
  if(b->refcnt==0)
  {
    b->buf_ticks=ticks;
  }
  
  release(&bcache.hashlocks[index]);
  //printf("brelse finish\n");
}

void
bpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

void
bunpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}


