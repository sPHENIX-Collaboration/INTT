# Requirement to use

- General library prefix 
- (optional) BCO peak information made by BCOFinder.cc in following link :  
   https://github.com/sPHENIX-Collaboration/INTT/tree/main/general_codes/Jaein/BCOFinder

# How to run 1) Making HitMap
- Compile through autogen.sh method as usual.
- cd macro
- root -l -b -q "Fun4All_Intt_HitMap.C(int nevents = 100, std::string i_file, std::string bco_file, std::string o_file, bool bcoflag)  
              nevents : # of event used to make hitmap  
              i_file : merged root file. The file should include event_based TTree  
              bco_file : input BCO file to apply the BCO cut BEFORE making HitMap  
              o_file : output(hitmap) file  
              bcoflag : flag to turn ON/OFF BCO cut // true : ON / false : OFF  
For example, root -l -b -q "Fun4All_Intt_HitMap.C(100,"your_input_file_with_full_path","your_BCO_file_with_full_path", true)"  
Or, you can specify your arguments inside the code directly.

- Check your outfile (hitmap for every half ladders)

# How to run 2) Gaussian Fitting/Classficiation

- Open InttChannelClassifier.cc
- Check file paths inside the code  
  std::string map_input_path   : hitmap file path (You may need to change the hitmap file name as well.)  
  std::string root_output_path : ROOT file output path  
  std::string csv_output_path  : CSV file output path  
- root -l -b -q 'InttChannelClassifier.cc({runnumber})'
- Check your root output file
  
