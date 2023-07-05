# Codes

## Contents and descriptions
* example/
	* An example repository which serves as a template with a preferred/typical Makefile that links against [INTT's Offline Packages Respository](https://github.com/sPHENIX-Collaboration/coresoftware/tree/master/offline/packages/intt), and a simple executable with basic includes (a "Hello World" that links against libintt.so and uses one of the headers there)
* intt_alignment
	* For analyzing survey data (in the form of specifically formatted .txt files) to obtain the align transforms to pass on to the survey group

## How to run these codes
0. For codes that link against [sPHENIX coresoftware](https://github.com/sPHENIX-Collaboration/coresoftware) libraries, you should be running them from a filesystem that has been configured as described in the [coresoftware README](https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/README.md)
1. Run `./main` where `main` is the name of the executable, including any extension.
	* You can run the exectuable from outside the directory by specifying the relative or full path to the executable
	* e.g, `./full/path/to/main` or `../../relative/path/to/main`
	* Some executables will take command line arguments and would be run as `main arg1 arg2`
	* There should be an indication which positional command line args it takes in corresponding README and guard clauses in the executable itself
2. If you need to build the executable, navigate to the directory and run `make`.
3. If you want to remove the local object files and binaries that have been built in that directory, run `make clean`.

