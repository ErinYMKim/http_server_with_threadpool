# the compiler to use
CC = gcc

# compiler flags:
# -g    adds debugging information to the executable file
# -Wall turns on most, but not all, compiler warnings
CFLAGS = -g -Wall

# include paths
INCLUDES = -I.

# the build target executable for the server
TARGET1 = server

# the build target executable for the client
TARGET2 = client

# Library for the tests
TEST_LIBS = -lcheck -lm -pthread

all: $(TARGET1) $(TARGET2)

$(TARGET1): thread_pool.c http_server.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET1) thread_pool.c http_server.c -lpthread

$(TARGET2): http_client.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET2) http_client.c -lpthread

clean:
	rm -f $(TARGET1) $(TARGET2)