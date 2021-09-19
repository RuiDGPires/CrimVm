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
