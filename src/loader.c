// Implementation of loader and linker

#include "vm.h"
#include <assert.h>
#define CRIMSEMBLY_TAG 0x4352494d // CRIM
#define LOAD_BUFFER_SIZE VM_PROG_MEM_SIZE

// NEEDS TO BE CHANGED!!!!!!!!!!11
void load(Vm vm, FILE *file){
	u8 buffer[LOAD_BUFFER_SIZE];

	u32 c = (u32) fread(buffer, sizeof(u8), LOAD_BUFFER_SIZE, file);	
	vm->loader.is_at_eof = c < LOAD_BUFFER_SIZE;

	vm->prog_length = 0;

	assert(buffer[0] == (u8) ((u32) CRIMSEMBLY_TAG >> 3*8) & 0xFF);
	assert(buffer[1] == (u8) ((u32) CRIMSEMBLY_TAG >> 2*8) & 0xFF);
	assert(buffer[2] == (u8) ((u32) CRIMSEMBLY_TAG >> 1*8) & 0xFF);
	assert(buffer[3] == (u8) ((u32) CRIMSEMBLY_TAG) & 0xFF);


	for (u32 p = 4; p < c;){
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
}

#define LINKER_BUFFER_SIZE 1024
#define MAX_WORD_SIZE 32 

bool is_whitespace(char c){
	return c == ' ' || c == '\n' || c == '\t';
}

// p is the pointer to the current index on buffer[], gets a word and dumps it to word_buffer[] returning the number of chars dumped (excluding whitespace)
int get_word(int *p, char buffer[], char word_buffer[]){
	if (*p == LINKER_BUFFER_SIZE)
		return 0;

	while(is_whitespace(buffer[*p]))
		*p += 1;	
	
	int c = 0;

	while(!is_whitespace(buffer[*p])){
		if (buffer[*p] == '\0' || *p == LINKER_BUFFER_SIZE - 1 || c == MAX_WORD_SIZE - 1)
			break;

		if (buffer[*p] == ','){
			if (c) break;
			else {
				word_buffer[0] == buffer[*p];
				word_buffer[1] == '\0';
				*p += 1;
				return 1;
			}
		}

		word_buffer[c++] = buffer[*p];
		*p += 1;
	}
	word_buffer[c] = '\0';
	return c;
}

#include <string.h>
enum arg_type{ARG_NONE = 0,
							ARG_REG,
							ARG_VAL, 
							ARG_MEM};

#define MAKE_ARG(type) type
// SWAP ARGUMENTS TO MAKE IT EASIER TO PARSE
#define MAKE_ARG2(t1, t2) t2*10 + t1

int parse_op(char word[], int *op){
	if (strcmp(word, "MVI") == 0){
		*op = OP_MVI;
		return MAKE_ARG2(ARG_REG, ARG_VAL);
	}
	if (strcmp(word, "MOV") == 0){
		*op = OP_MOV;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	if (strcmp(word, "ADD") == 0){
		*op = OP_ADD;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	if (strcmp(word, "DMP") == 0){
		*op = OP_DUMP;
		return MAKE_ARG(ARG_NONE); 
	}
	if (strcmp(word, "STORE") == 0){
		*op = OP_STORE;
		return MAKE_ARG2(ARG_MEM, ARG_REG); 
	}
	if (strcmp(word, "LOAD") == 0){
		*op = OP_LOAD;
		return MAKE_ARG2(ARG_REG, ARG_MEM); 
	}
	if (strcmp(word, "PSH") == 0){
		*op = OP_PUSH;
		return MAKE_ARG(ARG_REG); 
	}
	if (strcmp(word, "POP") == 0){
		*op = OP_POP;
		return MAKE_ARG(ARG_REG); 
	}
	if (strcmp(word, "SUB") == 0){
		*op = OP_SUB;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	if (strcmp(word, "CMP") == 0){
		*op = OP_CMP;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	return -1; // ERROR
}

// TODO : BETTER ERROR HANDLING

#include <assert.h>
#include <stdlib.h>
int expect_type(int type, int *p, char buffer[], int *p_out,  u8 out_buffer[]){
	char word[MAX_WORD_SIZE];
	get_word(p, buffer, word);
	u32 val;
	switch (type){
		case ARG_REG:
			assert(word[0] == 'R');
			assert(word[1] >= '0' && word[1] <= '9');
			assert(word[1] - '0' < R_COUNT);
			out_buffer[*p_out] = (u8) (word[1] - '0');	
			*p_out += 1;
			break;
		case ARG_MEM:
			assert(word[0] == 'm');
			assert(word[1] == '[');
			assert(word[2] == 'R');
			assert(word[3] >= '0' && word[1] < R_COUNT);
			assert(word[4] == ']');
			out_buffer[*p_out] = (u8) (word[3] - '0');
			*p_out += 1;
			break;
		case ARG_VAL:
			val = (u32) atoi(word); 
			for (int i = 3; i >= 0; i--){
				out_buffer[*p_out] = ((u8) val >> (8 * i)) & 0xFF;
				*p_out += 1;
			}
			break;
	}
}

// NEEDS TO BE CHANGED!!!!!!!!!!
int link(char fin[], char fout[]){
	FILE *in = fopen(fin, "r");	
	char buffer[LINKER_BUFFER_SIZE];

	int c = fread(buffer, sizeof(char), LINKER_BUFFER_SIZE - 1, in);
	buffer[c] = '\0';

	fclose(in);	
	int p = 0;
	char word[MAX_WORD_SIZE];

	u8 out_buffer[LINKER_BUFFER_SIZE];
	int p_out = 0;

	while(get_word(&p, buffer, word)){
		int op_code;
		int args = parse_op(word, &op_code);
		out_buffer[p_out++] = op_code; 

		assert(args >= 0);

		while(args != 0){
			expect_type(args % 10, &p, buffer, &p_out, out_buffer);	
			args /= 10;
		}
	}
	
	FILE *out = fopen(fout, "wb");
	// WRITE FILE TAG
	u8 tag_buff[4];
	for (int i = 0; i < 4; i++){
		tag_buff[i] = ((u32) CRIMSEMBLY_TAG >> (3 - i)*8 ) & 0xFF;
	}
	fwrite(tag_buff, sizeof(u8), 4, out);

	c = fwrite(out_buffer, sizeof(u8), p_out, out);

	fclose(out);	
	return 0;
}

int link_F(char fin[], FILE *out){
}

void loader_init(Loader *loader, Vm vm){
	*loader = (Loader){.load = load, .file = NULL, .vm = vm, .is_at_eof = FALSE};
}
