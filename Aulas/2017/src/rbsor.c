#include "rbsor.h"
#include "util.h"

/* Threads shared variable */
int **A;
int size = SIZE;
int threads = 0;
int msize = SIZE - 1;

/* Sync tools */
pthread_barrier_t barrier;
sem_t sem;

void init() {
	srand(time(0));
	A = (int**) malloc(sizeof(int*) * SIZE);
	for (int i = 0; i < SIZE; i++) {
		A[i] = (int*) malloc(sizeof(int) * SIZE);
		for (int j = 0; j < SIZE; j++) {
			A[i][j] = rand() % 10;
		}
	}
}

void start_seq() {
	TIME()
	// Red
	for (int i = 1; i < size - 1; i++) {
		for (int j = (2 - (i % 2)); j < size - 1; j += 2) {
			A[i][j] = ((A[i - 1][j] + A[i][j - 1] + A[i + 1][j] + A[i][j + 1]) / 4) * FACTOR;
		}
	}
	// Black
	for (int i = 1; i < size - 1; i++) {
		for (int j = ((i % 2) + 1); j < size - 1; j += 2) {
			A[i][j] = ((A[i - 1][j] + A[i][j - 1] + A[i + 1][j] + A[i][j + 1]) / 4) * FACTOR;
		}
	}
	ENDTIME()
}

void *parallel_pthread(void *arg) {
	int *id = (int *) arg;
	int stripe = get_stripe(threads, (size - 2));
	int init = get_init(*id, stripe) + 1;
	int end = get_end(init, stripe);
	if (*id == (threads - 1)) end--;
	// Red
	for (int i = init; i < end; i++) {
		for (int j = 2 - (i % 2); j < size - 1; j += 2) {
			A[i][j] = ((A[i - 1][j] + A[i][j - 1] + A[i + 1][j] + A[i][j + 1]) / 4) * FACTOR;
		}
	}
	pthread_barrier_wait(&barrier);
	// Black
	for (int i = init; i < end; i++) {
		for (int j = (i % 2) + 1; j < size - 1; j += 2) {
			A[i][j] = ((A[i - 1][j] + A[i][j - 1] + A[i + 1][j] + A[i][j + 1]) / 4) * FACTOR;
		}
	}
	return 0;
}

void start_openmp() {
	TIME()
	#pragma omp parallel num_threads(threads)
	{
		//Red
		#pragma omp for simd
		for (int i = 1; i < size - 1; i++) {
			for (int j = 2 - (i % 2); j < size - 1; j += 2) {
				A[i][j] = ((A[i - 1][j] + A[i][j - 1] + A[i + 1][j] + A[i][j + 1]) / 4) * FACTOR;
			}
		}
		//Black
		#pragma omp for simd
		for (int i = 1; i < size - 1; i++) {
			for (int j = (i % 2) + 1; j < size - 1; j += 2) {
				A[i][j] = ((A[i - 1][j] + A[i][j - 1] + A[i + 1][j] + A[i][j + 1]) / 4) * FACTOR;
			}
		}
	}
	ENDTIME()
}

int main(int argc, char **argv) {
	int prog = atoi(argv[2]);
	threads = atoi(argv[1]);
	init();

	/* Initialize barrier */
	pthread_barrier_init(&barrier, NULL, threads);

	switch (prog) {
		case 0: {
			start_seq();
			freetrix(A);
			break;
		}

		case 1: {
			pthread_t thread[threads];
			start_pthread(thread, threads, parallel_pthread);
			freetrix(A);
			break;
		}

		case 2: {
			start_openmp();
			freetrix(A);
			break;
		}

		default: {
			break;
		}
	}
	return 0;
}
