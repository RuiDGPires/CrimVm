#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#define INVALID_ITEM 0xFFFFFFFF

#include "util.h"

typedef struct ll_node *ll_Link;

ll_Link llCreate();
void llDestroy(ll_Link head);
void llAppend(ll_Link *head, char *, u32);
void llPush(ll_Link *, const char *, u32);
u32 llGet(ll_Link, const char *);
u32 llGetSize(ll_Link head);
void llTraversePRE(ll_Link, void(const char *, u32));
short int llIsEmpty(ll_Link head);
#endif
