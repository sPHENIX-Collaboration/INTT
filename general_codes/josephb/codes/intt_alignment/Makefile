SHELL=/bin/bash

GXX = g++

CXX = .cc
HXX = .h
OXX = .o
EXE = .exe

FLGS = $(shell root-config --cflags)
INCS = -I$(OFFLINE_MAIN)/include -isystem$(ROOTSYS)/include -I`root-config --incdir` 
LIBS = -L$(OFFLINE_MAIN)/lib -lintt -ltrack_io -lphool -lcdbobjects $(shell root-config --glibs)

BIN = ./bin/
SRC = ./src/

TRGT = $(BIN)make_cdbttree$(EXE)
# TRGT = $(BIN)main$(EXE)

OBJS = \
	$(BIN)eigen/InttSensorReader$(OXX) \
	$(BIN)eigen/InttLadderReader$(OXX) \
	$(BIN)eigen/Debug$(OXX) \
	$(BIN)make_cdbttree$(OXX)
	# $(BIN)main$(OXX)

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
