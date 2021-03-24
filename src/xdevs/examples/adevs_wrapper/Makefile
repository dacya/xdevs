CC = g++
CFLAGS = -std=c++11 -Wall -g
#CFLAGS = -std=c++11 -Wall -O3

XDEVS_SRC = $(shell find ../../core -type f -name '*.cpp')
XDEVS_OBJS = $(XDEVS_SRC:.cpp=.o)

SRC_BASE = AtomicADEVS.cpp Generator.cpp Job.cpp Transducer.cpp
SRC_GPT = main_gpt.cpp Gpt.cpp
OBJ_BASE = $(SRC_BASE:.cpp=.o)
OBJ_EFP = $(SRC_EFP:.cpp=.o)
OBJ_GPT = $(SRC_GPT:.cpp=.o)

all: main_gpt

main_gpt: $(XDEVS_OBJS) $(OBJ_BASE) $(OBJ_GPT)
	${CC} ${CFLAGS} $^ -o $@
	
%.o: %.cpp
	${CC} -c ${CFLAGS} $< -o $@

.PHONY: clean
clean:
	rm -f $(XDEVS_OBJS) $(OBJ_BASE) $(OBJ_GPT)

uninstall:
	rm -f main_gpt
