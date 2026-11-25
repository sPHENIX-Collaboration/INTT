#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

#source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.464
source /opt/sphenix/core/bin/sphenix_setup.sh
# source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.504


export MYINSTALL=/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/install

export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

echo $ROOT_INCLUDE_PATH | sed -e "s/:/\n/g"

process=$1
runnumber=$2
nEvents=$3
file_path_to_hot_channel_map=$4
save_tree=$5

echo process: ${process}
echo runnumber: ${runnumber}
echo nEvents: ${nEvents}
echo file_path_to_hot_channel_map: ${file_path_to_hot_channel_map}

root.exe -q -b Fun4All_HitCarryoverMonitor.cc\(${process},${runnumber},${nEvents},\"${file_path_to_hot_channel_map}\",${save_tree}\)

echo all done process the Fun4All_Intt_SanityCheck.C, process_id: ${process}
