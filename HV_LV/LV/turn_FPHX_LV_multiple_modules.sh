#!/bin/bash

if [ $# -ne 2 ] ; then
    echo "Usage: $0 [old/new] [on/off]"
    echo "       Here [old/new] means using the old or new network board on the controller module"
    echo "            [on/off] means turning on or off"
    exit
fi

if [ $1 == "old" ] ; then
    echo "run Wei-Che's script..."
elif [ $1 == "new" ] ; then

    address="10.20.34.137"
    port_address="4660"
    echo "The port and address of the controller module is ${address} ${port_address}"
    
    # 2 digits, You can find it from the switch on the module
    #module_id="01"
    echo "Turn all ports in the modules $2..."
    
    if [ ${2} == "on" ] ; then
	mode="FF"
    else
	mode="00"
    fi

    for module in `echo 1 2 4` ; do
	echo "Turning the module ${module} ${2}..."
	for port in `seq 0 3` ; do	
	    #command="echo \"\$E${module_id}${port}${mode}\" | telnet ${address} ${port_address}"	
	    command="echo \"\$E0${module}${port}${mode}\" | telnet ${address} ${port_address}"	
	    #echo $command
	    echo "\$E0${module}${port}${mode}" | telnet ${address} ${port_address} >/dev/null 2>/dev/null
	    echo -n -e "\r `echo \"${port}+1\" | bc`/4"
	    sleep 0.5s
	done
	echo ""
    done
    echo -e "\nDone"
else
    echo "Given 1st argument is $1"
    echo "It should be \"old\" or \"new\""
    exit
fi
