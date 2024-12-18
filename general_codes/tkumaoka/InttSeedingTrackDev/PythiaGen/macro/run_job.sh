#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}


Nevents=$1
particle_pT=$2
energy_range=$3
particle_species=$4
output_directory=$5
output_filename=$6
ANA_build=$7


echo Nevents = $Nevents
echo particle_pT = $particle_pT
echo energy_range = $energy_range
echo particle_species = $particle_species
echo output_directory = $output_directory
echo output_filename = $output_filename
echo ANA_build = $ANA_build

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.$ANA_build

export MYINSTALL=/sphenix/tg/tg01/commissioning/INTT/work/tkumaoka/InttSeedingTrackDev/PythiaGen/physiTuto/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
export ROOT_INCLUDE_PATH=/sphenix/tg/tg01/commissioning/INTT/repositories/macros/common:${ROOT_INCLUDE_PATH}

echo $ROOT_INCLUDE_PATH | sed -e "s/:/\n/g"

if [ ! -d "$output_directory" ]; then
  mkdir -p $output_directory
fi

sub_foldername=${particle_species}\_${particle_pT}GeV
final_output_directory=${output_directory}/${sub_foldername}
completed_foldername=${final_output_directory}/completed

if [ ! -d "${final_output_directory}" ]; then
    mkdir -p ${final_output_directory}
    mkdir -p ${final_output_directory}/TK_log
    mkdir -p ${completed_foldername}
fi

root.exe -q -b Fun4All_physiTuto.C\($Nevents,$particle_pT,$energy_range,\"$particle_species\",\"$final_output_directory\",\"$output_filename\"\)

output_filename_nosuffix=${output_filename%.root}

echo check the output_filename_nosuffix = ${output_filename_nosuffix} while the original is ${output_filename}

mv ${final_output_directory}/${output_filename} ${completed_foldername}

echo all done
