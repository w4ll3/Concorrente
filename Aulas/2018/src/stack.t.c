#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

typedef struct node {
    int (*data) (int value);
    struct node *next;
} node_t;

node_t *top = NULL;

void push(int (*item) (int value)) {
    node_t *nptr = malloc(sizeof(node_t));
    nptr -> data = item;
    nptr -> next = top -> next;
    top = nptr;
}

node_t *pop(node_t *head) {
    node_t *temp;
    if (top != NULL) {
        temp = top;
        top = top -> next;
    }
    return temp;
}

int fact(int num) {
	return num == 2 ? 2 : num * fact(num - 1);
}

int fib(int x) {
    if (x == 1) {
        return 1;
    } else {
        return fib(x - 1) + fib(x - 2);
    }
}

/* void wait() {
	while(1) {
		while(cond)
			pthread_cond_wait();
	}
}
*/

int main(int argc, char *argv[]) {
	push(&fact);
	push(&fib);
	node_t *func = pop(top);
	printf("%d.\n", func -> data(10));
	func = pop(top);
	printf("%d.\n", func -> data(10));
}
