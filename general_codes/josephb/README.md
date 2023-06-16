# Working repository for commits from Joseph Bertaux
## Please do not commit to this repository unless you have talked about it with me first

## Overview
* macros:
	* The code I will maintain here are specialized macros
	* Most will depend on libraries I have built under [INTT's Offline Packages Respository](https://github.com/sPHENIX-Collaboration/coresoftware/tree/master/offline/packages/intt)
	* Instructions for building the libraries on your system and using them can be found in the [Coresoftware README](https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/README.md) and in the [README](macros/README.md) in my macros subdirectory
* codes:
	* These are subdirectories containing (mostly) self contained, compilable C or C++ projects (there are also the rare Python ones...)
	* I say "(mostly) self contained" since, with some hacking, you can link against most [sPHENIX coresoftware](https://github.com/sPHENIX-Collaboration/coresoftware) by setting the proper envirnoment variables and/or sourcing the right scripts
	* If you haven't set these up on your environment, it might not work, but sometimes its worthwhile to try since it's better to compile things and also a good learning experience
