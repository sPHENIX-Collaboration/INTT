# InttJosephB
(`README.md` last updated Nov 10 2023)

## Description
* This is a project for my own development of INTT analysis and calibration tools
	* It is both a place for me to become better at using sPHEINX and ROOT software tools
	* As well as write earnestly useful software by imitating, improving, and specializing projects elsewhere
* I use a name-mangling scheme for the modules in this repository
	* The package and library name are the same (`intt_jb`)
	* Class names are suffixed my intials in a camel-case format (`...Jb`)
	* This allows them to be safely installed in locations where name conflict is likely

## Contents
### `InttHitJb`
My implementation of an INTT position class.
There are other versions due to Takashi and Chris, but I wanted to implement something
(1) to learn how to write a class that can be written to `TFile`s and `TTree`s, and
(2) that contains the following advantages and functionality:

* Minimal memory storage
	* The hardware position of a hit can be uniquely determined by its:
		* felix server (`flx_svr`, 0-7)
		* felix module/channel (`flx_chn`, 0-13)
		* chip index (`chp`, 0-25)
		* channel (`chn`, 0-127)
	* Any hardware parameterization is isomorphic to these fields and other values can be deduced from them
	* Method to assist in the InttEvtJb streamer to ensure minimal writing of bytes for each version
* Custom comparators for pointers to this class
	* Allows for sorted maps with ln(n) complexity lookup
	* Uses dynamic calls, so it will work for all versions

### `InttEvtJb`
My implementation of an INTT evt class. Stores a vector of hits as well as the full gtm\_bco using the first hit at the start of the event.

* Minimal memory storage
	* Uses a custom `Streamer(TBuffer&)` override to write the minimal number of bytes requried for each hit
	* In general this depends on the version of the hit
	* Avoids extra class information that would've been recursively generated with ROOT's method
	* Combind file storage is less than the sum of the original .prdf inputs (for example, 11.6G .root file from 13.3G of .prdf files)
* `begin()` and `end()` methods so the hits can be iterated over
	* Note that once the iterator is dereferenced, it is still a pointer

### `InttUnpackerJb`
An unpacker using these new classes
