/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 * 
 * Edited by Quentin Barnes, for HW07, Calvin University, Nov 2019
 */

#include <stdio.h>  /* I/O stuff */
#include <stdlib.h> /* calloc, etc. */
#include <string.h>
#include <omp.h> // OpenMP

void readArray(char *fileName, double **a, int *n);
double sumArray(double *a, int numValues);

int main(int argc, char *argv[])
{
  int howMany;
  double sum, startTime = 0.0, ioTime = 0.0, scatterTime = 0.0, reduceTime = 0.0;
  double *a;

  if (argc != 2)
  {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
    exit(1);
  }
  startTime = omp_get_wtime();

  readArray(argv[1], &a, &howMany);

  ioTime = omp_get_wtime() - startTime;

  startTime = omp_get_wtime();

  sum = sumArray(a, howMany);

  reduceTime = omp_get_wtime() - startTime;
  printf("The sum of the values in the input file '%s' is %g.  Io took %f, scatter took %f, and reduce took %f.\n",
         argv[1], sum, ioTime, scatterTime, reduceTime);

  // printf("Main, %s, %d, %g, %f, %f, %f\n", argv[1], 1, sum, ioTime, scatterTime, reduceTime);

  free(a);

  return 0;
}

/* readArray fills an array with values from a file.
 * Receive: fileName, a char*,
 *          a, the address of a pointer to an array,
 *          n, the address of an int.
 * PRE: fileName contains N, followed by N double values.
 * POST: a points to a dynamically allocated array
 *        containing the N values from fileName
 *        and n == N.
 */

void readArray(char *fileName, double **a, int *n)
{
  int count, howMany;
  double *tempA;
  FILE *fin;
  char *realFileName;

  // Set file name based on the provided input
  if (strcmp(fileName, "10k") == 0)
  {
    realFileName = "/home/cs/374/exercises/07/10k.txt";
  }
  else if (strcmp(fileName, "100k") == 0)
  {
    realFileName = "/home/cs/374/exercises/07/100k.txt";
  }
  else if (strcmp(fileName, "1m") == 0)
  {
    realFileName = "/home/cs/374/exercises/07/1m.txt";
  }
  else if (strcmp(fileName, "10m") == 0)
  {
    realFileName = "/home/cs/374/exercises/07/10m.txt";
  }
  else if (strcmp(fileName, "100m") == 0)
  {
    realFileName = "/home/cs/374/exercises/07/100m.txt";
  }
  else
  {
    realFileName = "No File Exists";
  }

  fin = fopen(realFileName, "r");
  if (fin == NULL)
  {
    fprintf(stderr, "\n*** Unable to open input file '%s'\n\n",
            realFileName);
    exit(1);
  }

  fscanf(fin, "%d", &howMany);
  tempA = calloc(howMany, sizeof(double));
  if (tempA == NULL)
  {
    fprintf(stderr, "\n*** Unable to allocate %d-length array",
            howMany);
    exit(1);
  }

  for (count = 0; count < howMany; count++)
    fscanf(fin, "%lf", &tempA[count]);

  fclose(fin);

  *n = howMany;
  *a = tempA;
}

/* sumArray sums the values in an array of doubles.
 * Receive: a, a pointer to the head of an array;
 *          numValues, the number of values in the array.
 * Return: the sum of the values in the array.
 */

double sumArray(double *a, int numValues)
{
  int i;
  double result = 0.0;

  for (i = 0; i < numValues; i++)
  {
    result += *a;
    a++;
  }

  return result;
}
