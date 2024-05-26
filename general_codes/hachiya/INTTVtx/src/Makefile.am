AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  InttXYVertexFinder.h \
  InttZVertexFinder.h \
  InttVertex.h \
  InttVertexv1.h \
  InttVertexMap.h \
  InttVertexMapv1.h \
  InttVertex3D.h \
  InttVertex3Dv1.h \
  InttVertex3DMap.h \
  InttVertex3DMapv1.h

noinst_HEADERS = \
  INTTZvtx.h \
  INTTXYvtx.h \
  InttVertexUtil.h 

lib_LTLIBRARIES = \
  libInttXYVertexFinder.la

libInttXYVertexFinder_la_SOURCES = \
  $(ROOTDICTS) \
  InttXYVertexFinder.cc \
  InttZVertexFinder.cc \
  InttVertexUtil.cc \
  InttVertexv1.cc \
  InttVertexMap.cc \
  InttVertexMapv1.cc \
  InttVertex3Dv1.cc \
  InttVertex3DMap.cc \
  InttVertex3DMapv1.cc

libInttXYVertexFinder_la_LIBADD = \
  -lphool \
  -lg4detectors_io \
  -ltrack_io \
  -ltrackbase_historic_io \
  -lglobalvertex \
  -lSubsysReco

ROOTDICTS = \
  InttVertex_Dict.cc \
  InttVertexv1_Dict.cc \
  InttVertexMap_Dict.cc \
  InttVertexMapv1_Dict.cc \
  InttVertex3D_Dict.cc \
  InttVertex3Dv1_Dict.cc \
  InttVertex3DMap_Dict.cc \
  InttVertex3DMapv1_Dict.cc 
  

pcmdir = $(libdir)

nobase_dist_pcm_DATA = \
  InttVertex_Dict_rdict.pcm \
  InttVertexv1_Dict_rdict.pcm \
  InttVertexMap_Dict_rdict.pcm \
  InttVertexMapv1_Dict_rdict.pcm \
  InttVertex3D_Dict_rdict.pcm \
  InttVertex3Dv1_Dict_rdict.pcm \
  InttVertex3DMap_Dict_rdict.pcm \
  InttVertex3DMapv1_Dict_rdict.pcm
  


# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;



BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libInttXYVertexFinder.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm

