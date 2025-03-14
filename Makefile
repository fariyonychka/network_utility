CC=g++
CFLAGS=-Wall -Wextra -std=c++17
SRC=src/main.cpp src/traceroute.cpp
OBJ=$(SRC:.cpp=.o)
TARGET=traceroute
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)