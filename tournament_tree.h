#ifndef trnmnt_tree_H_
#define trnmnt_tree_H_

typedef struct turnanmnt_tree {
    int *mutex_ids;
    int *thread_ids;
    int depth;
}trnmnt_tree;

trnmnt_tree* trnmnt_tree_alloc(int depth);
int trnmnt_tree_dealloc(trnmnt_tree* tree);
int trnmnt_tree_acquire(trnmnt_tree* tree,int ID);
int trnmnt_tree_release(trnmnt_tree* tree,int ID);
#endif
