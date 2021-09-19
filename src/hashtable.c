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

void ht_add(Hashtable *table, char name[], u32 val){
	llPush(&table->table[hash(name)], name, val);
}

u32 ht_get(Hashtable table, char name[]){
	return llGet(table.table[hash(name)], name);
}

