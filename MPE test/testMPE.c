/* testMPE.c is a simple drawing demo using MPI/MPE commands.
 *
 * The master process draws a centered black bullseye; 
 *  each worker process draws a centered concentric ring;
 *  then the master repeatedly:
 *   draws a 10-pixel circle wherever the user left-clicks
 *  until the user right-clicks, 
 *  after which the program terminates.
 *
 * Summer 2013, Joel Adams, Calvin College.
 */

#include <mpi.h>
#include <mpe.h>
#include <stdlib.h> /* getenv() */
#include <string.h> /* strcmp() */
#include <stdio.h>  /* printf(), etc. */

/* 
 * getDisplay() retrieves the DISPLAY environment info
 */

char *getDisplay()
{
   char *display = getenv("DISPLAY");
   if (strncmp(display, "(null)", 7) == 0)
   {
      fprintf(stderr, "\n*** Fatal: DISPLAY variable not set.\n");
      exit(1);
   }
   return display;
}

int main(int argc, char *argv[])
{
   const int WINDOW_WIDTH = 800;
   const int WINDOW_HEIGHT = 800;
   const int CIRCLE_RADIUS = 5;
   const int MOUSE_RIGHT_BUTTON = 3;
   int x = 0, y = 0;
   int id = 0, button = 0;
   MPE_XGraph canvas;
   char *display = getDisplay();

   /* initialize */
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   if (id == 0)
   {
      printf("Opening %dx%d window.\n", WINDOW_WIDTH, WINDOW_HEIGHT);
   }

   MPE_Open_graphics(&canvas, MPI_COMM_WORLD,
                     display,
                     -1, -1,
                     WINDOW_WIDTH, WINDOW_HEIGHT, 0);
   /*
     *  Draw a bulls-eye, with 
     *   -  master drawing bulls-eye;
     *   -  workers drawing concentric rings
     */
   MPE_Line_thickness(canvas, 4);
   if (id == 0)
   {
      MPE_Fill_circle(canvas,
                      WINDOW_WIDTH / 2,
                      WINDOW_HEIGHT / 2,
                      10,
                      MPE_BLACK);
   }
   else
   {
      MPE_Draw_circle(canvas,
                      WINDOW_WIDTH / 2,
                      WINDOW_HEIGHT / 2,
                      id * 25,
                      MPE_RED);
      MPE_Update(canvas);
   }

   /* 
     * Draw blue circles wherever the user left-clicks
     */
   if (id == 0)
   {
      printf("Click:\n- left mouse button to draw blue circles;\n");
      printf("- right mouse button to quit.\n");
      do
      {
         MPE_Get_mouse_press(canvas, &x, &y, &button);
         MPE_Fill_circle(canvas, x, y, CIRCLE_RADIUS, MPE_BLUE);
      } while (button != MOUSE_RIGHT_BUTTON);
   }

   /* clean up */
   MPE_Close_graphics(&canvas);
   MPI_Finalize();
   if (id == 0)
   {
      printf("Program complete.\n\n");
   }
   return 0;
}
