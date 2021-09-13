#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "defs.h"

#ifdef DEBUG
#ifndef __FUNCTION_NAME__ 
	#ifdef WIN32   //WINDOWS
		#define __FUNCTION_NAME__   __FUNCTION__  
	#else          //*NIX
		#define __FUNCTION_NAME__   __func__ 
	#endif
#endif

#define EXTRA() fprintf(stderr, "%s: ", __FUNCTION_NAME__)
#else
#define EXTRA() ;
#endif

#define THROW_ERROR(...) \
    do{\
			fflush(stdout);\
			EXTRA(); \
			fprintf(stderr, __VA_ARGS__); \
			fprintf(stderr, "\n");\
			if (errno)\
				fprintf(stderr, "\t%s\n", strerror(errno));\
			exit(-1);\
    }while(0)

#define ASSERT(expression, ...) \
	do{\
	if (expression);\
	else THROW_ERROR(__VA_ARGS__);\
	}while(0)

#endif
