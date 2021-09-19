#ifndef __HASTABLE_H__
#define __HASTABLE_H__

#define HASHTABLE_SIZE 113

#include "linkedlist.h"

typedef struct {
	ll_Link table[HASHTABLE_SIZE];	
} Hashtable;

Hashtable ht_create();
void ht_destroy(Hashtable);
void ht_add(Hashtable *, char *, u32);
u32 ht_get(Hashtable, char *);

#endif
