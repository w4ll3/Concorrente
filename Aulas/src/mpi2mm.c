/**
 * 2mm.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */

#include "util.h"

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "2mm.h"

/* Global arguments */
int ni, nj, nk, nl, T = 1;
double g_alpha, g_beta;
double *g_tmp;
double *g_A;
double *g_B;
double *g_C;
double *g_D;

/* Create barrier */
pthread_barrier_t barrier;
sem_t mutex;

/* Array initialization. */
static void init_array(double POLYBENCH_2D(A,NI,NK,ni,nl), double POLYBENCH_2D(B,NK,NJ,nk,nj), double POLYBENCH_2D(C,NL,NJ,nl,nj), double POLYBENCH_2D(D,NI,NL,ni,nl), double POLYBENCH_2D(tmp,NI,NJ,ni,nj)) {
	g_alpha = 32412;
	g_beta = 2123;
	g_A = (double *) malloc(sizeof(double) * NI * NI);
	g_B = (double *) malloc(sizeof(double) * NI * NI);
	g_C = (double *) malloc(sizeof(double) * NI * NI);
	g_D = (double *) malloc(sizeof(double) * NI * NI);
	g_tmp = (double *) malloc(sizeof(double) * NI * NI);
	for (int i = 0; i < ni; i++) {
		for (int j = 0; j < nk; j++) {
			A[i][j] = ((double) i*j) / ni;
			g_A[i + j] = A[i][j];
		}
	}
	for (int i = 0; i < nk; i++) {
		for (int j = 0; j < nj; j++) {
			B[i][j] = ((double) i*(j+1)) / nj;
			g_B[i + j] = B[i][j];
		}
	}
	for (int i = 0; i < nl; i++) {
		for (int j = 0; j < nj; j++) {
			C[i][j] = ((double) i*(j+3)) / nl;
			g_C[i + j] = C[i][j];
		}
	}
	for (int i = 0; i < ni; i++) {
		for (int j = 0; j < nl; j++) {
			D[i][j] = ((double) i*(j+2)) / nk;
			g_D[i + j] = D[i][j];
		}
	}
	for (int i = 0; i < ni; i++) {
		for (int j = 0; j < nj; j++) {
			g_tmp[i + j] = tmp[i][j];
		}
	}
}


void kernel_2mm_pthreads() {
	#ifdef PAPI
	PAPI_THREAD_INIT()
	#endif

	int init = get_init(*id, stripe);
	int end = get_end(init, stripe);

	for (int i = init; i < end; i++) {
		for (int j = 0; j < NJ; j++) {
			g_tmp[i][j] = 0;
			for (int k = 0; k < NK; ++k) {
				g_tmp[i][j] += g_alpha * g_A[i][k] * g_B[k][j];
			}
		}
	}
	//send(g_tmp)
	//generate whole tmp
	//recv(g_tmp)
	pthread_barrier_wait(&barrier);
	for (int i = init; i < end; i++) {
		for (int j = 0; j < NJ; j++) {
			g_D[i][j] *= g_beta;
			for (int k = 0; k < NJ; ++k) {
				g_D[i][j] += g_tmp[i][k] * g_C[k][j];
			}
		}
	}

	#ifdef PAPI
	sem_wait(&mutex);
	PAPI_UPDATE()
	sem_post(&mutex);
	#endif

	return NULL;
}

/* Main computational kernel. The whole function will be timed, including the call and return. */
/* Original sequential code */
void kernel_2mm() {
	/* D := alpha*A*B*C + beta*D */
	/* A[i][j] */
	/* i -> line */
	/* j -> column */

	for (int i = 0; i < ni; i++) {
		for (int j = 0; j < nj; j++) {
			g_tmp[i][j] = 0;
			for (int k = 0; k < nk; ++k) {
				g_tmp[i][j] += g_alpha * g_A[i][k] * g_B[k][j];
			}
		}
	}
	for(int i = 0; i < ni; i++) {
		for (int j = 0; j < nl; j++) {
			g_D[i][j] *= g_beta;
			for (int k = 0; k < nj; ++k) {
				g_D[i][j] += g_tmp[i][k] * g_C[k][j];
			}
		}
	}
}

int main(int argc, char** argv) {
	/* Retrieve which code to run */
	int prog = atoi(argv[2]);

	/* Retrieve problem size. */
	ni = NI;
	nj = NJ;
	nk = NK;
	nl = NL;

	/* Number of threads */
	T = atoi(argv[1]);

	/* Variable declaration/allocation. */
	POLYBENCH_2D_ARRAY_DECL(tmp,double,NI,NJ,ni,nj);
	POLYBENCH_2D_ARRAY_DECL(A,double,NI,NK,ni,nk);
	POLYBENCH_2D_ARRAY_DECL(B,double,NK,NJ,nk,nj);
	POLYBENCH_2D_ARRAY_DECL(C,double,NL,NJ,nl,nj);
	POLYBENCH_2D_ARRAY_DECL(D,double,NI,NL,ni,nl);

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
		// Initialize array(s).
		init_array(POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B), POLYBENCH_ARRAY(C), POLYBENCH_ARRAY(D), POLYBENCH_ARRAY(tmp));

		for (int i = 0; i < world_size; i++) {
			MPI_Send(g_a + (i * stripe), stripe, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(g_b + (i * stripe), stripe, MPI_INT, i, 1, MPI_COMM_WORLD);
		}

	} else {
		g_A = (int*) malloc(sizeof(int) * stripe);
		g_B = (int*) malloc(sizeof(int) * stripe);
		MPI_Recv(A, stripe, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(B, stripe, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}


	#ifdef PAPI
	ENDPAPI()
	#endif

	/* Be clean. */
	POLYBENCH_FREE_ARRAY(tmp);
	POLYBENCH_FREE_ARRAY(A);
	POLYBENCH_FREE_ARRAY(B);
	POLYBENCH_FREE_ARRAY(C);
	POLYBENCH_FREE_ARRAY(D);

	return 0;
}
