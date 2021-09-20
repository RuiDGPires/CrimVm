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
