/* program for generating all binary vectors of length n */
/* to be used in benchmarking processing speeds of individual computers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include<time.h>

int main(void)
{
  int n;                            /* length of vector */
  int i, top, cont, numb;
  int vec[31];
  clock_t start_t, end_t;        /* variables used in measuring time */
  double utime;

  n = 25;                                /* length of vector */
  for (i=1; i<=n; i++){vec[i] = 0;}  /* initialize */
  vec[0] = -1;
  cont = 1;                             /* loop variable */
  numb = 0;                            /* counts number of generated vectors */

  start_t = clock();                  /* start measuring time */

  while (cont == 1){                 /* main loop, vectors are generated in lexicographical order */ 
     numb++;
     top = n;                            /* find largest index top with vec[top] = 0 */
     while (vec[top]==1){top--;}
     if (top > 0){                       /* if current vector is not lexicographic max */
       vec[top] = 1;                   /* set vec[top] = 1 and */
       if (top < n){                     /* set vec[i]=0 for all indices i larger than top */ 
         for (i=top+1; i<=n; i++){vec[i] = 0;}
       }
    }
    else {cont = 0;}                   /* current vector is lexicographic max so stop */
  }

  end_t = clock();                    /* stop measuring time */

  utime = (double)(end_t-start_t)/CLOCKS_PER_SEC;  /* convert to user time */
  printf(" time =  %lf\n", utime);                                /* output result */
  printf("# of binary vecs of length %d = %d\n", n, numb);

  return(0);
}
