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
  InttRawHitQA.h \
  InttTrkrHitQA.h \
  InttCloneRawHitQA.h \
  InttFineDelayScan.h \
  InttFineDelayScanTrkrHit.h \
  InttQaCommon.h \
  TH2INTT.h

lib_LTLIBRARIES = \
  libInttQA.la

libInttQA_la_SOURCES = \
  InttRawHitQA.cc \
  InttTrkrHitQA.cc \
  InttCloneRawHitQA.cc \
  InttFineDelayScan.cc \
  InttFineDelayScanTrkrHit.cc \
  TH2INTT.cc

libInttQA_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lffaobjects \
  -lffarawobjects \
  -ltrack_io \
  -lintt

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libInttQA.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
