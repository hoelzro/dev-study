#ifndef TBT_H
#define TBT_H

struct tbt;

struct tbt *tbt_new(void);

int tbt_add(struct tbt *tree, int value);

void tbt_traverse(struct tbt *tree, void (*callback)(int, void *), void *udata);

void tbt_destroy(struct tbt *tree);

void tbt_delete(struct tbt *tree, int value);

void tbt_dump(struct tbt *tree);

int tbt_contains(struct tbt *tree, int value);

int tbt_height(struct tbt *tree);

#endif
