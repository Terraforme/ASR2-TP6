#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define usage() do{ printf("2 arg needed : n size of matrix, k range of values\n"); exit(EXIT_FAILURE); }while(0);

int main(int argc, char *argv[])
{
  if (argc != 3)
    usage ();

  int n, k;
  n = atoi(argv[1]);
  k = atoi(argv[2]);

  srand(time(NULL));
  printf("2\n");
  printf("%d %d\n",n,n);

  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
      printf(" %5d ", (rand() % (2*k+1)) - k);
    printf("\n");
  }


  printf("\n%d %d\n",n,n);

  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
      printf(" %5d ", (rand() % (2*k+1)) - k);
    printf("\n");
  }

  return 0;
}
