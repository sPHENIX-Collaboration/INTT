#!/bin/bash

i_format="/sphenix/lustre01/sphnxpro/commissioning/INTT/beam/beam_intt%d-%08d-0000.evt"
o_format="/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt_combined-%08d-0000.root"

show_help()
{
	printf "\n"
	printf "\tScript should be called as\n"
	printf "\t\tsh RawDataConverter.sh [run number]\n"
	printf "\tWhere [run number] is the run number\n"
	printf "\n"
	printf "\tThe input PRDF file names are assumed to have the format:\n"
	printf "\t\t%s [server] [run number]\n" $i_format
	printf "\tand the output ROOT file name is assumed to have the format:\n"
	printf "\t\t%s [run number]\n" $o_format
	printf "\n"
}

if [[ $# -lt 1 ]]
then
	show_help
	exit 1
fi

if [[ $1 == "-h" ]]
then
	show_help
	exit 0
fi

if [[ $# -ne 1 ]]
then
	show_help
	exit 1
fi

root -l "CombinedRawDataConverter.C(\"$i_format\", \"$o_format\", $1)"
exit 0
