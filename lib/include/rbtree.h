/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��װlinux������㷨
* @date��    2019-11-28
* @history�� 
* @User deatil:
1. ������
   RBRoot* rbtree_create();
2. ����ڵ�
   int rbtree_insert_node(RBRoot *root, KEY key, unsigned char *pData, unsigned datalen, RBUserNode **ppUserNode);
3. ɾ���ڵ�   
   void rbtree_delete_bykey(RBRoot *root, KEY key);
4. ��ѯ�ڵ�
   RBUserNode *rbtree_search(RBRoot *root, KEY key);
5. ��ȡ��Сkeyֵ�ڵ�
   RBUserNode *rbtree_search_first(RBRoot *root);
6. ��ȡ���keyֵ�ڵ�
   RBUserNode *rbtree_search_last(RBRoot *root);
7. ������
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

/* ������Ľڵ� */
typedef struct tagRBTreeNode
{
    unsigned char         color;   // ��ɫ(RED �� BLACK)
    struct tagRBTreeNode *left;    // ����
    struct tagRBTreeNode *right;   // �Һ���
    struct tagRBTreeNode *parent;  // �����
}RBNode;
 
/* ������ĸ� */
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

