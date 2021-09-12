#include "vm.h"
#include <errno.h>

u8 check_msbit(u32 val){
	return (val & 0x80000000) != 0;
}

void run(Vm vm){
	while(vm->pc < vm->prog_length){
		Operation op = vm->program[vm->pc];
		u32 res;
		switch(op.code){
			case OP_MVI:
				if (op.args[0] != R0)
					vm->regs[op.args[0]] = op.args[1];
				vm->pc++;
				break;
			case OP_MOV:
				if (op.args[0] != R0)
					vm->regs[op.args[0]] = vm->regs[op.args[1]];
				vm->pc++;
				break;
			case OP_ADD:
				res = vm->regs[op.args[0]] + vm->regs[op.args[1]];
				vm->flags = 0;

				// CHECK OVERFLOWS
				if ((check_msbit(vm->regs[op.args[0]]) || check_msbit(vm->regs[op.args[1]])) && !check_msbit(res))
					vm->flags |= FLG_OV;

				if (op.args[0] != R0)
					vm->regs[op.args[0]] = res;
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
				if (op.args[0] != R0)
					vm->regs[op.args[0]] = vm->mem[vm->regs[op.args[1]]];
				vm->pc++;
				break;
			case OP_PUSH:
				vm_push(vm->regs[op.args[0]], vm);
				vm->pc++;
				break;
			case OP_POP:
				if (op.args[0] != R0)
					vm->regs[op.args[0]] = vm_pop(vm);
				vm->pc++;
				break;
			case OP_SUB:
				res = vm->regs[op.args[0]] - vm->regs[op.args[1]];
				vm->flags = 0;
				if (check_msbit(res))
					vm->flags |= FLG_NEG;

				if (res == 0)
					vm->flags |= FLG_ZERO;
				
				if (op.args[0] != R0)
					vm->regs[op.args[0]] = res;
				vm->pc++;
				break;
			case OP_CMP:
				res = vm->regs[op.args[0]] - vm->regs[op.args[1]];
				vm->flags = 0;
				if (check_msbit(res))
					vm->flags |= FLG_NEG;

				if (res == 0)
					vm->flags |= FLG_ZERO;

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
	vm->regs[R0] = 0;
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

