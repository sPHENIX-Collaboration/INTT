#!/bin/bash

mother_directory="/home/inttdev/data/IR_DAQ_server/INTT_study_run/051123_LVDS_Scan"

servername=("intt0" "intt3" "intt4" "intt5" "intt7")

close_FC=(
    [0]=""        # note : intt0
    [1]=""        # note : intt1
    [2]=""        # note : intt2
    [3]=""        # note : intt3
    [4]=""        # note : intt4
    [5]=""        # note : intt5
    [6]=""        # note : intt6
    [7]=""        # note : intt7
)

file_pattern="*.txt"

for ((i=0; i<${#servername[@]}; i++)); 
do

    Port_ch=(0 1 2 3 4 5 6 7 8 9 10 11 12 13)
    closeFC="${close_FC[i]}"

    for CFC in $closeFC; 
    do
        Port_ch=(${Port_ch[@]/$CFC})
    done

    for file in $mother_directory"/data_analysis/"${servername[i]}/"file_list.txt";
    do
        echo "Processing file: $file"

        while IFS= read -r line; do
            # Process each line
            echo "Line: $line"

            for Open_FC in "${Port_ch[@]}"; 
            do
                nohup root -l -b -q calib_HL.cpp\(\"$mother_directory\",\"$line\",\"${servername[i]}\",$Open_FC\)&>run_log/${servername[i]}_$Open_FC.out&
                sleep 0.2
            done

        done < "$file"

    done

    sleep 300
    echo "-----------------------"

done