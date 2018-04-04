#include "../include/util.h"

double **rb_matrix;
pthread_barrier_t barrier;
int *ids, size, threads;

void *rbsor(int *arg) {
	int id = *((int*) arg);
	int stripe = (size - 2) / threads;
	int init = (id * stripe) + 1;
	int end = init + stripe;
	if (id == (threads - 1)) end--;
	// Red
	for (int i = init; i < end; i++) {
		for (int j = 2 - (i % 2); j < size - 1; j += 2) {
			rb_matrix[i][j] = ((rb_matrix[i - 1][j] + rb_matrix[i][j - 1] + rb_matrix[i + 1][j] + rb_matrix[i][j + 1]) / 0.25);
		}
	}
	pthread_barrier_wait(&barrier);
	// Black
	for (int i = init; i < end; i++) {
		for (int j = (i % 2) + 1; j < size - 1; j += 2) {
			rb_matrix[i][j] = ((rb_matrix[i - 1][j] + rb_matrix[i][j - 1] + rb_matrix[i + 1][j] + rb_matrix[i][j + 1]) / 0.25);
		}
	}
	pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	int c;
	struct option long_args[] = {
		{"size", required_argument, NULL, 's'},
		{"threads",  required_argument, NULL, 't'}
	};
	do {
		c = getopt_long(argc, argv, "s:t:", long_args, NULL);
		switch(c) {
			case -1:
			case 0: break;
			case 's': {
				size = atoi(optarg);
			} break;
			case 't': {
				threads = atoi(optarg);
			} break;
			default: {
				printf("Os seguintes paramêtros são obrigatório para a execução do programa: \
				\n\tDimensão da matriz (--size | -s). \
				\n\tNúmero de threads (--threads | -t).\n");
			} exit(EXIT_FAILURE);
		}
	} while(c != -1);
	rantrix(rb_matrix, size, 10000);
	pthread_t thread[threads];
	ids = (int*) calloc(threads, sizeof(int));
	pthread_barrier_init(&barrier, NULL, threads);
	for (int i = 0; i < threads; i++, (ids[i] = i))
		pthread_create(&thread[i], NULL, (void *) rbsor, (void *) &ids[i]);
	for (int i = 0; i < threads; i++)
		pthread_join(thread[i], NULL);
}