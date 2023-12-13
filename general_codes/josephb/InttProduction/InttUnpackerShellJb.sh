#!/bin/bash

i_format="/sphenix/lustre01/sphnxpro/commissioning/INTT/beam/beam_intt%d-%08d-0000.evt"
o_format="/sphenix/user/jbertaux/Data/ChannelClassification/beam_intt_combined-%08d-0000.root"

show_help()
{
	printf "\n"
	printf "\tScript should be called as\n"
	printf "\t\tsh InttUnpackerJb.sh [run number]\n"
	printf "\tWhere [run number] is the run number\n"
	printf "\n"
	printf "\tThe input PRDF file names are assumed to have the format:\n"
	printf "\t\t%s [server] [run number]\n" $i_format
	printf "\tand the output ROOT file name is assumed to have the format:\n"
	printf "\t\t%s [run number]\n" $o_format
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

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# root -q -b "InttUnpackerMacroJb.C(\"$i_format\", \"$o_format\", $1)"
root -l "InttUnpackerMacroJb.C(\"$i_format\", \"$o_format\", $1)"

printf "InttUnpackerMacroJb.sh:"
printf "\tFinished"

exit 0
