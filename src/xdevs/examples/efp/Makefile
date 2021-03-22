CC = g++
CFLAGS = -std=c++11 -Wall -g -O3

XDEVS_SRC = $(shell find ../../core -type f -name '*.cpp')
XDEVS_OBJS = $(XDEVS_SRC:.cpp=.o)

SRC_BASE = Generator.cpp Job.cpp Processor.cpp Transducer.cpp
SRC_EFP = main_efp.cpp Ef.cpp Efp.cpp
SRC_GPT = main_gpt.cpp Gpt.cpp
OBJ_BASE = $(SRC_BASE:.cpp=.o)
OBJ_EFP = $(SRC_EFP:.cpp=.o)
OBJ_GPT = $(SRC_GPT:.cpp=.o)

all: gpt efp

gpt: $(XDEVS_OBJS) $(OBJ_GPT) $(OBJ_BASE)
	${CC} ${CFLAGS} $^ -o $@

efp: $(XDEVS_OBJS) $(OBJ_EFP) $(OBJ_BASE)
	${CC} ${CFLAGS} $^ -o $@
	
%.o: %.cpp
	${CC} -c ${CFLAGS} $< -o $@

.PHONY: clean
clean:
	rm -f $(XDEVS_OBJS) $(OBJ_BASE) $(OBJ_GPT) $(OBJ_EFP) gpt efp
