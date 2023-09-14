#!/bin/bash
runnumber=$1
# test run number 25992
if [ -z $1 ]
then
  echo "Run Number is not defined."
  echo "Choose the run number you want to use : "  
  echo "bash run.sh {your_run_number}"
  exit 0
fi
root -l -q 'HotChannel.C('$runnumber',0)' &
root -l -q 'HotChannel.C('$runnumber',1)' &
root -l -q 'HotChannel.C('$runnumber',2)' &
root -l -q 'HotChannel.C('$runnumber',3)' &
root -l -q 'HotChannel.C('$runnumber',4)' &
root -l -q 'HotChannel.C('$runnumber',5)' &
root -l -q 'HotChannel.C('$runnumber',6)' &
root -l -q 'HotChannel.C('$runnumber',7)' &
