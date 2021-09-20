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
 
#include "linkedlist.h"

#define MAX_STRING_SIZE 32
struct ll_node{
	char name[MAX_STRING_SIZE];
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

void llAppend(ll_Link *head, char name[], u32 val){
  if (*head == NULL){
    *head = (ll_Link) malloc(sizeof(struct ll_node));
		ASSERT(head != NULL, "Error allocating memory");
		strcpy((*head)->name, name);
		(*head)->val = val;
    (*head)->next = NULL;
    (*head)->size = 1;
  }else{
    (*head)->size += 1;
    llAppend(&((*head)->next), name, val);
  }
}

void llPush(ll_Link *head, const char name[], u32 val){
  if (*head == NULL){
    *head = (ll_Link) malloc(sizeof(struct ll_node));
		ASSERT(head != NULL, "Error allocating memory");
		strcpy((*head)->name, name);
		(*head)->val = val;
    (*head)->next = NULL;
    (*head)->size = 1;
  }else{
		ll_Link new = (ll_Link) malloc(sizeof(struct ll_node));
		ASSERT(new != NULL, "Error allocating memory");
		strcpy(new->name, name);
		new->val = val;
    new->next = *head;
    new->size = (*head)->size + 1;
		(*head) = new;
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
