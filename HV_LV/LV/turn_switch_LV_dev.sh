#!/bin/bash

# check the argument, it must be only one
if [ $# -ne 1 ] ; then
    echo "Usage: $0 [on/off]"
    echo "       Here [on/off] means turning on or off"
    exit
fi

address="10.20.34.134"
port_address="9760"

# set mode (turning on or off)
if [ ${1} == "on" ] ; then
    mode="FFFF"
elif [ ${1} == "off" ] ; then
    mode="0000"
else
    # if neither on nor off is given, stop it
    echo "The argument should be \"on\" or \"off\"." 

    # show the usage by executing myself without an argument
    $0
    exit
fi

echo "Turning $1 the switch modules ${address} ${port_address}..."

# loop over all modules
for num in `seq -w 1 13`
do
    echo -n -e "\r${num}/13"
    echo "\$E${num}${mode}" | telnet ${address} ${port_address}   >/dev/null 2>/dev/null

    # it's necessary otherwise some commands don't work properly
    sleep 0.1s ;
done
echo -e "\nDone\n"
