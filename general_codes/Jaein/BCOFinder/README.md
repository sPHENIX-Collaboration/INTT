# BCOFinder

A macros for the BCO distribution and making CDBTTree including BCO peak value.

## General Introduction

### How To Run

1. Ensure you have merged (or individual) event based root files for every felix servers.
2. Check the inputFilePath inside the code. (merged_inputFilePath, inutFilePath)
3. Check the outputFIlePath inside the code. (outputFilePath, cdboutputFilePath)
4. Check number of events used to scan BCO distribution. (int n_event)
   - int n_event = -1 does scan every events. For cosmics, n_event = -1 is recommended.
5. Use ROOT interpreter mode to run the code. 
   - root -l -b -q "BCOFinder.cc({runnumber} , {true/false}, "{runtype}",{true/false})
   - Frist bool is the flag to switch ROOT file type. If your root file is merged one, use true.
   - Second bool is the flag to trun ON/OFF ADC0 cut.
