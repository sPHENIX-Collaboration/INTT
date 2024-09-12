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
  bco_ana_v1.h \
  bco_ana_v2.h \
  bco_ana_v4.h \
  bco_ana_v5.h \
  intt_event_pool.h

lib_LTLIBRARIES = \
  libintt_decoding_check.la

libintt_decoding_check_la_SOURCES = \
  bco_ana_v1.cc \
  bco_ana_v2.cc \
  bco_ana_v4.cc \
  bco_ana_v5.cc \
  intt_event_pool.cc

libintt_decoding_check_la_LIBADD = \
  -lEvent \
  -lffarawobjects \
  -lfun4all \
  -lfun4allraw \
  -lphool \
  -lphoolraw \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libintt_decoding_check.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
