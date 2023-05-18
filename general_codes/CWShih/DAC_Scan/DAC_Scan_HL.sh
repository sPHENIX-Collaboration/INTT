#!/bin/bash

set_name="100V_default_config"
servername=("intt0" "intt1" "intt2" "intt3" "intt4" "intt5" "intt6" "intt7")

close_FC=(
    [0]="3 4 5"   # note : intt0
    [1]=""        # note : intt1
    [2]=""        # note : intt2
    [3]="1 2"     # note : intt3
    [4]=""        # note : intt4
    [5]=""        # note : intt5
    [6]=""        # note : intt6
    [7]=""        # note : intt7
)

for ((i=0; i<${#servername[@]}; i++)); 
do
    Port_ch=(0 1 2 3 4 5 6 7 8 9 10 11 12 13)
    closeFC="${close_FC[i]}"

    for CFC in $closeFC; 
    do
        Port_ch=(${Port_ch[@]/$CFC})
    done

    for Open_FC in "${Port_ch[@]}"; 
    do
        nohup root -l -b -q DAC_Scan_HL.cpp\(\"${set_name}\",\"${servername[i]}\",\{$Open_FC\}\)&>run_log/${servername[i]}_$Open_FC.out&  #"${servername[i]} $Open_FC"
        sleep 0.3
    done

    echo "-----------------------"

done
