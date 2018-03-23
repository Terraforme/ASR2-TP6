#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

long N, P;
int *a;
long res = 0;
pthread_mutex_t mutex;

void *prodvec(void *j) {
	long pos = (long) j;
	for (long i = (N/P)*pos; i < (N/P)*(pos+1); i++) {
		pthread_mutex_lock(&mutex);
		res += a[i];
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	void *status;

	if (argc != 3)
		fprintf(stderr, "Usage %s/ N P \n", argv[0]);

	N = atoi(argv[1]);
	P = atoi(argv[2]);
	pthread_t threads[P];

	// Initialisation du mutex
	pthread_mutex_init(&mutex, NULL);

	if (N%P != 0)
		fprintf(stderr, "N mod P != 0, not possible\n");

	a = (int*) malloc (N*sizeof(int));
	for (long i=0; i<N; i++)
	  a[i] = 1;

	double t1 = (double)clock();

	for(long i=0; i<P; i++) {
	   pthread_create(&threads[i], NULL, prodvec, (void*) i);
	}

	for(long i=0; i<P; i++)
	  pthread_join(threads[i], &status);

	double t2 = (double)clock();

	printf ("Sum=%li (in %f sec)\n",res, (t2-t1)/CLOCKS_PER_SEC);
	free (a);

	pthread_exit(NULL);
}
