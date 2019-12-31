#include "types.h"
#include "user.h"
#include "stat.h"
#include "param.h"
#include "tournament_tree.h"

int alloc_num=0;

trnmnt_tree* trnmnt_tree_alloc(int depth){
    int tree_size=1<<depth;
    trnmnt_tree *mytree = malloc(sizeof (struct turnanmnt_tree));
    mytree->depth=depth;
    mytree->mutex_ids=malloc(sizeof(int)*tree_size);
    for(int i = 0; i<tree_size;i++){
        int mid=kthread_mutex_alloc();
        mytree->mutex_ids[i]=mid;
        if(mid<0){
            for(int j=0; j<i;j++){
                kthread_mutex_dealloc(mytree->mutex_ids[j]);
                }
            break;
        }
    }
    return mytree;
}

int trnmnt_tree_dealloc(trnmnt_tree* tree){
    int tree_size=1<<tree->depth;
    int mid;
    int res=0;
    if(tree==0){
     return -1;
    }
    if(alloc_num!=0){
        return -1;
    }
    if(tree_size>=0){
    for(int i = 0; i<tree_size;i++){
        mid=kthread_mutex_dealloc(tree->mutex_ids[i]);
        if(mid<0){
            res=-1;
        }
    }
    }
    if(res==0){
    tree->depth=-1;
    free(tree->mutex_ids);
    free(tree);}
    return res;
}


int trnmnt_tree_acquire(trnmnt_tree* tree,int ID){
    int p_id=(1<<tree->depth)+ID;
    int mid=0;
    int res=0;
    if(ID<(1<<tree->depth)){
            for(p_id = p_id/2;p_id>0;p_id=p_id/2){
                alloc_num++;
                mid=kthread_mutex_lock(tree->mutex_ids[p_id]);
                if(mid<0){
                    res=-1;
                    break;
                }
            }
    }

    return res;
}

int trnmnt_tree_release(trnmnt_tree* tree,int ID){
    int depth=tree->depth;
    int p_id=(1<<depth)+ID;
    int mutex_to_release[depth];
    int res=0;
    int i=0;
    if(ID<(1<<depth)){
            for(p_id = p_id/2;p_id>0;p_id=p_id/2){
                mutex_to_release[i]=tree->mutex_ids[p_id];
                alloc_num--;
                i++;
            }
    }
    for(i=depth-1;i>=0;--i){
        int mid=kthread_mutex_unlock(mutex_to_release[i]);
        if(mid<0){
            res=-1;
            break;
        }
    }
    return res;
}
