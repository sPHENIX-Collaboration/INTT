# For more information about editing this file see the sPHENIX Wiki entry:
#   https://wiki.sphenix.bnl.gov/index.php/Autotools#Makefile.am

# Library name:
#   intt_josephb

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include  \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ROOTSYS)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(MYINSTALL)/lib

################################################
# Name of the library to make
lib_LTLIBRARIES = \
  libintt_josephb.la

################################################
# Libraries you need to link against for compiliation
libintt_josephb_la_LIBADD = \
  -lffarawobjects \
  -lfun4all \
  -lintt \
  -lSubsysReco

################################################
# ROOT dictionaries
ROOTDICTS = \
  InttHitJb_Dict.cc

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  InttHitJb_Dict_rdict.pcm

################################################
# Source files
pkginclude_HEADERS = \
  InttHitJb.h \
  InttProductionJb.h

libintt_josephb_la_SOURCES = \
  $(ROOTDICTS) \
  InttHitJb.cc \
  InttProductionJb.cc

################################################
# Linking tests

BUILT_SOURCES = \
  testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = \
  testexternals.cc

testexternals_LDADD = \
  libintt_josephb.la

################################################
# Makefile rules

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
