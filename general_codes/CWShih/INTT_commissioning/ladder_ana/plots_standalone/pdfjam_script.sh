#!/bin/bash
folder_name=folder_calib_packv5_042523_0143

#folder_name=$1

pdfjam --fitpaper true ${folder_name}/*_B?L????/Overall_ampl_total_turnOn_*.pdf -o ${folder_name}/Overall_ampl_total_turnOn_all.pdf #2>/dev/null

pdfjam --fitpaper true ${folder_name}/*_B?L????/Overall_ampl_total_width_*.pdf -o ${folder_name}/Overall_ampl_total_width_all.pdf #2>/dev/null

pdfjam --fitpaper true ${folder_name}/*_B?L????/Overall_ch_entry_B?L????.pdf -o ${folder_name}/Overall_ampl_total_ch_entry_all.pdf #2>/dev/null

pdfjam --fitpaper true ${folder_name}/*_B?L????/Overall_ch_entry_ampl_*.pdf -o ${folder_name}/Overall_ampl_total_ch_entry_ampl_all.pdf #2>/dev/null

echo plots merge done
