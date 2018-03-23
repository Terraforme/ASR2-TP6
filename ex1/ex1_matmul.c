#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Une matrice sera considérée comme étant un objet */
/* Je demande à ce que l'entrée du programme soit sur stdin */

typedef struct matrix matrix;
struct matrix
{
  int ** mat;
  int n;
  int p;
};

matrix* init_matrix(int n, int p);
matrix* mul_matrix(matrix* a, matrix* b);
void free_matrix(matrix* a);
void read_matrix(matrix *a);
void print_matrix(matrix *a);


matrix* init_matrix(int n, int p)
/* pour initialiser une matrice */
{
  matrix * m = NULL;
  m = malloc(sizeof(matrix));

  if (m == NULL)
    perror("malloc");

  m->n = n;
  m->p = p;
  m->mat = malloc(n * sizeof(int*));
  for (int i = 0; i<n; i++)
    m->mat[i] = malloc(p*sizeof(int));

  return m;
}

matrix* mul_matrix(matrix* a, matrix* b)
/* Pour multipler deux matrices : on doit vérifier que celles-ci ont des tailles cohérentes */
{
  matrix *c = NULL;

  c = init_matrix(a->n, b->p);

  for (int i=0; i<c->n; i++)
    for (int j=0; j<c->p; j++)
    {
      c->mat[i][j] = 0;
      for (int k=0; k<a->p; k++)
        c->mat[i][j] += a->mat[i][k] * b->mat[k][j];
    }

  return c;
}

void free_matrix(matrix* a)
/* Une fonction pour libérer une matrice */
{
  for (int i = 0; i<a->n; i++)
    free(a->mat[i]);
  free(a->mat);

  free(a);
}

void read_matrix(matrix *a)
/* Lit une matrice depuis stdin : ne gère pas les corruptions de fichiers / fichiers pas aux normes */
{
  for (int i=0; i<a->n; i++)
    for (int j=0; j<a->p; j++)
      scanf("%d", &(a->mat[i][j]));
}

void print_matrix(matrix *a)
{
  for (int i=0; i<a->n; i++)
  {
    for (int j=0; j<a->p; j++)
      printf(" %5.d ", a->mat[i][j]);
    printf("\n");
  }
}


int main(void)
{
  int n_mat;
  scanf("%d\n", &n_mat);

  /* Par chance, on sait qu'on se restreint à 2 matrices */
  int n, p;
  matrix *a, *b, *c;
  scanf("%d %d\n", &n, &p);
  a = init_matrix(n, p);
  read_matrix(a);

  scanf("%d %d\n", &n, &p);
  b = init_matrix(n, p);
  read_matrix(b);

  //print_matrix(a);
  //print_matrix(b);

  c = mul_matrix(a, b);
  print_matrix(c);

  free_matrix(a);
  free_matrix(b);
  free_matrix(c);

  return 0;
}
