AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I/sphenix/u/nukazuka/INTT_work/work/genki/repos/INTT/general_codes/hachiya/F4AInttRead/install/include/inttread \
  -isystem$(ROOTSYS)/include \
  -isystem`root-config --incdir`

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -ltrack_io \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  InttAnalysisCosmic.h

lib_LTLIBRARIES = \
  libInttAnalysisCosmic.la

# $(ROOTDICTS)

#libevent_la_SOURCES = \
#	event.cc

libInttAnalysisCosmic_la_SOURCES = \
	InttAnalysisCosmic.cc \
	event.cc \
	event.hh

libInttAnalysisCosmic_la_LIBADD = \
  -L$(OFFLINE_MAIN)/lib \
	-lphool \
	-lfun4all \
	-ltrack \
	-lSubsysReco


#-L. -levent

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

#ROOTDICTS = \
#  InttAnalysisCosmic_Dict.cc

#pcmdir = $(libdir)

#nobase_dist_pcm_DATA = \
#  InttAnalysisCosmic_Dict_rdict.pcm

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libInttAnalysisCosmic.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

%_Dict.cc: %.h %LinkDef.h
	rootcling -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#libevent.so: event.cc event.hh

lib%.so: 
	$(CXX) $(CFLAGS) -shared -fPIC `root-config --cflags` $(CFLAGS_SHARE) $(AM_CPPFLAGS) -o $@  $< $(LDFLAGS)
#	$(COMPILER) $(CFLAGS) $(CFLAGS_SHARE) $(INCLUDE) -o $@  $< $(LDFLAGS)

#libInttAnalysisCosmic.so: libevent.so
libInttAnalysisCosmic.o: event.cc event.hh

#just to get the dependency
#%_Dict_rdict.pcm: %_Dict.cc ;
#%_rdict.pcm: %_Dict.cc ;

clean-local:
	rm -f $(BUILT_SOURCES) *Dict* *.pcm 
