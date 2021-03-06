#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct tbt_node {
    struct tbt_node *left;
    struct tbt_node *right;
    struct tbt_node *next;
    int height;

    int value;
};

struct tbt {
    struct tbt_node *root;
    struct tbt_node *start;
};

struct tbt *
tbt_new(void)
{
    struct tbt *tree;

    tree = malloc(sizeof(struct tbt));
    if(! tree) {
        return NULL;
    }

    tree->root  = NULL;
    tree->start = NULL;

    return tree;
}

static void
_add_helper(struct tbt_node *node, struct tbt_node *new_node, struct tbt_node **start, struct tbt_node *cont)
{
    int child_height;

    if(new_node->value < node->value) {
        if(node->left) {
            _add_helper(node->left, new_node, start, node);
        } else {
            node->left     = new_node;
            new_node->next = node;
            *start         = new_node;
        }

        child_height = node->left->height;
    } else if(new_node->value > node->value) {
        if(node->right) {
            _add_helper(node->right, new_node, &(node->next), cont);
        } else {
            node->right    = new_node;
            node->next     = new_node;
            new_node->next = cont;
        }

        child_height = node->right->height;
    }

    if(child_height + 1 > node->height) {
        node->height = child_height + 1;
    }
}

int
tbt_add(struct tbt *tree, int value)
{
    struct tbt_node *new_node = NULL;

    new_node = malloc(sizeof(struct tbt_node));
    memset(new_node, 0, sizeof(struct tbt_node));
    new_node->value  = value;
    new_node->height = 1;

    if(! new_node) {
        return 0;
    }

    if(! tree->root) {
        tree->root  = new_node;
        tree->start = new_node;
        return 1;
    }

    _add_helper(tree->root, new_node, &(tree->start), NULL);
    return 1;
}

void
tbt_traverse(struct tbt *tree, void (*callback)(int, void *), void *udata)
{
    struct tbt_node *node = tree->start;
    while(node) {
        callback(node->value, udata);
        node = node->next;
    }
}

void
tbt_destroy(struct tbt *tree)
{
    struct tbt_node *node = tree->start;
    while(node) {
        struct tbt_node *old_node = node;
        node = node->next;
        free(old_node);
    }
    free(tree);
}

static struct tbt_node *
_find_rightmost_node(struct tbt_node *node, struct tbt_node **rightmost_parent)
{
    while(node->right) {
        *rightmost_parent = node;
        node              = node->right;
    }

    return node;
}

static void
_fix_threading(struct tbt_node *node, struct tbt_node *parent, struct tbt_node **start)
{
    struct tbt_node **previous = NULL;

    if(node->left) {
        struct tbt_node *throwaway = NULL;
        struct tbt_node *rightmost = NULL;

        rightmost = _find_rightmost_node(node->left, &throwaway);
        previous  = &(rightmost->next);
    } else {
        if(! parent || parent->left == node) {
            previous = start;
        } else { /* is right child */
            previous = &(parent->next);
        }
    }

    *previous = node->next;
}

static void
_perform_node_delete(struct tbt_node *node, struct tbt_node *parent, struct tbt_node **start, struct tbt_node **node_source)
{
    if(node->left && node->right) {
        struct tbt_node *rightmost_parent = node;
        struct tbt_node *rightmost        = _find_rightmost_node(node->left,
            &rightmost_parent);

        node->value = rightmost->value;

        if(rightmost_parent == node) {
            _perform_node_delete(rightmost, rightmost_parent,
                start, &(rightmost_parent->left));
        } else {
            _perform_node_delete(rightmost, rightmost_parent,
                &(rightmost_parent->next), &(rightmost_parent->right));
        }
    } else if(node->left || node->right) {
        struct tbt_node *child = node->left
            ? node->left
            : node->right;

        _fix_threading(node, parent, start);

        *node_source = child;
        free(node);
    } else { /* no children */
        _fix_threading(node, parent, start);

        *node_source = NULL;
        free(node);
    }
}

void
tbt_delete(struct tbt *tree, int value)
{
    struct tbt_node **start = &(tree->start);
    struct tbt_node *parent = NULL;
    struct tbt_node *node   = tree->root;

    while(node) {
        int node_value = node->value;

        if(value == node_value) {
            struct tbt_node **node_source = NULL;

            if(parent) {
                node_source = parent->left == node
                    ? &(parent->left)
                    : &(parent->right);
            } else { /* root node */
                node_source = &(tree->root);
            }

            _perform_node_delete(node, parent, start, node_source);
            break;
        } else if(value < node_value) {
            if(node->left) {
                parent = node;
                node   = node->left;
            } else {
                break;
            }
        } else {
            if(node->right) {
                start  = &(node->next);
                parent = node;
                node   = node->right;
            } else {
                break;
            }
        }
    }
}

static void
_dump_node(struct tbt_node *node, int level)
{
    int i;

    for(i = 0; i < level; i++) {
        printf("  ");
    }
    printf("%p -> %p [%d]\n", node, node->next, node->value);

    if(node->left) {
        _dump_node(node->left, level + 1);
    }

    if(node->right) {
        _dump_node(node->right, level + 1);
    }
}

void
tbt_dump(struct tbt *tree)
{
    printf("start = %p\n", tree->start);
    _dump_node(tree->root, 0);
}

int
tbt_contains(struct tbt *tree, int value)
{
    struct tbt_node *node = tree->root;

    while(node && node->value != value) {
        if(value < node->value) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    return node ? 1 : 0;
}

static int
_height_helper(struct tbt_node *node)
{
    if(node == NULL) {
        return 0;
    } else {
        int left_height;
        int right_height;

        left_height  = _height_helper(node->left);
        right_height = _height_helper(node->right);

        if(left_height > right_height) {
            return left_height + 1;
        } else  {
            return right_height + 1;
        }
    }
}

int
tbt_height(struct tbt *tree)
{
    return tree->root ? tree->root->height : 0;
}
