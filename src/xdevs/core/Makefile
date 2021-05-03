VERSION = 1.1.0
CC = g++
CFLAGS = -std=c++11 -Wall -O3

XDEVS_SRC = $(shell find . -type f -name '*.cpp')
XDEVS_OBJS = $(XDEVS_SRC:.cpp=.o)

XDEVS_LIB_NAME = libxdevs
XDEVS_LIB = $(XDEVS_LIB_NAME)-$(VERSION).a
XDEVS_LIB_DIR = ../../../lib

all: $(XDEVS_LIB) install clean

$(XDEVS_LIB): ${XDEVS_OBJS}
	ar cr $(XDEVS_LIB) $^

%.o: %.cpp %.h
	${CC} -c ${CFLAGS} $< -o $@

install: $(XDEVS_LIB)
	@if [ ! -d $(XDEVS_LIB_DIR) ]; then \
		mkdir $(XDEVS_LIB_DIR); \
	fi
	mv $(XDEVS_LIB) $(XDEVS_LIB_DIR)/
	@if [ -f $(XDEVS_LIB_NAME).a ]; then \
		rm $(XDEVS_LIB_NAME).a; \
	fi

.PHONY: clean
clean:
	-rm -f $(XDEVS_OBJS) $(XDEVS_LIB)

