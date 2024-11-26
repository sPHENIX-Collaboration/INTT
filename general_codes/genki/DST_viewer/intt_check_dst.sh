#!/bin/bash

# If a single argument is not given, show how to use it
if [ $# -ne 1 ] ;
then
    echo " [Usage]: ${0##*/} [path to a DST file]"
    exit
fi

if [ ${1} == "debug" ] ;
then
    time root -l -q -b /sphenix/tg/tg01/commissioning/INTT/work/genki/repos/INTT/general_codes/genki/DST_viewer/show_dst_contents.cc
elif [ ! -f ${1} ] ;
then
    echo "${1} is not found."
    exit
fi

#time root -l -q -b /sphenix/tg/tg01/commissioning/INTT/work/genki/repos/INTT/general_codes/genki/DST_viewer/show_dst_contents.cc
