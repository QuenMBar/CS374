/* spmd2.c
 * ... illustrates the SPMD pattern in OpenMP,
 * 	using the commandline arguments 
 *      to control the number of threads.
 *
 * Joel Adams, Calvin College, November 2009.
 *
 * Usage: ./spmd2 [numThreads]
 *
 * Exercise:
 * - Compile & run with no commandline args 
 * - Rerun with different commandline args,
 *    note race condition
 * - Uncomment private clause, recompile,
 *    rerun, compare results
 */

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int id, numThreads;

    printf("\n");
    if (argc > 1)
    {
        omp_set_num_threads(atoi(argv[1]));
    }

#pragma omp parallel private(id)
    {
        id = omp_get_thread_num();
        numThreads = omp_get_num_threads();
        printf("Hello from thread %d of %d\n", id, numThreads);
    }

    printf("\n");
    return 0;
}
