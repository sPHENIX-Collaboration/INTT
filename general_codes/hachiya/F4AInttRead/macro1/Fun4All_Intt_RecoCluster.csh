#!/bin/csh
setenv HOME /phenix/u/$LOGNAME
source /etc/csh.login
foreach i (/etc/profile.d/*.csh)
  source $i
end
source $HOME/.login
source /opt/sphenix/core/bin/sphenix_setup.csh -n
source /opt/sphenix/core/bin/setup_local.csh /sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/F4AInttRead/install

#@ nevents = "100"
#set ifile   = "/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00021048-0000_event_base_ana.root"
#set ofile   = "AnaTutorial_run21048.root"
#set dacfile = "dac_21048.txt"
#set bcofile = "bco_21048.txt"
#set adc7flag = "true"
#@ runno    = 21048

set nevents  = $1
set ifile    = $2
set ofile    = $3
set dacfile  = $4
set bcofile  = $5
set adc7flag = $6
set runno    = $7

echo $nevents
echo $ifile
echo $ofile
echo $dacfile
echo $bcofile
echo $adc7flag
echo $runno

#echo 'Fun4All_Intt_RecoCluster.C('$nevents',"'"$ifile"'","'"$ofile"'","'"$dacfile"'","'"$bcofile"'",'"$adc7flag"','"$runno"')'
#root -b -q 'Fun4All_Intt_RecoCluster.C('$nevents',"'"$ifile"'","'"$ofile"'","'"$dacfile"'","'"$bcofile"'",'"$adc7flag"','"$runno"')'
echo Fun4All_Intt_RecoCluster.C\($nevents,\"$ifile\",\"$ofile\",\"$dacfile\",\"$bcofile\",$adc7flag,$runno\)
root -b -q  Fun4All_Intt_RecoCluster.C\($nevents,\"$ifile\",\"$ofile\",\"$dacfile\",\"$bcofile\",$adc7flag,$runno\)
  
#echo "all done"
