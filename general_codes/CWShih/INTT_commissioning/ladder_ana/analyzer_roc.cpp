#include "analyzer.cpp"

void analyzer_roc(int single_moduleid, int ROC_index)
{

    gErrorIgnoreLevel = kWarning;
  
    //int single_moduleid = 1;
    TString DataIn_directory = "/home/inttdev/data/IR_DAQ_server/INTT_study_run/050523_LVDS_Scan/intt2";
    TString file_name = "calib_packv5_050523_1156_intt2_LVDS_Scan_8_all_FC.root";
    TString Full_MapIn = "/home/inttdev/map/intt2_map.txt";
    
    TString DataOut_directory = "/home/inttdev/data/IR_DAQ_server/INTT_study_run/050523_LVDS_Scan/data_analyzer/intt2";
    
    // int ROC_index = 0;
    int threshold = 15;
    bool full_plots = true;


    int ampl_cut = 54; 
    double used_sigma = 4.5; 
    int classify_base = 0; 
    bool output_basic = 0; 
    bool output_dead_channel_tree = false;

    analyzer * file_1 = new analyzer(DataIn_directory, Full_MapIn, DataOut_directory, ROC_index, file_name, threshold, ampl_cut, used_sigma, classify_base, output_basic, output_dead_channel_tree,1);
    
    // todo : this is for the multi process.
    vector<int> input_module_id = {single_moduleid};

    file_1 -> check_ladder(input_module_id, full_plots, "ampl_total" /*"ampl_total ampl_single"*/);

    cout<<"run finish"<<endl;
}
