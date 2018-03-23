#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define usage() do{printf("One argument expected : (int) - number of children\n"); exit(EXIT_FAILURE);} while(0);

/* Une matrice sera vue comme un objet int*
On passera les dimensions des matrices en argument aux fonctions */

int* init_matrix(int n, int p)
/* Initialise une matrice de taille n*p remplie de 0 */
{
  int* matrix = (int*) mmap(NULL, sizeof(int) * n * p, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  memset(matrix, 0, sizeof(int) * n * p);
  return matrix;
}

void free_matrix(int *matrix, int n, int p)
/* Désalloue la matrice en paramètres */
{
  munmap(matrix, sizeof(int) * n * p);
  return;
}

void read_matrix(int *matrix, int n, int p)
/* Lis la matrice sur stdin */
{
  for (int i=0; i<n*p; i++)
    scanf("%d", &matrix[i]);
}

void print_matrix(int *matrix, int n, int p)
/* Affiche la matrice à l'écran */
{
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<p; j++)
      printf(" %5d ", matrix[i*n+j]);
    printf("\n");
  }
}

/* ----------------- LES CALCULS ------------------- */

/* Une matrice n×p est vue comme un tableau de taille n * p
Donc si on a k fils, on attribut au fils i le calcul du segment

[i*d, (i+1)*d[ où d = n * p / k

car :
(k * d) = n * p donc d = n * p / k

Les fils sont numérotés de 0 à k-1. Le dernier fils calcul de [(k-1)*d à max(n*p, k*d)[
pour être sûrs que tout est bien calculé ! */


typedef struct Mat_data Mat_data;
struct Mat_data
/* On va faire c = a × c */
{
  int *a;
  int *b;
  int *c; // matrice de résultat
  int n, p, m; // a sera de taille n*p, b de taille p*m et c de taille n*m
};

void free_Mat_data(Mat_data *mat)
{
  free_matrix(mat->a, mat->n, mat->p);
  free_matrix(mat->b, mat->p, mat->m);
  free_matrix(mat->c, mat->n, mat->m);

  free(mat);
  return;
}

typedef struct Data Data;
struct Data
/* La structure de données qu'on passera en argument aux threads */
/* Le thread calculera les coefficients sur [inf, sup[ */
{
  Mat_data* md;
  int inf;
  int sup;
};

void* compute_seg(void *arg)
{
  /* Quelques variables pour la lisibilité */
  Data data = * (Data*) arg;
  Mat_data mat = *(data.md);

  //printf("Thread with data : inf %d sup %d \n", data.inf, data.sup);

  for (int index=data.inf; index<data.sup; index++)
  /* On calcule les coefficients demandés */
  {
    mat.c[index] = 0;
    int i = index / mat.p;
    int j = index % mat.m;
    for (int k=0; k<mat.p; k++)
      mat.c[index] += mat.a[i*mat.p+k] * mat.b[k*mat.m+j];
  }

  return NULL;
}



int main(int argc, char* argv[])
/* On peut utiliser le deuxième argument pour demander de renvoyer le temps d'
exécution tronqué. */
{
  if (argc < 2)
    usage ();
  int n_threads = atoi(argv[1]);

  pthread_t* threads = malloc(n_threads * sizeof(pthread_t));
  Data* data_threads = malloc(n_threads * sizeof(Data));

  Mat_data* mat = malloc(sizeof(Mat_data));

  if (threads == NULL || data_threads == NULL || mat == NULL)
    perror("malloc");

  /* Lecture des entrées */
  int n_mat;
  scanf("%d", &n_mat);

  int n, p, m;
  scanf("%d %d", &n, &p);
  mat->n = n; mat->p = p;
  mat->a = init_matrix(n, p);
  read_matrix(mat->a, n, p);

  scanf("%d %d", &p, &m);
  mat->m = m;
  mat->b = init_matrix(p, m);
  read_matrix(mat->b, p, m);

  mat->c = init_matrix(mat->n, mat->m);

  /* Affichage de l'entrée */
  //print_matrix(mat->a, n, p);
  //print_matrix(mat->b, p, m);


  /* Calcul, lancement des threads  */

  struct timespec t0;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i=0; i<n_threads; i++)
    if (i != n_threads - 1)
    {
      data_threads[i].md = mat;
      data_threads[i].inf = (i * n * m) / n_threads;
      data_threads[i].sup = ((i+1) * n * m) / n_threads;
      pthread_create(&threads[i], NULL, &compute_seg, &data_threads[i]);
    }
    else
    {
      data_threads[i].md = mat;
      data_threads[i].inf = (i * n * m) / n_threads;
      data_threads[i].sup = n * m;
      pthread_create(&threads[i], NULL, &compute_seg, &data_threads[i]);
    }

  for (int i=0; i<n_threads; i++)
    pthread_join(threads[i], NULL);

  struct timespec t1;
  clock_gettime(CLOCK_MONOTONIC, &t1);
  double t = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1000000000.;

  /* Affichage */
  if (argc == 2)
    print_matrix(mat->c, mat->n, mat->m);
  else
    printf("%ld\n", (long int) (1000000* t));

  free_Mat_data(mat);
  free(threads);
  free(data_threads);

  return 0;
}
