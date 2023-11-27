SHELL=/bin/bash

GXX = g++

CXX = .cc
HXX = .h
OXX = .o
EXE = .exe

FLGS = $(shell root-config --cflags)
INCS = -I$(OFFLINE_MAIN)/include -isystem$(ROOTSYS)/include -I`root-config --incdir` 
LIBS = -L$(OFFLINE_MAIN)/lib -lintt -ltrack_io $(shell root-config --glibs)#-lRooFit -lRooFitCore -lRooFitMore

BIN = ./bin/
SRC = ./src/

TRGT = $(BIN)main$(EXE)

SRCS = \
	test \
	channel_classifier

OBJS = $(patsubst %, $(BIN)%$(OXX), $(SRCS))

$(TRGT): $(OBJS)
	@echo -e "\n"$@
	@mkdir -p $(@D)
	$(GXX) $(FLGS) $(INCS) -o $@ $^ $(LIBS)

$(BIN)%$(OXX): $(SRC)%$(CXX)
	@echo -e "\n"$@
	@mkdir -p $(@D)
	$(GXX) $(FLGS) $(INCS) -c $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm $(TRGT) $(OBJS)

.PHONY: objs
objs:
	@echo $(OBJS)
