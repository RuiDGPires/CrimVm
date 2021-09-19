#include "vm.h"
#include "util.h"

u8 check_msbit(u32 val){
	return (val & 0x80000000) != 0;
}

void run(Vm vm){
	while(vm->pc < vm->prog_length){
		Operation op = vm->program[vm->pc];
		u32 res;
		switch(op.code){
			case OP_MVI:
				res = op.args[1];
				vm->pc++;
				goto store_res;
			case OP_MOV:
				res =  vm->regs[op.args[1]];
				vm->pc++;
				goto store_res;
			case OP_ADD:
				res = vm->regs[op.args[0]] + vm->regs[op.args[1]];
				vm->flags = 0;

				// CHECK OVERFLOWS
				if ((check_msbit(vm->regs[op.args[0]]) || check_msbit(vm->regs[op.args[1]])) && !check_msbit(res))
					vm->flags |= FLG_OV;

				vm->pc++;
				goto store_res;
			case OP_DUMP:
				printf("%d\n", vm_pop(vm));
				vm->pc++;
				break;
			case OP_STORE:
				vm->mem[vm->regs[op.args[0]]] = vm->regs[op.args[1]];
				vm->pc++;
				break;
			case OP_LOAD:
				res = vm->mem[vm->regs[op.args[1]]];
				vm->pc++;
				goto store_res;
			case OP_PUSH:
				vm_push(vm->regs[op.args[0]], vm);
				vm->pc++;
				break;
			case OP_POP:
				res = vm_pop(vm);
				vm->pc++;
				goto store_res;
			case OP_SUB:
				res = vm->regs[op.args[0]] - vm->regs[op.args[1]];
				vm->flags = 0;
				if (check_msbit(res))
					vm->flags |= FLG_NEG;

				if (res == 0)
					vm->flags |= FLG_ZERO;
				
				vm->pc++;
				goto store_res;
			case OP_CMP:
				res = vm->regs[op.args[0]] - vm->regs[op.args[1]];
				vm->flags = 0;
				if (check_msbit(res))
					vm->flags |= FLG_NEG;

				if (res == 0)
					vm->flags |= FLG_ZERO;

				vm->pc++;
				break;
			case OP_AND:
				res = vm->regs[op.args[0]] & vm->regs[op.args[1]];
				vm->pc++;
				goto store_res;
			case OP_OR:
				res = vm->regs[op.args[0]] | vm->regs[op.args[1]];
				vm->pc++;
				goto store_res;
			case OP_XOR:
				res = vm->regs[op.args[0]] ^ vm->regs[op.args[1]];
				vm->pc++;
				goto store_res;
			case OP_NOT:
				res = ~(vm->regs[op.args[0]]);
				vm->pc++;
				goto store_res;
			case OP_BR:
				if (op.args[0] == 0)
					res = 1;
				else if (op.args[0] & 0x80){ // IF NEGATION
					op.args[0] = op.args[0] & 0x7F; // CLEAR MOST SIGNIFICANT BIT
					res = !(vm->flags & op.args[0]);
				}else
					res = vm->flags & op.args[0];
			
				if (res)	
					vm->pc += op.args[1];
				else
					vm->pc++;
				break;
			case OP_JMP:
				if (op.args[0] == 0)
					res = 1;
				else if (op.args[0] & 0x80){ // IF NEGATION
					op.args[0] = op.args[0] & 0x7F; // CLEAR MOST SIGNIFICANT BIT
					res = !(vm->flags & op.args[0]);
				}else
					res = vm->flags & op.args[0];
			
				if (res){
					vm->regs[RE] = vm->pc + 1;
					vm->pc = op.args[1];
				}
				else
					vm->pc++;

				break;
			case OP_RET:
				vm->pc = vm->regs[RE];
				break;
			default:
				THROW_ERROR("Unkown Operation \'%d\'", op.code);

			store_res:
				if (op.args[0] != R0)
					vm->regs[op.args[0]] = res;	
				break;
		}	
	} 
}

void vm_push(u32 val, Vm vm){
	vm->mem[--vm->regs[SP]] = val;
}

u32 vm_pop(Vm vm){
	return vm->mem[vm->regs[SP]++]; 
}

void vm_init(Vm vm){
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
}

void vm_destroy(Vm vm){
	if (vm->loader.file != NULL)
		fclose(vm->loader.file);
}

void vm_load(Vm vm, char filename[]){
	FILE *file = fopen(filename, "r");

	if (file == NULL) THROW_ERROR("An error occured while trying to open the file \'%s\'", filename);

	vm->loader.file = file;
	vm->loader.load(vm, file);
}

