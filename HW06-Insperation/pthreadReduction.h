/* pthreadReduction.h implements the Reduction pattern using pthreads
 * and the already implemented Barrier pattern
 *
 * author     Ty Vredeveld
 * why        Project 6, CS 374
 * where      Calvin University
 * date       October 25, 2019
 */

#include "pthreadBarrier.h" // pthreadBarrier()

long double * localSumArray = NULL;

/* perform a reduction sum on the array of thread-local values
 * Parameters:
 * 	- localSumArray, a long double pointer
 *  - globalSum, a volatile long double pointer
 *  - threadID, an int
 *  - numThreads, an int
 * Postcondition:
 *  - the computed value of pi is written to the address of globalSum,
 *     which can be seen back in main() in calcPI2.c
 */
void pthreadReductionSum(long double localSum, volatile long double * globalSum, int numThreads, int threadID) {

	// let the root thread take care of allocating
	// and deallocating the dynamic array as needed
	if (threadID == 0) {
		localSumArray = malloc(numThreads*sizeof(long double));
	}

	pthreadBarrier(numThreads);

	// before we start the reduction have every thread
	// insert its local sum into the shared array
	localSumArray[threadID] = localSum;

	int currentWorkingThreads = numThreads;
	int offset = numThreads / 2;

	while (currentWorkingThreads > 1) {
		pthreadBarrier(currentWorkingThreads);
		
		// let the non-working threads terminate early
		if (threadID < offset) {
			localSumArray[threadID] += localSumArray[threadID + offset];
		} else {
			return;
		}

		currentWorkingThreads /= 2;
		offset /= 2;		
	}

	*globalSum = localSumArray[0];

	/*
	 * at this point every thread but the root will 
	 * have returned, so only one thread will be 
	 * deallocating the allocated memory
	 */
	free(localSumArray);
}
