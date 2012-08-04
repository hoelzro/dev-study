#include <stdio.h>
#include "tbt.h"

void
print_node(int value, void *data)
{
    printf("value = %d\n", value);
}

int
main(void)
{
    struct tbt *tree;

    tree = tbt_new();

    tbt_add(tree, 4);
    tbt_add(tree, 1);
    tbt_add(tree, 6);
    tbt_add(tree, 7);
    tbt_add(tree, 5);
    tbt_add(tree, 2);
    tbt_add(tree, 3);

    tbt_traverse(tree, print_node, NULL);

    tbt_destroy(tree);

    return 0;
}
