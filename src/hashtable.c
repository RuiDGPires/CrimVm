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

