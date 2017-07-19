/* Include util header */
#include "util.h"

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "2mm.h"

/* Include polybench common header. */
#include <polybench.h>

/* Include MPI */
#include "mpi.h"

/* Global arguments */
int ni, nj, nk, nl, T = 1;
double g_alpha = 32412, g_beta = 2123;
double *g_tmp;
double *g_A;
double *g_B;
double *g_C;
double *g_D;

/* Array initialization. */
static void init_array(double POLYBENCH_2D(A,NI,NK,ni,nl), double POLYBENCH_2D(B,NK,NJ,nk,nj), double POLYBENCH_2D(C,NL,NJ,nl,nj), double POLYBENCH_2D(D,NI,NL,ni,nl), double POLYBENCH_2D(tmp,NI,NJ,ni,nj)) {
	g_A = (double *) malloc(sizeof(double) * SIZE * SIZE);
	g_B = (double *) malloc(sizeof(double) * SIZE * SIZE);
	g_C = (double *) malloc(sizeof(double) * SIZE * SIZE);
	g_D = (double *) malloc(sizeof(double) * SIZE * SIZE);
	g_tmp = (double *) malloc(sizeof(double) * SIZE * SIZE);

	for (int i = 0; i < ni; i++) {
		for (int j = 0; j < nk; j++) {
			A[i][j] = ((double) i*j) / ni;
			g_A[i * SIZE + j] = A[i][j];
		}
	}
	for (int i = 0; i < nk; i++) {
		for (int j = 0; j < nj; j++) {
			B[i][j] = ((double) i*(j+1)) / nj;
			g_B[i * SIZE + j] = B[i][j];
		}
	}
	for (int i = 0; i < nl; i++) {
		for (int j = 0; j < nj; j++) {
			C[i][j] = ((double) i*(j+3)) / nl;
			g_C[i * SIZE + j] = C[i][j];
		}
	}
	for (int i = 0; i < ni; i++) {
		for (int j = 0; j < nl; j++) {
			D[i][j] = ((double) i*(j+2)) / nk;
			g_D[i * SIZE + j] = D[i][j];
		}
	}
	for (int i = 0; i < ni; i++) {
		for (int j = 0; j < nj; j++) {
			g_tmp[i * SIZE + j] = tmp[i][j];
		}
	}
}


void kernel_2mm(int id, int stripe, int world_size) {

	int init = id * stripe;
	int end = init + stripe;

	for (int i = init; i < end; i++) {
		for (int j = 0; j < NJ; j++) {
			g_tmp[i * SIZE + j] = 0;
			for (int k = 0; k < NK; ++k) {
				g_tmp[i * SIZE + j] += g_alpha * g_A[i * SIZE + k] * g_B[k * SIZE + j];
			}
		}
	}
	for (int i = init; i < end; i++) {
		for (int j = 0; j < NJ; j++) {
			g_D[i * SIZE + j] *= g_beta;
			for (int k = 0; k < NJ; ++k) {
				g_D[i * SIZE + j] += g_tmp[i * SIZE + k] * g_C[k * SIZE + j];
			}
		}
	}
	if(id != 0) MPI_Send(g_D + (id * stripe * SIZE), stripe * SIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);

}

int main(int argc, char** argv) {
	/* Retrieve problem size. */
	ni = NI;
	nj = NJ;
	nk = NK;
	nl = NL;
	
	MPI_Init(NULL, NULL);
	
	int world_size;
	int world_rank;

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int stripe = get_stripe(world_size);


	if (!world_rank) {

		/* Variable declaration/allocation. */
		POLYBENCH_2D_ARRAY_DECL(tmp,double,NI,NJ,ni,nj);
		POLYBENCH_2D_ARRAY_DECL(A,double,NI,NK,ni,nk);
		POLYBENCH_2D_ARRAY_DECL(B,double,NK,NJ,nk,nj);
		POLYBENCH_2D_ARRAY_DECL(C,double,NL,NJ,nl,nj);
		POLYBENCH_2D_ARRAY_DECL(D,double,NI,NL,ni,nl);

		init_array(POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B), POLYBENCH_ARRAY(C), POLYBENCH_ARRAY(D), POLYBENCH_ARRAY(tmp));

		for (int i = 1; i < world_size; i++) {
			MPI_Send(g_A + (i * stripe * SIZE), stripe * SIZE, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
			MPI_Send(g_B, SIZE * SIZE, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
			MPI_Send(g_C, SIZE * SIZE, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
			MPI_Send(g_D + (i * stripe * SIZE), stripe * SIZE, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
		}

	} else {
		g_A = (double *) malloc(sizeof(double) * SIZE * SIZE);
		g_B = (double *) malloc(sizeof(double) * SIZE * SIZE);
		g_C = (double *) malloc(sizeof(double) * SIZE * SIZE);
		g_D = (double *) malloc(sizeof(double) * SIZE * SIZE);
		g_tmp = (double *) malloc(sizeof(double) * SIZE * SIZE);
		MPI_Recv(g_A + (world_rank * stripe * SIZE), stripe * SIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(g_B, SIZE * SIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(g_C, SIZE * SIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(g_D + (world_rank * stripe * SIZE), stripe * SIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	kernel_2mm(world_rank, stripe, world_size);

	if (world_rank == 0) {
		for (int i = 1; i < world_size; i++) {
			MPI_Recv(g_D + (i * stripe * SIZE), stripe * SIZE, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	MPI_Finalize();

	return 0;
}
