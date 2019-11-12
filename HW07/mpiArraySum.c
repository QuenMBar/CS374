/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 * 
 * Edited by Quentin Barnes, for HW07, Calvin University, Nov 2019
 * Changed to use MPI for multithreading the program.  Master worker used for reading 
 * and scattering the data, and then blocks used for parellelizing the loop and reduce. 
 */

#include <stdio.h>  /* I/O stuff */
#include <stdlib.h> /* calloc, etc. */
#include <string.h>
#include <mpi.h>

void readArray(char *fileName, double **a, int *n);
double sumArray(double *a, int numValues);

int main(int argc, char *argv[])
{
  int howMany;
  double sum, startTime = 0.0, ioTime = 0.0, scatterTime = 0.0, reduceTime = 0.0;
  double *aRcv;
  double *aSend;
  // double *aRcv;
  int numProcs, myRank, chunkSize;

  if (argc != 2)
  {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
    exit(1);
  }

  //Init mpi
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  startTime = MPI_Wtime();

  //Only process 0 reads the array in
  if (myRank == 0)
  {
    readArray(argv[1], &aSend, &howMany);
  }
  ioTime = MPI_Wtime() - startTime;
  startTime = MPI_Wtime();

  //Broadcasts the size of the array to the other threads and then sends them each chunks using mpi scatter
  MPI_Bcast(&howMany, 1, MPI_INT, 0, MPI_COMM_WORLD);
  chunkSize = howMany / numProcs;
  aRcv = (double *)malloc(chunkSize * sizeof(double));
  MPI_Scatter(aSend, chunkSize, MPI_DOUBLE, aRcv, chunkSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  scatterTime = MPI_Wtime() - startTime;
  startTime = MPI_Wtime();

  //The thread goes and reduces its chunk and 0 will receive the total
  sum = sumArray(aRcv, chunkSize);

  reduceTime = MPI_Wtime() - startTime;

  MPI_Barrier(MPI_COMM_WORLD);

  //If 0, print the results
  if (myRank == 0)
  {
    printf("The sum of the values in the input file '%s' is %g.  Io took %f, scatter took %f, and reduce took %f.\n",
           argv[1], sum, ioTime, scatterTime, reduceTime);

    // printf("MPI, %s, %d, %g, %f, %f, %f\n", argv[1], numProcs, sum, ioTime, scatterTime, reduceTime);
    free(aSend);
    free(aRcv);
  }

  MPI_Finalize();
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
  double resultLocal = 0.0, resultGlobal;

  for (i = 0; i < numValues; i++)
  {
    resultLocal += a[i];
  }

  //Uses mpi reduce to give process 0 the true total
  MPI_Reduce(&resultLocal, &resultGlobal, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  return resultGlobal;
}
