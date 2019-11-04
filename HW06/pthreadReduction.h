/** pthreadReduction.h is a file that impelments the pthreadReductionSum
 * This is a function that gets all the local values from the threads
 * and uses barriers and reduction pattern to reduce the sums to one value
 *
 * who: Quentin Barnes
 * why: Homework 6, 374
 * where: Calvin University
 * date: Nov 4, 2019
 */
#include "pthreadBarrier.h"
long double *sumArray;

/**Does summation using the reduction pattern in log(n) time
 * Takes: localSum, a long double
 * pi, a volatile long double
 * numThreads, an int
 * threadID, an int
 * returns: Pi, a volatile long double
 */
void pthreadReductionSum(long double localSum, volatile long double *pi, int numThreads, int threadID)
{
    //Let the main thread allocate the array
    if (threadID == 0)
    {
        sumArray = malloc(numThreads * sizeof(long double));
    }

    //Once it has done that, all the threads write their values to the
    //their IDs position in the array, and the 0 array checks to see if its the only thread
    pthreadBarrier(numThreads);
    sumArray[threadID] = localSum;
    if (numThreads == 1 && threadID == 0)
    {
        //Return main threads pi if its the only one
        *pi = sumArray[0];
        return;
    }

    //Wait for threads, and then calculate the number of threads to keep
    pthreadBarrier(numThreads);
    double threadsKeep = numThreads / 2;

    while (threadsKeep > 0.5)
    {
        //if your id is less than the keep threshold, then add your value plus another value
        if (threadID < threadsKeep)
        {
            sumArray[threadID] += sumArray[threadID + (int)threadsKeep];
        }
        //else break out of loop
        else
        {
            break;
        }
        //Wait for threads to keep and then cut that number in half
        pthreadBarrier(threadsKeep);
        threadsKeep /= 2;
    }

    //if main thread, save pi as your value, and deallocate sum array
    if (threadID == 0)
    {
        *pi = sumArray[0];
        free(sumArray);
    }

    return;
}
