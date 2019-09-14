/* RanksRing.c
 * A program that uses message passing and the master worker style of
 * parallel computing to pass a message throuhg all the process and have 
 * them add their ids to a string that the master prints in the end
 * 
 * Written By Quentin Barnes
 */

#include <stdio.h>  // printf()
#include <mpi.h>    // MPI
#include <stdlib.h> // malloc()
#include <string.h> // strlen()

int main(int argc, char **argv)
{
    int id = -1, numWorkers = -1;

    //Make strings empty pointers
    char *sendString = NULL;
    char *receivedString = NULL;
    MPI_Status status;

    //Init MPI and set id and numWorkers
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);

    if (id == 0)
    { // process 0 is the master
        //Start timer
        double startTime = 0.0, totalTime = 0.0;
        startTime = MPI_Wtime();

        //Allocate memory based on id number and numWorkers
        int SizeSend = (4 * (id + 1)) * sizeof(char);
        int SizeRec = (4 * numWorkers) * sizeof(char);
        sendString = (char *)malloc(SizeSend);
        receivedString = (char *)malloc(SizeRec);

        //Add id to string
        sprintf(sendString, "%d", id);

        //If only one process, send to self, else send to the next process
        if (numWorkers == 1)
        {
            MPI_Send(sendString, SizeSend, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(sendString, SizeSend, MPI_CHAR, (id + 1), 1, MPI_COMM_WORLD);
        }

        //Wait to recieve from last process and print
        MPI_Recv(receivedString, SizeRec, MPI_CHAR, (numWorkers - 1), 2, MPI_COMM_WORLD, &status);
        printf("\nFinal string: \"%s\"\n", receivedString);

        //Print the time
        totalTime = MPI_Wtime() - startTime;
        printf("Message Ring took %f\n\n", totalTime);
    }
    else
    { // processes with ids > 0 are workers
        //Allocate memory based on id number
        int SizeSend = (4 * (id + 1)) * sizeof(char);
        int SizeRec = (4 * id) * sizeof(char);
        sendString = (char *)malloc(SizeSend);
        receivedString = (char *)malloc(SizeRec);

        //Recive from the process before it
        MPI_Recv(receivedString, SizeRec, MPI_CHAR, (id - 1), 1, MPI_COMM_WORLD, &status);

        //Add its own id to the string
        sprintf(sendString, "%s %d", receivedString, id);

        //If its the last string, send to master, else send to the next process
        if (id == (numWorkers - 1))
        {
            MPI_Send(sendString, SizeSend, MPI_CHAR, 0, 2, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(sendString, SizeSend, MPI_CHAR, (id + 1), 1, MPI_COMM_WORLD);
        }
    }

    //Free strings
    free(sendString);
    free(receivedString);

    //Close MPI
    MPI_Finalize();
    return 0;
}
