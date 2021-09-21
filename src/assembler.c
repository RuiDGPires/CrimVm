// MIT License

// Copyright (c) 2021 Rui Pires

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
 
#include "vm.h"
#include "util.h"
#include "preprocessor.h"
#include <pthread.h>

static void *mallocWithError(size_t size){
  void *p = malloc(size);
  ASSERT(p != NULL, "Unable to allocate memory");
  return p;
}

#define BUFFER_SIZE 1024 
#define DUMP_SIZE BUFFER_SIZE / 2 

static Hashtable symb_table;

static pthread_mutex_t reading_mutex, writing_mutex;
static pthread_cond_t reading_can_produce, reading_can_consume, writing_can_produce, writing_can_consume;
static u32 reading_producer_index, reading_consumer_index, writing_producer_index, writing_consumer_index;

static char reading_buffer[BUFFER_SIZE];
static bool reading_buffer_free;

static u8 writing_buffer[BUFFER_SIZE];
static bool writing_buffer_free;

// THREAD CONTROL //////////
static void mutexLock(pthread_mutex_t *mutex) {
	ASSERT(pthread_mutex_lock(mutex) == 0, "Error locking mutex");
}

static void mutexUnlock(pthread_mutex_t *mutex) {
	ASSERT(pthread_mutex_unlock(mutex) == 0, "Error unlocking mutex");
}

static void waitCondition(pthread_cond_t *cond, pthread_mutex_t *mutex){
	ASSERT(pthread_cond_wait(cond, mutex) == 0, "Unable to wait for condition");
}

static void signalCondition(pthread_cond_t *cond){
	ASSERT(pthread_cond_signal(cond) == 0, "Unable to signal condition");
}
///////////////////////////


static u32 get_distance_in_buffer(u32 a, u32 b){
	return a <= b? b - a: BUFFER_SIZE - a + b;  
}

static void *readFile(void *arg){
	char *filename = (char *) arg;
	FILE *file = fopen(filename, "r");
	ASSERT(file != NULL, "Unable to open file: %s", filename);

	int c;
	char tmp[DUMP_SIZE];

	do{
		c = fread(tmp, sizeof(char), DUMP_SIZE, file);

		mutexLock(&reading_mutex);
		// Wait until able to write
		while (get_distance_in_buffer(reading_producer_index, reading_consumer_index) < c + 1)
			waitCondition(&reading_can_produce, &reading_mutex);

		for (int i = 0; i < c; i++){
			reading_buffer[(reading_producer_index + i) % BUFFER_SIZE] = tmp[i];
		}

		reading_producer_index = (reading_producer_index + c) % BUFFER_SIZE;

		signalCondition(&reading_can_consume);
		mutexUnlock(&reading_mutex);	
	}while(c != 0);

	// Check if any error occured
	if (ferror(file)) THROW_ERROR("An error occured while reading file");

	fclose(file);	
	mutexLock(&reading_mutex);

	reading_buffer_free = TRUE;
	signalCondition(&reading_can_consume);

	mutexUnlock(&reading_mutex);
	pthread_exit(NULL);
	return NULL;
}

static bool is_whitespace(char c){
	return c == ' ' || c == '\n' || c == '\t';
}

static char parse_special_char(char c){
	switch(c){
		case 'n':
			return '\n';
		case 't':
			return '\t';
		case '\\':
			return '\\';
		case '0':
			return '\0';
		case '"':
			return '"';
	}
	return -1;
}

static bool ends_in_dot = FALSE;
static int get_word(char *word_buffer){
	u32 p = 0;
	bool is_comment = FALSE;
	bool is_string = FALSE;
	bool read_slash = FALSE;

	if (ends_in_dot)
		ends_in_dot = FALSE;

	while(1){
		mutexLock(&reading_mutex);
		u32 dist = get_distance_in_buffer(reading_consumer_index, reading_producer_index);
		ASSERT(p < MAX_WORD_SIZE, "WORD IS TOO LARGE");
		while(!reading_buffer_free && dist <= 1){
			waitCondition(&reading_can_consume, &reading_mutex);
			dist = get_distance_in_buffer(reading_consumer_index, reading_producer_index);
		}

		// EOF
		if (reading_buffer_free && dist == 1){
			word_buffer[p] = '\0';
			mutexUnlock(&reading_mutex);
			break;
		} 

		reading_consumer_index = (reading_consumer_index + 1) % BUFFER_SIZE;
		char c = reading_buffer[reading_consumer_index];

		signalCondition(&reading_can_produce);
		mutexUnlock(&reading_mutex);


		if (!is_comment && !read_slash && c == '\\'){
			read_slash = TRUE; 
			continue;
		}
		
		// Special chars
		if (!is_comment && read_slash){
			word_buffer[p++] = parse_special_char(c);
			read_slash = FALSE;
			continue;
		}

		if (!is_comment && c == '"'){
			is_string = !is_string;
			if (!is_string){
				word_buffer[p] = '\0';
				break;
			}
			continue;
		}
		if (!is_comment && is_string){
			word_buffer[p++] = c;
			continue;
		}

		// Comments
		if (is_comment){
			is_comment = c != '\n';
		}else	if (word_buffer[0] != '\'' || p != 1)
			is_comment = c == ';';

		if (c == '\0' || is_whitespace(c) || is_comment){ 
			if (is_comment) continue;
			if (p || c == '\0'){
				word_buffer[p] = '\0';
				break;
			}else continue;	
		}else{ 
			// Special case for dot
			if (c == '.'){
				ends_in_dot = TRUE;
				word_buffer[p] = '\0';
				break;
			}else
				word_buffer[p++] = c;
		}	
	}
	return p;
}

enum arg_type{ARG_NONE = 0,
							ARG_REG,
							ARG_VAL, 
							ARG_MEM,
							ARG_FLAG,
							ARG_LABEL_OR_VAL,
							ARG_LABEL_OR_OFFSET,
							ARG_STR,
							};

#define MAKE_ARG(type) type
// SWAP ARGUMENTS TO MAKE IT EASIER TO PARSE
#define MAKE_ARG2(t1, t2) t2*10 + t1

static int parse_op(char word[], int *op){
	if (strcmp(word, "MVI") == 0){
		*op = OP_MVI;
		return MAKE_ARG2(ARG_REG, ARG_VAL);
	}
	else if (strcmp(word, "MOV") == 0){
		*op = OP_MOV;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	else if (strcmp(word, "ADD") == 0){
		*op = OP_ADD;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	else if (strcmp(word, "STORE") == 0){
		*op = OP_STORE;
		return MAKE_ARG2(ARG_MEM, ARG_REG); 
	}
	else if (strcmp(word, "LOAD") == 0){
		*op = OP_LOAD;
		return MAKE_ARG2(ARG_REG, ARG_MEM); 
	}
	else if (strcmp(word, "PSH") == 0){
		*op = OP_PUSH;
		return MAKE_ARG(ARG_REG); 
	}
	else if (strcmp(word, "POP") == 0){
		*op = OP_POP;
		return MAKE_ARG(ARG_REG); 
	}
	else if (strcmp(word, "SUB") == 0){
		*op = OP_SUB;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	else if (strcmp(word, "CMP") == 0){
		*op = OP_CMP;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	else if (strcmp(word, "BR") == 0){
		*op = OP_BR;
		return MAKE_ARG2(ARG_FLAG, ARG_LABEL_OR_OFFSET);	
	}
	else if (strcmp(word, "JMP") == 0){
		*op = OP_JMP;
		return MAKE_ARG2(ARG_FLAG, ARG_LABEL_OR_VAL);	
	}
	else if (strcmp(word, "RET") == 0){
		*op = OP_RET;
		return MAKE_ARG(ARG_NONE);	
	}
	else if (strcmp(word, "AND") == 0){
		*op = OP_AND;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	else if (strcmp(word, "OR") == 0){
		*op = OP_OR;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	else if (strcmp(word, "XOR") == 0){
		*op = OP_XOR;
		return MAKE_ARG2(ARG_REG, ARG_REG);	
	}
	else if (strcmp(word, "NOT") == 0){
		*op = OP_NOT;
		return MAKE_ARG(ARG_REG);	
	}else if (strcmp(word, "SHR") == 0){
		*op = OP_SHR;
		return MAKE_ARG(ARG_REG);		
	}else if (strcmp(word, "SHL") == 0){
		*op = OP_SHL;
		return MAKE_ARG(ARG_REG);		
	}else if (strcmp(word, "INC") == 0){
		*op = OP_INC;
		return MAKE_ARG(ARG_REG);		
	}else if (strcmp(word, "DEC") == 0){
		*op = OP_DEC;
		return MAKE_ARG(ARG_REG);		
	}else if (strcmp(word, "INC") == 0){
		*op = OP_INC;
		return MAKE_ARG(ARG_REG);		
	}else if (strcmp(word, "MUL") == 0){
		*op = OP_MUL;
		return MAKE_ARG2(ARG_REG, ARG_REG);		
	}else if (strcmp(word, "DIV") == 0){
		*op = OP_DIV;
		return MAKE_ARG2(ARG_REG, ARG_REG);		
	}else if (strcmp(word, "STR") == 0){
		*op = OP_STR;
		return MAKE_ARG2(ARG_MEM, ARG_STR);		
	}else if (strcmp(word, "END") == 0){
		*op = OP_END;
		return MAKE_ARG(ARG_NONE);
	}else if (strcmp(word, "%define") == 0){
		// Ignore the two next words
		get_word(word);
		get_word(word);
		*op = IGNORE;
		if (get_word(word))
			return parse_op(word, op);
		else
			return MAKE_ARG(ARG_NONE);		

	// TRAP ROUTINES
	}else if (strcmp(word, "DMP") == 0){
		*op = TRP_DUMP;
		return MAKE_ARG(ARG_NONE);	
	}else if (strcmp(word, "GET") == 0){
		*op = TRP_GETC;
		return MAKE_ARG(ARG_NONE);		
	}else if (strcmp(word, "OUT") == 0){
		*op = TRP_OUT;
		return MAKE_ARG(ARG_NONE);		
	}else if (strcmp(word, "PRNT") == 0){
		*op = TRP_PRNT;
		return MAKE_ARG(ARG_NONE);		
	} else{
		// SKIP LABELS
		u32 last_char_index = strlen(word) - 1;
		if (word[last_char_index] == ':'){
			*op = IGNORE;
			return MAKE_ARG(ARG_NONE);	
		}
		else THROW_ERROR("Unkown operation: %s", word);
	}
}

static void write_to_buffer(u8 val){
	mutexLock(&writing_mutex);


	while(get_distance_in_buffer(writing_producer_index, writing_consumer_index) == 1)
		waitCondition(&writing_can_produce, &writing_mutex);

	writing_buffer[writing_producer_index] = val;
	writing_producer_index = (writing_producer_index + 1) % BUFFER_SIZE;

	signalCondition(&writing_can_consume);
	mutexUnlock(&writing_mutex);
}

static void write_to_buffer_u32(u32 val){
	for (int i = 3; i >= 0; i--)
		write_to_buffer((val >> (8*i)) & 0xFF); // Write the offset
}

static bool is_number(char c){
	return c >= '0' && c <= '9';
}

static u32 hex_to_u32(char hex[]) {
    u32 val = 0;
    while (*hex) {
        // get current character then increment
        u8 byte = *hex++; 
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
					else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
        // shift 4 to make space for new digit, and add the 4 bits of the new digit 
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}


static u32 flags = 0;
static u32 pc = 0;

static u8 parse_register(char c){
	if (c == 'E')
		return RE;
	ASSERT(c >= '0' && c <= '9', "Unexpected Token: %c", c);
	ASSERT(c - '0' < R_COUNT, "Invalid register: R%d", c - '0');

	return c - '0';
}


static void expect_type(u8 type){
	char word[MAX_WORD_SIZE];
	get_word(word);
	u32 val;
	u32 aux;

	switch (type){
		case ARG_REG:
			ASSERT(word[0] == 'R', "Unexpected Token: %c", word[0]);
			write_to_buffer(parse_register(word[1]));	
			break;
		case ARG_STR:
			{
				u8 i = 0;
				for(; word[i] != '\0' && i < MAX_WORD_SIZE; i++);

				ASSERT(word[i] == '\0', "String size is too big");

				// WRITE THE SIZE OF THE STRING IN 8 BITS
				write_to_buffer(i);
				for (u8 j = 0; j < i; j++)
					write_to_buffer(word[j]);
			}
			break;
		case ARG_MEM:
			ASSERT(word[0] == 'm', "Unexpected Token: %c", word[0]);
			ASSERT(word[1] == '[', "Unexpected Token: %c", word[1]);
			ASSERT(word[2] == 'R', "Unexpected Token: %c", word[2]);
			ASSERT(word[4] == ']', "Unexpected Token: %c", word[4]);
			// Check if there is any offset
			if (word[5] == '['){
				write_to_buffer(parse_register(word[3]) | 0x80); // Set the most significant bit of the register value to signal an offset
				if (word[6] == 'R'){ // IF IT IS A REGISTER
					write_to_buffer(parse_register(word[7]) | 0x80); // Set the ms bit
					ASSERT(word[8] == ']', "Unexpected Token: %c", word[8]);
				}else{
					u32 offset = 0;
					u32 i;
					for (i = 6; i < MAX_WORD_SIZE-1 && word[i] != ']'; i++);
					ASSERT(word[i] == ']', "Word exceeds max size");
					word[i] = '\0';
					offset = (u32) atol(&word[6]);
					write_to_buffer_u32(offset); // Write the offset
				}
			}else{
				write_to_buffer(parse_register(word[3]));
			}

			break;

		case ARG_LABEL_OR_OFFSET:
		case ARG_LABEL_OR_VAL:
			if (is_number(word[0])) goto arg_val;
			
			for (aux = 0; word[aux] != '\0' && word[aux] != ':'; aux++) ;
			
			// IF LABEL SYMB
			
			val = ht_get(symb_table, word);
			ASSERT(val != INVALID_ITEM, "Label not defined: %s", word);

			// IF OFFSET
			if (type == ARG_LABEL_OR_OFFSET)
				val -= pc -1;


			write_to_buffer_u32(val); 
			break;

		case ARG_FLAG:
			aux = 0;
			if (strcmp(word, "N") == 0)
				flags |= FLG_NEG;
			else if (strcmp(word, "NN") == 0){
				flags |= FLG_POS;
				flags |= FLG_ZERO;
			} else if (strcmp(word, "P") == 0)
				flags |= FLG_POS;
			else if (strcmp(word, "NP") == 0){
				flags |= FLG_NEG;
				flags |= FLG_ZERO;
			} else if (strcmp(word, "Z") == 0)
				flags |= FLG_ZERO;
			else if (strcmp(word, "NZ") == 0){
				flags |= FLG_NEG;
				flags |= FLG_POS;
			} else if (strcmp(word, "O") == 0)
				flags |= FLG_OV;

			else THROW_ERROR("Unkown flag: %s", word);
			break;
		case ARG_VAL:
		arg_val:
			if (word[0] == '\''){
				ASSERT(word[2] == '\'', "Char literals must be contained inside \' \'");
				val = word[1]; 
			}else if (word[0] == '0' && word[1] == 'x')
				val = hex_to_u32(&word[2]);
			else if (is_number(word[0]))
				val = (u32) atoi(word); 
			else
				val = ht_get(symb_table, word);
			
			write_to_buffer_u32(val);
			break;
	}
}

static void *convertFile(void *arg){
	char word[MAX_WORD_SIZE];

	while(get_word(word)){
		int op_code;
		int args = parse_op(word, &op_code);
		if (op_code == IGNORE) continue;		

		pc++;
		write_to_buffer(op_code);

		ASSERT(args >= 0, "Unkown error");

		for(; args != 0; args /= 10){
			u8 type = args % 10;
	
			// Flag system
			if (type == ARG_FLAG){
				flags = 0;
				if (!ends_in_dot){
					write_to_buffer(0);
					continue;	
				}else{
					while(ends_in_dot){
						expect_type(ARG_FLAG);
					}
					write_to_buffer((u8) flags);
					continue;
				}
			}
		

			expect_type(type);

			if (ends_in_dot)
				THROW_ERROR("Unexpected symbol '.'");
		}
	}
	mutexLock(&writing_mutex);
	writing_buffer_free = TRUE;
	signalCondition(&writing_can_consume);
	mutexUnlock(&writing_mutex);
	pthread_exit(NULL);
	return NULL;
}

static void *writeFile(void *arg){
	char *filename = (char *) arg;
	FILE *file = fopen(filename, "wb");
	if (file == NULL) THROW_ERROR("Unable to open file: %s", filename);

	
	u8 tag[] = {(CRIMSEMBLY_TAG >> 3*8) & 0xFF,
							(CRIMSEMBLY_TAG >> 2*8) & 0xFF,
							(CRIMSEMBLY_TAG >> 1*8) & 0xFF,
							(CRIMSEMBLY_TAG >> 0*8) & 0xFF};

	fwrite(tag, sizeof(u8), 4, file);

	u32 start = ht_get(symb_table, "_start");
	u8 start_vec[4];
	for (int i = 0; i < 4; i++)
		start_vec[3 - i] = (start >> i * 8) & 0xFF;

	fwrite(start_vec, sizeof(u8), 4, file);

	u8 tmp[BUFFER_SIZE];

	while(!writing_buffer_free || get_distance_in_buffer(writing_consumer_index, writing_producer_index) != 1){	
		mutexLock(&writing_mutex);


		u32 dist = get_distance_in_buffer(writing_consumer_index, writing_producer_index);

		while (!writing_buffer_free && dist < DUMP_SIZE){
			waitCondition(&writing_can_consume, &writing_mutex);
			dist = get_distance_in_buffer(writing_consumer_index, writing_producer_index);
		}
		

		for (int i = 0; i < dist - 1; i++){
			tmp[i] = writing_buffer[(writing_consumer_index + i + 1) % BUFFER_SIZE];
		}

		writing_consumer_index = (writing_consumer_index + dist - 1) % BUFFER_SIZE;
		signalCondition(&writing_can_produce);
		mutexUnlock(&writing_mutex);	

		fwrite(tmp, sizeof(u8), dist - 1, file);
	}
	
	fclose(file);	
	pthread_exit(NULL);
	return NULL;
}

static pthread_t *createReadingThread(char file[]){
	pthread_t *thread = (pthread_t *) mallocWithError(sizeof(pthread_t));
	
	if (pthread_create(thread, NULL, readFile, (void *) file))
			THROW_ERROR("Couldnt create thread");
	return thread;
}

static pthread_t *createConvertingThread(){
	pthread_t *thread = (pthread_t *) mallocWithError(sizeof(pthread_t));
	
	if (pthread_create(thread, NULL, convertFile, NULL))
			THROW_ERROR("Couldnt create thread");
	return thread;
}

static pthread_t *createWritingThread(char file[]){
	pthread_t *thread = (pthread_t *) mallocWithError(sizeof(pthread_t));
	
	if (pthread_create(thread, NULL, writeFile, (void *) file))
			THROW_ERROR("Couldnt create thread");
	return thread;
}


void assemble(char *file_in, char *file_out){
	symb_table = preprocess(file_in);
	pthread_mutex_init(&reading_mutex, NULL);	
	pthread_mutex_init(&writing_mutex, NULL);	

	pthread_cond_init(&reading_can_consume, NULL);
	pthread_cond_init(&reading_can_produce, NULL);
	pthread_cond_init(&writing_can_consume, NULL);
	pthread_cond_init(&writing_can_produce, NULL);

	reading_consumer_index = BUFFER_SIZE -1;
	writing_consumer_index = BUFFER_SIZE -1;
	reading_producer_index = 0;
	writing_producer_index = 0;

	reading_buffer_free = FALSE;
	writing_buffer_free = FALSE;

	// CREATE WORKING THREADS 
	pthread_t *readingThread = createReadingThread((void *) file_in);
	pthread_t *convertingThread = createConvertingThread();
	pthread_t *writingThread = createWritingThread((void *) file_out);

	// WAIT UNTIL PROCESSING IS FINISHED
	pthread_join(*readingThread, NULL);
	pthread_join(*convertingThread, NULL);
	pthread_join(*writingThread, NULL);

	free(readingThread);
	free(convertingThread);
	free(writingThread);
	
	ht_destroy(symb_table);
	pthread_mutex_destroy(&reading_mutex);
	pthread_mutex_destroy(&writing_mutex);
}
