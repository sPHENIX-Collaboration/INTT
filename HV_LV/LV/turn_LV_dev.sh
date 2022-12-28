#!/bin/bash

script_dir="${HOME}/power"
#script="FVTX_Dist_weiche_beta.pl"
script="turn_FPHX_LV.sh"
script_switch="turn_switch_LV.sh"
script_all_onoff_dir="${HOME}/script"

echo "  +------------------------------------------------------------------------------------------+"
echo "  | This is a shell script to turn on and off the LV system (ROC power and FPHX chip power). |"
echo "  | You need to give one of the argumnet \"on\" or \"off\" to use this script.               |"
echo "  | This script uses:                                                                        |"
echo "  |   - ${script_all_onoff_dir}/turn_switch_LV.sh                                            |"
echo "  |   - ${script_dir}/${script}                                                              |"
echo "  | If you have some problems, check those files are in the directory or not.                |"
echo "  +------------------------------------------------------------------------------------------+"

# check whther only 1 argument is given or not
if [ $# -ne 1 ]
then
    echo "Usage: $0 [on/off]"
    exit
fi

# check whether correct argument is given or not
if [[ $1 != "on" && $1 != "off" ]]
then
    echo "Wrong argument \"$1\" was given."
    echo "only \"on\" or \"off\" are available"
    exit
fi


# move to the directory where the perl script is
cd ${script_dir}
     
# in the case of turning on
echo "Turning LV $1..."
if [ $1 == "on" ]
   then

       ${script_all_onoff_dir}/${script_switch} on       
       sleep 1s

       ${script_all_onoff_dir}/${script} new on
#       perl ${script} on south
#       sleep 1s
#
#       perl ${script} enable south
else

    # in the case of turning off
#    perl ${script} disable south
#    sleep 1s
#
#    perl ${script} off south
#    sleep 1s
    ${script_all_onoff_dir}/${script} new off

    ${script_all_onoff_dir}/${script_switch} off
fi
