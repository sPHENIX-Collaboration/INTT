COMPILER = g++
CFLAGS    = -g -Wall -Wextra -Winit-self -Wno-unused-parameter -Wfloat-equal -std=c++11 -O2
CFLAGS   += `root-config --cflags`
LDFLAGS   =
LDFLAGS  += `root-config --libs`
INCLUDE   = -I../include

ROOTCINT = rootcint
ROOTFLAGS   = `root-config --cflags`
ROOTLIBS    = `root-config --libs`

TRACKMC = TrackMC
EVENT   = Event

SOURCES  = $(wildcard *.cc)
OBJECTS  = $(SOURCES:.cc=.o)
DEPENDS  = $(OBJECTS:.o=.d)
LIBS     = $(wildcard *.so)

all: test

clustering: INTThit.o ../lib/functions.h make_cluster.cc make_cluster.hh
        $(COMPILER) $(CFLAGS) $@.cc -o $@ $^ $(LDFLAGS)




TrackMC: TrackMC.hh LinkDef.h
        $(ROOTCINT) -f $@Dict.cc -rmf lib$@dict.rootmap -rml lib$@.so -c $^
        $(COMPILER) $(CFLAGS) $(ROOTLIBS) -shared $@Dict.cc $@.cc -o lib$@.so

Event: Event.hh LinkDef.h
        $(ROOTCINT) -f $@Dict.cc -rmf lib$@dict.rootmap -rml lib$@.so -c $^
        $(COMPILER) $(CFLAGS) -L. -lTrackMC $(ROOTLIBS) -shared $@Dict.cc $@.cc -o lib$@.so

test: $(TRACKMC) $(EVENT)
        root -q -b -l test.cc

ana: test
        root -q -b -l ana.cc

install: $(LIBS)
        install -s $^ ../session7_solution/lib

clean:
        rm -f *.so *.rootmap
#       @rmdir --ignore-fail-on-non-empty `readlink -f $(OBJDIR)`

#$(TARGET): $(OBJECTS) $(LIBS)
#       $(COMPILER) -o $@ $^ $(LDFLAGS)

#%.o: %.cc
#       @[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
#       $(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<


#-include $(DEPENDS)
