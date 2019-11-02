/* bankAccount.h simulates a simple bank account
 *
 * Joel Adams, Calvin College, Fall 2013.
 */
#include "pthreadBarrier.h"
long double *sumArray;

void pthreadReductionSum(long double localSum, volatile long double *pi, int numThreads, int threadID)
{
    if (threadID == 0)
    {
        sumArray = malloc(numThreads * sizeof(long double));
    }

    pthreadBarrier(numThreads);
    sumArray[threadID] = localSum;
    if (numThreads == 1 && threadID == 0)
    {
        *pi = sumArray[0];
        return;
    }
    pthreadBarrier(numThreads);

    double currentThreads = numThreads / 2;

    while (currentThreads > 0.5)
    {
        if (threadID < currentThreads)
        {
            sumArray[threadID] += sumArray[threadID + (int)currentThreads];
        }
        else
        {
            break;
        }
        pthreadBarrier(currentThreads);
        currentThreads /= 2;
    }

    if (threadID == 0)
    {
        *pi = sumArray[0];
        free(sumArray);
    }

    return;
}
