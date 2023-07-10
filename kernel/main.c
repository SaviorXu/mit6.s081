#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){//判断当前的CPU的ID是否为主CPU
    consoleinit();//控制台初始化
    printfinit();//打印模式初始化
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator 物理页分配策略的初始化
    kvminit();       // create kernel page table创建内核页表，进行页表初始化，页表相当于是链表
    kvminithart();   // turn on paging打开分页机制
    procinit();      // process table创建进程表
    trapinit();      // trap vectors初始化中断异常处理程序
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller设置相应外部中断的处理程序
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache磁盘缓冲初始化
    iinit();         // inode table磁盘节点的初始化
    fileinit();      // file table文件系统的初始化
    virtio_disk_init(); // emulated hard disk磁盘初始化
    userinit();      // first user process创建第一个用户进程
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();        
}
