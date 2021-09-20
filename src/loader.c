// Implementation of loader and linker

#include "vm.h"
#include "util.h"

#define LOAD_BUFFER_SIZE VM_PROG_MEM_SIZE

u32 u32_from_buffer(u32 *p, u8 buffer[]){
	u32 val = 0;
	for (int i = 0; i < 4; i++)
		val = (val << 8) | (buffer[(*p)++] & 0xFF); 	
	return val;
}

void load(Vm vm, FILE *file){
	u8 buffer[LOAD_BUFFER_SIZE];
	
	u32 c = (u32) fread(buffer, sizeof(u8), LOAD_BUFFER_SIZE, file);	
	ASSERT(c < LOAD_BUFFER_SIZE, "FILE IS TOO BIG TO HOLD IN MEMORY");

	vm->prog_length = 0;

	ASSERT(c > 4, "Invalid file");
	ASSERT((buffer[0] == (u8) ((u32) CRIMSEMBLY_TAG >> 3*8)) & 0xFF, "Invalid file");
	ASSERT((buffer[1] == (u8) ((u32) CRIMSEMBLY_TAG >> 2*8)) & 0xFF, "Invalid file");
	ASSERT((buffer[2] == (u8) ((u32) CRIMSEMBLY_TAG >> 1*8)) & 0xFF, "Invalid file");
	ASSERT((buffer[3] == (u8) ((u32) CRIMSEMBLY_TAG)) & 0xFF, "Invalid file");

	vm->pc = 0;

	for (int i = 0; i < 4; i++)
		vm->pc = (vm->pc << 8) | buffer[4+i];
	
	for (u32 p = 8; p < c;){
		// Get the location to store the new operation and increment program count immediately
		Operation *OP = &(vm->program[(vm->prog_length)++]);
		OP->code = buffer[p++];
		switch (OP->code){
			// RECEIVES (u8, u8)
			case OP_MOV:
			case OP_ADD:
			case OP_STORE:
			case OP_LOAD:
			case OP_SUB:
			case OP_CMP:
			case OP_AND:
			case OP_OR:
			case OP_XOR:
				OP->args[0] = buffer[p++];
				OP->args[1] = buffer[p++];
				break;

			// RECEIVES (u8, u32)
			case OP_MVI:
			case OP_BR:
			case OP_JMP:
				OP->args[0] = buffer[p++];	
				OP->args[1] = u32_from_buffer(&p, buffer); 
				break;

			// RECEIVES (u8)
			case OP_PUSH:
			case OP_POP:
			case OP_NOT:
			case OP_SHR:
			case OP_SHL:
			case OP_INC:
			case OP_DEC:
				OP->args[0] = buffer[p++];
				break;

			// REVEIVES ()
			case OP_DUMP:
			case OP_RET:
			case OP_END:
			case TRP_GETC:
			case TRP_OUT:
			case TRP_PRNT:
			break;

			// OTHERS
			case LABEL:
				break;
			default:
				THROW_ERROR("Unkown OPCode");
		}
	}	
}

void loader_init(Loader *loader, Vm vm){
	*loader = (Loader){.load = load, .file = NULL, .vm = vm, .is_at_eof = FALSE};
}
