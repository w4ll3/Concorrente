#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void *sen3(void *args) {
  float *x = (float *) args;
  x[1] = pow(sin(x[0]), 3);
}

void sqrcos(void *args) {
  float *x = (float *) args;
  x[2] = sqrt(cos(x[0]));
}

int main(int argc, char *argv[]) {
  pthread_t threads[2];
  float values[3];
  printf("Digite o valor de x: ");
  scanf("%f", &values[0]);
  printf("\nO valor de x é %f.\n", values[0]);
  pthread_create(&threads[0], NULL, (void *) sen3, values);
  pthread_create(&threads[1], NULL, (void *) sqrcos, values);
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  printf("%f. %f. \n", values[1] + values[2], pow(sin(values[0]), 3) + sqrt(cos(values[0])));
}
