#include "vm.h"
#include <errno.h>

enum op_code {OP_MVI = 0, OP_MOV, OP_ADD, OP_DUMP, OP_STORE, OP_LOAD, OP_PUSH, OP_POP};

void run(Vm vm){
	while(vm->pc < vm->prog_length){
		Operation op = vm->program[vm->pc];
		switch(op.code){
			case OP_MVI:
				vm->regs[op.args[0]] = op.args[1];
				vm->pc++;
				break;
			case OP_MOV:
				vm->regs[op.args[0]] = vm->regs[op.args[1]];
				vm->pc++;
				break;
			case OP_ADD:
				vm_push(vm->regs[op.args[0]] + vm->regs[op.args[1]], vm);
				vm->pc++;
				break;
			case OP_DUMP:
				printf("%d\n", vm_pop(vm));
				vm->pc++;
				break;
			case OP_STORE:
				vm->mem[vm->regs[op.args[0]]] = vm->regs[op.args[1]];
				vm->pc++;
				break;
			case OP_LOAD:
				vm->regs[op.args[0]] = vm->mem[vm->regs[op.args[1]]];
				vm->pc++;
				break;
			case OP_PUSH:
				vm_push(vm->regs[op.args[0]], vm);
				vm->pc++;
				break;
			case OP_POP:
				vm->regs[op.args[0]] = vm_pop(vm);
				vm->pc++;
				break;
			default:
				printf("Unkown Operation: %d\n", op.code);
				return;
		}	
	} 
}

void vm_push(u32 val, Vm vm){
	vm->mem[--vm->regs[SP]] = val;
}

u32 vm_pop(Vm vm){
	return vm->mem[vm->regs[SP]++]; 
}

#define LOAD_BUFFER_SIZE VM_PROG_MEM_SIZE

void load(Vm vm, FILE *file){
	u8 buffer[LOAD_BUFFER_SIZE];

	u32 c = (u32) fread(buffer, sizeof(u8), LOAD_BUFFER_SIZE, file);	
	vm->loader.is_at_eof = c < LOAD_BUFFER_SIZE;

	vm->prog_length = 0;

	for (u32 p = 0; p < c;){
		// Get the location to store the new operation and increment program count immediately
		Operation *OP = &(vm->program[(vm->prog_length)++]);
		OP->code = buffer[p++];
		switch (OP->code){
			case OP_MVI:
				OP->args[0] = buffer[p++];	
				OP->args[1] = 0; 
				for (int i = 0; i < 4; i++)
					OP->args[1] = (OP->args[1] << i * 8) | buffer[p++];	
				break;
			case OP_MOV:
				OP->args[0] = buffer[p++];
				OP->args[1] = buffer[p++];
				break;
			case OP_ADD:
				OP->args[0] = buffer[p++];
				OP->args[1] = buffer[p++];
				break;
			case OP_DUMP:
				break;
			case OP_STORE:
				OP->args[0] = buffer[p++];
				OP->args[1] = buffer[p++];
				break;
			case OP_LOAD:
				OP->args[0] = buffer[p++];
				OP->args[1] = buffer[p++];
				break;
			case OP_PUSH:
				OP->args[0] = buffer[p++];
				break;
			case OP_POP:
				OP->args[0] = buffer[p++];
				break;
		}
	}	
}

int vm_init(Vm vm){
	vm->loader = (Loader){.load = load, .file = NULL, .vm = vm, .is_at_eof = FALSE};
	vm->pc = 0;
	vm->prog_length = 0;
	vm->parent = NULL;
	for (int i = 0; i < R_COUNT; i++)
		vm->regs[i] = 0;

	vm->regs[SP] = VM_MEM_SIZE;
	vm->driver = (Driver){.run = run, .vm = vm};
	return 0;
}

int vm_destroy(Vm vm){
	if (vm->loader.file != NULL)
		fclose(vm->loader.file);
	return 0;
}

int vm_load(Vm vm, char filename[]){
	FILE *file = fopen(filename, "r");

	if (file == NULL) return errno; 

	vm->loader.file = file;
	vm->loader.load(vm, file);
	return 0;
}
