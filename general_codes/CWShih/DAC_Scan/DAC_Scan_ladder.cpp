#include "DAC_Scan_ladder.h"

void DAC_Scan_ladder()
{

    vector<vector<int>> adc_setting_run =
	{	
		{8  , 12 , 16 , 20 , 24 , 28 , 32 , 36 },
		{28 , 32 , 36 , 40 , 44 , 48 , 52 , 56 },
		{48 , 52 , 56 , 60 , 64 , 68 , 72 , 76 },
		{68 , 72 , 76 , 80 , 84 , 88 , 92 , 96 },
		{88 , 92 , 96 , 100, 104, 108, 112, 116},
		{108, 112, 116, 120, 124, 128, 132, 136},
		{128, 132, 136, 140, 144, 148, 152, 156},
		{148, 152, 156, 160, 164, 168, 172, 176}
	};

    LadderDAC * class_test = new LadderDAC("chip", "B1L114N", 1, "A1", 3, adc_setting_run);
    
    int chip_id;
    int adc;

    TString file_dire = "/home/inttdev/work/cwshih/DAC_Scan/test_file";
    TString plot_folder_dire = "/home/inttdev/work/cwshih/DAC_Scan/test_file/output_randevent_2BKG";
    TString set_name = "test_rand_event_2BKG_random_run_event";

    for (int i = 0; i < adc_setting_run.size(); i++)
    {
        TFile * aaa = new TFile(Form("%s/DACScan_test_twoBKG_randevent_%i.root",file_dire.Data(),i),"read");
        TTree * tree = (TTree *)aaa->Get("tree");
        cout<<"----- test -----"<<endl;
		tree -> SetBranchAddress ("chip_id",&chip_id);
        cout<<"----- test -----"<<endl;
        tree -> SetBranchAddress ("adc",&adc);
        cout<<"run "<<i<<" "<<tree->GetEntries()<<endl;

        for (int i1 = 0; i1 < tree->GetEntries(); i1++)
        {
            tree -> GetEntry(i1);
            class_test -> Fill(i, chip_id, 0, adc);
        }
    }

    
    
    class_test -> Weight();
    class_test -> Fill_final();
    class_test -> Fit();

    class_test -> Output_bin_plots(plot_folder_dire, set_name, true);
    class_test -> Output_comb_plots(plot_folder_dire, set_name);
    class_test -> Output_final_plots(plot_folder_dire, set_name);
    class_test -> Output_final_plots(plot_folder_dire, set_name, 0);
    class_test -> Output_root(plot_folder_dire, set_name);

    class_test -> Spit_BinContent_All();

    
}