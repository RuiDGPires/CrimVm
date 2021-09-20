#ifndef __VM_H__
#define __VM_H__

#include <stdio.h>
#include <stdint.h>
#include "defs.h" 

#define VM_PROG_MEM_SIZE 16384 
#define VM_MEM_SIZE 1024

#define OP_MAX_ARGS 3

enum op_code {OP_MVI = 0, OP_MOV, OP_ADD,  OP_STORE, OP_LOAD, OP_PUSH, OP_POP, OP_SUB, OP_CMP, OP_BR, OP_JMP, OP_RET, OP_AND, OP_OR, OP_XOR, OP_NOT, OP_SHR, OP_SHL,  OP_END, OP_INC, OP_DEC, LABEL};
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
} Operation;

// R0 is ALWAYS equal to 0. Other registers include 3 general purpose registers, a stack pointer and a return register.
enum reg_type{R0 = 0, R1, R2, R3, SP, RE, R_COUNT};
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
