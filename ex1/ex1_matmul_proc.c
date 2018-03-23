#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define usage() do{printf("One argument expected : (int) - number of children\n"); exit(EXIT_FAILURE);} while(0);

/* Comme les TD-man l'ont fait remarqué,
pour répartir le travail sur les différentes lignes de la matrices entre différents
processus, il est plus simple de se ramener à des tableaux unidimensionnels.

De plus, on va utiliser `mmap` donc bon... je recode tout. */


/* Une matrice sera vue comme un objet int*
On passera les dimensions des matrices en arguments aux fonctions */

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

void mul_over(int *a, int *b, int *c,
             int n,  int m,  int p,
             int inf, int sup)
/* Calcule les coefficients [int, sup[ de a × b dans c */
{
  for (int l=inf; l<sup; l++)
  {
    int i = l/m;
    int j = l%p;
    c[l] = 0;
    for (int k=0; k<m; k++)
      c[l] += a[i*m+k] * b[k*p+j]; // somme des a_i_k × b_k_j
  }
}


int main(int argc, char* argv[])
{
  if (argc < 2)
    usage ();
  int n_threads = atoi(argv[1]);

  /* ------ Lecture des entrées ------ */

  int n_mat;
  scanf("%d", &n_mat); // on lit le nombre de matrices

  int n_a, p_a, n_b, p_b;
  int *a, *b;
  scanf("%d %d", &n_a, &p_a);
  a = init_matrix(n_a, p_a);
  read_matrix(a, n_a, p_a);

  //printf("a:\n");
  //print_matrix(a, n_a, p_a);

  scanf("%d %d", &n_b, &p_b);
  b = init_matrix(n_b, p_b);
  read_matrix(b, n_b, p_b);

  //printf("\nb:\n");
  //print_matrix(b, n_b, p_b);

  int *c = init_matrix(n_a, p_b);


  /* ----------- Lancement des fils ----------- */
  int i = 0;
  int* status = malloc(n_threads * sizeof(int));

  struct timespec t0;
  clock_gettime(CLOCK_MONOTONIC, &t0);

  while (i < n_threads && (status[i] = fork()))
    i ++;

  /* ----------- Calculs ------------- */
  if (i != n_threads) // Si on a un fils
  {
    if (i != n_threads - 1)
    {
      //printf("Hello je m'occupe de %d à %d\n", (i * n_a * p_b) / n_threads, ((i+1) * n_a * p_b) / n_threads - 1);
      mul_over(a, b, c, n_a, p_a, p_b, (i * n_a * p_b) / n_threads, ((i+1) * n_a * p_b) / n_threads);
    }
    else
    {
      //printf("Hello je m'occupe de %d à %d\n", (i * n_a * p_b) / n_threads, n_a*p_b-1);
      mul_over(a, b, c, n_a, p_a, p_b, (i * n_a * p_b) / n_threads, n_a*p_b);
    }
  }
  else // ici c'est le père
  {
    for (int i=0; i<n_threads; i++)
      wait(status + i);

    struct timespec t1;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double t = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1000000000.;

      //printf("\na×b:\n");
    if (argc == 2)
      print_matrix(c, n_a, p_b);
    else
      printf("%ld\n", (long int) (1000000* t));


    free_matrix(a, n_a, p_a);
    free_matrix(b, n_b, p_b);

  }
  return 0;
}
