CC = g++
CFLAGS = -std=c++11 -Wall -O3

XDEVS_LIB_DIR = ../../../../lib
SRC_BASE = Generator.cpp Job.cpp Processor.cpp Transducer.cpp
SRC_EFP = main_efp.cpp Ef.cpp Efp.cpp
SRC_GPT = main_gpt.cpp Gpt.cpp
OBJ_BASE = $(SRC_BASE:.cpp=.o)
OBJ_EFP = $(SRC_EFP:.cpp=.o)
OBJ_GPT = $(SRC_GPT:.cpp=.o)

all: gpt efp

gpt: $(OBJ_GPT) $(OBJ_BASE)
	${CC} ${CFLAGS} $^ -o $@ -L ./$(XDEVS_LIB_DIR) -lxdevs-1.0.0

efp: $(OBJ_EFP) $(OBJ_BASE)
	${CC} ${CFLAGS} $^ -o $@ -L ./$(XDEVS_LIB_DIR) -lxdevs-1.0.0
	
%.o: %.cpp
	${CC} -c ${CFLAGS} $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ_BASE) $(OBJ_GPT) $(OBJ_EFP) gpt efp
