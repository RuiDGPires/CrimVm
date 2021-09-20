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
 
#include "preprocessor.h"
#include "util.h"
#include <pthread.h>

#define BUFFER_SIZE 1024 
#define DUMP_SIZE BUFFER_SIZE / 2 

static Hashtable symb_table;

static pthread_mutex_t reading_mutex;
static pthread_cond_t reading_can_produce, reading_can_consume;
static u32 reading_producer_index, reading_consumer_index;

static char reading_buffer[BUFFER_SIZE];
static bool reading_buffer_free;

static u32 start = 0;
static bool start_is_defined = FALSE;

static void *mallocWithError(size_t size){
  void *p = malloc(size);
  ASSERT(p != NULL, "Unable to allocate memory");
  return p;
}

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

#define MAX_WORD_SIZE 32 

static int get_word(char *word_buffer){
	u32 p = 0;
	bool is_comment = FALSE;

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

		// Comments
		if (is_comment)
			is_comment = c != '\n';
		else
			is_comment = c == ';';
		
		if (c == '.' || c == 0 || c == '\0' || is_whitespace(c) || is_comment) 
			if (p || c == '\0'){
				word_buffer[p] = '\0';
				break;
			}else continue;	
		else{ 
			word_buffer[p++] = c;
		}	
	}
	return p;
}

static void parse_word(char word[], u32 *pc){
	if (strcmp(word, "MVI") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "MOV") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "ADD") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "DMP") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "STORE") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "LOAD") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "PSH") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "POP") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "SUB") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "CMP") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "BR") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "JMP") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "RET") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "AND") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "OR") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "XOR") == 0){
		(*pc)++;
	}
	else if (strcmp(word, "NOT") == 0){
		(*pc)++;
	}else if (strcmp(word, "SHR") == 0){
		(*pc)++;
	}else if (strcmp(word, "SHL") == 0){
		(*pc)++;
	}else if (strcmp(word, "INC") == 0){
		(*pc)++;
	}else if (strcmp(word, "DEC") == 0){
		(*pc)++;
	}else if (strcmp(word, "MUL") == 0){
		(*pc)++;
	}else if (strcmp(word, "DIV") == 0){
		(*pc)++;
	}else if (strcmp(word, "END") == 0){
		(*pc)++;
	}else if (strcmp(word, "GET") == 0){
		(*pc)++;
	}else if (strcmp(word, "OUT") == 0){
		(*pc)++;
	}else if (strcmp(word, "PRNT") == 0){
		(*pc)++;
	}else if (strcmp(word, "_start:")  == 0){
		ASSERT(start_is_defined == FALSE, "_start can only be defined once");
		start = *pc;
		start_is_defined = TRUE;
	}else{
		// CHECK IF IS LABEL
		u32 last_char_index = strlen(word) - 1;
		if (word[last_char_index] == ':'){
			word[last_char_index] = '\0';
			ht_add(&symb_table, word, *pc);
		}
	}
}

static void *convertFile(void *arg){
	char word[MAX_WORD_SIZE];
	u32 pc = 0;

	while(get_word(word))
		 parse_word(word, &pc);

	ht_add(&symb_table, "_start", start);
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

Hashtable preprocess(char *file_in){
	symb_table = ht_create();
	pthread_mutex_init(&reading_mutex, NULL);	

	pthread_cond_init(&reading_can_consume, NULL);
	pthread_cond_init(&reading_can_produce, NULL);

	reading_consumer_index = BUFFER_SIZE -1;
	reading_producer_index = 0;

	reading_buffer_free = FALSE;

	// CREATE WORKING THREADS 
	pthread_t *readingThread = createReadingThread((void *) file_in);
	pthread_t *convertingThread = createConvertingThread();

	// WAIT UNTIL PROCESSING IS FINISHED
	pthread_join(*readingThread, NULL);
	pthread_join(*convertingThread, NULL);

	free(readingThread);
	free(convertingThread);
	
	pthread_mutex_destroy(&reading_mutex);
	return symb_table;
}
