# a great resource:
# https://makefiletutorial.com/

SHELL=/bin/bash

TRGS = main.exe

SRC_DIR = ./src
OBJ_DIR = ./bld

CC = cc
GG = g++

FLAGS = `root-config --cflags`
INCS = -I`root-config --incdir` #already included with root-config --cflags
LIBS = `root-config --evelibs` -lTMVA -lTMVAGui -lRooFitCore -lRooFitMore -lRooFit

SRCS = $(shell find $(SRC_DIR) -name *.$(CC))
OBJS = $(patsubst $(SRC_DIR)/%.$(CC),$(OBJ_DIR)/%.o,$(SRCS))

$(TRGS) : $(OBJS)
	mkdir -p $(dir $@)
	$(GG) $(FLAGS) $(INCS) -o $@  $(OBJS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(CC)
	mkdir -p $(dir $@)
	$(GG) $(FLAGS) $(INCS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

.PHONY: srcs
srcs:
# list the path to each source relative to the root project directory
	@echo $(SRCS)

.PHONY: objs
objs:
# list the path to each object relative to the root project directory
	@echo $(OBJS)
