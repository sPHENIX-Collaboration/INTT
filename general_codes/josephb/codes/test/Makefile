#!/bin/bash

GXX = g++

CXX = cc
HXX = h
OXX = o

EXE = exe

###---Make edits below---###

FLGS = $(shell root-config --cflags)
INCS = -I$(MYINSTALL)/include -I$(OFFLINE_MAIN)/include -isystem$(ROOTSYS)/include
LIBS = -L$(MYINSTALL)/lib -lintt -L$(OFFLINE_MAIN)/lib -lcdbobjects -lphool -lphparameter $(shell root-config --glibs)

TRGS = \
	main.$(EXE)

OBJS = \
	main.$(OXX)

###---Make edits above---###

.PRECIOUS: $(OBJS)
all: $(TRGS) $(OBJS)

%.$(EXE): %.$(OXX)
	$(GXX) $(FLGS) $(INCS) -o $@ $^ $(LIBS)

%.$(OXX): %.$(CXX)
	$(GXX) $(FLGS) $(INCS) -c $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm $(TRGS) $(OBJS)
