#include "vm.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>

enum sc_code {SYS_OPEN = 0, SYS_CLOSE, SYS_WRITE, SYS_READ, SYS_REMOVE};
enum fopen_mode {F_READ = 0, F_WRITE};

static void get_array(char *buff, u32 address, u32 size, Vm vm){
	for (u32 i = 0; i < size; i++)
		buff[i] = (char) vm->mem[address++];
}

static void put_array(char *buff, u32 address, u32 size, Vm vm){
	for (u32 i = 0; i < size; i++)
		vm->mem[address++] = (u32) buff[i];
}

static void get_string(char *buff, u32 address, Vm vm){
	u8 p = 0;
	// get file name
	do{
		buff[p++] = vm->mem[address++];	
	}while(buff[p - 1]);
}

//--------------------------------------

static void vm_open(Vm vm){
	u32 mode_ = vm_pop(vm);
	u32 address = vm_pop(vm);

	char fname[MAX_WORD_SIZE];

	get_string(fname, address, vm);
	
	u32 flags = 0;
	
	mode_t mode;
	switch(mode_){
		case F_READ:
			mode = O_RDONLY;
			break;
		case F_WRITE:
			flags |= O_CREAT | O_WRONLY;
			mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
			break;
	}

	u32 fd = open(fname, flags, mode);

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

	get_array(tmp, address, size, vm);

	vm_push((u32) write(fd, tmp, size), vm);
}

static void vm_read(Vm vm){
	u32 size = vm_pop(vm);
	u32 address = vm_pop(vm);
	u32 fd = vm_pop(vm);

	char tmp[size];

	vm_push((u32) read(fd, tmp, size), vm);

	put_array(tmp, address, size, vm);
}

static void vm_remove(Vm vm){
	u32 address = vm_pop(vm);
	char fname[MAX_WORD_SIZE];

	get_string(fname, address, vm);

	vm_push((u32) remove(fname), vm);
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
		case SYS_REMOVE:
			vm_remove(vm);
			break;
		default:
			THROW_ERROR("Unkown system call");
	}
}
