#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "tournament_tree.h"

int treeid;
int tid;
int even = 0;
int mid;

//fibonachi
void fib()
{

  int n= 10;
  int f[n+2];
  int i;
  printf(1, "The Fib Series of 10: 0, 1, ");
  f[0] = 0;
  f[1] = 1;
  for (i = 2; i < n; i++)
  {
      f[i] = f[i-1] + f[i-2];
      printf(1, "%d, ",f[i]);
  }
   printf(1, "\n");
  kthread_exit();
}

//factorial
void fact()
{
    int res=1;
    int n=5;
    for(int i=1; i<=n; i++){
        res=res*i;
    }
    printf(1, "The fact of 5 is: %d \n",res);
 kthread_exit();

}

//print thread id
void MyPrinter(){
    printf(1, "tid = %d , ", kthread_id());
     kthread_exit();

}

//lock and unlock mutex
void
mutex_test(void)
{
    int lock=kthread_mutex_lock(mid);
    if(lock==0){
        printf(1, "succesful to lock\n");
    }
    else{
        printf(1,"usuccesful to lock\n");
    }
    even++;
    even++;
    printf(1, "should be even : %d\n", even);
    int unlock=kthread_mutex_unlock(mid);
    if(unlock==0){
        printf(1, "succesful to unlock\n");
    }
    else{
        printf(1,"usuccesful to unlock\n");
    }
    kthread_exit();
}


trnmnt_tree* my_tree;
void
my_tree_tests1(){
    sleep(200);
    my_tree=trnmnt_tree_alloc(3);
    int lock = trnmnt_tree_acquire(my_tree,5);
    if(lock>=0){
        printf(1, "tree1 succesful to lock as should be\n");
    }
    else{
        printf(1,"tree1 usuccesful to lock\n");
    }
    sleep(200);
    //critical section
    even++;
    even++;
    printf(1, "tree1 should be even : %d\n", even);
    //release id = 5
    int unlock = trnmnt_tree_release(my_tree,5);
    if(unlock>=0){
        printf(1, "tree1 succesful to unlock as should be\n");
    }
    else{
        printf(1,"tree usuccesful to unlock\n");
    }
    sleep(300);
    trnmnt_tree_dealloc(my_tree);
    kthread_exit();
}



void
my_tree_tests2(){
    sleep(400);
    int lock = trnmnt_tree_acquire(my_tree,5);
    if(lock>=0){
        printf(1, "tree2 succesful to lock\n");
    }
    else{
        printf(1,"tree2 usuccesful to lock as should be\n");
    }
    //critical section
    even++;
    even++;
    printf(1, "tree2 should be even : %d\n", even);
    //release id = 5
    sleep(300);
    int unlock = trnmnt_tree_release(my_tree,5);
    if(unlock>=0){
        printf(1, "tree2 succesful to unlock\n");
    }
    else{
        printf(1,"tree2 usuccesful to unlock as should be\n");
    }
    trnmnt_tree_dealloc(my_tree);
    kthread_exit();
}

int main(int argc, char** argv){
    // 2 *********************************
     printf(1, "*********************************START KTHREAD TESTS*********************************\n");
    void * my_stack=malloc(4000);
    int tid1 = kthread_create(fib,my_stack);
    kthread_join(tid1);
    int tid2 = kthread_create(fact,my_stack);
    kthread_join(tid2);
    for (int i=0;i<14;i++){
        tid = kthread_create(MyPrinter,my_stack);
        kthread_join(tid);
    }
    printf(1,"\n");
    // 3.1 *********************************
    printf(1, "*********************************START MUTEX TESTS*********************************\n");
    mid=kthread_mutex_alloc();
    tid = kthread_create(mutex_test,my_stack);
    kthread_join(tid);
    kthread_mutex_dealloc(mid);
    tid = kthread_create(mutex_test,my_stack);
    mid=kthread_mutex_alloc();
    kthread_join(tid);
    kthread_mutex_dealloc(mid);
    printf(1,"\n");
    // 3.2 *********************************
    printf(1, "*********************************START TUURNAMENT TREE TESTS*********************************\n");
    //alloc a new tree with depth=3
    trnmnt_tree* tree;
    tree=trnmnt_tree_alloc(3);
    //try to lock id = 5
    int lock = trnmnt_tree_acquire(tree,5);
    if(lock>=0){
        printf(1, "main tree succesful to lock\n");
    }
    else{
        printf(1,"main tree usuccesful to lock\n");
    }
    //critical section
    even++;
    even++;
    printf(1, "main tree should be even : %d\n", even);
    //release id = 5
    int unlock = trnmnt_tree_release(tree,5);
    if(unlock>=0){
        printf(1, "main tree succesful to unlock\n");
    }
    else{
        printf(1,"main tree usuccesful to unlock\n");
    }
    trnmnt_tree_dealloc(tree);
    int treeid[2];
    // tests the tree for more then one thread
    void * my_stack1=malloc(4000);
    void * my_stack2=malloc(4000);

    printf(1, "*********************************START TUURNAMENT TREE THREADS*********************************\n");
    for(int i=0; i<2; i++){
        if(i==0)
            treeid[i] = kthread_create(my_tree_tests1,my_stack1);
        else
            treeid[i] = kthread_create(my_tree_tests2,my_stack2);

    }
    for(int i=0;i<2;i++){
            kthread_join(treeid[i]);

    }
    free(my_stack);
    free(my_stack1);
    free(my_stack2);
    exit();
}
