#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.452

echo $ROOT_INCLUDE_PATH | sed -e "s/:/\n/g"

process_id=$1

# root.exe -q -b Fun4All_Intt_RecoCluster.C\(${process_id}\)
./Run ${process_id}

echo all done process the 
