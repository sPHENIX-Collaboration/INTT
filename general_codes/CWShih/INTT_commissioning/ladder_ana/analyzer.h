#ifndef analyzer_h
#define analyzer_h

#include "ladder_cali.cpp"

struct map_format{
    int module_id;
    TString port;
    TString ladder;
};

class analyzer
{
    public :
        analyzer(TString DataIn_directory, TString Full_MapIn, TString DataOut_directory, int ROC_index, TString file_name, int threshold, int ampl_cut, double used_sigma, int classify_base, bool output_basic, bool output_dead_channel_tree);
        analyzer(TString DataIn_directory, TString Full_MapIn, TString DataOut_directory, int ROC_index, TString file_name, int threshold, int ampl_cut, double used_sigma, int classify_base, bool output_basic, bool output_dead_channel_tree, int width_check_mode);

        void ReadMap();
        void check_ladder(vector<int> checked_ladder_module, bool full_plots, TString ch_draw_option);
        void dump_root_data();

        vector <int> GetModuleRoot;
        

        // note : the defined parameter
        TString DataIn_directory;
        TString Full_MapIn;
        TString DataOut_directory;
        TString file_name;
        int ROC_index;
        int threshold;
        int ampl_cut;
        double used_sigma;
        int classify_base;
        bool output_basic;
        bool output_dead_channel_tree;

        int N_ladder;
 
        vector<map_format> map_info;
        vector<ladder_cali * > ladder_array;
        vector<int> checked_ladder_module;
        bool full_plots;
        TString ch_draw_option;

    private : 
        int width_check_mode;
};

#endif