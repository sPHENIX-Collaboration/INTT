SHELL=/bin/bash

GXX = g++

HXX = h
CXX = cc
OXX = o
EXE = exe

FLGS = $(shell root-config --cflags) -g
INCS = -I$(OFFLINE_MAIN)/include -isystem$(ROOTSYS)/include -I`root-config --incdir` 
LIBS = -L$(OFFLINE_MAIN)/lib -lintt -ltrack_io -lcdbobjects $(shell root-config --glibs)

BIN = ./bin/
SRC = ./src/

REQS = \
	write_local._$(CXX)_$(OXX)_$(EXE)_ \
	check_local._$(CXX)_$(OXX)_$(EXE)_ \
	compare._$(CXX)_$(OXX)_$(EXE)_ \
	compare_jaein._$(CXX)_$(OXX)_$(EXE)_ \
	write_cdbttree._$(CXX)_$(OXX)_$(EXE)_ \
	cheng_wei._$(CXX)_$(OXX)_$(EXE)_ \
	channel_classifier_helper._$(HXX)_$(CXX)_$(OXX)_ \
	channel_classifier/base._$(HXX)_$(CXX)_$(OXX)_ \
	channel_classifier/ver_1._$(HXX)_$(CXX)_$(OXX)_

HXX_BASENAMES := $(foreach R,$(REQS),$(if $(findstring _$(HXX)_,$(suffix $R)),$(basename $R)))
CXX_BASENAMES := $(foreach R,$(REQS),$(if $(findstring _$(CXX)_,$(suffix $R)),$(basename $R)))
OXX_BASENAMES := $(foreach R,$(REQS),$(if $(findstring _$(OXX)_,$(suffix $R)),$(basename $R)))
EXE_BASENAMES := $(foreach R,$(REQS),$(if $(findstring _$(EXE)_,$(suffix $R)),$(basename $R)))

OXX_BASENAMES := $(filter-out $(EXE_BASENAMES),$(OXX_BASENAMES))

HXXS := $(HXX_BASENAMES:%=$(SRC)%.$(HXX))
CXXS := $(CXX_BASENAMES:%=$(SRC)%.$(CXX))
OXXS := $(OXX_BASENAMES:%=$(BIN)%.$(OXX))
EXES := $(EXE_BASENAMES:%=$(BIN)%.$(EXE))

.PRECIOUS: $(OXXS)
all: $(EXES) $(OXXS)

# $(OXXS): $(HXXS) $(CXXS)
# This line says each object is dependent on ALL source files
# Which is undesireable

$(BIN)%.$(EXE): $(BIN)%.$(OXX) $(OXXS)
	@echo -e "\n"
	@echo -e "Target: "$@
	@echo -e "Dependencies: "$^
	@mkdir -p $(@D)
	$(GXX) $(FLGS) $(INCS) -o $@ $^ $(LIBS)
	@echo -e "\n"

$(BIN)%.$(OXX): $(SRC)%.$(CXX) $(if $(findstring %,$(HXX_BASENAMES)),$(SRC)%.$(HXX))
	@echo -e "\n"
	@echo -e "Target: "$@
	@echo -e "Dependencies: "$^
	@mkdir -p $(@D)
	$(GXX) $(FLGS) $(INCS) -c $< -o $@ $(LIBS)
	@echo -e "\n"

.PHONY: clean
clean:
	rm $(EXES) $(OXXS)

.PHONY: oxxs
oxxs:
	@echo $(OXXS)
