CC = g++
CFLAGS = -std=c++11 -Wall -O3

XDEVS_SRC = $(shell find . -type f -name '*.cpp')
XDEVS_OBJS = $(XDEVS_SRC:.cpp=.o)

XDEVS_LIB = libxdevs.a
XDEVS_LIB_DIR = ../../../lib

all: $(XDEVS_LIB)

$(XDEVS_LIB): ${XDEVS_OBJS}
	ar rcs $(XDEVS_LIB) $^

%.o: %.cpp %.h
	${CC} -c ${CFLAGS} $< -o $@

install: $(XDEVS_LIB)
	@if [ ! -d $(XDEVS_LIB_DIR) ]; then \
		mkdir $(XDEVS_LIB_DIR); \
	fi
	mv $(XDEVS_LIB) $(XDEVS_LIB_DIR)/

.PHONY: clean
clean:
	-rm -f $(XDEVS_OBJS) $(XDEVS_LIB)

