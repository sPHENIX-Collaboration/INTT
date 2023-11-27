#!/bin/bash

I_FORMAT="/sphenix/lustre01/sphnxpro/commissioning/INTT/beam/beam_intt%d-%08d-0000.evt"
O_FORMAT="/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt_combined-%08d-0000.root"

show_help()
{
	printf "\n"
	printf "\tScript should be called as\n"
	printf "\t\tsh channel_classify.sh [run number]"
	printf "\twhere [run number] is the run number to be analyzed\n"
	printf "\n"
	printf "\tThe script checks if the expected combined .root file exists\n"
	printf "\tand runs the InttCombinedRawDataConverter if it does not\n"
	printf "\n"
	printf "\tThe script then runs the classification executable\n"
	printf "\n"
}

if [[ $# -ne 1 ]]
then
	show_help
	exit 1
fi

if [[ $1 == "-h" ]]
then
	show_help
	exit 0
fi

# The following subsection is imitated from the sPHENIX Wiki
# 	https://wiki.sphenix.bnl.gov/index.php/Condor

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="/sphenix/u/jbertaux/MYINSTALL"
export DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )	# The location of this shell script

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

printf -v O_FILE ${O_FORMAT} $1 # Output with respect to .evt -> .root converter, the input to the classifier
if [[ ! -f ${O_FILE} ]]
then
	printf "\n"
	printf "(channel_classify.sh) Output file\n"
	printf "\t${O_FILE}\n"
	printf "(channel_classify.sh) Does not exist and the converter will be run to produce it...\n"
	printf "\n"
	root -l "${DIR}/macro/combined_raw_data_converter.C(\"${I_FORMAT}\", \"${O_FORMAT}\", $1)"
	printf "\n"
	printf "(channel_classify.sh) ...finished\n"
	printf "\n"
fi

# Main execution block
	printf "(channel_classify.sh) Running local classification executable...\n"
	printf "\n"
	${DIR}/bin/main.exe $1 ${O_FILE}
	printf "\n"
	printf "(channel_classify.sh) ...finished\n"
	printf "\n"
# ~Main execution block

printf "\n"
printf "channel_classify.sh:\n"
printf "\tFinished\n"
printf "\n"

exit 0
