#!/bin/bash

#array=(0 1 2 3 4 5 6)
#array=(7 8 9 10 11 12 13)
module_array=(0 1 2 3 4 5 6 7 8 9 10 11 12 13) 
roc_array_0=(0 0 0 0 0 0 0 1 1 1 1 1 1 1)
roc_array_1=(2 2 2 2 2 2 2 3 3 3 3 3 3 3)
roc_array_2=(4 4 4 4 4 4 4 5 5 5 5 5 5 5) 

rm run_*.out

for ((i=0; i<${#module_array[@]}; i++));
do
    echo ROC ${roc_array_2[i]}, module ${module_array[i]}
    nohup root -l -b -q analyzer_roc.cpp\(${module_array[i]},${roc_array_2[i]}\)&>run_$i.out&
    sleep 0.2
done


aaa=(1 2 3 4)

#for ((i=0; i<${#aaa[@]}; i++));
#do
#  echo "$i ${aaa[$i]}"
#done


# declare an array called array and define 3 vales
# array=( one two three )
# for ( j=0; j<${array}; j++ );
# do
# 	echo ${array[j]}
# done
