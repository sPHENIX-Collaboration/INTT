AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
  libInttQAEval.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OPT_SPHENIX)/lib

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(OPT_SPHENIX)/include \
  -isystem$(ROOTSYS)/include

pkginclude_HEADERS = \
  InttQAEval.h

libInttQAEval_la_SOURCES = \
  InttQAEval.cc

libInttQAEval_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OPT_SPHENIX)/lib \
  -lffamodules \
  -lphool \
  -lcdbobjects \
  -lodbc++

# linking tests
BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD = libInttQAEval.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
