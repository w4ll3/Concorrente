#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>

#ifdef MPI
	#include "mpi.h"
#endif

#define MICROSEC 0.000001

extern int nthreads;
extern int verbose;

void freetrix(float **m_a, int size);

double elapsed;

#define TIME()	struct timespec start, finish; \
				clock_gettime(CLOCK_MONOTONIC, &start);

#define ENDTIME()	clock_gettime(CLOCK_MONOTONIC, &finish); \
					elapsed = (finish.tv_sec - start.tv_sec); \
					elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0; \
					printf("%lf\n", elapsed);


// Prints an error message and exits.
extern void error(const char *msg);

// Generates a normal number.
extern double normalnum(double mu, double sigma);

// Generates a random number.
extern unsigned randnum(void);

// Generates a pseudo-random real number uniformly distributed between 0.0 and 1.0. 
extern double prngnum(void);
extern void prng_set_seed(int seed);

// Safe calloc().
extern void *scalloc(size_t nmemb, size_t size);

// Safe malloc().
extern void *smalloc(size_t size);

// Initializes the random number generator.
extern void srandnum(int seed);

// Safe realloc().
extern void *srealloc(void *p, size_t size);

// Prints a warning message.
extern void warning(const char *msg);

// Initializes power measurement utility.
extern void power_init(void);

// Terminates power measurement utility.
extern double power_end(void);

// Computers the difference between two timers.
extern uint64_t timer_diff(uint64_t t1, uint64_t t2);

// Gets the current timer value
extern uint64_t timer_get(void);	

// Initializes the timer.
extern void timer_init(void);

//Vector.
struct vector {
	int size;        /* Size.     */
	float *elements; /* Elements. */
};

//Opaque pointer to a vector.
typedef struct vector * vector_t;

//Opaque pointer to a constant vector.
typedef const struct vector * const_vector_t;

//Clusters data. 
int *kmeans(vector_t *data, int npoints, int ncentroids, float mindistance);

//Returns the size of a vector.
#define vector_size(v) \
	(((vector_t) (v))->size)

//Clears a vector.
extern void vector_clear(struct vector *v);

//Creates a vector.
extern vector_t vector_create(int n);

//Destroys a vector.
extern void vector_destroy(vector_t v);

//Computes the euclidean distance between two points.
extern float vector_distance(const_vector_t a, const_vector_t b);

//Tests if two vectors are equal.
extern int vector_equal(const_vector_t a, const_vector_t b);

//Assigns a vector to another.
extern vector_t vector_assign(vector_t v1, const_vector_t v2);

//Subtracts two vectors.
extern vector_t vector_sub(vector_t v1, const_vector_t v2);

//Adds two vectors.
extern vector_t vector_add(vector_t v1, const_vector_t v2);

//Multiplies a vector by a scalar.
extern vector_t vector_mult(vector_t v, float scalar);

//Fills up vector with random numbers.
extern vector_t vector_random(vector_t v);

//Returns the element [i] in a vector.
#define VECTOR(v, i) \
	(((vector_t)(v))->elements[(i)])