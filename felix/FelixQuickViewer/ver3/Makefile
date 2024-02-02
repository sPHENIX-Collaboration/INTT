COMPILER = g++
#COMPILER = clang++
#COMPILER = g++-4.4
CFLAGS    = -g -Wall -Wextra -Winit-self -Wno-unused-parameter -Wfloat-equal -std=c++11
CFLAGS   += `root-config --cflags`
LDFLAGS   =
LDFLAGS  += `root-config --libs`
LIBS      =
INCLUDE   = -I../include

#TARGET   = ../bin/$(shell basename `readlink -f ..`)
TARGET = FelixQuickViewer.exe

#SOURCES  = $(wildcard *.cc)
#SOURCES  = BaseClass.cc        FelixQuickViewer.cc HistMaker.cc        Plotter.cc          ReportMaker.cc      Viewer.cc
#SOURCES  = FelixQuickViewer.cc BaseClass.cc HistMaker.cc
SOURCES  = FelixQuickViewer.cc BaseClass.cc
OBJECTS  = $(SOURCES:.cc=.o)

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	rm -f $(OBJECTS)  $(TARGET) a.out
