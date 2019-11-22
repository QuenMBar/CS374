/* sieve.c finds prime numbers using a parallel/MPI version
 *  of Eratosthenes Sieve.
 * Based on an implementation by Quinn
 * Modified by Ryan Holt to correctly handle '-np 1', Fall 2007
 * Modified by Nathan Dykhuis to handle larger ranges, Fall 2009
 * 
 * Edited by Quentin Barnes, for HW08, Calvin University, Nov 2019
 * Changed to use omp to further parallelize the computation of primes.
 * Every MPI thread can now spawn omp threads.
 *
 * Usage: mpirun -np N ./sieve <upperBound> <OMPThreads>
 */

#include "mpi.h"
#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  //Init Vars
  int i;
  int n;
  int index;
  int size;
  int prime;
  int count;
  int global_count;
  int first;
  long int high_value;
  long int low_value;
  int comm_rank;
  int comm_size;
  int global_size;
  int omp_id;
  int omp_size;
  int global_id;
  int mpi_wide_count;
  char *marked;
  double runtime;

  //Start MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  //Keep code the same, and add up rank and size of both of them and substatute it
  MPI_Barrier(MPI_COMM_WORLD);
  runtime = -MPI_Wtime();

  // Check for the command line argument.
  if (argc != 3)
  {
    if (comm_rank == 0)
      printf("\nPlease supply an upper bound and the omp threads.\n\n");
    MPI_Finalize();
    exit(1);
  }

  //Get upper limit
  n = atoi(argv[1]);
  //Get num omp threads and compute the total size
  omp_size = atoi(argv[2]);
  omp_set_num_threads(omp_size);
  global_size = comm_size * omp_size;

  // Bail out if all the primes used for sieving are not all held by
  // process zero.

  if ((2 + (n - 1 / global_size)) < (int)sqrt((double)n))
  {
    if (comm_rank == 0)
      printf("Too many processes.\n");
    MPI_Finalize();
    exit(1);
  }

  mpi_wide_count = 0;
// Figure out this process's share of the array, as well as the integers
// represented by the first and last array elements.
#pragma omp parallel private(global_id, omp_id, low_value, high_value, size, marked, index, first, i, count)
  {
    //Get rank of OMP threads and then compute separate ids for every omp threads one every mpi thread
    omp_id = omp_get_thread_num();
    global_id = (comm_rank * omp_size) + omp_id;
    low_value = 2 + (long int)(global_id) * (long int)(n - 1) / (long int)global_size;
    high_value = 1 + (long int)(global_id + 1) * (long int)(n - 1) / (long int)global_size;
    size = high_value - low_value + 1;

    marked = (char *)calloc(size, sizeof(char));

    if (marked == NULL)
    {
      printf("Cannot allocate enough memory.\n");
      MPI_Finalize();
      exit(1);
    }

    if (global_id == 0)
      index = 0;
    prime = 2;

    do
    {
      if (prime * prime > low_value)
      {
        first = prime * prime - low_value;
      }
      else
      {
        if ((low_value % prime) == 0)
          first = 0;
        else
          first = prime - (low_value % prime);
      }

      for (i = first; i < size; i += prime)
        marked[i] = 1;

        //Barriers to make sure same primes are being used always
#pragma omp barrier
      if (global_id == 0)
      {
        while (marked[++index])
          ;
        prime = index + 2;
      }

      if (global_size > 1 && omp_id == 0)
        MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);

#pragma omp barrier
    } while (prime * prime <= n);

    count = 0;

    for (i = 0; i < size; i++)
      if (marked[i] == 0)
      {
        count++;
      }
//Compute the counts for omp threads to one count per mpi thread
#pragma omp atomic
    mpi_wide_count += count;

    free(marked);
  }

  if (comm_size > 1)
  {
    MPI_Reduce(&mpi_wide_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  }
  else
  {
    global_count = mpi_wide_count;
  }

  runtime += MPI_Wtime();

  if (comm_rank == 0)
  {
    printf("In %f seconds we found %d primes less than or equal to %d.\n",
           runtime, global_count, n);
  }

  MPI_Finalize();
  return 0;
}
