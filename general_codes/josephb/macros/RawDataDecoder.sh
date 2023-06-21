#!/bin/bash

i_format="/sphenix/lustre01/sphnxpro/commissioning/INTT/beam/beam_intt%d-%08d-0000.evt"
o_format="/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt%d-%08d-0000.root"

show_help()
{
	printf "\n"
	printf "\tScript should be called as\n"
	printf "\t\tsh RawDataDecoder.sh [run number] [server]\n"
	printf "\tWhere [run number] is the run number, and\n"
	printf "\tWhere [server] is the felix server\n"
	printf "\n"
	printf "\tThe input PRDF file name is assumed to have the format:\n"
	printf "\t\t%s [server] [run number]\n" $i_format
	printf "\tand the output  DST file name is assumed to have the format:\n"
	printf "\t\t%s [server] [run number]\n" $o_format
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

if [[ $# -ne 2 ]]
then
	show_help
	exit 1
fi

root -l "RawDataDecoder.C(\"$i_format\", \"$o_format\", $1, $2)"
exit 0
