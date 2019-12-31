#ifndef KTHREAD_H_
#define KTHREAD_H_

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "proc.h"
#include "kthread.h"

extern struct {
    struct spinlock lock;
    struct proc proc[NPROC];
} ptable;

extern int nexttid;
extern int ProcAlive();
extern void trapret(void);
extern void forkret(void);
extern void unlock_wakeup(void *chan);

struct {
    struct spinlock mlock;
    struct kthread_mutex_t mutexes[MAX_MUTEXES];
} mutextable;


void
minit(void)
{
  initlock(&mutextable.mlock, "m_lock");
}

int i=0;
int
kthread_create(void (*start_func)(), void* stack){
  char *st;
  struct thread *t;
  struct proc *p = myproc();
  i++;
  acquire(&ptable.lock);
  for(t = p->threads; t<&myproc()->threads[NTHREAD]; t++){
    if(t->state == UNUSE){
      goto found;
    }
  }
  release(&ptable.lock);
  return -1;
found:
  t->tid=nexttid++;
  t->proc=p;
  t->state=NEW;
  release(&ptable.lock);

  // Allocate kernel stack.
  if((t->kstack = kalloc()) == 0){
    t->state = UNUSE;
    return 0;
  }
  st = t->kstack + KSTACKSIZE;
  // Leave room for trap frame.compatible type for argument 1 of ‘acquire’
  st -= sizeof *t->tf;
  t->tf = (struct trapframe*)st;
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  st -= 4;
  *(uint*)st = (uint)trapret;
  st -= sizeof *t->context;
  t->context = (struct context*)st;
  memset(t->context, 0, sizeof *t->context);
  t->context->eip = (uint)forkret;

  *t->tf=*mythread()->tf;
  t->tf->eip = (uint)start_func;
  t->tf->esp = (uint)(stack);

  acquire(&ptable.lock);
  t->state = READY;
  release(&ptable.lock);
  return t->tid;
}


int
kthread_id(){
  if(mythread()->tid<=0 || mythread()->tid>nexttid){
    return -1;
  }
    return mythread()->tid;
}

void
kthread_exit(){
    acquire(&ptable.lock);
        if (!ProcAlive()){
        release(&ptable.lock);
        exit();
    }
    mythread()->killed=0;
    unlock_wakeup(myproc());
    mythread()->state = TERMINATE;
    sched();
    panic("kthread exit");
}


int
kthread_join(int thread_id){
    struct thread *t;
    acquire(&ptable.lock);
        for(t = myproc()->threads ; t<&myproc()->threads[NTHREAD];t++){
            if(t->tid == thread_id){
                goto found;
            }
        }
        release(&ptable.lock);
        return -1;
        found:
        while(t->state != TERMINATE && t->state != UNUSE){
                sleep(myproc(), &ptable.lock);
        }
         if(t->state == TERMINATE){
            if (t->kstack)
                kfree(t->kstack);
            t->kstack=0;
            t->proc = 0;
            t->state = UNUSE;
            t->chan=0;
            t->context = 0;
            t->tid=0;
         }
        release(&ptable.lock);
        return 0;
}
int mutexID=-1;

int kthread_mutex_alloc(){
  int res=-1;
     struct kthread_mutex_t *m;
     acquire(&mutextable.mlock);
         for(m = mutextable.mutexes; m < &(mutextable.mutexes[MAX_MUTEXES]); m++){
           mutexID++;
           if(m->state==UNLOCK){
            m->ID=mutexID;
            m->state=LOCK;
            m->lock=0;

            res=m->ID;
            break;
           }
        }
    release(&mutextable.mlock);
    return res;
}

int
kthread_mutex_dealloc(int mutex_id){
  struct kthread_mutex_t *m;
  int res=-1;
  acquire(&mutextable.mlock);
  for(m = mutextable.mutexes; m < &mutextable.mutexes[MAX_MUTEXES]; m++){
        if(m->ID == mutex_id && m->state==LOCK && m->lock==0){
                m->state = UNLOCK;
                m->ID= -1;
                m->my_thread=0;
                res=0;
        }
  }
  release(&mutextable.mlock);
  return res;
}


int
kthread_mutex_lock(int mutex_id){
    int res=-1;
    struct kthread_mutex_t *m;
    if(mutex_id<0 || mutex_id>MAX_MUTEXES)
        return res;
    acquire(&mutextable.mlock);
      for(m = mutextable.mutexes; m < &mutextable.mutexes[MAX_MUTEXES]; m++){
          if(m->ID==mutex_id){
              goto found;
          }
      }
    release(&mutextable.mlock);
    return res;
    found:
    if(m->my_thread==mythread()){
            release(&mutextable.mlock);
    return res;
    }
    while(m->lock==1 ){
        sleep(m,&mutextable.mlock);
    }
    m->lock = 1;
    m->my_thread=mythread();
    release(&mutextable.mlock);
    res=0;
    return res;
}

int
kthread_mutex_unlock(int mutex_id){
    int res=-1;
    struct kthread_mutex_t *m;
    if(mutex_id<0 || mutex_id>MAX_MUTEXES)
        return res;
            acquire(&mutextable.mlock);

      for(m = mutextable.mutexes; m < &mutextable.mutexes[MAX_MUTEXES]; m++){
          if(m->ID==mutex_id){
              goto found;
          }
      }
    release(&mutextable.mlock);
    return res;
    found:
    if(m->my_thread!=mythread()){
        release(&mutextable.mlock);
        return res;
    }
    if (m->state == UNLOCK){
        return res;
    }
    m->lock = 0;
    m->my_thread=0;
    unlock_wakeup(m);
    release(&mutextable.mlock);
    res=0;
    return res;
 }
// int
// kthread_mutex_unlock(int mutex_id){

//     struct kthread_mutex_t *m = &mutextable.mutexes[mutex_id];
//     if (m->state == LOCK){
//         return -1;
//     }

//      acquire(&ptable.lock);
//      m->lock = 0;

//      unlock_wakeup(m);
//      release(&ptable.lock);
//      return 0;
// }


#endif /* KTHREAD_H */
