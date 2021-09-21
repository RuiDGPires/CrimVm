#include "vm.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>

enum sc_code {SYS_OPEN = 0, SYS_CLOSE, SYS_WRITE, SYS_READ};
enum fopen_mode {F_READ = 1, F_WRITE};

static void vm_open(Vm vm){
	u32 address = vm_pop(vm);

	char fname[MAX_WORD_SIZE];
	u8 p = 0;
	// get file name
	do{
		fname[p++] = vm->mem[address++];	
	}while(fname - 1);

	u32 fd = open(fname, 0);

	vm_push(fd, vm);
}

static void vm_close(Vm vm){
	close(vm_pop(vm));
}

static void vm_write(Vm vm){
	u32 size = vm_pop(vm);
	u32 address = vm_pop(vm);
	u32 fd = vm_pop(vm);

	char tmp[size];

	for (u32 i = 0; i < size; i++)
		tmp[i] = (char) vm->mem[address++];

	vm_push((u32) write(fd, tmp, size), vm);
}

static void vm_read(Vm vm){
	u32 size = vm_pop(vm);
	u32 address = vm_pop(vm);
	u32 fd = vm_pop(vm);

	char tmp[size];

	vm_push((u32) read(fd, tmp, size), vm);

	for (u32 i = 0; i < size; i++)
		vm->mem[address++] = (u32) tmp[i];
}

void vm_syscall(u32 id, Vm vm){
	switch (id){
		case SYS_OPEN:
			vm_open(vm);
			break;
		case SYS_CLOSE:
			vm_close(vm);
			break;
		case SYS_WRITE:
			vm_write(vm);
			break;
		case SYS_READ:
			vm_read(vm);
			break;
		default:
			THROW_ERROR("Unkown system call");
	}
}
