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

int main(int argc, char *argv[]){
	if (argc != 2) return 1;

	Vm_ vm;
	errCheck(vm_init(&vm));
	errCheck(vm_load(&vm, argv[1]));

	// Pass the control to the VM
	vm.driver.run(&vm);

	errCheck(vm_destroy(&vm));

	return 0;
}
