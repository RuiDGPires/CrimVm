// MIT License

// Copyright (c) 2021 Rui Pires

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
 
//--------------------------------------------------------
// LINKED LIST

#include "util.h"

typedef struct ll_node *ll_Link;

struct ll_node{
	char name[MAX_WORD_SIZE];
	u32 val;
  u32 size;
  ll_Link next;
};

ll_Link llCreate(){
  return NULL;
}

void llDestroy(ll_Link head){
  if (head == NULL) return;
  llDestroy(head->next);
  free(head);
}

bool llPut(ll_Link *head, char name[], u32 val){
  if (*head == NULL){
    *head = (ll_Link) malloc(sizeof(struct ll_node));
		ASSERT(head != NULL, "Error allocating memory");
		strcpy((*head)->name, name);
		(*head)->val = val;
    (*head)->next = NULL;
    (*head)->size = 1;
		return 1;
  }else{
		int res = strcmp(name, (*head)->name); 
		if (res == 0)
			return 0;
		else if (res > 0){
			(*head)->size += 1;
			return llPut(&((*head)->next), name, val);		
		}else{
			ll_Link new_ = (ll_Link) malloc(sizeof(struct ll_node));
			ASSERT(new_ != NULL, "Error allocating memory");
			strcpy(new_->name, name);
			new_->val = val;
			new_->next = *head;
			new_->size = (*head)->size + 1;
			(*head) = new_;
			return 1;
		}
	}
}

u32 llGet(ll_Link head, const char name[]){
	if (head == NULL)
		return INVALID_ITEM; 

	if (strcmp(head->name, name) == 0)
		return head->val;
	else
		return llGet(head->next, name);
}

unsigned int llGetSize(ll_Link head){
  return head == NULL? 0: (unsigned int) head->size;
}

short int llIsEmpty(ll_Link head){
  return head == NULL; 
}

void llTraversePRE(ll_Link head, void func(const char *, u32)){
  if (head == NULL) return;
  func(head->name, head->val);
  llTraversePRE(head->next, func);
}

static __attribute__((unused)) void printStuff(const char *name, u32 val){
	printf("[%s: %d]", name, val);
}

//---------------------------------------------------------
//---------------------------------------------------------
// HASHTABLE
#include "hashtable.h"


u32 hash(const char *str){
    u32 hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % HASHTABLE_SIZE;
}

Hashtable ht_create(){
	Hashtable table;
	for (u32 i = 0; i < HASHTABLE_SIZE; i++)
		table.table[i] = llCreate();
	return table;
}

void ht_destroy(Hashtable table){
	for (u32 i = 0; i < HASHTABLE_SIZE; i++)
		llDestroy(table.table[i]);
}

bool ht_add(Hashtable *table, char name[], u32 val){
	return llPut(&table->table[hash(name)], name, val);
}

u32 ht_get(Hashtable table, char name[]){
	return llGet(table.table[hash(name)], name);
}

