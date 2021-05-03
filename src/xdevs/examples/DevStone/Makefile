CC = g++
CFLAGS = -std=c++11 -g -Wall -O3

XDEVS_SRC = $(shell find ../../core -type f -name '*.cpp')
XDEVS_OBJ = $(XDEVS_SRC:.cpp=.o)

EXAMPLE_SRC = $(shell find . -type f -name '*.cpp')
EXAMPLE_OBJ = $(EXAMPLE_SRC:.cpp=.o)
EXAMPLE_NAME = devstone

all: $(XDEVS_OBJ) $(EXAMPLE_OBJ)
	${CC} ${CFLAGS} $^ -o ${EXAMPLE_NAME}

%.o: %.cpp
	${CC} -c ${CFLAGS} $< -o $@

.PHONY: clean
clean:
	rm -f $(XDEVS_OBJ) $(EXAMPLE_OBJ)

uninstall:
	rm -f $(EXAMPLE_NAME)
