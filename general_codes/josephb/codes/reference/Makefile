#!/bin/bash

GXX = g++

CXX = cc
HXX = h
OXX = o

###---Make edits below---###

FLGS = $(shell root-config --cflags)
INCS = -I$(OFFLINE_MAIN)/include -isystem$(ROOTSYS)/include
LIBS = -L$(OFFLINE_MAIN)/lib -lintt $(shell root-config --glibs)

TRGT = main
OBJS = main.$(OXX)

###---Make edits above---###

$(TRGT): $(OBJS)
	$(GXX) $(FLGS) $(INCS) -o $@ $^ $(LIBS)

%.$(OXX): %.$(CXX)
	$(GXX) $(FLGS) $(INCS) -c $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm $(TRGT) $(OBJS)
