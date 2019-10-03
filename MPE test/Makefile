PROG    = testMPE

CC      = mpicc
CFLAGS  = -DMPE_GRAPHICS -Wall
LFLAGS  = -L/usr/X11R6/lib -lmpe -lX11 -lm

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) $(PROG).c -o $(PROG) $(LFLAGS) 

run:
	mpirun -np 1 ./$(PROG)

clean:
	rm -f $(PROG) *.o *~ *# PI*

