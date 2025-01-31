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
  InttCluster.h \
  InttEvent.h \
  InttClusterAnalyzer.h \
  InttVertexXY.h

lib_LTLIBRARIES = \
  libInttClusterAnalyzer.la

libInttClusterAnalyzer_la_SOURCES = \
  InttCluster.cc \
  InttEvent.cc \
  InttClusterAnalyzer.cc \
  InttVertexXY.cc

libInttClusterAnalyzer_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -ltrack \
  -lintt

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libInttClusterAnalyzer.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
