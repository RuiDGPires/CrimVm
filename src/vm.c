#include "vm.h"
#include <errno.h>

enum op_code {};

void run(Vm vm){
	while(vm->pc < vm->prog_length){

	} 
}

void load(Vm vm, FILE *file){
	u32 c = (u32) fread(&(vm->prog_mem[vm->prog_length]), sizeof(u32), VM_PROG_MEM, file);	
	vm->loader.is_at_eof = c < VM_PROG_MEM - vm->prog_length;
	
	vm->prog_length += c;
}

int vm_init(Vm vm){
	vm->loader = (Loader){.load = load, .file = NULL, .vm = vm, .is_at_eof = FALSE};
	vm->pc = 0;
	vm->prog_length = 0;
	vm->parent = NULL;
	vm->driver = (Driver){.run = run, .vm = vm};
}

int vm_destroy(Vm vm){
	if (vm->loader.file != NULL)
		fclose(vm->loader.file);
}

int vm_load(Vm vm, char filename[]){
	FILE *file = fopen(filename, "r");

	if (file == NULL) return errno; 

	vm->loader.file = file;
	vm->loader.load(vm, file);
	return 0;
}
