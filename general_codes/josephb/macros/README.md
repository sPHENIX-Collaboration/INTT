# Macros

## Contents and descriptions
* Example.C and Example.sh
	* These are minimal implementations the mostly serve as templates or references for what certain includes and syntax should be.
* CombinedRawDataConverter.C and CombinedRawDataConverter.sh
	* Very similar to the RawDataConverter.C and RawDataConverter.sh described below
	* However, these will search for and combine .evt files from all INTT FEEs in a single .root file
	* Needs to be cross-checked but works at a superficial level at least for now
* RawDataDecoder.C and RawDataDecoder.s
	* These are for producing one DST file per one PRDF file (for now)
	* run `sh RawDataDecoder.sh -h` for more information
	* I may need to change the default location of the output files
* RawDataConverter.C and RawDataConverter.sh
	* These are for producing one ROOT file per one PRDF file (for now)
	* The output is a ROOT TTree with branches for decoded information
	* The entries are organized by event, so values that vary hit-to-hit have multiple instances per event
	* run `sh RawDataConverter.sh -h` for more information
	* I may need to change the default location of the output files and potentially the format
* ConvenctionCheck.C
	* This creates 2D histograms with a layout commensurate with the actual layout of any ladder of the INTT
	* run `root ConventionCheck.C` to run the macro; it will automatically produce several histograms
	* The bin content is set based on a hardware index, so in effect a ladder is drawn with how the strips are labeled
	* It is designed to show how we are labeling locations in hardware so:
		* We can check for consistency over different conventions (e.g., against the tracking group's convention)
		* We can verify we are labeling and referring to the detector in the way we want

## How to run these macros
Ideally, if you have a recent build of the [sPHENIX coresoftware](https://github.com/sPHENIX-Collaboration/coresoftware) repository, you can do
```
root MacroName.C
```
where `MacroName.C` is the root macro you want to run.

For convenience, I tend to create accompanying bash scripts to run the macros, since the syntax for passing arguments to a root macro is awkward. So alternatively you can do something such as
```
sh MacroName.sh arg1 arg2
```
or
```
./MacroName.sh arg1 arg2
```
where `MacroName.sh` is a shell script that will run `MarcoName.C` with arg1 and arg2 passed as commandline args. Macros will detail how many arguments they need and what they are. Usually either the shell script or the macro (normally both) will have guard clauses that tell you what was wrong with the given arguments and return if there was an issue.

## Setup
However, it is more likely you need to:

0. You should be on a filesystem where sPHENIX setup scripts and builds are available
1. Clone the contents [sPHENIX coresoftware](https://github.com/sPHENIX-Collaboration/coresoftware) to your system
2. Follow the instructions from the [coresoftware README](https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/README.md) to set up your build environment and `make install` the needed libraries.
	* You will likely only need to navigate to the [INTT's Offline Packages Respository](https://github.com/sPHENIX-Collaboration/coresoftware/tree/master/offline/packages/intt) and `make install` those libraries
3. Once these have been done, you can run the macro by following the instructions above
