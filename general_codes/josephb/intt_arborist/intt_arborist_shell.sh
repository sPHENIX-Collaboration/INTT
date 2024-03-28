#!/bin/bash

# You may need to change these lines to suit your needs
# If you do not have an install directory, set MYINSTAL=""
i_format="/sphenix/lustre01/sphnxpro/commissioning/INTT/beam/beam_intt%d-%08d-0000.evt"
o_format="/direct/sphenix+u/jbertaux/sphnx_software/INTT/general_codes/josephb/intt_arborist/dat/combined-%08d.root"
MYINSTALL="/sphenix/u/jbertaux/MYINSTALL"

show_help() {
	printf "\n"
	printf "\tusage:\n"
	printf "\t\t./intt_arborish_shell.sh [run number]\n"
	printf "\n"
	printf "\tThe input PRDF file names are assumed to have the format:\n"
	printf "\t\t${i_format} [fee] [run number]\n"
	printf "\tand the output ROOT file name will be formated as:\n"
	printf "\t\t${o_format} [run number]\n"
	printf "\n"
}

if [[ $# -ne 1 ]]; then
	show_help
	exit 1
fi

if [[ $1 == "-h" ]]; then
	show_help
	exit 0
fi

# The following subsection is imitated from the sPHENIX Wiki
# 	https://wiki.sphenix.bnl.gov/index.php/Condor

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
if [ -n "${MYINSTALL}" ]; then
	export MYINSTALL=${MYINSTALL}
	source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
fi

root -q -b "intt_arborist_macro.C(\"$i_format\", \"$o_format\", $1)"

printf "intt_arborist_shell.sh:\n"
printf "\tfinished\n"

exit 0
