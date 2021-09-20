#include "vm.h"
#include "util.h"

u8 check_msbit(u32 val){
	return (val & 0x80000000) != 0;
}

bool check_flags(u8 vm_flags, u8 cond){
	return (vm_flags & cond) || cond == 0;
}

u32 get_offset(u32 arg, Vm vm){
	if (arg == 0)
		return 0;

	if (arg & 0x80000000){
		return vm->regs[arg & 0xFF];
	}else{
		return arg;
	}
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
			case OP_STORE:
				vm->mem[vm->regs[op.args[0]] + get_offset(op.args[2], vm)] = vm->regs[op.args[1]];
				vm->pc++;
				break;
			case OP_LOAD:
				res = vm->mem[vm->regs[op.args[1]] + get_offset(op.args[2], vm)];
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
				else if (res == 0)
					vm->flags |= FLG_ZERO;
				else
					vm->flags |= FLG_POS;
				
				vm->pc++;
				goto store_res;
			case OP_CMP:
				res = vm->regs[op.args[0]] - vm->regs[op.args[1]];
				vm->flags = 0;

				if (check_msbit(res))
					vm->flags |= FLG_NEG;
				else if (res == 0)
					vm->flags |= FLG_ZERO;
				else
					vm->flags |= FLG_POS;

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
				if (check_flags(vm->flags, op.args[0]))
					vm->pc += op.args[1];
				else
					vm->pc++;
				break;
			case OP_JMP:
				if (check_flags(vm->flags, op.args[0])){
					vm->regs[RE] = vm->pc + 1;
					vm->pc = op.args[1];
				}
				else
					vm->pc++;

				break;
			case OP_RET:
				vm->pc = vm->regs[RE];
				break;
			case OP_SHL:
				res = vm->regs[op.args[0]] << 1;
				vm->pc++;
				goto store_res;
			case OP_SHR:
				res = vm->regs[op.args[0]] >> 1;
				vm->pc++;
				goto store_res;
			case OP_INC:
				res = vm->regs[op.args[0]] + 1;
				vm->pc++;
				goto store_res;
			case OP_DEC:
				res = vm->regs[op.args[0]] - 1;
				vm->pc++;
				goto store_res;
			case OP_END:
				return;


			// Trap routines
			case TRP_GETC:
				vm_push((u32) getchar(), vm);	
				break;	
			// Pops a pointer from the stack and prints until it reaches a \0
			case TRP_PRNT:
				for(u32 p = vm_pop(vm); vm->mem[p]; p++) {
					res = vm->mem[p]; 
					fputc((char) res, stdout);
				}
				fflush(stdout);	
				vm->pc++;
				break;
			case TRP_OUT:
				res = vm_pop(vm);
				fputc((char) res, stdout);
				fflush(stdout);
				vm->pc++;
				break;
			case TRP_DUMP:
				printf("%d\n", vm_pop(vm));
				fflush(stdout);
				vm->pc++;
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

