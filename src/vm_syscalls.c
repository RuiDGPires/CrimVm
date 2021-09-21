#include "vm.h"

enum sc_code {SYS_FOPEN = 0, SYS_FCLOSE};
enum fopen_mode {F_READ = 1, F_WRITE};

static void vm_fopen(Vm vm){
	u32 address = vm_pop(vm);

	char fname[MAX_WORD_SIZE];
	u8 p = 0;
	// get file name
	do{
		fname[p++] = vm->mem[address++];	
	}while(fname - 1);

	char flag[1];
	switch(vm->mem[address++]){
		case F_READ:
			flag[0] = 'r';
			break;
		case F_WRITE:
			flag[0] = 'w';
			break;
	}
	u64 fp = (u64) fopen(fname, flag);

	vm_push(fp >> 32, vm);
	vm_push(fp & 0xFFFFFFFF, vm);
}

static void vm_fclose(Vm vm){
	u64 fp = vm_pop(vm);
	fp |= (u64) vm_pop(vm) << 32;

	fclose((FILE *) fp);
}

void vm_syscall(u32 id, Vm vm){
	switch (id){
		case SYS_FOPEN:
			vm_fopen(vm);
			break;
		case SYS_FCLOSE:
			vm_fclose(vm);
			break;
	}
}
