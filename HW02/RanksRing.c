/* masterServer.c
 * ... illustrates the basic master-worker pattern in MPI ...
 * Joel Adams, Calvin College, November 2009.
 */

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI
#include <stdlib.h>  // malloc()
#include <string.h>  // strlen()

int main(int argc, char **argv)
{
    int id = -1, numWorkers = -1, length = -1;
    char hostName[MPI_MAX_PROCESSOR_NAME];

    char *sendString = NULL;
    char *receivedString = NULL;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);
    MPI_Get_processor_name(hostName, &length);

    if (id == 0)
    { // process 0 is the master
        int SizeSend = (4 * (id+1)) * sizeof(char);
        int SizeRec = (4 * numWorkers) * sizeof(char);
        sendString = (char*) malloc(SizeSend);
        receivedString = (char*) malloc(SizeRec);
        sprintf(sendString, "%d", id);
        if (numWorkers == 1)
        {
            printf("Sending to self\n");
            MPI_Send(sendString, SizeSend, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        } else
        {
            printf("Sending to %d\n", (id+1));
            MPI_Send(sendString, SizeSend, MPI_CHAR, id+1, 1, MPI_COMM_WORLD);
        }
        
        MPI_Recv(receivedString, SizeRec, MPI_CHAR, numWorkers-1, 2, MPI_COMM_WORLD, &status);
        printf("\n%s\n", receivedString);
        
    }
    else
    { // processes with ids > 0 are workers
        int SizeSend = (4 * (id+1)) * sizeof(char);
        int SizeRec = (4 * id) * sizeof(char);
        sendString = (char*) malloc(SizeSend);
        receivedString = (char*) malloc(SizeRec);
        MPI_Recv(receivedString, SizeRec, MPI_CHAR, id-1, 1, MPI_COMM_WORLD, &status);
        sprintf(sendString, "%s %d", receivedString, id);
        printf("\nProcess %d is sending: %s: ", id, sendString);
        if (id == (numWorkers - 1))
        {
            printf("To 0\n\n");
            MPI_Send(sendString, SizeSend, MPI_CHAR, 0, 2, MPI_COMM_WORLD);
        } else
        {
            printf("To %d", (id+1));
            MPI_Send(sendString, SizeSend, MPI_CHAR, id+1, 2, MPI_COMM_WORLD);
        }
    }


    free(sendString); free(receivedString);
    MPI_Finalize();
    return 0;
}
