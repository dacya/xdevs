CC = g++
CFLAGS = -std=c++11 -Wall -g -O3
#CFLAGS = -std=c++11 -Wall -O3


XDEVS_SRC = $(shell find ../../core -type f -name '*.cpp')
XDEVS_OBJS = $(XDEVS_SRC:.cpp=.o)
#XDEVS_LIB_DIR = ../../../../lib

SRC_BASE = Generator.cpp Job.cpp Processor.cpp Transducer.cpp
SRC_EFP = main_efp.cpp Ef.cpp Efp.cpp
SRC_GPT = main_gpt.cpp Gpt.cpp
OBJ_BASE = $(SRC_BASE:.cpp=.o)
OBJ_EFP = $(SRC_EFP:.cpp=.o)
OBJ_GPT = $(SRC_GPT:.cpp=.o)

all: main_gpt main_efp

main_gpt: $(XDEVS_OBJS) $(OBJ_BASE) $(OBJ_GPT)
	${CC} ${CFLAGS} $^ -o $@

#main_gpt: $(OBJ_BASE) $(OBJ_GPT)
#	${CC} ${CFLAGS} $^ -o $@ -L ./$(XDEVS_LIB_DIR) -lxdevs-1.0.0

main_efp: $(XDEVS_OBJS) $(OBJ_BASE) $(OBJ_EFP)
	${CC} ${CFLAGS} $^ -o $@

#main_efp: $(OBJ_BASE) $(OBJ_EFP)
#	${CC} ${CFLAGS} $^ -o $@ -L ./$(XDEVS_LIB_DIR) -lxdevs-1.0.0

%.o: %.cpp
	${CC} -c ${CFLAGS} $< -o $@

.PHONY: clean
clean:
	rm -f $(XDEVS_OBJS) $(OBJ_BASE) $(OBJ_GPT) $(OBJ_EFP)
#clean:
#	rm -f $(OBJ_BASE) $(OBJ_GPT) $(OBJ_EFP)

uninstall:
	rm -f main_gpt main_efp
