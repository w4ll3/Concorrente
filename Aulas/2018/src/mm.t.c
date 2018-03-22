#include "../include/util.h"

float **m_a, **m_b, **m_c;
float real_time, proc_time, mflops;
long long flpins;
int values[5];

void init() {
	if(values[1] != values[2]) {
		printf("O número de linhas da matriz B necessita ser igual ao número de colunas da matriz A\n");
		exit(EXIT_FAILURE);
	} else if(values[4] < 2) {
		printf("Por favor digite um número maior que um para threads.\n");
		exit(EXIT_FAILURE);
	}
	m_a = (float**) malloc(sizeof(float*) * values[0]);
	m_b = (float**) malloc(sizeof(float*) * values[2]);
	m_c = (float**) malloc(sizeof(float*) * values[0]);
	if(values[0] != values[1] || values[2] != values[3]) {
		for(int i = 0; i < values[1]; i++) {
			m_a[i] = (float*) malloc(sizeof(float) * values[1]);
			for(int j = 0; j < values[1]; j++)
				m_a[i][j] = rand() % 100 / rand() % 100 + 1;
		}
		for(int i = 0; i < values[3]; i++) {
			m_b[i] = (float*) malloc(sizeof(float) * values[3]);
			for(int j = 0; j < values[1]; j++)
				m_b[i][j] = rand() % 100 / rand() % 100 + 1;
		}
		for(int i = 0; i < values[0]; i++) {
			m_c[i] = (float*) malloc(sizeof(float) * values[3]);
			for(int j = 0; j < values[3]; j++)
				m_c[i][j] = 0;
		}
	} else {
		for(int i = 0; i < values[1]; i++) {
			m_a[i] = (float*) malloc(sizeof(float) * values[1]);
			m_b[i] = (float*) malloc(sizeof(float) * values[3]);
			m_c[i] = (float*) malloc(sizeof(float) * values[3]);
			for(int j = 0; j < values[1]; j++) {
				m_a[i][j] = rand() % 1000 / (rand() % 100 + 1);
				m_b[i][j] = rand() % 1000 / (rand() % 100 + 1);
				m_c[i][j] = 0;
			}
		}
	}
}

void *mm(void *args) {
	int id = *((int*) args);
	int stripe = ceil(values[0] / values[4]);
	int begin = id * stripe;
	int end = begin + stripe;
	for(int i = begin; i < end; i++)
		for(int j = 0; j < values[0]; j++)
			for(int k = 0; k < values[3]; k++)
				m_c[i][j] += m_a[i][k] * m_b[k][j];
}

int main(int argc, char *argv[]) {
	// PAPI_library_init(PAPI_VER_CURRENT);
	// PAPI_thread_init(pthread_self);
	srand(time(NULL));
	int c;
	struct option long_args[] = {
		{"la", required_argument, NULL, 'a'},
		{"ca", required_argument, NULL, 'b'},
		{"lb", required_argument, NULL, 'c'},
		{"cb", required_argument, NULL, 'd'},
		{"threads",  required_argument, NULL, 't'}
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
				\n\tNúmero de colunas da matriz B (--cb | -d). \
				\n\tNúmero de threads (--threads | -t).\n");
			} exit(EXIT_FAILURE);
		}
	} while(c != -1);
	init();
	TIME()
	pthread_t *threads = calloc(values[4], sizeof(pthread_t));
	int *ids = (int*) calloc(sizeof(int) * values[4], 0);
	for(int i = 0; i < values[4]; i++, (ids[i] = i))
		pthread_create(&threads[i], NULL, (void*) mm, (void*) &ids[i]);
	for(int i = 0; i < values[4]; i++)
		pthread_join(threads[i], NULL);
	ENDTIME()
	printf("%d\n", elapsed);
	// printf("time: %f | proc_time: %f, mflops: %f, flpins: %lld\n", real_time, proc_time, mflops, flpins);
	exit(EXIT_SUCCESS);
}
