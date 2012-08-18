#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tbt.h"

typedef void (*permute_callback)(const int *, size_t, void *);

typedef struct {
    int element;
    permute_callback callback;
    void *udata;
} permute_helper_context;

static void
permute_helper_callback(const int *perm, size_t perm_len, void *udata)
{
    size_t i;
    int temp;
    permute_helper_context *ctx = (permute_helper_context *) udata;
    int workingcopy[perm_len + 1];

    memset(workingcopy, 0, sizeof(workingcopy));
    memcpy(workingcopy + 1, perm, sizeof(int) * perm_len);
    workingcopy[0] = ctx->element;

    for(i = 0; i <= perm_len; i++) {
        ctx->callback(workingcopy, perm_len + 1, ctx->udata);

        if(i != perm_len) {
            temp               = workingcopy[i];
            workingcopy[i]     = workingcopy[i + 1];
            workingcopy[i + 1] = temp;
        }
    }
}

static void
permute(const int *sequence, size_t seq_len, permute_callback callback, void *udata)
{
    if(seq_len == 1) {
        callback(sequence, seq_len, udata);
    } else {
        permute_helper_context context = {
            .element  = sequence[0],
            .callback = callback,
            .udata    = udata
        };

        permute(sequence + 1, seq_len - 1, permute_helper_callback, &context);
    }
}

static struct tbt *
construct_tree_from_sequence(const int *seq, size_t seq_len)
{
    struct tbt *tree = NULL;
    size_t i;

    tree = tbt_new();
    assert(tree);

    for(i = 0; i < seq_len; i++) {
        tbt_add(tree, seq[i]);
    }

    return tree;
}

static void
check_traversal_order(int value, void *udata)
{
    int *expected = (int *) udata;

    assert(value == *expected);
    (*expected)++;
}

struct gap_info {
    int omitted;
    int expected;
};

static void
check_traversal_order_with_gap(int value, void *udata)
{
    struct gap_info *info = (struct gap_info *) udata;

    if(info->expected == info->omitted) {
        info->expected++;
    }

    assert(value == info->expected);
    info->expected++;
}

static void
construct_and_check_tree(const int *perm, size_t perm_len, void *udata)
{
    struct tbt *tree = NULL;
    int expected;
    int i;

    tree = construct_tree_from_sequence(perm, perm_len);

    expected = 1;
    tbt_traverse(tree, check_traversal_order, &expected);

    tbt_destroy(tree);

    for(i = 1; i <= 7; i++) {
        struct gap_info gap_info = {
            .omitted  = i,
            .expected = 1
        };

        tree = construct_tree_from_sequence(perm, perm_len);
        tbt_delete(tree, i);
        tbt_traverse(tree, check_traversal_order_with_gap, &gap_info);
        tbt_destroy(tree);
    }
}

int
main(void)
{
    int sequence[7] = { 1, 2, 3, 4, 5, 6, 7 };

    permute(sequence, sizeof(sequence) / sizeof(int),
        construct_and_check_tree, NULL);

    return 0;
}
