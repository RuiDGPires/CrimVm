#include "vm.h"
#include <errno.h>


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

int vm_init(Vm vm){
	loader_init(&(vm->loader), vm);
	vm->pc = 0;
	vm->prog_length = 0;
	vm->parent = NULL;
	for (int i = 0; i < R_COUNT; i++)
		vm->regs[i] = 0;

	vm->regs[SP] = VM_MEM_SIZE;
	vm->flags = 0;
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

