// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct{
  struct spinlock lock;
  uint ref[PGNUM];
}count;

void
kinit()
{
  initlock(&count.lock,"count");
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  {
    count.ref[(uint64)(p-KERNBASE)/PGSIZE]=1;
    kfree(p);
  }
    
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  int index=(uint64)(pa-KERNBASE)/PGSIZE;
  acquire(&count.lock);
  count.ref[index]--;
  release(&count.lock);
  //printf("kfree:index=%d count.ref[index]=%d\n",index,count.ref[index]);
  if(count.ref[index]>0)
  {
    return;
  }

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  //printf("1\n");
  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);
  //printf("2\n");
  if(r)
  {
    memset((char*)r, 5, PGSIZE); // fill with junk
    uint64 index=(uint64)((uint64)r-(uint64)KERNBASE)/PGSIZE;
    //printf("r=%p index=%d\n",r,index);
    acquire(&count.lock);
    count.ref[index]=1;
    release(&count.lock);
  }
    
  //printf("3\n");
  
  
  //printf("4\n");
  return (void*)r;
}

void AddRef(uint64 index)
{
  //printf("index=%d\n",index);
  acquire(&count.lock);
  count.ref[index]++;
  release(&count.lock);
}

int GetRef(uint64 pa)
{
  uint64 index=(pa-KERNBASE)/PGSIZE;
  return count.ref[index];
}