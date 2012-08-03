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
tbt_new(int value)
{
    struct tbt *tree;

    tree = malloc(sizeof(struct tbt));
    if(! tree) {
        return NULL;
    }

    tree->root = malloc(sizeof(struct tbt_node));
    if(! tree->root) {
        return NULL;
    }
    tree->start = tree->root;

    memset(tree->root, sizeof(struct tbt_node), 0);
    tree->root->value = value;

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
    memset(new_node, sizeof(struct tbt_node), 0);
    new_node->value = value;

    if(! new_node) {
        return 0;
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
