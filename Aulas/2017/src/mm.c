#include "util.h"
#include "mm.h"

/* Threads shared variables */
int **A;
int **B;
int **C;
int size = SIZE;
int threads = 0;

/* Sequential dummy code */
void start_seq() {
	TIME()
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	ENDTIME()
}

/* Pthread parallel version of dummy sequential */
void *parallel_pthread(void *arg) {
	int *id = (int *) arg;
	int stripe = size / threads;
	int init = (*id) * stripe;
	int end = init + stripe;
	for (int i = init; i < end; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	return 0;
}

/* OpenMP version of dummy code */
void start_openmp() {
	TIME()
	#pragma omp parallel for num_threads(threads)
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	ENDTIME()
}

void init() {
	srand(time(0));
	A = (int**) malloc(sizeof(int*) * SIZE);
	B = (int**) malloc(sizeof(int*) * SIZE);
	C = (int**) malloc(sizeof(int*) * SIZE);
	for (int i = 0; i < SIZE; i++) {
		A[i] = (int*) malloc(sizeof(int) * SIZE);
		B[i] = (int*) malloc(sizeof(int) * SIZE);
		C[i] = (int*) malloc(sizeof(int) * SIZE);
		for (int j = 0; j < SIZE; j++) {
			A[i][j] = rand() % 2;
			B[i][j] = rand() % 2;
			C[i][j] = 0;
		}
	}
}

int main(int argc, char **argv) {
	int prog = atoi(argv[2]);
	threads = atoi(argv[1]);
	init();
	switch (prog) {
		case 0: {
			start_seq();
			freetrix(A);
			freetrix(B);
			freetrix(C);
			break;
		}

		case 1: {
			pthread_t thread[threads];
			start_pthread(thread, threads, parallel_pthread);
			freetrix(A);
			freetrix(B);
			freetrix(C);
			break;
		}

		case 2: {
			start_openmp();
			freetrix(A);
			freetrix(B);
			freetrix(C);
			break;
		}

		default: {
			break;
		}
	}
	return 0;
}
