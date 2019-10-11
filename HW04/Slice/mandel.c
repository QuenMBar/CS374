/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 * 
 * This program computes and displays the Mandelbrot set.  This program uses slices to 
 * parallelize it and then uses gather to group all the slices into one array.
 * It times this and then displays the time and the picture computed.
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
   int n = 0,
       ix = 0,
       iy = 0,
       button = 0,
       numColumnsPerProcess = 0,
       rank = -1,
       numWorkers = -1,
       dummyVar = 0;
   double spacing = 0.005,
          x = 0.0,
          y = 0.0,
          c_real = 0.0,
          c_imag = 0.0,
          x_center = 1.0,
          y_center = 0.0;
   MPE_XGraph graph;

   //Init mpi
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);

   /**
    * Create a array to store values. Final size is WINDOW_SIZExWINDOW_SIZE but init depends on chunks
    * Possible scatter, def gather
    * Process 0 can display the final results
   */

   //Calculate the size of chunks and create the arrays with that size
   numColumnsPerProcess = WINDOW_SIZE / numWorkers;

   short computeArray[numColumnsPerProcess][WINDOW_SIZE];

   MPI_Barrier(MPI_COMM_WORLD);

   //Start timer
   double startTime = 0.0, totalTime = 0.0;
   startTime = MPI_Wtime();

   //Compute the n per x and y.  Save that in an array[x][y]
   //Start at rank and add the number of processes each time
   for (ix = rank; ix < numWorkers * numColumnsPerProcess; ix += numWorkers)
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

         computeArray[dummyVar][iy] = n;
      }
      dummyVar++;
   }

   //Create the gather array and gather all the arrays
   short gatherArray[WINDOW_SIZE][WINDOW_SIZE];

   MPI_Gather(computeArray, numColumnsPerProcess * WINDOW_SIZE, MPI_SHORT,
              gatherArray, numColumnsPerProcess * WINDOW_SIZE, MPI_SHORT, 0, MPI_COMM_WORLD);

   //If there is remainers, handle them
   if ((numWorkers % 2) == 1 && rank == 0)
   {
      for (ix = numWorkers * numColumnsPerProcess; ix < 1024; ix++)
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

            gatherArray[ix][iy] = n;
         }
      }
   }

   //If rank 0, open the graphics window and draw the gathered array
   if (rank == 0)
   {
      // totalTime = MPI_Wtime() - startTime;
      MPE_Open_graphics(&graph, MPI_COMM_WORLD,
                        getDisplay(),
                        -1, -1,
                        WINDOW_SIZE, WINDOW_SIZE, 0);
      for (ix = 0; ix < WINDOW_SIZE; ix++)
      {
         for (iy = 0; iy < WINDOW_SIZE; iy++)
         {
            //Math done to align the slices with the window
            if (gatherArray[ix][iy] < 50)
            {
               MPE_Draw_point(graph, ((ix % (WINDOW_SIZE / numWorkers)) * numWorkers) + (ix / (WINDOW_SIZE / numWorkers)), iy, MPE_RED);
            }
            else
            {
               MPE_Draw_point(graph, ((ix % (WINDOW_SIZE / numWorkers)) * numWorkers) + (ix / (WINDOW_SIZE / numWorkers)), iy, MPE_BLACK);
            }
         }
      }

      //Stop timer
      totalTime = MPI_Wtime() - startTime;

      //Close window and print time
      printf("\nClick in the window to continue...\n");
      MPE_Get_mouse_press(graph, &ix, &iy, &button);

      printf("Time: %f\n", totalTime);
      MPE_Close_graphics(&graph);
   }
   MPI_Finalize();
   return 0;
}
