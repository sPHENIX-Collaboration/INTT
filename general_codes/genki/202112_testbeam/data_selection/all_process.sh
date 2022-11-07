#!/bin/bash

if [ $# -ne 2 ] ;
then
    echo "Usage: $0 [run_num] [is_MC]"
    exit
fi


run=$1
is_MC=$2

data_dir="data/ELPH/"
threshold=5

#int trim_clone_hit( string data_dir = "data/ELPH/", int run_num = 89, int threshold = 5, bool is_MC = false )
echo root -q -b "macro/ELPH/data_selection/trim_clone_hit.cc( \"${data_dir}\", ${run}, ${threshold}, ${is_MC} )"
#time root -q -b "macro/ELPH/data_selection/trim_clone_hit.cc( \"${data_dir}\", ${run}, ${threshold}, ${is_MC} )"


#int make_bco_grouping( int run_num = 89, bool is_MC = false )
echo root -q -b "macro/ELPH/data_selection/make_bco_grouping.cc( ${run}, ${is_MC})"
time root -q -b "macro/ELPH/data_selection/make_bco_grouping.cc( ${run}, ${is_MC})"


#int make_cluster( int run_num = 89, bool is_MC = false  )
echo root -q -b -l "macro/ELPH/data_selection/make_cluster.cc( ${run}, ${is_MC})"
time root -q -b -l "macro/ELPH/data_selection/make_cluster.cc( ${run}, ${is_MC})"
