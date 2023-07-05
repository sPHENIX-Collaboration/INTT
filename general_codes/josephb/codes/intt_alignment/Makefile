SHELL=/bin/bash

GXX = g++

CXX = cc
HXX = h
OXX = o

FLGS = $(shell root-config --cflags)
INCS = -I$(OFFLINE_MAIN)/include -isystem$(ROOTSYS)/include -I`root-config --incdir` 
LIBS = -L$(OFFLINE_MAIN)/lib -lintt $(shell root-config --glibs)

BIN = ./bin/
SRC = ./src/

TRGT = ./main.exe

OBJS = \
	$(BIN)test.$(OXX) \
	$(BIN)align/AlignTransform.$(OXX) \
	$(BIN)align/AngleConvention.$(OXX) \
	$(BIN)align/TaitBryanZYX.$(OXX) \
	$(BIN)dtctr/InttSensorReader.$(OXX) \
	#$(BIN)dtctr/InttSensorSurveyReader.$(OXX) \

$(TRGT): $(OBJS)
	mkdir -p $(@D)
	@$(GXX) $(FLGS) $(INCS) -o $@ $^ $(LIBS)

$(BIN)%.$(OXX): $(SRC)%.$(CXX)
	mkdir -p $(@D)
	@$(GXX) $(FLGS) $(INCS) -c $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm $(TRGT) $(OBJS)

.PHONY: objs
objs:
	@echo $(OBJS)
