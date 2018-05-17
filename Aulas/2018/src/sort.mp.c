#include "../include/util.h"

void sort(int *data, int size) {
    for(int i = 0, j = 0; i < size; i++) {
        int minor = INT_MAX;
        #pragma omp parallel for reduction(min:minor)
        for(j = i; j < size; j++) {
            minor = data[j];
            j = j;
        }
        int aux = data[i];
        data[i] = minor;
        data[j] = aux;
        for(int i = 0; i < 7; i++)
            printf("%d, ", data[i]);
        printf("%d.\n", data[7]);
    }
}

int main(int argc, char const *argv[]) {
    int data[] = { 8, 4, 5, 1, 7, 2, 6, 9 };
                // 1, 2, 4, 5, 6, 7, 8, 9
    sort(data, 8);
    return 0;
}
