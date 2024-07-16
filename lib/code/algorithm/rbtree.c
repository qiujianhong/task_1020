/*
**********************************************************
@文件名称: rbtree.c
@功能说明: 红黑树操作实现
**********************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rbtree.h"

#define RED        0    // 红色节点
#define BLACK    1    // 黑色节点

#define rb_parent(r)       ((r)->parent)
#define rb_color(r)        ((r)->color)
#define rb_is_red(r)       (RED == (r)->color)
#define rb_is_black(r)     (BLACK == (r)->color)
#define rb_set_black(r)     do { (r)->color = BLACK; } while (0)
#define rb_set_red(r)       do { (r)->color = RED; } while (0)
#define rb_set_parent(r,p)  do { (r)->parent = (p); } while (0)
#define rb_set_color(r,c)   do { (r)->color = (c); } while (0)



/*
 * 销毁红黑树
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
 * 对红黑树的节点(x)进行左旋转
 *
 * 左旋示意图(对节点x进行左旋)：
 *      px                              px
 *     /                               /
 *    x                               y               
 *   /  \      --(左旋)-->           / \                #
 *  lx   y                          x  ry    
 *     /   \                       /  \
 *    ly   ry                     lx  ly 
 *
 *
 */
void rbtree_left_rotate(RBRoot *root, RBNode *x)
{
    // 设置x的右孩子为y
    RBNode *y = x->right;
 
    // 将 “y的左孩子” 设为 “x的右孩子”；
    // 如果y的左孩子非空，将 “x” 设为 “y的左孩子的父亲”
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
 
    // 将 “x的父亲” 设为 “y的父亲”
    y->parent = x->parent;
 
    if (x->parent == NULL)//修改红黑树的根节点
    {
        //tree = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
        root->node = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
    }
    else
    {
        if (x->parent->left == x)
            x->parent->left = y;    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
        else
            x->parent->right = y;    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
    }
     
    // 将 “x” 设为 “y的左孩子”
    y->left = x;
    // 将 “x的父节点” 设为 “y”
    x->parent = y;
}
 
/*
 * 对红黑树的节点(y)进行右旋转
 *
 * 右旋示意图(对节点y进行左旋)：
 *            py                               py
 *           /                                /
 *          y                                x                 
 *         /  \      --(右旋)-->            /  \ 
 *        x   ry                           lx   y 
 *       / \                                   / \
 *      lx  rx                                rx  ry
 *
 */
static void rbtree_right_rotate(RBRoot *root, RBNode *y)
{
    // 设置x是当前节点的左孩子。
    RBNode *x = y->left;
 
    // 将 “x的右孩子” 设为 “y的左孩子”；
    // 如果"x的右孩子"不为空的话，将 “y” 设为 “x的右孩子的父亲”
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
 
    // 将 “y的父亲” 设为 “x的父亲”
    x->parent = y->parent;
 
    if (y->parent == NULL)
    {
        //tree = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
        root->node = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
    }
    else
    {
        if (y == y->parent->right)
            y->parent->right = x;    // 如果 y是它父节点的右孩子，则将x设为“y的父节点的右孩子”
        else
            y->parent->left = x;    // (y是它父节点的左孩子) 将x设为“x的父节点的左孩子”
    }
 
    // 将 “y” 设为 “x的右孩子”
    x->right = y;
 
    // 将 “y的父节点” 设为 “x”
    y->parent = x;
}


/*
 * 红黑树插入修正函数
 *
 * 在向红黑树中插入节点之后(失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 插入的结点        // 对应《算法导论》中的z
 */
void rbtree_insert_fixup(RBRoot *root, RBNode *pNewNode)
{
    RBNode *parent, *gparent;
 
    // 若“父节点存在，并且父节点的颜色是红色”
    while ((parent = rb_parent(pNewNode)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);
 
        //若“父节点”是“祖父节点的左孩子”
        if (parent == gparent->left)
        {
            // Case 1条件：叔叔节点是红色
            {
                RBNode *uncle = gparent->right;
                if (uncle && rb_is_red(uncle))//没有节点进入该分支，如何构造？
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    pNewNode = gparent;
                    continue;
                }
            }
 
            // Case 2条件：叔叔是黑色，且当前节点是右孩子,叔叔不存在，也认为是黑色
            if (parent->right == pNewNode)//插入80节点时，先左旋，后右旋
            {
                RBNode *tmp;
                rbtree_left_rotate(root, parent);
                tmp = parent;
                parent = pNewNode;
                pNewNode = tmp;
            }
 
            // Case 3条件：叔叔是黑色，且当前节点是左孩子。
            rb_set_black(parent);//旋转前设置好颜色
            rb_set_red(gparent);//旋转前设置好颜色
            rbtree_right_rotate(root, gparent);
        }
        else//若父节点是祖父节点的右孩子
        {
            // Case 1条件：叔叔节点是红色
            {
                RBNode *uncle = gparent->left;//当插入60时，调整颜色即可，调整颜色后不符合红黑树，递归进行
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    pNewNode = gparent;
                    continue;//继续进行调整
                }
            }
 
            // Case 2条件：叔叔是黑色，且当前节点是左孩子，插入30时，先右旋，后左旋
            if (parent->left == pNewNode)
            {
                RBNode *tmp;
                rbtree_right_rotate(root, parent);
                tmp = parent;
                parent = pNewNode;
                pNewNode = tmp;
            }
 
            // Case 3条件：叔叔是黑色，且当前节点是右孩子。
            rb_set_black(parent);//旋转前设置好颜色
            rb_set_red(gparent);//旋转前设置好颜色
            rbtree_left_rotate(root, gparent);
        }
    }
 
    // 将根节点设为黑色
    rb_set_black(root->node);
}


/*
 * 添加节点：将节点(pNewNode)插入到红黑树中
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 插入的结点        // 对应《算法导论》中的z
 */
void rbtree_insert(RBRoot *root, RBNode *pFather, RBNode *pNewNode)
{
    RBUserNode *pUserNode = (RBUserNode *)pNewNode;
    RBUserNode *pUserFater = (RBUserNode *)pFather;
    // 1. 将节点添加到二叉查找树中。
    rb_parent(pNewNode) = pFather;//找到父节点并把要插入节点的父节点的指针修改
    //修改父节点的子节点指针
    if (pFather != NULL)
    {
        if (pUserNode->key < pUserFater->key)
            pFather->left = pNewNode;      // 情况2：若“node所包含的值” < “y所包含的值”，则将node设为“y的左孩子”
        else
            pFather->right = pNewNode;     // 情况3：(“node所包含的值” >= “y所包含的值”)将node设为“y的右孩子”
    }
    else
    {
        root->node = pNewNode;             // 情况1：若y是空节点，则将node设为根
    }
 
    // 2. 设置节点的颜色为红色
    pNewNode->color = RED;
 
    // 3. 将它重新修正为一颗二叉查找树
    rbtree_insert_fixup(root, pNewNode);
}





/*
 * 红黑树删除修正函数
 *
 * 在从红黑树中删除插入节点之后(红黑树失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 待修正的节点
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
                // Case 1: x的兄弟w是红色的 
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_left_rotate(root, parent);
                other = parent->right;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                (!other->right || rb_is_black(other->right)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的 
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->right || rb_is_black(other->right))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。 
                    rb_set_black(other->left);
                    rb_set_red(other);
                    rbtree_right_rotate(root, other);
                    other = parent->right;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
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
                // Case 1: x的兄弟w是红色的 
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_right_rotate(root, parent);
                other = parent->left;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                (!other->right || rb_is_black(other->right)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的 
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->left || rb_is_black(other->left))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。 
                    rb_set_black(other->right);
                    rb_set_red(other);
                    rbtree_left_rotate(root, other);
                    other = parent->left;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
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
 * 删除结点
 *
 * 参数说明：
 *     tree 红黑树的根结点
 *     node 删除的结点
 */
void rbtree_delete(RBRoot *root, RBNode *node)
{
    RBNode *child, *parent;
    int color;
 
    // 被删除节点的"左右孩子都不为空"的情况。
    if ( (node->left!=NULL) && (node->right!=NULL) )
    {
        // 被删节点的后继节点。(称为"取代节点")
        // 用它来取代"被删节点"的位置，然后再将"被删节点"去掉。
        RBNode *replace = node;
 
        // 获取后继节点
        replace = replace->right;
        while (replace->left != NULL)
            replace = replace->left;
 
        // "node节点"不是根节点(只有根节点不存在父节点)
        if (rb_parent(node))
        {
            if (rb_parent(node)->left == node)
                rb_parent(node)->left = replace;
            else
                rb_parent(node)->right = replace;
        }
        else
            // "node节点"是根节点，更新根节点。
            root->node = replace;
 
        // child是"取代节点"的右孩子，也是需要"调整的节点"。
        // "取代节点"肯定不存在左孩子！因为它是一个后继节点。
        child = replace->right;
        parent = rb_parent(replace);
        // 保存"取代节点"的颜色
        color = rb_color(replace);
 
        // "被删除节点"是"它的后继节点的父节点"
        if (parent == node)
        {
            parent = replace;
        }
        else
        {
            // child不为空
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
    // 保存"取代节点"的颜色
    color = node->color;
 
    if (child)
        child->parent = parent;
 
    // "node节点"不是根节点
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
* @brief     ：创建红黑树
* @param[in] ：
* @param[out]：
* @return    ：RBRoot *    根节点
* @Create    : 王津剑
* @Date      ：2019-11-28
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
* @brief     : 销毁红黑树
* @param[in] ：RBRoot * root   根节点
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-28
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
* @brief     ：新建结点(节点键值为key)，并将其插入到红黑树中
* @param[in] ：RBRoot *pRoot         根节点
               KEY key               key值
               unsigned char *pData  用户数据
               unsigned int datalen  数据长度
* @param[out]：RBUserNode **ppUserNode 叶子节点
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-11-28
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

    /* 添加到树中 */
    rbtree_insert(root, parent, &userNode->rbNode);
    *ppUserNode = userNode;

    return RBTREE_SUCCESS;
}

/**********************************************************************
* @name      : rbtree_delete_bykey
* @brief     ：删除键值为key的节点
* @param[in] ：RBRoot *pRoot         根节点
               KEY key               key值
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-28
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
* @brief     ：删除指定节点
* @param[in] ：RBRoot *pRoot         根节点
               RBUserNode *pUserNode 节点
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-28
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
* @brief     ：搜索键值为key的节点
* @param[in] ：RBRoot *pRoot         根节点
               KEY key               key值
* @param[out]：
* @return    ：RBUserNode *          节点
* @Create    : 王津剑
* @Date      ：2019-11-28
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
* @brief     ：查找"红黑树"最小key值节点
* @param[in] ：RBRoot *pRoot      根节点
* @param[out]：
* @return    ：RBUserNode *       叶子节点
* @Create    : 王津剑
* @Date      ：2019-11-28
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
* @brief     ：查找"红黑树"最大key值节点
* @param[in] ：RBRoot *pRoot      根节点
* @param[out]：
* @return    ：RBUserNode *       叶子节点
* @Create    : 王津剑
* @Date      ：2019-11-28
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




