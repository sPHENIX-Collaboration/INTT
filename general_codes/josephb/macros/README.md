# Macros

## Contents and descriptions
* Reference.C and Reference.sh
	* These are minimal implementations the mostly serve as templates or references for what certain includes and syntax should be.
* ...

## How to run these macros
Ideally, if you have a recent build of the [sPHENIX coresoftware](https://github.com/sPHENIX-Collaboration/coresoftware) repository, you can do
```
root MacroName.C
```
where `MacroName.C` is the root macro you want to run.

For convenience, I tend to create accompanying bash scripts to run the macros, since the syntax for passing arguments to a root macro is awkward. So alternatively you can do something such as
```
bash MacroName.sh arg1 arg2
```
or
```
./MacroName.sh arg1 arg2
```
where `MacroName.sh` is a shell script that will run `MarcoName.C` with arg1 and arg2 passed as commandline args. Macros will detail how many arguments they need and what they are. Usually either the shell script or the macro (normally both) will have guard clauses that tell you what was wrong with the given arguments and return if there was an issue.

However, it is more likely you need to:
0. You should be on a filesystem where sPHENIX setup scripts and builds are available
1. Clone the contents [sPHENIX coresoftware](https://github.com/sPHENIX-Collaboration/coresoftware) to your system
2. Follow the instructions from the [coresoftware README](https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/README.md) to set up your build environment and `make install` the needed libraries.
	* You will likely only need to navigate to the [INTT's Offline Packages Respository](https://github.com/sPHENIX-Collaboration/coresoftware/tree/master/offline/packages/intt) and `make install` those libraries
3. Once these have been done,
