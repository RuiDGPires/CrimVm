#ifndef __VM_H__
#define __VM_H__

#include <stdio.h>
#include <stdint.h>

#include "defs.h"

typedef int32_t i32;
typedef uint32_t u32;

// Smallest size possible for bool
typedef uint8_t bool;

#define TRUE 1
#define FALSE 0


#define VM_PROG_MEM 1024
#define VM_MEM 1024

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

typedef struct vm_s {
	Loader loader;
	// MemManager memmanager;
	// Scheduler scheduler;

	u32 prog_mem[VM_PROG_MEM];
	u32 prog_length;
	u32 pc;

	u32 vm_mem[VM_MEM];

	u32 regs[4];
	Driver driver;

	Vm parent;
} Vm_;

// Returns error code
int NNULL vm_init(Vm);
int NNULL vm_destroy(Vm);
int NNULL vm_load(Vm, char *);

#endif
