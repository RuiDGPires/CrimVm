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
 
#ifndef __VM_H__
#define __VM_H__

#include <stdio.h>
#include "defs.h" 

#define VM_PROG_MEM_SIZE 16384 
#define VM_MEM_SIZE 1024

#define OP_MAX_ARGS 2 

enum op_code {OP_MVI = 0, OP_MOV, OP_ADD,  OP_STORE, OP_LOAD, OP_PUSH, OP_POP, OP_SUB, OP_CMP, OP_BR, OP_JMP, OP_RET, OP_AND, OP_OR, OP_XOR, OP_NOT, OP_SHR, OP_SHL,  OP_END, OP_INC, OP_DEC, OP_MUL, OP_DIV, OP_STR, LABEL};
enum trap_code {TRP_GETC = 0x30, TRP_OUT, TRP_PRNT, TRP_DUMP};

enum flags {FLG_ZERO = 1	<< 0,
						FLG_POS = 1		<< 1,
						FLG_NEG = 1		<< 2,
					 	FLG_OV = 1		<< 3,};

typedef struct vm_s *Vm;

typedef void (*f_drv_run)(Vm); 

typedef struct driver_s {
	f_drv_run run;
	Vm vm;
} Driver;

typedef void (*f_ldr_load)(Vm, FILE *);

typedef struct loader_s {
	f_ldr_load load;
	FILE *file;
	Vm vm;
	bool is_at_eof;	
} Loader;

typedef struct {
	u8 code;
	u32 args[OP_MAX_ARGS];	
	u32 u32_aux;
	u8 *u8p_aux;
} Operation;

// R0 is ALWAYS equal to 0. Other registers include 3 general purpose registers, a stack pointer and a return register.
enum reg_type{R0 = 0, R1, R2, R3, R4, SP, RE, R_COUNT};
typedef struct vm_s {
	Loader loader;
	// MemManager memmanager;
	// Scheduler scheduler;

	Operation program[VM_PROG_MEM_SIZE];
	u32 prog_length;
	u32 pc;

	u32 mem[VM_MEM_SIZE];

	u32 regs[R_COUNT];
	u8 flags;
	Driver driver;

	Vm parent;
} Vm_;

// Returns error code
void NNULL vm_init(Vm);
void NNULL vm_destroy(Vm);
void NNULL vm_load(Vm, char *);
void NNULL vm_push(u32, Vm);
u32 NNULL vm_pop(Vm);

void NNULL loader_init(Loader *, Vm);
void assemble(char *, char *);

#endif
