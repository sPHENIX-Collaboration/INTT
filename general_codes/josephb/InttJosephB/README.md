# InttJosephB
(`README.md` last updated Nov 10 2023)

## Description
* This is a project for my own development of INTT analysis and calibration tools
	* It is both a place for me to become better at using sPHEINX and ROOT software tools
	* As well as write earnestly useful software by imitating, improving, and specializing projects elsewhere
* I use a name-mangling scheme for the modules in this repository
	* The package and library name are the same (`intt_josephb`)
	* Class names are suffixed my intials in a camel-case format (`...Jb`)
	* This allows them to be safely installed in locations where name conflict is likely

## Contents
### `InttHitJb`
My implementation of an INTT hit class.
There are other versions due to Takashi and Chris, but I wanted to implement something
(1) to learn how to write a class that can be written to `TFile`s and `TTree`s, and
(2) that contains the following advantages and functionality:

* Minimal memory storage
	* The hardware position of a hit can be uniquely determined by its:
		* felix server (`flx_svr`, 0-7)
		* felix module/channel (`flx_chn`, 0-13)
		* chip index (`chp`, 0-25)
		* channel (`chn`, 0-127)
	* Any hardware parameterization is isomorphic to these fields
	* These are read directly from hardware and are the best to use
	* All data members are wrapped in a struct with specified bitfields
		* All data members fit into a total 9 bytes
* Implementation of `operator<` and `operator==`
	* The other comparison operators are implemented in terms of these
	* This implementation allows `InttHitJb` to be used as the key type for sorted STL containers

### `InttProductionJb`
This is a `SubsysReco` module designed to analyze and unpack PRDF files into ROOT files for subsystem specific analysis.
It uses the same decoder as the offical unpacker and 
