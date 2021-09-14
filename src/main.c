#include "vm.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void run_binary(char file_path[]){
	Vm_ vm;
	vm_init(&vm);
	vm_load(&vm, file_path);

	// Pass the control to the VM
	vm.driver.run(&vm);

	vm_destroy(&vm);
}


// Check if file extension is .cas
int check_file_extension(char filename[], char extension[]){
	int length = strlen(filename);
	int ext_len = strlen(extension);
	
	char *last_chars = &(filename[length-ext_len]);
	return strcmp(last_chars, extension) == 0;
}

int main(int argc, char *argv[]){
	bool flag_outfile = FALSE, flag_assemble = FALSE, flag_run = FALSE, flag_assemble_and_run = FALSE;
	char *out_name = NULL;
	char *filename = NULL;

	if (argc == 1)
		THROW_ERROR("Invalid usage, please use -h to see supported arguments");

	if (argc == 2){
			run_binary(argv[1]);
			return 0;
	}

	bool request_cas_file = FALSE, request_run_file = FALSE, request_out_file = FALSE;
	
	// CANT HAVE -o and -r at the same time
	for (int i = 1; i < argc; i++){
			if (request_cas_file) {
				ASSERT(check_file_extension(argv[i], ".cas"), "File must have .cas extension to be assembled");
				filename = argv[i];
				request_cas_file = FALSE;
				continue;
			}
		 	if (request_run_file) {
				filename = argv[i];
				request_run_file = FALSE;
				continue;
			}
		 	if (request_out_file) {
				out_name = argv[i];
				request_out_file = FALSE;
				continue;
			}

			if (strcmp(argv[i], "-ar") == 0){
				request_cas_file = TRUE;
				flag_assemble_and_run = TRUE;
				continue;
			} else if (strcmp(argv[i], "-a") == 0){
				flag_assemble = TRUE;
				request_cas_file = TRUE;			
				continue;
			} else if (strcmp(argv[i], "-r") == 0){
				flag_run = TRUE;
				request_run_file = TRUE;			
				continue;
			} else if (strcmp(argv[i], "-o") == 0){
				flag_outfile = TRUE;
				request_out_file = TRUE;
				continue;
			}

			THROW_ERROR("Invalid usage, please use -h to see supported arguments");
	}
	ASSERT(!request_out_file && !request_cas_file && !request_run_file, "Please provide a file name");		

	if (flag_outfile && (flag_run || flag_assemble_and_run))
		THROW_ERROR("Can't have -o and -r at the same time");
	if (flag_assemble && flag_run)
		THROW_ERROR("Can't have -a and -r at the same time, use -ar instead");

	int name_length = strlen(filename);
	char new_name[name_length -1];
	if ((flag_assemble || flag_assemble_and_run) && !flag_outfile){
		strcpy(new_name, filename);
		new_name[name_length-3] = 'o';
		new_name[name_length-2] = '\0';
		out_name = new_name;
	}
	
	if (flag_assemble)
		assemble(filename, out_name);
	else if (flag_run)
		run_binary(filename);
	else if (flag_assemble_and_run){
		char tmpfile[] = "/tmp/casXXXXXX";
		if (mkstemp(tmpfile) < 0) THROW_ERROR("Error creating temporary file");
		assemble(filename, tmpfile);
		run_binary(tmpfile);
		remove(tmpfile);
	}else
		THROW_ERROR("Invalid usage, please use -h to see supported arguments");
}
