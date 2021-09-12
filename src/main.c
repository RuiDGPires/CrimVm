#include "defs.h"
#include "vm.h"

#include <stdio.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define ABORT_ON_ERROR

void errCheck(int e){
	if (e){
		fflush(stdout);
		fprintf(stderr, "An error occured: %s\n", strerror(e));
#ifdef ABORT_ON_ERROR
	exit(e);
#else
	return;
#endif		
	}
	else return;
}

void run_binary(char file_path[]){
	Vm_ vm;
	errCheck(vm_init(&vm));
	errCheck(vm_load(&vm, file_path));

	// Pass the control to the VM
	vm.driver.run(&vm);

	errCheck(vm_destroy(&vm));
}

void assemble(char fin[], char fout[]){
	link(fin, fout);
}

int main(int argc, char *argv[]){
	switch (argc){
		case 2:
		 run_binary(argv[1]);
		 break;
		case 3:
		 assemble(argv[1], argv[2]);
	}
}
