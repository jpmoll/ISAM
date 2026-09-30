CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

OBJS = main.o datos.o indice.o overflow.o registro.o tablas/bus.o tablas/coordenada.o
TARGET = isam

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.dat

.PHONY: clean
