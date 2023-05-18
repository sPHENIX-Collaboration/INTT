#!/bin/bash

mother_directory="/home/inttdev/data/IR_DAQ_server/INTT_study_run/BCO_window"
set_name="test_set"

file=$mother_directory/data_analysis/$set_name/file_list.txt

# servername=("intt0" "intt1" "intt2" "intt3" "intt4" "intt5" "intt6" "intt7")

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

while IFS= read -r line; do
    # Process each line
    echo "Line: $line"
    servername=$(echo "$line" | grep -o 'intt[0-7]\+')
    echo servername $servername

    nohup root -l -b -q multiplicity_easy.cpp\(\"$mother_directory\",\"$set_name\",\"$line\",\"$servername\"\)&>run_log/$servername.out&
    sleep 0.4

    # for Open_FC in "${Port_ch[@]}"; 
    # do
    #     nohup root -l -b -q calib_HL.cpp\(\"$mother_directory\",\"$line\",\"${servername[i]}\",$Open_FC\)&>run_log/${servername[i]}_$Open_FC.out&
    #     sleep 0.2
    # done

done < "$file"

