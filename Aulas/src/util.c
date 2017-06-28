#include "util.h"

#ifdef PAPI
int event_set[] = { PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_FP_INS, PAPI_TOT_CYC, PAPI_TOT_INS };
long long fcounter[PAPI_SIZE] = { 0 };
long long counters[PAPI_SIZE];

#define PAPI_UPDATE() \
	PAPI_read_counters(counters, PAPI_SIZE); \
	for (int i = 0; i < PAPI_SIZE; i++) \
		fcounter[i] += counters[i];

#define START_PAPI() \
	PAPI_library_init(PAPI_VER_CURRENT); \
	PAPI_start_counters(event_set, PAPI_SIZE);
	

#define PAPI_THREAD_INIT() \
	PAPI_register_thread(); \
	PAPI_start_counters(event_set, PAPI_SIZE);

#define ENDPAPI() \
	PAPI_read_counters(fcounter, PAPI_SIZE); \
	for (int i = 0; i < PAPI_SIZE; i++) \
		printf("%lld\n", fcounter[i]);

#endif
	
static unsigned long next = 1;

int mrand(void) {
	next = next * 1103515245 + 12345;
	return ((unsigned)(next / 65536) % 32768);
}

void msrand(unsigned seed) { next = seed; }

int get_stripe(int tnum) {
	return (int) ceil(((float) SIZE) / tnum);
}

int get_init(int id, int stripe) {
	return id * stripe;
}

int get_end(int init, int stripe) {
	return MIN(init + stripe, SIZE);
}

void start_pthread(pthread_t *thread, int threads, void *(*parallel_pthread)(void*)) {
	int ids[threads];
	for (int i = 0; i < threads; i++) {
		ids[i] = i;
		pthread_create(&thread[i], NULL, (void*) parallel_pthread, (void *) (ids + i));
	}
	for (int i = 0; i < threads; i++) {
		pthread_join(thread[i], NULL);
	}
}

void print(TYPEDEF **matrix) {
	printf("%s\n", "");
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			#ifdef INT
			printf("%3d ", matrix[i][j]);
			#else
			printf("%4.1f ", matrix[i][j]);
			#endif

		}
		printf("%s\n", "");
	}
	printf("%s\n", "");
}

void reset(TYPEDEF **A) {
	for (int j = 0; j < SIZE; j++) {
		for (int k = 0; k < SIZE; k++) {
			A[j][k] = 0;
		}
	}
}

void freetrix(TYPEDEF **A) {
	for (int i = 0; i < SIZE; i++) {
		free(A[i]);
	}
	A = 0;
	free(A);
}

void minit(TYPEDEF ***A, int fact) {
	msrand(2);
	(*A) = (TYPEDEF**) malloc(sizeof(TYPEDEF*) * SIZE);
	for (int i = 0; i < SIZE; i++) {
		(*A)[i] = (TYPEDEF*) malloc(sizeof(TYPEDEF) * SIZE);
		for (int j = 0; j < SIZE; j++) {
			(*A)[i][j] = mrand() % fact + 1;
		}
	}
}

void zinit(TYPEDEF ***A) {
	srand(time(0));
	(*A) = (TYPEDEF**) malloc(sizeof(TYPEDEF*) * SIZE);
	for (int i = 0; i < SIZE; i++) {
		(*A)[i] = (TYPEDEF*) malloc(sizeof(TYPEDEF) * SIZE);
		for (int j = 0; j < SIZE; j++) {
			(*A)[i][j] = 0;
		}
	}
}