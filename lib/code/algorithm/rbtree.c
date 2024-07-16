/*
**********************************************************
@�ļ�����: rbtree.c
@����˵��: ���������ʵ��
**********************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rbtree.h"

#define RED        0    // ��ɫ�ڵ�
#define BLACK    1    // ��ɫ�ڵ�

#define rb_parent(r)       ((r)->parent)
#define rb_color(r)        ((r)->color)
#define rb_is_red(r)       (RED == (r)->color)
#define rb_is_black(r)     (BLACK == (r)->color)
#define rb_set_black(r)     do { (r)->color = BLACK; } while (0)
#define rb_set_red(r)       do { (r)->color = RED; } while (0)
#define rb_set_parent(r,p)  do { (r)->parent = (p); } while (0)
#define rb_set_color(r,c)   do { (r)->color = (c); } while (0)



/*
 * ���ٺ����
 */
static void rbtree_destroy(RBNode *pTreeNode)
{
    RBUserNode *pUserNode = NULL;
    if (NULL == pTreeNode)
    {
        return;
    }
    
    pUserNode = (RBUserNode *)pTreeNode;
    
    if (pUserNode->rbNode.left != NULL)
        rbtree_destroy(pUserNode->rbNode.left);
    if (pUserNode->rbNode.right != NULL)
        rbtree_destroy(pUserNode->rbNode.right);
 
    free(pUserNode);
}

/*
 * �Ժ�����Ľڵ�(x)��������ת
 *
 * ����ʾ��ͼ(�Խڵ�x��������)��
 *      px                              px
 *     /                               /
 *    x                               y               
 *   /  \      --(����)-->           / \                #
 *  lx   y                          x  ry    
 *     /   \                       /  \
 *    ly   ry                     lx  ly 
 *
 *
 */
void rbtree_left_rotate(RBRoot *root, RBNode *x)
{
    // ����x���Һ���Ϊy
    RBNode *y = x->right;
 
    // �� ��y�����ӡ� ��Ϊ ��x���Һ��ӡ���
    // ���y�����ӷǿգ��� ��x�� ��Ϊ ��y�����ӵĸ��ס�
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
 
    // �� ��x�ĸ��ס� ��Ϊ ��y�ĸ��ס�
    y->parent = x->parent;
 
    if (x->parent == NULL)//�޸ĺ�����ĸ��ڵ�
    {
        //tree = y;            // ��� ��x�ĸ��ס� �ǿսڵ㣬��y��Ϊ���ڵ�
        root->node = y;            // ��� ��x�ĸ��ס� �ǿսڵ㣬��y��Ϊ���ڵ�
    }
    else
    {
        if (x->parent->left == x)
            x->parent->left = y;    // ��� x�������ڵ�����ӣ���y��Ϊ��x�ĸ��ڵ�����ӡ�
        else
            x->parent->right = y;    // ��� x�������ڵ�����ӣ���y��Ϊ��x�ĸ��ڵ�����ӡ�
    }
     
    // �� ��x�� ��Ϊ ��y�����ӡ�
    y->left = x;
    // �� ��x�ĸ��ڵ㡱 ��Ϊ ��y��
    x->parent = y;
}
 
/*
 * �Ժ�����Ľڵ�(y)��������ת
 *
 * ����ʾ��ͼ(�Խڵ�y��������)��
 *            py                               py
 *           /                                /
 *          y                                x                 
 *         /  \      --(����)-->            /  \ 
 *        x   ry                           lx   y 
 *       / \                                   / \
 *      lx  rx                                rx  ry
 *
 */
static void rbtree_right_rotate(RBRoot *root, RBNode *y)
{
    // ����x�ǵ�ǰ�ڵ�����ӡ�
    RBNode *x = y->left;
 
    // �� ��x���Һ��ӡ� ��Ϊ ��y�����ӡ���
    // ���"x���Һ���"��Ϊ�յĻ����� ��y�� ��Ϊ ��x���Һ��ӵĸ��ס�
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
 
    // �� ��y�ĸ��ס� ��Ϊ ��x�ĸ��ס�
    x->parent = y->parent;
 
    if (y->parent == NULL)
    {
        //tree = x;            // ��� ��y�ĸ��ס� �ǿսڵ㣬��x��Ϊ���ڵ�
        root->node = x;            // ��� ��y�ĸ��ס� �ǿսڵ㣬��x��Ϊ���ڵ�
    }
    else
    {
        if (y == y->parent->right)
            y->parent->right = x;    // ��� y�������ڵ���Һ��ӣ���x��Ϊ��y�ĸ��ڵ���Һ��ӡ�
        else
            y->parent->left = x;    // (y�������ڵ������) ��x��Ϊ��x�ĸ��ڵ�����ӡ�
    }
 
    // �� ��y�� ��Ϊ ��x���Һ��ӡ�
    x->right = y;
 
    // �� ��y�ĸ��ڵ㡱 ��Ϊ ��x��
    y->parent = x;
}


/*
 * �����������������
 *
 * ���������в���ڵ�֮��(ʧȥƽ��)���ٵ��øú�����
 * Ŀ���ǽ������������һ�ź������
 *
 * ����˵����
 *     root ������ĸ�
 *     node ����Ľ��        // ��Ӧ���㷨���ۡ��е�z
 */
void rbtree_insert_fixup(RBRoot *root, RBNode *pNewNode)
{
    RBNode *parent, *gparent;
 
    // �������ڵ���ڣ����Ҹ��ڵ����ɫ�Ǻ�ɫ��
    while ((parent = rb_parent(pNewNode)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);
 
        //�������ڵ㡱�ǡ��游�ڵ�����ӡ�
        if (parent == gparent->left)
        {
            // Case 1����������ڵ��Ǻ�ɫ
            {
                RBNode *uncle = gparent->right;
                if (uncle && rb_is_red(uncle))//û�нڵ����÷�֧����ι��죿
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    pNewNode = gparent;
                    continue;
                }
            }
 
            // Case 2�����������Ǻ�ɫ���ҵ�ǰ�ڵ����Һ���,���岻���ڣ�Ҳ��Ϊ�Ǻ�ɫ
            if (parent->right == pNewNode)//����80�ڵ�ʱ����������������
            {
                RBNode *tmp;
                rbtree_left_rotate(root, parent);
                tmp = parent;
                parent = pNewNode;
                pNewNode = tmp;
            }
 
            // Case 3�����������Ǻ�ɫ���ҵ�ǰ�ڵ������ӡ�
            rb_set_black(parent);//��תǰ���ú���ɫ
            rb_set_red(gparent);//��תǰ���ú���ɫ
            rbtree_right_rotate(root, gparent);
        }
        else//�����ڵ����游�ڵ���Һ���
        {
            // Case 1����������ڵ��Ǻ�ɫ
            {
                RBNode *uncle = gparent->left;//������60ʱ��������ɫ���ɣ�������ɫ�󲻷��Ϻ�������ݹ����
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    pNewNode = gparent;
                    continue;//�������е���
                }
            }
 
            // Case 2�����������Ǻ�ɫ���ҵ�ǰ�ڵ������ӣ�����30ʱ����������������
            if (parent->left == pNewNode)
            {
                RBNode *tmp;
                rbtree_right_rotate(root, parent);
                tmp = parent;
                parent = pNewNode;
                pNewNode = tmp;
            }
 
            // Case 3�����������Ǻ�ɫ���ҵ�ǰ�ڵ����Һ��ӡ�
            rb_set_black(parent);//��תǰ���ú���ɫ
            rb_set_red(gparent);//��תǰ���ú���ɫ
            rbtree_left_rotate(root, gparent);
        }
    }
 
    // �����ڵ���Ϊ��ɫ
    rb_set_black(root->node);
}


/*
 * ��ӽڵ㣺���ڵ�(pNewNode)���뵽�������
 *
 * ����˵����
 *     root ������ĸ�
 *     node ����Ľ��        // ��Ӧ���㷨���ۡ��е�z
 */
void rbtree_insert(RBRoot *root, RBNode *pFather, RBNode *pNewNode)
{
    RBUserNode *pUserNode = (RBUserNode *)pNewNode;
    RBUserNode *pUserFater = (RBUserNode *)pFather;
    // 1. ���ڵ���ӵ�����������С�
    rb_parent(pNewNode) = pFather;//�ҵ����ڵ㲢��Ҫ����ڵ�ĸ��ڵ��ָ���޸�
    //�޸ĸ��ڵ���ӽڵ�ָ��
    if (pFather != NULL)
    {
        if (pUserNode->key < pUserFater->key)
            pFather->left = pNewNode;      // ���2������node��������ֵ�� < ��y��������ֵ������node��Ϊ��y�����ӡ�
        else
            pFather->right = pNewNode;     // ���3��(��node��������ֵ�� >= ��y��������ֵ��)��node��Ϊ��y���Һ��ӡ�
    }
    else
    {
        root->node = pNewNode;             // ���1����y�ǿսڵ㣬��node��Ϊ��
    }
 
    // 2. ���ýڵ����ɫΪ��ɫ
    pNewNode->color = RED;
 
    // 3. ������������Ϊһ�Ŷ��������
    rbtree_insert_fixup(root, pNewNode);
}





/*
 * �����ɾ����������
 *
 * �ڴӺ������ɾ������ڵ�֮��(�����ʧȥƽ��)���ٵ��øú�����
 * Ŀ���ǽ������������һ�ź������
 *
 * ����˵����
 *     root ������ĸ�
 *     node �������Ľڵ�
 */
static void rbtree_delete_fixup(RBRoot *root, RBNode *node, RBNode *parent)
{
    RBNode *other;
 
    while ((!node || rb_is_black(node)) && node != root->node)
    {
        if (parent->left == node)
        {
            other = parent->right;
            if (rb_is_red(other))
            {
                // Case 1: x���ֵ�w�Ǻ�ɫ�� 
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_left_rotate(root, parent);
                other = parent->right;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                (!other->right || rb_is_black(other->right)))
            {
                // Case 2: x���ֵ�w�Ǻ�ɫ����w����������Ҳ���Ǻ�ɫ�� 
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->right || rb_is_black(other->right))
                {
                    // Case 3: x���ֵ�w�Ǻ�ɫ�ģ�����w�������Ǻ�ɫ���Һ���Ϊ��ɫ�� 
                    rb_set_black(other->left);
                    rb_set_red(other);
                    rbtree_right_rotate(root, other);
                    other = parent->right;
                }
                // Case 4: x���ֵ�w�Ǻ�ɫ�ģ�����w���Һ����Ǻ�ɫ�ģ�����������ɫ��
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->right);
                rbtree_left_rotate(root, parent);
                node = root->node;
                break;
            }
        }
        else
        {
            other = parent->left;
            if (rb_is_red(other))
            {
                // Case 1: x���ֵ�w�Ǻ�ɫ�� 
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_right_rotate(root, parent);
                other = parent->left;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                (!other->right || rb_is_black(other->right)))
            {
                // Case 2: x���ֵ�w�Ǻ�ɫ����w����������Ҳ���Ǻ�ɫ�� 
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->left || rb_is_black(other->left))
                {
                    // Case 3: x���ֵ�w�Ǻ�ɫ�ģ�����w�������Ǻ�ɫ���Һ���Ϊ��ɫ�� 
                    rb_set_black(other->right);
                    rb_set_red(other);
                    rbtree_left_rotate(root, other);
                    other = parent->left;
                }
                // Case 4: x���ֵ�w�Ǻ�ɫ�ģ�����w���Һ����Ǻ�ɫ�ģ�����������ɫ��
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->left);
                rbtree_right_rotate(root, parent);
                node = root->node;
                break;
            }
        }
    }
    if (node)
        rb_set_black(node);
}

/*
 * ɾ�����
 *
 * ����˵����
 *     tree ������ĸ����
 *     node ɾ���Ľ��
 */
void rbtree_delete(RBRoot *root, RBNode *node)
{
    RBNode *child, *parent;
    int color;
 
    // ��ɾ���ڵ��"���Һ��Ӷ���Ϊ��"�������
    if ( (node->left!=NULL) && (node->right!=NULL) )
    {
        // ��ɾ�ڵ�ĺ�̽ڵ㡣(��Ϊ"ȡ���ڵ�")
        // ������ȡ��"��ɾ�ڵ�"��λ�ã�Ȼ���ٽ�"��ɾ�ڵ�"ȥ����
        RBNode *replace = node;
 
        // ��ȡ��̽ڵ�
        replace = replace->right;
        while (replace->left != NULL)
            replace = replace->left;
 
        // "node�ڵ�"���Ǹ��ڵ�(ֻ�и��ڵ㲻���ڸ��ڵ�)
        if (rb_parent(node))
        {
            if (rb_parent(node)->left == node)
                rb_parent(node)->left = replace;
            else
                rb_parent(node)->right = replace;
        }
        else
            // "node�ڵ�"�Ǹ��ڵ㣬���¸��ڵ㡣
            root->node = replace;
 
        // child��"ȡ���ڵ�"���Һ��ӣ�Ҳ����Ҫ"�����Ľڵ�"��
        // "ȡ���ڵ�"�϶����������ӣ���Ϊ����һ����̽ڵ㡣
        child = replace->right;
        parent = rb_parent(replace);
        // ����"ȡ���ڵ�"����ɫ
        color = rb_color(replace);
 
        // "��ɾ���ڵ�"��"���ĺ�̽ڵ�ĸ��ڵ�"
        if (parent == node)
        {
            parent = replace;
        }
        else
        {
            // child��Ϊ��
            if (child)
                rb_set_parent(child, parent);
            parent->left = child;
 
            replace->right = node->right;
            rb_set_parent(node->right, replace);
        }
 
        replace->parent = node->parent;
        replace->color = node->color;
        replace->left = node->left;
        node->left->parent = replace;
 
        if (color == BLACK)
            rbtree_delete_fixup(root, child, parent);
        return ;
    }
 
    if (node->left !=NULL)
        child = node->left;
    else
        child = node->right;
 
    parent = node->parent;
    // ����"ȡ���ڵ�"����ɫ
    color = node->color;
 
    if (child)
        child->parent = parent;
 
    // "node�ڵ�"���Ǹ��ڵ�
    if (parent)
    {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    }
    else
        root->node = child;
 
    if (color == BLACK)
        rbtree_delete_fixup(root, child, parent);
}






/**********************************************************************
* @name      : rbtree_create
* @brief     �����������
* @param[in] ��
* @param[out]��
* @return    ��RBRoot *    ���ڵ�
* @Create    : ����
* @Date      ��2019-11-28
* @Update    :
**********************************************************************/
RBRoot* rbtree_create()
{
    RBRoot *root = (RBRoot *)malloc(sizeof(RBRoot));
    if(NULL != root)
    {
        root->node = NULL;
    }
    
    return root;
}

/**********************************************************************
* @name      : rbtree_destroy_all
* @brief     : ���ٺ����
* @param[in] ��RBRoot * root   ���ڵ�
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-28
* @Update    :
**********************************************************************/
void rbtree_destroy_all(RBRoot *root)
{
    if (root != NULL)
        rbtree_destroy(root->node);
    free(root);
}

/**********************************************************************
* @name      : rbtree_insert_node
* @brief     ���½����(�ڵ��ֵΪkey)����������뵽�������
* @param[in] ��RBRoot *pRoot         ���ڵ�
               KEY key               keyֵ
               unsigned char *pData  �û�����
               unsigned int datalen  ���ݳ���
* @param[out]��RBUserNode **ppUserNode Ҷ�ӽڵ�
* @return    ��������
* @Create    : ����
* @Date      ��2019-11-28
* @Update    :
**********************************************************************/
int rbtree_insert_node(RBRoot *root, KEY key, unsigned char *pData, unsigned int datalen, RBUserNode **ppUserNode)
{
    RBUserNode *userNode = NULL;
    RBNode     *tempNode = NULL;
    RBNode     *parent = NULL;

    if(NULL == root || NULL == ppUserNode || 0 == datalen)
    {
        return RBTREE_ERROR_INPUT;
    }
    
    tempNode = root->node;
    
    while(tempNode)
    {
        userNode = (RBUserNode *)tempNode;
        parent = tempNode;
        if(key < userNode->key)
        {
            tempNode = tempNode->left;
        }
        else if (key > userNode->key)
        {
            tempNode = tempNode->right;
        }
        else
            return RBTREE_ERROR_EXIST;
    }

    userNode = (RBUserNode *)malloc(sizeof(RBUserNode) + datalen);
    if(NULL == userNode)
    {
        return RBTREE_ERROR_MALLOC;
    }
    memset(userNode, 0, sizeof(RBUserNode) + datalen);
    userNode->key = key;
    userNode->datalen = datalen;
    if(NULL != pData)
    {
        memcpy(userNode->data, pData, datalen);
    }

    /* ��ӵ����� */
    rbtree_insert(root, parent, &userNode->rbNode);
    *ppUserNode = userNode;

    return RBTREE_SUCCESS;
}

/**********************************************************************
* @name      : rbtree_delete_bykey
* @brief     ��ɾ����ֵΪkey�Ľڵ�
* @param[in] ��RBRoot *pRoot         ���ڵ�
               KEY key               keyֵ
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-28
* @Update    :
**********************************************************************/
void rbtree_delete_bykey(RBRoot *root, KEY key)
{
    RBUserNode *pUserNode = NULL;

    pUserNode = rbtree_search(root, key);
    if(NULL == pUserNode)
    {
        return;
    }
    rbtree_delete(root, &(pUserNode->rbNode));

    free(pUserNode);
}

/**********************************************************************
* @name      : rbtree_delete_bynode
* @brief     ��ɾ��ָ���ڵ�
* @param[in] ��RBRoot *pRoot         ���ڵ�
               RBUserNode *pUserNode �ڵ�
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-28
* @Update    :
**********************************************************************/
void rbtree_delete_bynode(RBRoot *root, RBUserNode *pUserNode)
{
    if(NULL == pUserNode)
    {
        return;
    }
    rbtree_delete(root, &(pUserNode->rbNode));
    free(pUserNode);
}

/**********************************************************************
* @name      : rbtree_search
* @brief     ��������ֵΪkey�Ľڵ�
* @param[in] ��RBRoot *pRoot         ���ڵ�
               KEY key               keyֵ
* @param[out]��
* @return    ��RBUserNode *          �ڵ�
* @Create    : ����
* @Date      ��2019-11-28
* @Update    :
**********************************************************************/
RBUserNode *rbtree_search(RBRoot *root, KEY key)
{
    RBUserNode *userNode = NULL;
    RBNode     *tempNode = NULL;

    if(NULL == root)
    {
        return NULL;
    }
    
    tempNode = root->node;
    
    while(tempNode)
    {
        userNode = (RBUserNode *)tempNode;
        if(key < userNode->key)
        {
            tempNode = tempNode->left;
        }
        else if (key > userNode->key)
        {
            tempNode = tempNode->right;
        }
        else
            return userNode;
    }

    return NULL;
}

/**********************************************************************
* @name      : rbtree_search_first
* @brief     ������"�����"��Сkeyֵ�ڵ�
* @param[in] ��RBRoot *pRoot      ���ڵ�
* @param[out]��
* @return    ��RBUserNode *       Ҷ�ӽڵ�
* @Create    : ����
* @Date      ��2019-11-28
* @Update    :
**********************************************************************/
RBUserNode *rbtree_search_first(RBRoot *root)
{
    RBNode     *tempNode = NULL;

    if(NULL == root || NULL == root->node)
    {
        return NULL;
    }
    
    tempNode = root->node;
    while(tempNode->left)
    {
        tempNode = tempNode->left;
    }

    return (RBUserNode *)tempNode;
}

/**********************************************************************
* @name      : rbtree_search_last
* @brief     ������"�����"���keyֵ�ڵ�
* @param[in] ��RBRoot *pRoot      ���ڵ�
* @param[out]��
* @return    ��RBUserNode *       Ҷ�ӽڵ�
* @Create    : ����
* @Date      ��2019-11-28
* @Update    :
**********************************************************************/
RBUserNode *rbtree_search_last(RBRoot *root)
{
    RBNode     *tempNode = NULL;

    if(NULL == root || NULL == root->node)
    {
        return NULL;
    }
    
    tempNode = root->node;
    while(tempNode->right)
    {
        tempNode = tempNode->right;
    }

    return (RBUserNode *)tempNode;
}




