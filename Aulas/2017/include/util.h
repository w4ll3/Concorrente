# ifndef _UTIL_H
#	define _UTIL_H
# endif

# if !defined(DEBUG) && !defined(MINI) && !defined(SMALL) && !defined(MEDIUM) && !defined(BIG) && !defined(LARGE) && !defined(GIANT) && !defined(HUGE) && !defined(WTF) && !defined(NI)
#	define DEBUG
# endif

# ifndef SIZE

#	ifdef STANDARD_DATASET
#		define SIZE 1024
#	endif

#	ifdef MINI
#		define SIZE 512
#	endif

#	ifdef SMALL
#		define SIZE 1024
#	endif

#	ifdef MEDIUM
#		define SIZE 2048
#	endif

#	ifdef BIG
#		define SIZE 4096
#	endif

#	ifdef LARGE
#		define SIZE 8192
#	endif

#	ifdef GIANT
#		define SIZE 16384
#	endif

#	ifdef HUGE
#		define SIZE 32768
#	endif

#	ifdef WTF
#		define SIZE 65536
#	endif

# endif

/* PAPI */
#ifdef PAPI
#include <papi.h>
#define PAPI_SIZE 6
#endif

#ifdef INT
#define TYPEDEF int
#else
#define TYPEDEF double
#endif

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <semaphore.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* Start pthread */
void start_pthread(pthread_t *thread, int threads, void *(*parallel_pthread)(void*));

/* Divide problem functions */
int get_stripe(int tnum);
int get_init(int id, int stripe);
int get_end(int init, int stripe);

/* Debug purposes only */
void print(TYPEDEF **matrix);

/* Reset everything matrix */
void reset(TYPEDEF **A);

/* Malloc Matrix */
int mrand(void);
void msrand(unsigned seed);
void init(TYPEDEF ***A, int fact);
void minit(TYPEDEF ***A, int fact);
void zinit(TYPEDEF ***A);

/* Be clean */
void freetrix(TYPEDEF **A);

/* Line */

typedef struct structure {
	TYPEDEF data;
	struct structure *next;
} node;

typedef struct {
	node *beggin, *end;
} line;