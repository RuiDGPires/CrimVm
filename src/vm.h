#ifndef __VM_H__
#define __VM_H__

#include <stdio.h>
#include <stdint.h>

#include "defs.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef uint8_t u8;

// Smallest size possible for bool
typedef uint8_t bool;

#define TRUE 1
#define FALSE 0


#define VM_PROG_MEM_SIZE 1024
#define VM_MEM_SIZE 1024

#define OP_MAX_ARGS 2

enum op_code {OP_MVI = 0, OP_MOV, OP_ADD, OP_DUMP, OP_STORE, OP_LOAD, OP_PUSH, OP_POP, OP_SUB, OP_CMP};

enum flags {FLG_ZERO = 1	<< 0,
						FLG_NEG = 1		<< 1,
					 	FLG_OV = 1		<< 2,
						};

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

// R0 is ALWAYS equal to 0. Other registers include 2 general purpose registers, a stack pointer and a return register.
enum reg_type{R0 = 0, R1, R2, SP, RE, R_COUNT};
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
int NNULL vm_init(Vm);
int NNULL vm_destroy(Vm);
int NNULL vm_load(Vm, char *);
static void NNULL vm_push(u32, Vm);
static NNULL u32 vm_pop(Vm);

void NNULL loader_init(Loader *, Vm);
int link(char *, char *);
int link_F(char *, FILE *);

#endif
