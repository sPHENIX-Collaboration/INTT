#!/bin/bash                                                                                                                                    
#echo $1    

folder_direction="/home/5202011/INTT_cal/INTT_cal_test/ladder_cali"
ladder_folder="/ladder_files"
template_version="/template_v1"
 
while read STRING
do

    echo we are now working one : $STRING
    rm $folder_direction$ladder_folder/$STRING/bad_channel_summary.root    
    rm $folder_direction$ladder_folder/$STRING/bad_channel_detail.txt
    rm $folder_direction$ladder_folder/$STRING/output_cut_value.txt
    cp $folder_direction/cut_finder_folder/output_cut_value.txt $folder_direction$ladder_folder/$STRING/
	#rm $folder_direction$STRING/ladder_summary.c
    echo all bad_channel_summary.root have been deleted
    echo all bad_channel_detail.txt   have been deleted    
    sleep 2
    root -l -b -q $folder_direction$template_version/ladder_summary.c\(\"$folder_direction$ladder_folder/$STRING\"\) 
    sleep 2
done

root -l -b -q $folder_direction/final_results.c\(\"$folder_direction\"\)



