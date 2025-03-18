AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  InttMbdv2.h

lib_LTLIBRARIES = \
  libInttMbdv2.la

libInttMbdv2_la_SOURCES = \
  InttMbdv2.cc

libInttMbdv2_la_LIBADD = \
  -lphool \
  -lffaobjects \
  -lffarawobjects \
  -lSubsysReco \
  -lcentrality \
  -lcalotrigger \
  -ltrackbase_historic_io \
  -ltrack_io \
  -lmbd_io \
  -lepd_io \
  -leventplaneinfo_io

libInttMbdv2_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -I$(ROOTSYS)/include \
  -lfun4all \
  -lg4eval \
  -lcentrality_io

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libInttMbdv2.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
