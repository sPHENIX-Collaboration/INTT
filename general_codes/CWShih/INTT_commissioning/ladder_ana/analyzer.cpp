#include "analyzer.h"

analyzer::analyzer(TString DataIn_directory, TString Full_MapIn, TString DataOut_directory, int ROC_index, TString file_name, int threshold, int ampl_cut, double used_sigma, int classify_base, bool output_basic, bool output_dead_channel_tree)
    :DataIn_directory(DataIn_directory),
    Full_MapIn(Full_MapIn),
    DataOut_directory(DataOut_directory),
    file_name(file_name), 
    ROC_index(ROC_index),
    threshold(threshold), 
    ampl_cut(ampl_cut), 
    used_sigma(used_sigma), 
    classify_base(classify_base), 
    output_basic(output_basic), 
    output_dead_channel_tree(output_dead_channel_tree),
    width_check_mode(0)
    {
        map_info.clear();
        ladder_array.clear();
        vector<int> checked_ladder_module;

        analyzer::ReadMap();
        N_ladder = map_info.size();

    }

analyzer::analyzer(TString DataIn_directory, TString Full_MapIn, TString DataOut_directory, int ROC_index, TString file_name, int threshold, int ampl_cut, double used_sigma, int classify_base, bool output_basic, bool output_dead_channel_tree, int width_check_mode)
    :DataIn_directory(DataIn_directory),
    Full_MapIn(Full_MapIn),
    DataOut_directory(DataOut_directory),
    file_name(file_name), 
    ROC_index(ROC_index),
    threshold(threshold), 
    ampl_cut(ampl_cut), 
    used_sigma(used_sigma), 
    classify_base(classify_base), 
    output_basic(output_basic), 
    output_dead_channel_tree(output_dead_channel_tree),
    width_check_mode(width_check_mode)
    {
        map_info.clear();
        ladder_array.clear();
        vector<int> checked_ladder_module;

        analyzer::ReadMap();
        N_ladder = map_info.size();

    }

void analyzer::ReadMap()
{
    //note : the full map directory is given.
    // TString map_name = file_name; map_name = map_name.ReplaceAll(".root","_map.txt");
    
    map_format map_info_unit;

    unsigned long len_unsigned_long = -1;

    fstream map_file;
    map_file.open(Form("%s", Full_MapIn.Data()), ios::in);
    if (map_file.is_open()) 
    {
        string data;
        while (getline(map_file, data)) 
        {
            if ( TString(data[0]) == "#"  )  { continue; }
            if ( TString(data[0]) == " "  )  { continue; }
            if ( TString(data[0]) == ""   )  { continue; }
            if ( TString(data[0]) == "  " )  { continue; }
            if (TString(data).Length() < 2)  { continue; }
            if ( data.find("B")   == len_unsigned_long)     { continue; }

            TString data_TString(data);
            data_TString.ReplaceAll("\t"," "); // note : to replace the "tab, \t" by the space
            TObjArray * data_split = data_TString.Tokenize(" ");

            map_info_unit.module_id = (( (TObjString *) data_split->At(0) ) -> String()).Atoi();
            map_info_unit.port      = ( (TObjString *) data_split->At(1) ) -> String();
            map_info_unit.ladder    = ( (TObjString *) data_split->At(2) ) -> String();
            map_info.push_back(map_info_unit);
        }
        map_file.close();
    }
    
    
    cout<<"-----------------------Map content----------------------------"<<endl;
    for (int i = 0; i < map_info.size(); i++)
    {
        cout<<map_info[i].module_id<<" "<<map_info[i].port<<" "<<map_info[i].ladder<<" "<<endl;
    }

}

void analyzer::dump_root_data()
{
    TFile *f1 = TFile::Open(Form("%s/%s", DataIn_directory.Data(), file_name.Data()));
	TTree *Gettree = (TTree *)f1->Get("tree");

    int adc = 0;
	int ampl = 0;
	int chip_id = 0;
	int fpga_id = 0;
	int module = 0;
	int chan_id = 0;
	int fem_id = 0;
	int bco_full = 0;
	int bco = 0;
	int event = 0;

    Gettree->SetBranchAddress("adc", &adc);
	Gettree->SetBranchAddress("ampl", &ampl);
	Gettree->SetBranchAddress("chip_id", &chip_id);
	Gettree->SetBranchAddress("fpga_id", &fpga_id);
	Gettree->SetBranchAddress("module", &module);
	Gettree->SetBranchAddress("chan_id", &chan_id);
	Gettree->SetBranchAddress("fem_id", &fem_id);
	Gettree->SetBranchAddress("bco", &bco);
	Gettree->SetBranchAddress("bco_full", &bco_full);
	Gettree->SetBranchAddress("event", &event);

    int total_size = Gettree->GetEntriesFast();
	cout << "Total event from the root tree : " << total_size << endl;   

    for (int i = 0; i < total_size; i++)
	{
		Gettree->GetEntry(i);
        for (int i1 = 0; i1 < ladder_array.size(); i1++)
        {
            if (module == ladder_array[i1] -> GetFelixCh())
            {
                ladder_array[i1] -> Fill(chip_id, chan_id, ampl, adc);
                break;
            }
        }

        if (i%100000 == 0) cout<<"running : "<<i<<endl;
    }

    f1 -> Close();
}

// note : checked_ladder_module for the Felix channel, not the index.
void analyzer::check_ladder(vector<int> checked_ladder_module = {}, bool full_plots = false, TString ch_draw_option = "ampl_total ampl_single")
{

    // note : define the ladders to be checked.
    vector<map_format> final_list; final_list.clear();

    // note : convert the Felix channel to the index
    if (checked_ladder_module.size() == 0)
    {
        final_list = map_info;
    }
    else 
    {
        for (int i = 0; i < checked_ladder_module.size(); i++)
        {
            for (int i1 = 0; i1 < map_info.size(); i1++)
            {
                if ( checked_ladder_module[i] == map_info[i1].module_id )
                {
                    final_list.push_back(map_info[i1]);
                    break;
                }
            }
        }
    }

    // note : create the ladder class
    ladder_cali * ladder_unit;
    for (int i = 0; i < final_list.size(); i++)
    {
        cout<<"preparing for : "<<final_list[i].ladder<<" Felix ch : "<<final_list[i].module_id<<endl; 
        ladder_array.push_back(ladder_unit);
        if (width_check_mode == 0)
        {
            ladder_array[i] = new ladder_cali(final_list[i].ladder, ROC_index, final_list[i].port, final_list[i].module_id, threshold, ampl_cut, used_sigma, classify_base, output_basic, output_dead_channel_tree);
        }
        else 
        {
            ladder_array[i] = new ladder_cali(final_list[i].ladder, ROC_index, final_list[i].port, final_list[i].module_id, threshold, ampl_cut, used_sigma, classify_base, output_basic, output_dead_channel_tree, width_check_mode);
        }
    }

    // note : dump the data into the corresponding ladders
    analyzer::dump_root_data();

    // note : create the folder for each ladder
    TString output_folder = file_name; output_folder = output_folder.ReplaceAll(".root","");
    system( Form( "mkdir %s/folder_%s", DataOut_directory.Data(), output_folder.Data() ) );
    for (int i = 0; i < final_list.size(); i++)
    {
        system( Form( "mkdir %s/folder_%s/%i_%s", DataOut_directory.Data(), output_folder.Data(), final_list[i].module_id, final_list[i].ladder.Data()) );
    }

    //note : run the functions.
    vector<int> draw_plot_range = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}; 

    for (int i = 0; i < final_list.size(); i++)
    {
        TString plot_folder_dire = Form( "%s/folder_%s/%i_%s", DataOut_directory.Data(), output_folder.Data(), final_list[i].module_id, final_list[i].ladder.Data());

        ladder_array[i] -> Fit();
        ladder_array[i] -> Overall_info();
        ladder_array[i] -> Classifier();
        ladder_array[i] -> Draw_bad_ch_plot(plot_folder_dire, file_name);
        ladder_array[i] -> Output_bad_ch_root(plot_folder_dire, file_name );
        ladder_array[i] -> Output_ch_property_root(plot_folder_dire, file_name );
        if (full_plots == true) {ladder_array[i] -> Draw_ch_plot(draw_plot_range, plot_folder_dire, file_name, ch_draw_option);}
        ladder_array[i] -> Draw_overall_info(plot_folder_dire, file_name);
        ladder_array[i] -> Output_bad_ch_txt(plot_folder_dire);
    }

    
}

