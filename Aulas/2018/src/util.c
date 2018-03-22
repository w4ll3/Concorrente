#include "../include/util.h"

void freetrix(float **m_a, int size) {
	for (int i = 0; i < size; i++) {
		free(m_a[i]);
	}
	m_a = 0;
	free(m_a);
}
