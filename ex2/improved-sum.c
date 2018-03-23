#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

long N, P;
int *a;
long res = 0;

void *prodvec(void *j) {
	long pos = (long) j;
	long long temp = 0;
	for (long i = (N/P)*pos; i < (N/P)*(pos+1); i++) {
		temp += a[i];
	}
	return (void*) temp;
}

int main(int argc, char *argv[]) {
	void *status;

	if (argc != 3)
		fprintf(stderr, "Usage %s/ N P \n", argv[0]);

	N = atoi(argv[1]);
	P = atoi(argv[2]);
	pthread_t threads[P];


	if (N%P != 0)
		fprintf(stderr, "N mod P != 0, not possible\n");

	a = (int*) malloc (N*sizeof(int));
	for (long i=0; i<N; i++)
	  a[i] = 1;

	struct timespec t0;
	clock_gettime(CLOCK_MONOTONIC, &t0);

	for(long i=0; i<P; i++) {
	   pthread_create(&threads[i], NULL, prodvec, (void*) i);
	}

	for(long i=0; i<P; i++)
	{
		pthread_join(threads[i], &status);
		res += (long long) status;
	}

	struct timespec t1;
	clock_gettime(CLOCK_MONOTONIC, &t1);

	double t = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 10000000000.;

	printf ("Sum=%li (in %f sec)\n",res, t);
	free (a);

	pthread_exit(NULL);
}
