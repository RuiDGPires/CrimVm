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
