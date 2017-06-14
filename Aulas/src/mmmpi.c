#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "util.h"

int t_num = 4;
int exit_sig = 1;

line line;

void mm() {
	int A = (int**) malloc(sizeof(int*) * SIZE);
	int B = (int**) malloc(sizeof(int*) * SIZE);
	int C = (int**) malloc(sizeof(int*) * SIZE);

	for (int i = 0; i < SIZE; i++) {
		A[i] = (int*) malloc(sizeof(int) * SIZE);
		B[i] = (int*) malloc(sizeof(int) * SIZE);
		C[i] = (int*) malloc(sizeof(int) * SIZE);
		for (int j = 0; j < SIZE; j++) {
			A[i][j] = rand() % 2;
			B[i][j] = rand() % 2;
			C[i][j] = 0;
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

void (*functions[])() = { mm };

void get_task() {
	lock();
	int svc = remove(&line)
	unlock();
	(*functions[svc])();
}

void manage(pthread_t *threads) {
	for (int i = 0; i < t_num; i++) {
		pthread_create(threads, NULL, (void) get_task, NULL);
	}

	while(exit_sig) {

	}
}

int main(int argc, char **argv) {
	pthread_t threads[t_num];
	initiate(&line);
	manage(&threads);
}