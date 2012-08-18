#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct tbt_node {
    struct tbt_node *left;
    struct tbt_node *right;
    struct tbt_node *next;

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

int
tbt_add(struct tbt *tree, int value)
{
    struct tbt_node *node     = tree->root;
    struct tbt_node *new_node = NULL;
    struct tbt_node *cont     = NULL;
    struct tbt_node **start   = &(tree->start);

    new_node = malloc(sizeof(struct tbt_node));
    memset(new_node, 0, sizeof(struct tbt_node));
    new_node->value = value;

    if(! new_node) {
        return 0;
    }

    if(! tree->root) {
        tree->root  = new_node;
        tree->start = new_node;
        return 1;
    }

    while(1) {
        int node_value = node->value;

        if(value <= node_value) {
            if(node->left) {
                cont = node;
                node = node->left;
            } else {
                node->left     = new_node;
                new_node->next = node;
                *start         = new_node;
                break;
            }
        } else {
            if(node->right) {
                start = &(node->next);
                node  = node->right;
            } else {
                node->right    = new_node;
                node->next     = new_node;
                new_node->next = cont;
                break;
            }
        }
    }

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
