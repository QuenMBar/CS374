TARGET = calcPI2
CC = gcc
CFLAGS = -Wall -ansi -pedantic -std=c99 -I/usr/include/openmpi
LFLAGS = -o $(TARGET) -lpthread -lmpi

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c $(LFLAGS)

clean:
	rm -f $(TARGET) *.o *~ *#


