/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 */

#include <stdio.h>  /* I/O stuff */
#include <stdlib.h> /* calloc, etc. */
#include <string.h>
#include <omp.h> // OpenMP
#include <mpi.h>

void readArray(char *fileName, double **a, int *n);
double sumArray(double *a, int numValues);

int main(int argc, char *argv[])
{
  int howMany, id;
  double sum;
  double *a;

  if (argc != 3)
  {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile> <numProcesses>\n\n");
    exit(1);
  }

  if (argc > 1)
  {
    omp_set_num_threads(atoi(argv[2]));
  }

#pragma omp parallel private(id, numThreads)
  {
    id = omp_get_thread_num();

    if (id == 0)
    {
      readArray(argv[1], &a, &howMany);
    }
#pragma omp barrier
  }
  sum = sumArray(a, howMany);
  printf("The sum of the values in the input file '%s' is %g\n",
         argv[1], sum);

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

  // Set file name
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

#pragma omp parallel for reduction(+ \
                                   : result)
  for (i = 0; i < numValues; i++)
  {
    result += *a;
    a++;
  }

  return result;
}
