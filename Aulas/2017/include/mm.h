# ifndef _MM_H
# 	define _MM_H
# endif

/* Sequential dummy code */
void start_seq();

/* Pthread parallel version of dummy sequential */
void *parallel_pthread(void *arg);

/* OpenMP version of dummy code */
void start_openmp();

/* Initialize matrices with random values */
void init();