// Implementation of loader and linker

#include "vm.h"
#include "util.h"

#define LOAD_BUFFER_SIZE VM_PROG_MEM_SIZE

// NEEDS TO BE CHANGED!!!!!!!!!!11
void load(Vm vm, FILE *file){
	u8 buffer[LOAD_BUFFER_SIZE];

	bool is_first_time = TRUE;
	
	u32 c = 0;
	do{
		c = (u32) fread(buffer, sizeof(u8), LOAD_BUFFER_SIZE, file);	
		vm->loader.is_at_eof = c < LOAD_BUFFER_SIZE;

		// IF ITS TIME FIRST READ TO THE FILE, CHECK FOR THE CRIM FILE TAG
		if (is_first_time){
			vm->prog_length = 0;

			ASSERT(c > 4, "Invalid file");
			ASSERT((buffer[0] == (u8) ((u32) CRIMSEMBLY_TAG >> 3*8)) & 0xFF, "Invalid file");
			ASSERT((buffer[1] == (u8) ((u32) CRIMSEMBLY_TAG >> 2*8)) & 0xFF, "Invalid file");
			ASSERT((buffer[2] == (u8) ((u32) CRIMSEMBLY_TAG >> 1*8)) & 0xFF, "Invalid file");
			ASSERT((buffer[3] == (u8) ((u32) CRIMSEMBLY_TAG)) & 0xFF, "Invalid file");
		}

		for (u32 p = is_first_time? 4: 0; p < c;){
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
				case OP_SUB:
					OP->args[0] = buffer[p++];
					OP->args[1] = buffer[p++];
					break;
				case OP_CMP:
					OP->args[0] = buffer[p++];
					OP->args[1] = buffer[p++];
					break;
			}
		}	

		is_first_time = FALSE;
	}while(c != 0);
}

void loader_init(Loader *loader, Vm vm){
	*loader = (Loader){.load = load, .file = NULL, .vm = vm, .is_at_eof = FALSE};
}
