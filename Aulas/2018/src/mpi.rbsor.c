#include "../include/util.h"

double **rb_matrix;
pthread_barrier_t barrier;
float real_time, proc_time, mflops;
long long flpins;
int *ids, size, threads;
int world_size;
int world_rank;

void rantrix(int max) {
	rb_matrix = (double**) malloc(sizeof(double*) * size);
	double *rb_data = (double*) malloc(sizeof(double) * size * size);
	for(int i = 0; i < size; i++) {
		rb_matrix[i] = &(rb_data[size * i]);
		for(int j = 0; j < size; j++)
			rb_matrix[i][j] = rand() % size / rand() % size + 1;
	}
}

void rbsor() {
	int stripe = (size - 2) / world_size;
	int init = (world_rank * stripe) + 1;
	int end = init + stripe;
	if (world_rank == (world_size - 1)) end--;
	// Red
	for (int i = init; i < end; i++) {
		for (int j = 2 - (i % 2); j < size - 1; j += 2) {
			rb_matrix[i][j] = ((rb_matrix[i - 1][j] + rb_matrix[i][j - 1] + rb_matrix[i + 1][j] + rb_matrix[i][j + 1]) / 0.25);
		}
	}
	MPI_Allgather(&(rb_matrix[init][end]), stripe * size - 1, MPI_FLOAT, &(rb_matrix[0][0]), stripe * size - 1, MPI_FLOAT, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	// Black
	for (int i = init; i < end; i++) {
		for (int j = (i % 2) + 1; j < size - 1; j += 2) {
			rb_matrix[i][j] = ((rb_matrix[i - 1][j] + rb_matrix[i][j - 1] + rb_matrix[i + 1][j] + rb_matrix[i][j + 1]) / 0.25);
		}
	}
}

int main(int argc, char *argv[]) {
	srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

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
	if(!world_rank) rantrix(10000);
	MPI_Bcast(&(rb_matrix[0][0]), size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
}