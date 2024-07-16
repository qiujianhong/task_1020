/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   封装linux红黑树算法
* @date：    2019-11-28
* @history： 
* @User deatil:
1. 创建树
   RBRoot* rbtree_create();
2. 插入节点
   int rbtree_insert_node(RBRoot *root, KEY key, unsigned char *pData, unsigned datalen, RBUserNode **ppUserNode);
3. 删除节点   
   void rbtree_delete_bykey(RBRoot *root, KEY key);
4. 查询节点
   RBUserNode *rbtree_search(RBRoot *root, KEY key);
5. 获取最小key值节点
   RBUserNode *rbtree_search_first(RBRoot *root);
6. 获取最大key值节点
   RBUserNode *rbtree_search_last(RBRoot *root);
7. 销毁树
   void rbtree_destroy_all(RBRoot *root);
*********************************************************************
*/

#ifndef    __RBTREE_H_
#define __RBTREE_H_


#ifdef __cplusplus
    extern "C" {
#endif

#define RBTREE_SUCCESS       0
#define RBTREE_ERROR_INPUT   -1
#define RBTREE_ERROR_MALLOC  -2
#define RBTREE_ERROR_EXIST   -3

typedef unsigned long long KEY;

/* 红黑树的节点 */
typedef struct tagRBTreeNode
{
    unsigned char         color;   // 颜色(RED 或 BLACK)
    struct tagRBTreeNode *left;    // 左孩子
    struct tagRBTreeNode *right;   // 右孩子
    struct tagRBTreeNode *parent;  // 父结点
}RBNode;
 
/* 红黑树的根 */
typedef struct tagRBTreeRoot
{
    RBNode *node;
}RBRoot;

typedef struct tagRBTreeUserNode
{
    RBNode        rbNode;
    KEY           key;

    unsigned int  datalen;
    unsigned char data[0];
}RBUserNode;

RBRoot* rbtree_create();
void rbtree_destroy_all(RBRoot *root);
int rbtree_insert_node(RBRoot *root, KEY key, unsigned char *pData, unsigned datalen, RBUserNode **ppUserNode);
void rbtree_delete_bykey(RBRoot *root, KEY key);
void rbtree_delete_bynode(RBRoot *root, RBUserNode *pUserNode);
RBUserNode *rbtree_search(RBRoot *root, KEY key);
RBUserNode *rbtree_search_first(RBRoot *root);
RBUserNode *rbtree_search_last(RBRoot *root);


#ifdef __cplusplus
}
#endif

#endif

