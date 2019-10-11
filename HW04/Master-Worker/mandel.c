/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 * 
 * This program computes and displays the Mandelbrot set.  This program uses the 
 * master worker scheme to parallelize it.  The master sends the Xs to be processed and
 * the workers process them and send them back.  As the master recives them it displays them.
 * Time is computed and shown at the end.
 * 
 * Edited By Quentin Barnes
 * Why   HomeWork 4, CS 374
 * Where Calvin University, Gold Lab
 * Date  8/10/19
 */

#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>
#include <mpe.h>
#include "display.h"
#include <stdlib.h>

/* compute the Mandelbrot-set function for a given
 *  point in the complex plane.
 *
 * Receive: doubles x and y,
 *          complex c.
 * Modify: doubles ans_x and ans_y.
 * POST: ans_x and ans_y contain the results of the mandelbrot-set
 *        function for x, y, and c.
 */
void compute(double x, double y, double c_real, double c_imag,
             double *ans_x, double *ans_y)
{
   *ans_x = x * x - y * y + c_real;
   *ans_y = 2 * x * y + c_imag;
}

/* compute the 'distance' between x and y.
 *
 * Receive: doubles x and y.
 * Return: x^2 + y^2.
 */
double distance(double x, double y)
{
   return x * x + y * y;
}

int main(int argc, char *argv[])
{
   //Make variables
   const int WINDOW_SIZE = 1024;
   short computeArray[WINDOW_SIZE];
   int n = 0,
       ix = 0,
       iy = 0,
       button = 0,
       rank = -1,
       numWorkers = -1,
       dummyVar = 1,
       sendTag = 1,
       workerX = -1;
   double spacing = 0.005,
          x = 0.0,
          y = 0.0,
          c_real = 0.0,
          c_imag = 0.0,
          x_center = 1.0,
          y_center = 0.0;
   MPE_XGraph graph;
   MPI_Status status;

   //Init mpi
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);

   //If rank 0, time the program, draw the graph, and mannage workers
   if (rank == 0)
   {

      double startTime = 0.0, totalTime = 0.0;
      startTime = MPI_Wtime();
      MPE_Open_graphics(&graph, MPI_COMM_WORLD,
                        getDisplay(),
                        -1, -1,
                        WINDOW_SIZE, WINDOW_SIZE, 0);

      //If it is the only process, compute and draw the program
      if (numWorkers == 1)
      {
         for (ix = 0; ix < WINDOW_SIZE; ix++)
         {
            for (iy = 0; iy < WINDOW_SIZE; iy++)
            {
               c_real = (ix - 400) * spacing - x_center;
               c_imag = (iy - 400) * spacing - y_center;
               x = y = 0.0;
               n = 0;

               while (n < 50 && distance(x, y) < 4.0)
               {
                  compute(x, y, c_real, c_imag, &x, &y);
                  n++;
               }

               if (n < 50)
               {
                  MPE_Draw_point(graph, ix, iy, MPE_YELLOW);
               }
               else
               {
                  MPE_Draw_point(graph, ix, iy, MPE_BLACK);
               }
            }
         }
      }
      else
      {
         //If its not then compute row 0 and draw it
         for (iy = 0; iy < WINDOW_SIZE; iy++)
         {
            c_real = (rank - 400) * spacing - x_center;
            c_imag = (iy - 400) * spacing - y_center;
            x = y = 0.0;
            n = 0;

            while (n < 50 && distance(x, y) < 4.0)
            {
               compute(x, y, c_real, c_imag, &x, &y);
               n++;
            }

            if (n < 50)
            {
               MPE_Draw_point(graph, rank, iy, MPE_YELLOW);
            }
            else
            {
               MPE_Draw_point(graph, rank, iy, MPE_BLACK);
            }
         }

         /**Then for all the rows after, receive a row from another process 
 * and send it the next row to be computed.  Once all the rows have been 
 * computed just receive the row and tell the process to end.
 * Once received the row, then draw it and wait to receive the next one.
 */
         for (dummyVar = 0; dummyVar < WINDOW_SIZE - 1; dummyVar++)
         {
            MPI_Recv(computeArray, WINDOW_SIZE, MPI_SHORT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            workerX = dummyVar + numWorkers;
            if (dummyVar + numWorkers >= WINDOW_SIZE)
            {
               sendTag = 0;
            }
            MPI_Send(&workerX, 1, MPI_INT, status.MPI_SOURCE, sendTag, MPI_COMM_WORLD);

            for (iy = 0; iy < WINDOW_SIZE; iy++)
            {
               if (computeArray[iy] < 50)
               {
                  MPE_Draw_point(graph, status.MPI_TAG, iy, MPE_YELLOW);
               }
               else
               {
                  MPE_Draw_point(graph, status.MPI_TAG, iy, MPE_BLACK);
               }
            }
         }
      }

      //Print time and close window
      totalTime = MPI_Wtime() - startTime;
      printf("Time: %f\n\n", totalTime);
      printf("\nClick in the window to continue...\n");
      MPE_Get_mouse_press(graph, &ix, &iy, &button);
   }
   else
   {
      //if not master process compute rank's row
      for (iy = 0; iy < WINDOW_SIZE; iy++)
      {
         c_real = (rank - 400) * spacing - x_center;
         c_imag = (iy - 400) * spacing - y_center;
         x = y = 0.0;
         n = 0;

         while (n < 50 && distance(x, y) < 4.0)
         {
            compute(x, y, c_real, c_imag, &x, &y);
            n++;
         }

         computeArray[iy] = n;
      }

      //Send it to master
      MPI_Send(computeArray, WINDOW_SIZE, MPI_SHORT, 0, rank, MPI_COMM_WORLD);

      /**Until told to end.  Receive a new row to calc, and then calculate it and 
 * save it to the array.  Then send it to the master and wait to receive a 
 * a new row.
 */
      while (1 == 1)
      {
         MPI_Recv(&workerX, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
         if (status.MPI_TAG == 0)
         {
            break;
         }
         for (iy = 0; iy < WINDOW_SIZE; iy++)
         {
            c_real = (workerX - 400) * spacing - x_center;
            c_imag = (iy - 400) * spacing - y_center;
            x = y = 0.0;
            n = 0;

            while (n < 50 && distance(x, y) < 4.0)
            {
               compute(x, y, c_real, c_imag, &x, &y);
               n++;
            }

            computeArray[iy] = n;
         }

         MPI_Send(computeArray, WINDOW_SIZE, MPI_SHORT, 0, workerX, MPI_COMM_WORLD);
      }
   }
   MPI_Finalize();
   return 0;
}
