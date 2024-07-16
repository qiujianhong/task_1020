/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��ֲlinux�ں�����
* @date��    2019-11-29
* @history�� 
* @User deatil:
1. �����ݽṹ�����LIST_HEAD_T��Ϊһ����Ա
2. ͨ��LIST_HEAD_T��ȡ�ṹ��, ʹ��
    LIST_GET_STRUCT(pos, type, member) 
3. �����ʼ��
    void list_head_init(LIST_HEAD_T *pList);
4. ������ͷ���ӽڵ�
    void list_add(LIST_HEAD_T *pNew, LIST_HEAD_T *pList);
5. ������β���ӽڵ�
    void list_add_tail(LIST_HEAD_T *pNew, LIST_HEAD_T *pList);
6. ɾ���ڵ�
    void list_del(LIST_HEAD_T *pEntry);
7. ��������
    LIST_FOR_EACH(pos, head)
    LIST_FOR_EACH_SAFE(pos, n, head)
*********************************************************************
*/

#ifndef    __LIST_H_
#define __LIST_H_


#ifdef __cplusplus
    extern "C" {
#endif

typedef struct tagListHead 
{
    struct tagListHead *next, *prev;
}LIST_HEAD_T;

/*
* �ж������Ƿ�Ϊ��
* head    LIST_HEAD_T ����ͷ
*/
#define LIST_IS_EMPTY(head)  (((LIST_HEAD_T *)(head))->next == (head))

/*
* ����
* pos     LIST_HEAD_T ָ��
* head    LIST_HEAD_T ����ͷ
*/
#define LIST_FOR_EACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/*
* ��ȫ����
* pos     LIST_HEAD_T ָ��
* n       LIST_HEAD_T ��ʱָ��
* head    LIST_HEAD_T ����ͷ
*/
#define LIST_FOR_EACH_SAFE(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

/* 
* ��ȡԭʼ�ṹ��
* pos     LIST_HEAD_T ָ��
* type    �ṹ������
* member  LIST_HEAD_T ��Ա����
*/
#define LIST_GET_STRUCT(pos, type, member) (type *)((char *)pos - (char *)((type *)0)->member)

void list_head_init(LIST_HEAD_T *pList);
void list_add(LIST_HEAD_T *pNew, LIST_HEAD_T *pList);
void list_add_tail(LIST_HEAD_T *pNew, LIST_HEAD_T *pList);
void list_del(LIST_HEAD_T *pEntry);


#ifdef __cplusplus
}
#endif

#endif

