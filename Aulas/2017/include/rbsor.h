# ifndef _RBSOR_H
#	define _RBSOR_H
# endif

#define FACTOR 4

/* Initialize global matrices */
void init();

/* Sequential dummy code */
void start_seq();

/* Pthread parallel version of dummy sequential */
void *parallel_pthread(void *arg);

/* OpenMP version of dummy code */
void start_openmp();
