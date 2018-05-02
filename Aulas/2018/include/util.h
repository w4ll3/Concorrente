#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include "mpi.h"

void freetrix(float **m_a, int size);
void rantrix(double **m, int size, int max);

double elapsed;

#define TIME()	struct timespec start, finish; \
				clock_gettime(CLOCK_MONOTONIC, &start);

#define ENDTIME()	clock_gettime(CLOCK_MONOTONIC, &finish); \
					elapsed = (finish.tv_sec - start.tv_sec); \
					elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0; \
					printf("%lf\n", elapsed);