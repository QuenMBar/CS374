/* calcPI2.c calculates PI using POSIX threads.
 * Since PI == 4 * arctan(1), and arctan(x) is the 
 *  integral from 0 to x of (1/(1+x*x),
 *  the for loop below approximates that integration.
 *
 * Joel Adams, Calvin College, Fall 2013.
 *
 * Usage: ./calcPI2 [numIntervals] [numThreads]
 *
 * Extended to use the reduction pattern instead of the mutual
 * exclusion pattern when calculating PI - involves passing the
 * local sum calculated by each thread from the computePI 
 * function to pthreadReduction
 *
 * author     Ty Vredeveld
 * why        Project 6, CS 374
 * where      Calvin University
 * date       October 28, 2019
 */

#include <stdio.h>            // printf(), fprintf(), etc.
#include <stdlib.h>           // strtoul(), exit(), ...
#include <pthread.h>          // pthreads
#include <mpi.h>              // MPI_Wtime()
#include "pthreadReduction.h" // pthreadReductionSum()

// global variables (shared by all threads)
volatile long double pi = 0.0; // our approximation of PI
unsigned long intervals = 0;   // how finely we chop up the integration
unsigned long numThreads = 0;  // how many threads we use

/* process the command-line arguments
 * Parameters: argc, an int; and argv a char**.
 * Postcondition:
 *  - non-locals intervals and numThreads have been defined.
 *     according to the values the user specified when
 *     calcPI2 was invoked.
 */
void processCommandLine(int argc, char **argv)
{
    if (argc == 3)
    {
        intervals = strtoul(argv[1], 0, 10);
        numThreads = strtoul(argv[2], 0, 10);
    }
    else if (argc == 2)
    {
        intervals = strtoul(argv[1], 0, 10);
        numThreads = 1;
    }
    else if (argc == 1)
    {
        intervals = 1;
        numThreads = 1;
    }
    else
    {
        fprintf(stderr, "\nUsage: calcPI2 [intervals] [numThreads]\n\n");
        exit(1);
    }
}

/* compute PI using the parallel for loop pattern
 * Parameters: arg, a void* 
 * Preconditions: 
 *   - non-locals intervals and numThreads are defined.
 *   - arg contains the address of our thread's ID.
 * Postcondition: non-local pi contains our approximation of PI.
 *                 (after pthreadReductionSum has been called)
 */
void *computePI(void *arg)
{
    long double x,
        width,
        localSum = 0;
    unsigned long i;
    unsigned long threadID = *((unsigned long *)arg);

    width = 1.0 / intervals;

    for (i = threadID; i < intervals; i += numThreads)
    {
        x = (i + 0.5) * width;
        localSum += 4.0 / (1.0 + x * x);
    }

    localSum *= width;

    pthreadReductionSum(localSum, &pi, numThreads, threadID);

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t *threads;       // dynamic array of threads
    unsigned long *threadIDs; // dynamic array of thread id #s
    unsigned long i = 0;      // loop control variable
    double startTime = 0,     // timing variables
        stopTime = 0;

    processCommandLine(argc, argv);

    threads = malloc(numThreads * sizeof(pthread_t));
    threadIDs = malloc(numThreads * sizeof(unsigned long));

    startTime = MPI_Wtime();

    for (i = 0; i < numThreads; i++)
    { // fork threads
        threadIDs[i] = i;
        pthread_create(&threads[i], NULL, computePI, threadIDs + i);
    }

    for (i = 0; i < numThreads; i++)
    { // join them
        pthread_join(threads[i], NULL);
    }

    stopTime = MPI_Wtime();

    printf("Estimation of pi is %32.30Lf in %lf secs\n", pi, stopTime - startTime);
    printf("(actual pi value is 3.141592653589793238462643383279...)\n");

    free(threads);
    free(threadIDs);

    return 0;
}
