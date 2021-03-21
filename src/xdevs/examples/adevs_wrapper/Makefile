CC = g++
CFLAGS = -std=c++11 -Wall -O3

XDEVS_LIB_DIR = ../../../../lib
SRC_BASE = AtomicADEVS.cpp Generator.cpp Job.cpp Transducer.cpp
SRC_GPT = main_gpt.cpp Gpt.cpp
OBJ_BASE = $(SRC_BASE:.cpp=.o)
OBJ_EFP = $(SRC_EFP:.cpp=.o)
OBJ_GPT = $(SRC_GPT:.cpp=.o)

all: gpt

gpt: $(OBJ_GPT) $(OBJ_BASE)
	${CC} ${CFLAGS} $^ -o $@ -L ./$(XDEVS_LIB_DIR) -lxdevs-1.0.0
	
%.o: %.cpp
	${CC} -c ${CFLAGS} $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ_BASE) $(OBJ_GPT) gpt
