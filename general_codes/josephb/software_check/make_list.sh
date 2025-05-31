#!/usr/bin/bash

# creates file lists for all subsystems from known locations in lustre

if [ $# -ne 2 ]; then
cat << EOF
    Usage: $0 [type] [run number]
    Creates needed lists of input files for all subsystems
EOF
	exit 0
fi

type=$1
runnumber=$(printf "%08d" $2)

mkdir -p list

# gl1daq list
/bin/ls -1 /sphenix/lustre01/sphnxpro/physics/GL1/${type}/GL1_*-${runnumber}-* > list/gl1daq.list
if [ ! -s list/gl1daq.list ]; then
  echo gl1daq.list empty, removing it
  rm  list/gl1daq.list
fi

# mvtx list
for i in {0..5}; do
	ls -1 /sphenix/lustre01/sphnxpro/physics/MVTX/${type}/${type}_mvtx${i}-${runnumber}-* > list/mvtx${i}.list
	if [ ! -s list/mvtx${i}.list ]; then
		echo mvtx${i}.list empty, removing it
		rm list/mvtx${i}.list
	fi
done

# intt list
for i in {0..7}; do
	/bin/ls -1 /sphenix/lustre01/sphnxpro/physics/INTT/${type}/${type}_intt${i}-${runnumber}-* > list/intt${i}.list
	if [ ! -s list/intt${i}.list ]; then
		echo intt${i}.list empty, removing it
		rm list/intt${i}.list
	fi
done

# tpc list
for i in {0..23}; do
	ebdc=$(printf "%02d" $i)
	/bin/ls -1 /sphenix/lustre01/sphnxpro/physics/tpc/${type}/TPC_ebdc${ebdc}_${type}-${runnumber}-* > list/tpc${ebdc}.list
	if [ ! -s list/tpc${ebdc}.list ]
	then
		echo tpc${ebdc}.list empty, removing it
		rm  list/tpc${ebdc}.list
	fi
done

# tpot
/bin/ls -1 /sphenix/lustre01/sphnxpro/physics/TPOT/${type}/TPOT_ebdc39_${type}-${runnumber}-* > list/tpot.list
if [ ! -s list/tpot.list ]
then
  echo tpot.list empty, removing it
  rm  list/tpot.list
fi

