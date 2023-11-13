# InttProduction 
(`README.md` last updated Nov 13 2023)

## Description
* Directory to manage submission of unpacker jobs to Condor.
* I may also put keep small scripts that can be run locally here
	* e.g., `Test.C`
To use:
* Do `sh WriteCondorJob.sh [run number]` to start a condor job unpacking that run into a file
	* You can check where by looking at the job output under `out/out_[run number].txt`
	* Or by checking the value `o_format` in `InttUnpackerShellJb.sh`

To personalize:
* Edit the variable `o_format` in `InttUnpackerShellJb.sh`:4 to your desired output directory
	* Please do this
	* It's set to somewhere under my username
	* You might not have write permission there
	* I don't want you to write files there if you do have permissions
* Edit the variable `MYINSTALL` in `InttUnpackerShellJb.sh`:38 to your install path
	* If you intend to use the workflow as is, you need to have my `intt_josephb` package built and installed
* You should add the `out/`, `err/`, and `job/` subdirectories to your `.gitignore`
	* By default calling 

## Contents
### `InttUnpackerMacroJb.C`
The ROOT macro that actually uses and calls my library, starts `Fun4All`, and processes PRDF files.
* Currently, the only analysis module in the `Fun4All` workflow is my own custom unpacker
* The input and output file formats are passed as arguments to the macro by `InttUnpackerShellJb.sh`
	* Therefore, this macro should not need to be edited unless the `Fun4All` analysis being performed must be updated
	* But in that case, it would be better to write a separate macro and bash wrapper

### `InttUnpackerShellJb.sh`
The bash script wrapper that runs `InttUnpackerMacroJb.C`
* Sources and exports the things you would expect so the macro can find the libraries it needs
	* This includes my own `$MYINSTALL` path, which is hardcoded as a variable in with the same name
	* The input and output formats are also set here; the output is put in one of my directories
* Otherwise, the script gets other variables in general ways
* To 

### `WriteCondorJob.sh`
This creates a condor job submission file (and if necessary, a job/ subdirectory here) and submits that job to condor
* Run as
	* `sh WriteCondorJob.sh [run number]`
	* where `[run number]` is the run number you want to unpack
* Variables (such as `USER`, `LOGNAME`, `HOME`, and the script location) are determined in the script
	* Thus, you shouldn't need to edit it
	* However, you may want to change the options in the condor job submission file it writes
