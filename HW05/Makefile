TARGET = calcPI
CC = mpicc
CFLAGS = -Wall -c -std=c99 -I/opt/intel/advisor/include
LFLAGS = -o $(TARGET) -lm -ldl -L/usr/X11R6/lib -lX11

$(TARGET): $(TARGET).o integral.o
	$(CC) $(TARGET).o integral.o $(LFLAGS)

$(TARGET).o: $(TARGET).c integral.h
	$(CC) $(CFLAGS) $(TARGET).c

integral.o: integral.c integral.h
	$(CC) $(CFLAGS) integral.c

clean:
	rm -f $(TARGET) *.o *~ *#

