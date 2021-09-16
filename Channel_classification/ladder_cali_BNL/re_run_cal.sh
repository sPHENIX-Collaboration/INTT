#!/bin/bash                                                                                                                                    
#echo $1    

folder_direction="/home/5202011/INTT_cal/INTT_cal_test/ladder_cali"
ladder_folder="/ladder_files"
template_version="/template_v1"
 
while read STRING
do

    echo we are now working on : $STRING
    cd $folder_direction$ladder_folder/$STRING/
    rm -r folder_NCU_fphx_raw_module* 
    rm multi_run_status.txt
    rm bad_channel_summary.root    
    rm bad_channel_detail.txt
    rm bad_channel_map.pdf
    rm calibration_ana_code_multi.c
    rm run_output run_output.txt
    cp $folder_direction$template_version/calibration_ana_code_multi.c .
 
    sleep 2
   
    nohup sh run.sh &>run_output.txt&

    #cp $folder_direction/cut_finder_folder/output_cut_value.txt $folder_direction$ladder_folder/$STRING/
    #rm $folder_direction$STRING/ladder_summary.c
    #echo all bad_channel_summary.root have been deleted
    #echo all bad_channel_detail.txt   have been deleted    
    sleep 2
    #root -l -b -q $folder_direction$template_version/ladder_summary.c\(\"$folder_direction$ladder_folder/$STRING\"\) 
    #sleep 2
done

#root -l -b -q $folder_direction/final_results.c\(\"$folder_direction\"\)



