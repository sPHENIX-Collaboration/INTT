#include "analyzer.cpp"

void analyzer_standalone_run()
{

    gErrorIgnoreLevel = kWarning;
  
    int single_moduleid = 1;
    TString DataIn_directory = "/home/inttdev/data/IR";
    TString file_name = "calib_packv5_042123_1354.root";
    TString Full_MapIn = "/home/inttdev/data/IR/calib_packv5_042123_1354_map.txt";
    
    TString DataOut_directory = "/home/inttdev/work/cwshih/calib_db/file_logger_PSQL/ladder_ana/plots_standalone";
    
    int ROC_index = 0;
    int threshold = 15;
    bool full_plots = false;


    int ampl_cut = 54; 
    double used_sigma = 4.5; 
    int classify_base = 0; 
    bool output_basic = 0; 
    bool output_dead_channel_tree = false;

    analyzer * file_1 = new analyzer(DataIn_directory, Full_MapIn, DataOut_directory, ROC_index, file_name, threshold, ampl_cut, used_sigma, classify_base, output_basic, output_dead_channel_tree,1);
    
    // todo : this is for the multi process.
    vector<int> input_module_id = {single_moduleid};

    file_1 -> check_ladder(input_module_id, full_plots, /*"ampl_total"*/"ampl_total ampl_single");

    cout<<"run finish"<<endl;
}
