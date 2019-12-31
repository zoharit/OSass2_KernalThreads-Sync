#ifndef KTHREAD_H
#define KTHREAD_H
#define MAX_STACK_SIZE 4000
#define MAX_MUTEXES 4096

enum mutex_state {UNLOCK ,LOCK};

struct kthread_mutex_t{
    int ID;
    enum mutex_state state;
    int lock;
    struct thread * my_thread;
};

int kthread_create(void (*start_func)(), void* stack);
int kthread_id();
int kthread_mutex_alloc();
int kthread_mutex_dealloc(int mutex_id);
int kthread_mutex_lock(int mutex_id);
int kthread_mutex_unlock(int mutex_id);
void minit();

trnmnt_tree* trnmnt_tree_alloc(int depth);
int trnmnt_tree_dealloc(trnmnt_tree* tree);
int trnmnt_tree_acquire(trnmnt_tree* tree,int ID);
int trnmnt_tree_release(trnmnt_tree* tree,int ID);
#endif