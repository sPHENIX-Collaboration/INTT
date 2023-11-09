# For more information about editing this file see the sPHENIX Wiki entry:
#   https://wiki.sphenix.bnl.gov/index.php/Autotools#Makefile.am

# To start to edit this file, do a find and replace for the following package name:
#   intt_example_ana
# Replace with the library name of your choice
# I would recommend choosing something commensurate with the package name in configure.ac

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include  \
  -isystem$(ROOTSYS)/include

# I recommend linking against your own $MYINSTALL location
# This way, you can have your modified builds if you need to test something
# NOTE:
#   You may need to change `MYINSTALL` below
#   If you exported you install location to a different variable name
#   Or you can hardcode a path but I'd advise against that
AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ROOTSYS)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(MYINSTALL)/lib

# Libraries you need to link against for compiliation
# NOTE:
#   You should link against fun4all and SubsysReco if you want to write a Fun4All module
#   You must also link against libraries you will need for analysis
#   e.g. INTT, but most likely also trackbase or other sybsystems for cross-analysis
libintt_example_ana_la_LIBADD = \
  -lffarawobjects \
  -lfun4all \
  -lintt \
  -lSubsysReco

# You need to edit this 
lib_LTLIBRARIES = \
  libintt_example_ana.la

# You should add header (.h) files here
pkginclude_HEADERS = \
  InttExampleAna.h

# You should add implementation (.cc) files here
libintt_example_ana_la_SOURCES = \
  InttExampleAna.cc

################################################
# linking tests

BUILT_SOURCES = \
  testexternals.cc

noinst_PROGRAMS = \
  testexternals_intt_example_ana

testexternals_intt_example_ana_SOURCES = \
  testexternals.cc

testexternals_intt_example_ana_LDADD = \
  libintt_example_ana.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################################################

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
