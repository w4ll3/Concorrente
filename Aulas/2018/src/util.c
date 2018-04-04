#include "../include/util.h"

void freetrix(float **m_a, int size) {
	for (int i = 0; i < size; i++) {
		free(m_a[i]);
	}
	m_a = 0;
	free(m_a);
}

void rantrix(double **m, int size, int max) {
	m = (double**) malloc(sizeof(double*) * size);
	for(int i = 0; i < size; i++) {
		m[i] = (double*) malloc(sizeof(double) * size);
		for(int j = 0; j < size; j++)
			m[i][j] = (double) (rand() % max);
	}
}