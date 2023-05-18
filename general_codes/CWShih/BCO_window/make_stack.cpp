#include "draw_style.h"

// TString color_code [8]={"#343434","#1A3947","#566575","#797983","#EFBD9D","#FCA26E","#F5751D","#F5321D"};
TString color_code_2 [8]={"#CC768D","#19768D","#DDA573","#009193","#941100","#1A3947","#A08144","#517E66"};

void make_stack(TString mother_folder_directory, TString set_name, bool set_logY = true)
{

    // TString mother_folder_directory = "/home/inttdev/data/IR_DAQ_server/INTT_study_run/BCO_window";
    // TString set_name = "test_set";
    // bool set_logY = true;

    vector<TString> file_list_vec; file_list_vec.clear();

    TString ana_directory       = mother_folder_directory + "/data_analysis/" + set_name;
    TString file_list_directory = ana_directory + "/file_list.txt";

    unsigned long len_unsigned_long = -1;

    fstream map_file;
    map_file.open(Form("%s", file_list_directory.Data()), ios::in);
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
            
            TString file_name = TString(std::regex_replace(data, std::regex("^.*/"), "")).ReplaceAll(".root","");

            file_list_vec.push_back(file_name);
        }
        map_file.close();
    }

    THStack * stack_hist = new THStack("stack_hist",Form("Stack, set : %s",set_name.Data()));

    vector<TH1F *> hist_vec(file_list_vec.size(),NULL);

    TLegend * legend = new TLegend (0.75, 0.6, 0.9, 0.90);
    legend -> SetTextSize (0.028);

    //todo : easy assumption, all the servers are, used intt0 to intt7
    //todo : if you have less server to be ploted, modify here.
    for (int i = 0; i < file_list_vec.size(); i++)
    {
        TFile * file_in = new TFile(Form("%s/multiplicity_intt%i_%s.root",ana_directory.Data(),i,file_list_vec[i].Data()));
        
        hist_vec[i] = (TH1F *) file_in->Get(Form("intt%i_hist",i));
        hist_vec[i] -> SetFillColorAlpha(TColor::GetColor(color_code_2[i]),0.6);
        hist_vec[i] -> SetLineWidth(0);
        hist_vec[i] -> GetXaxis() -> SetTitle("N_hit, single event");
        hist_vec[i] -> GetYaxis() -> SetTitle("Entry");
        hist_vec[i] -> SetStats(0);
        
        stack_hist -> Add(hist_vec[i]);

        legend -> AddEntry (hist_vec[i], Form("intt%i",i), "f");
    }

    TCanvas * c1 = new TCanvas("c1","c1",850 ,800);
    TPad * pad_c1 = new TPad(Form("pad_c1"), "", 0.0, 0.0, 1.0, 1.0);
    Characterize_Pad(pad_c1, 0.15,  0.1,  0.1,  0.12, set_logY, 0);
    pad_c1 -> Draw();
    pad_c1 -> cd();
    
    stack_hist -> Draw();   

    stack_hist -> GetXaxis() -> SetTitle("N_hit, single event");
    stack_hist -> GetYaxis() -> SetTitle("Entry");

    legend -> Draw("same");

    TString output_plot_name = Form("%s_stack", set_name.Data());
    output_plot_name += (set_logY == true) ? "_log" : "_linear";    

    c1 -> Print( Form("%s/%s.pdf",ana_directory.Data(), output_plot_name.Data()) );
	c1 -> Clear();




}