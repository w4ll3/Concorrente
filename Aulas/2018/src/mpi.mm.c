#include "../include/util.h"

float **m_a, **m_b, **m_c;
float real_time, proc_time, mflops;
long long flpins;
int values[5];
int world_size;
int world_rank;
int chunk_i;
int chunk_j;

void init() {
	if(values[1] != values[2]) {
		printf("O número de linhas da matriz B necessita ser igual ao número de colunas da matriz A\n");
		exit(EXIT_FAILURE);
	}
	m_a = (float**) malloc(sizeof(float*) * values[0]);
	float *d_a = (float*) malloc(sizeof(float) * values[0] * values[1]);
	m_b = (float**) malloc(sizeof(float*) * values[2]);
	float *d_b = (float*) malloc(sizeof(float) * values[2] * values[3]);
	m_c = (float**) malloc(sizeof(float*) * values[0]);
	float *d_c = (float*) malloc(sizeof(float) * values[0] * values[3]);
	if(!world_rank) {
		if(values[0] != values[1] || values[2] != values[3]) {
			for(int i = 0; i < values[1]; i++) {
				m_a[i] = &(d_a[values[1] * i]);
				for(int j = 0; j < values[1]; j++)
					m_a[i][j] = rand() % 100 / rand() % 100 + 1;
			}
			for(int i = 0; i < values[3]; i++) {
				m_b[i] = &(d_b[values[3] * i]);
				for(int j = 0; j < values[1]; j++)
					m_b[i][j] = rand() % 100 / rand() % 100 + 1;
			}
			for(int i = 0; i < values[0]; i++) {
				m_c[i] = &(d_c[values[3] * i]);
				for(int j = 0; j < values[3]; j++)
					m_c[i][j] = 0;
			}
		} else {
			for(int i = 0; i < values[1]; i++) {
				m_a[i] = &(d_a[values[1] * i]);
				m_b[i] = &(d_b[values[3] * i]);
				m_c[i] = &(d_c[values[3] * i]);
				for(int j = 0; j < values[1]; j++) {
					m_a[i][j] = rand() % 1000 / (rand() % 100 + 1);
					m_b[i][j] = rand() % 1000 / (rand() % 100 + 1);
					m_c[i][j] = 0;
				}
			}
		}
	}
}

void mm() {
	for(int i = 0; i < chunk_i; i++)
		for(int j = 0; j < chunk_j; j++)
			for(int k = 0; k < chunk_j; k++)
				m_c[i][j] += m_a[i][k] * m_b[k][j];
}

int main(int argc, char *argv[]) {
	// PAPI_library_init(PAPI_VER_CURRENT);
	// PAPI_thread_init(pthread_self);
	srand(time(NULL));

    //MPI Stuff
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int c;
	struct option long_args[] = {
		{"la", required_argument, NULL, 'a'},
		{"ca", required_argument, NULL, 'b'},
		{"lb", required_argument, NULL, 'c'},
		{"cb", required_argument, NULL, 'd'},
		{"threads",  optional_argument, NULL, 't'}
	};
	do {
		c = getopt_long(argc, argv, "a:b:c:d:t:", long_args, NULL);
		switch(c) {
			case -1:
			case 0: break;
			case 'a': {
				values[0] = atoi(optarg);
			} break;
			case 'b': {
				values[1] = atoi(optarg);
			} break;
			case 'c': {
				values[2] = atoi(optarg);
			} break;
			case 'd': {
				values[3] = atoi(optarg);
			} break;
			case 't': {
				values[4] = atoi(optarg);
			} break;
			default: {
				printf("Os seguintes paramêtros são obrigatório para a execução do programa: \
				\n\tNúmero de linhas da matriz A (--la | -a). \
				\n\tNúmero de colunas da matriz A (--ca | -b). \
				\n\tNúmero de linhas da matriz B (--lb | -c). \
				\n\tNúmero de colunas da matriz BIT (--cb | -d). \
				\n\tNúmero de threads (--threads | -t).\n");
			} exit(EXIT_FAILURE);
		}
	} while(c != -1);
	
	init();

	MPI_Bcast(&(m_a[0][0]), (values[0] / world_size) * (values[3] / world_size), MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(m_b[0][0]), (values[0] / world_size) * (values[3] / world_size), MPI_FLOAT, 0, MPI_COMM_WORLD);

	chunk_i = values[0] / world_size;
	chunk_j = values[3] / world_size;

	TIME()
    if (world_rank == 0) {
		mm();
        for (int i = 1; i < world_size; i++)
            MPI_Recv(&(m_c[i * chunk_i][i * chunk_j]), chunk_i * chunk_j, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else MPI_Send(&(m_c[0][0]), chunk_i * chunk_j, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
	ENDTIME()
	// http://din.uem.br/~anderson/PC.T2.pdf
	MPI_Finalize();
}
