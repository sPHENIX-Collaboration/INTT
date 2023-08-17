SHELL=/bin/bash

GXX = g++

CXX = .cc
HXX = .h
OXX = .o
EXE = .exe

FLGS = $(shell root-config --cflags)
INCS = -I$(OFFLINE_MAIN)/include -isystem$(ROOTSYS)/include -I`root-config --incdir` 
LIBS = -L$(OFFLINE_MAIN)/lib -lintt -ltrack_io $(shell root-config --glibs)

BIN = ./bin/
SRC = ./src/

TRGT = $(BIN)main$(EXE)

OBJS = \
	$(BIN)test$(OXX) \
	$(BIN)eigen/InttSensorReader$(OXX) \
	$(BIN)eigen/InttLadderReader$(OXX)
	#$(BIN)align/AlignTransform$(OXX) \
	#$(BIN)align/AngleConvention$(OXX) \
	#$(BIN)align/TaitBryanZYX$(OXX) \
	#$(BIN)dtctr/InttLadderReader$(OXX)

$(TRGT): $(OBJS)
	@mkdir -p $(@D)
	$(GXX) $(FLGS) $(INCS) -o $@ $^ $(LIBS)

$(BIN)%$(OXX): $(SRC)%$(CXX)
	@mkdir -p $(@D)
	$(GXX) $(FLGS) $(INCS) -c $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm $(TRGT) $(OBJS)

.PHONY: objs
objs:
	@echo $(OBJS)
