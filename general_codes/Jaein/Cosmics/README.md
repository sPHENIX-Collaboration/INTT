# InttCosmicsFinder

A macros for the CosmicsFinder for INTT under fun4all framework.

## General Introduction

1. HitMapGenerator
   - InttHitMap generator for cosmics run.
   - This module should be linked before working on macro directory
2. ClusterTTree
   - Main analysis module to find cosmics track / to save ClusterTTree after fitting is performed.
   - This module should be linked before working on macro directory.
3. macro
   - All macros are exsisted here.
   - User can be able to create CDB hot channel and cluster DST file after hot channel masking.

### How To Run
## Macro directory
Please check the inside of code for each step, especially path and name of input/output file

1. Execute Fun4All_Intt_HitMap.C 
- INTTRAWHIT DST file is required. Output is HitMap for every half ladders.
   - root -l -b -q 'Fun4All_Intt_HitMap.C(int run_num, int nevents, string inputfile)'

2. Execute InttChannelClassifier_cosmics.cc 
 - Do classification to peak up hot channel, will give you cdb_{runnumber}.root and InttHotDeadMap_{runnumber}_30.root
   - root -l -b -q 'InttChannelClassifier_cosmics.cc(int run_num)'

3. Check cdb_output_path inside Fun4All_Intt_Combiner_run_base.C and execute it.
- Make DST file which includes TRKR_HITSET after hotchannel masking.
   - root -l -b -q 'Fun4All_Intt_Combiner_run_base.C(int run_num, int nEvents, is_debug=true)'
  *Note : Do not change the is_debug flag if it is not necessary.
4. Execute Fun4All_Clustering.C
 -  Do Clustering will give you DST file includes TRKR_CLUSTER.
5. Execute Fun4All_CosmicAnalysis.C

