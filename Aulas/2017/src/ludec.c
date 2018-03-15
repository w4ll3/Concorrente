#include "ludec.h"
#include "util.h"

#define pOPENMP() print(A); start_openmp(); print(A);
#define pSEQ() print(A); start_seq(); print(A);
#define pTHREAD() print(A); start_pthread(thread, threads, parallel_pthread); print(A);

int size = SIZE;
int threads = 0;
omp_lock_t lock;
pthread_barrier_t barrier;

double **A;

void start_seq() {
	TIME()
	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			A[j][i] = A[j][i]/A[i][i];
		}
		for (int j = i + 1; j < size; j++) {
			for (int k = i + 1; k < size; k++) {
				A[j][k] -= A[j][i] * A[i][k];
			}
		}
	}
	ENDTIME()
}

void start_openmp() {
	TIME()
	#pragma omp parallel num_threads(threads)
	{
		for (int i = 0; i < size; i++) {
			#pragma omp for simd
			for (int j = i + 1; j < size; j++) {
				A[j][i] = A[j][i]/A[i][i];
			}
			#pragma omp for simd
			for (int j = i + 1; j < size; j++) {
				for (int k = i + 1; k < size; k++) {
					A[j][k] -= A[j][i] * A[i][k];
				}
			}
		}
	}
	ENDTIME()
}

void *parallel_pthread(void *arg) {
	int *id = (int *) arg;
	int stripe = size / threads;
	int init = (*id) * stripe;
	int end = init + stripe;
	TIME()
	for (int i = init; i < end; i++) {
		for (int j = i; j < size; j++) {
			for (int k = 0; k < i; k++) {
				A[i][j] -= A[i][k] * A[k][j];
			}
		}
		for (int j = i + 1; j < size; j++) {
			for (int k = 0; k < i; k++) {
				A[j][i] -= A[j][k] * A[k][i];
			}
			A[j][i] /= A[i][i];
		}
		pthread_barrier_wait(&barrier);
	}
	ENDTIME()
}

int main(int argc, char **argv) {
	int prog = atoi(argv[2]);
	threads = atoi(argv[1]);
	pthread_barrier_init(&barrier, NULL, threads);
	minit(&A, 10);
	switch (prog) {
		case 0: {
			// pSEQ()
			start_seq();
			break;
		}

		case 1: {
			pthread_t thread[threads];
			// pTHREAD()
			start_pthread(thread, threads, parallel_pthread);
			break;
		}

		case 2: {
			// pOPENMP()
			start_openmp();
			break;
		}

		default: {
			break;
		}
	}
	freetrix(A);
	return 0;
}
