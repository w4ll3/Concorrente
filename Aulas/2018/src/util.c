#include "../include/util.h"

void freetrix(float **m_a, int size) {
	for (int i = 0; i < size; i++) {
		free(m_a[i]);
	}
	m_a = 0;
	free(m_a);
}

/*
 * Prints an error message and exits.
 */
void error(const char *msg) {
	fprintf(stderr, "error: %s\n", msg);
	exit(EXIT_FAILURE);
}

/*
 * Prints a warning message.
 */
void warning(const char *msg) {
	fprintf(stderr, "warning: %s\n", msg);
}

/*
 * Safe malloc().
 */
void *smalloc(size_t size) {
	void *p;
	
	p = malloc(size);
	
	/* Failed to allocate memory. */
	if (p == NULL)
		error("cannot malloc()");
	
	return (p);
}

/*
 * Safe calloc().
 */
void *scalloc(size_t nmemb, size_t size) {
	void *p;
	
	p = calloc(nmemb, size);
	
	/* Failed to allocate memory. */
	if (p == NULL)
		error("cannot calloc()");
	
	return (p);
}

/*
 * Safe realloc().
 */
void *srealloc(void *p, size_t size) {
	
	p = realloc(p, size);
	
	/* Failed to allocate memory. */
	if (p == NULL)
		error("cannot realloc()");
	
	return (p);
}


#define RANDNUM_W 521288629;
#define RANDNUM_Z 362436069;

static unsigned randum_w = RANDNUM_W;
static unsigned randum_z = RANDNUM_Z;

/*
 * Initializes the random number generator.
 */
void srandnum(int seed) {
	unsigned w, z;

	w = (seed * 104623) & 0xffffffff;
	randum_w = (w) ? w : RANDNUM_W;
	z = (seed * 48947) & 0xffffffff;
	randum_z = (z) ? z : RANDNUM_Z;
}

/*
 * Generates a random number.
 */
unsigned randnum(void) {
	unsigned u;
	
	/* 0 <= u < 2^32 */
	randum_z = 36969 * (randum_z & 65535) + (randum_z >> 16);
	randum_w = 18000 * (randum_w & 65535) + (randum_w >> 16);
	u = (randum_z << 16) + randum_w;
	
	return (u);
}

/*
 * Generates a normal number.
 */
double normalnum(double mu, double sigma) {
	double U1, U2, W, mult;
	static double X1, X2;
	static int call = 0;
 
	if (call == 1) {
		call = !call;
		return (mu + sigma * (double) X2);
	}
 
	do {
		U1 = -1 + ((double) randnum() / RAND_MAX) * 2;
		U2 = -1 + ((double) randnum() / RAND_MAX) * 2;
		W = pow (U1, 2) + pow (U2, 2);
	} while (W >= 1 || W == 0);
 
	mult = sqrt ((-2 * log (W)) / W);
	X1 = U1 * mult;
	X2 = U2 * mult;
 
	call = !call;
 
	return (mu + sigma * (double) X1);
}

/*
 * pRNG based on http://www.cs.wm.edu/~va/software/park/park.html
 *
 * Returns a pseudo-random real number uniformly distributed 
 * between 0.0 and 1.0. 
 */
#define MODULUS    2147483647
#define MULTIPLIER 48271

static long prngseed = 0;
double prngnum() {
  const long Q = MODULUS / MULTIPLIER;
  const long R = MODULUS % MULTIPLIER;
        long t;

  t = MULTIPLIER * (prngseed % Q) - R * (prngseed / Q);
  if (t > 0) 
    prngseed = t;
  else 
    prngseed = t + MODULUS;
  return ((double) prngseed / MODULUS);
}

void prng_set_seed(int seed) {
	prngseed = seed;
}

/* Timer residual error. */
static uint64_t timer_error = 0;

/*
 * Gets the current timer value.
 */
uint64_t timer_get(void) {
	uint64_t ret;
	struct timeval t;

	gettimeofday(&t, NULL);
	ret = 1000000 * ((uint64_t) t.tv_sec);
	ret += (uint64_t) t.tv_usec;

	return ret;
}

/*
 * Computers the difference between two timers.
 */
uint64_t timer_diff(uint64_t t1, uint64_t t2) {
	return (t2 - t1 - timer_error);
}

/*
 * Initializes the timer.
 */
void timer_init(void) {
  uint64_t start, end;
  
  start = timer_get();
  end = timer_get();
  
  timer_error = (end - start);
}

// Creates a vector.
struct vector *vector_create(int n) {
	struct vector *v;
	
	v = smalloc(sizeof(struct vector));
	
	/* Initilize vector. */
	v->size = n;
	v->elements = calloc(n, sizeof(float));

	return (v);
}

// Adds two vectors.
struct vector *vector_add(struct vector *v1, const struct vector *v2) {
	int i; /* Loop index.  */
	int n; /* Vector size. */
	
	n = vector_size(v1);
	
	/* Add vectors. */
	for (i = 0; i < n; i++)
		VECTOR(v1, i) += VECTOR(v2, i);
	
	return (v1);
}

// Assigns a vector to another.
struct vector *vector_assign(struct vector *v1, const struct vector *v2) {
	int i; /* Loop index.  */
	int n; /* Vector size. */
	
	n = vector_size(v1);
	
	/* Add vectors. */
	for (i = 0; i < n; i++)
		VECTOR(v1, i) = VECTOR(v2, i);
	
	return (v1);
}

// Destroys a vector.
void vector_destroy(struct vector *v) {
	free(v->elements);
	free(v);
}

// Computes the euclidean distance between two points.
float vector_distance(const struct vector *a, const struct vector *b) {
	int i;          /* Loop index. */
	float distance; /* Distance.   */

	distance = 0;

	/* Computes the euclidean distance. */
	for (i = 0; i < a->size; i++)
		distance +=  pow(VECTOR(a, i) - VECTOR(b, i), 2);
	distance = sqrt(distance);
	
	return (distance);
}

// Tests if two vectors are equal.
int vector_equal(const struct vector *a, const struct vector *b) {
	int i;
	
	/* Test all elements. */
	for (i = 0; i < a->size; i++) {
		if (VECTOR(a, i) != VECTOR(b, i))
			return (0);
	}
	
	return (1);
}

// Multiplies a vector by a scalar.
struct vector *vector_mult(struct vector *v, float scalar) {
	int i; /* Loop index.  */
	int n; /* Vector size. */
	
	n = vector_size(v);
	
	/* Add vectors. */
	for (i = 0; i < n; i++)
		VECTOR(v, i) *= scalar;
	
	return (v);
}

// Fills up vector with random numbers.
struct vector *vector_random(struct vector *v) {
	int i;
	
	/* Fill vector. */
	for (i = 0; i < vector_size(v); i++)
		VECTOR(v, i) = randnum() & 0xffff;

	return (v);
}

// Subtracts two vectors.
struct vector *vector_sub(struct vector *v1,const struct vector *v2) {
	int i; /* Loop index.  */
	int n; /* Vector size. */
	
	/* Invalid argument. */
	if (vector_size(v1) != vector_size(v2))
		return (NULL);
	
	n = vector_size(v1);
	
	/* Subtract vectors. */
	for (i = 0; i < n; i++)
		VECTOR(v1, i) -= VECTOR(v2, i);
	
	return (v1);
}

// Clears a vector.
void vector_clear(struct vector *v) {
	memset(v->elements, 0, sizeof(float)*vector_size(v));	
}