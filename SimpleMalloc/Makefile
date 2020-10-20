OBJS = simplemalloc.o multimalloc.o malloctest.o atomic_lock.o main.o

CC	 	= gcc
TARGET 	= main
CFLAGS  = -Wall -g
CLIB	= -lpthread
INCLUDE = ./

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(CLIB) -I $(INCLUDE)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET) $(CLIB)

default:
	make $(TARGET)

clean:
	rm -rf *.o $(TARGET)
