#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "util.h"

int mm(int init, int end, int A[], int B[]) {
	int C[end - init][end - init] = { 0 };
	for (int i = init; i < end; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	return 0;
}

void init(int *A, int *B) {
	srand(time(0));
	for (int i = 0; i < SIZE * SIZE; i++) {
		A[i] = rand() % 2;
		B[i] = rand() % 2;
	}
}

int main(int argc, char **argv) {
	MPI_Init(NULL, NULL);
	
	int world_size;
	int world_rank;

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int stripe, begin, end;

	stripe = get_stripe(world_size);
	begin = world_rank * stripe;
	end = begin + stripe;

	if (!world_rank) {
		
		srand(time(0));

		int A = (int*) malloc(sizeof(int) * SIZE * SIZE);
		int B = (int*) malloc(sizeof(int) * SIZE * SIZE);

		init(A, B);

		for (int i = 0; i < world_size; i++) {
			MPI_Send(A[], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(B[], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
		}

	} else {
		int A = (int*) malloc(sizeof(int) * stripe * stripe);
		int B = (int*) malloc(sizeof(int) * stripe * stripe);
		MPI_Recv(A, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(B, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}





	mm(data[0], data[1], A, B);

	return 0;
}