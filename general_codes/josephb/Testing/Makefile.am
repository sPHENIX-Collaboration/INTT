AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

lib_LTLIBRARIES = \
  libtest.la

libtest_la_LIBADD = \
  -lphool \
  -lSubsysReco

pkginclude_HEADERS = \
  foo.h \
  test.h

libtest_la_SOURCES = \
  foo.cc \
  test.cc

ROOTDICTS = \
  foo_Dict.cc \
  test_Dict.cc
 
pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  foo_Dict_rdict.pcm \
  test_Dict_rdict.pcm

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libtest.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
