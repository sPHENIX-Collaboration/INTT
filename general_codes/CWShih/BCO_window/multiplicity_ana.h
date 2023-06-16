#ifndef Multiplicity_ana_h
#define Multiplicity_ana_h

TString color_code [8]={"#343434","#1A3947","#566575","#797983","#EFBD9D","#FCA26E","#F5751D","#F5321D"};
TString color_code_2 [8]={"#CC768D","#19768D","#DDA573","#009193","#6E9193","#941100","#A08144","#517E66"};

struct map_format{
    int module_id;
    TString port;
    TString ladder;
};

struct full_hit_info {
    int FC;
    int chip_id;
    int chan_id;
    int adc;
};

struct hit_info {
    int chip_id;
    int chan_id;
    int adc;
};

map<TString,int> port_map = {
    {"A1", 0}, {"A2", 1}, {"A3", 2}, 
    {"B1", 3}, {"B2", 4}, {"B3", 5}, 
    {"C1", 6}, {"C2", 7}, {"C3", 8}, 
    {"D1", 9}, {"D2", 10}, {"D3", 11}
};

// note : the reason not to start with "0" is because that the return value would be 0 if the match not found 
map<TString,int> layer_map = {
    {"B0L0",1},
    {"B0L1",2},
    {"B1L0",3},
    {"B1L1",4}
};

class Multiplicity
{
    public :
        Multiplicity(TString ladder_name, int ROC_index, TString ladder_port, int Felix_ch, int chip_N_bin, int chip_L_edge, int chip_R_edge);

        void Fill( vector<hit_info> HL_single_event_info ); // note : hit_info : chip_id, chan_id, adc
        void Draw_plot( TString plot_folder_dire, TString set_name );
        void Output_root( TString plot_folder_dire, TString set_name );


    private : 
        vector<int> event_hit_vec; // note : 26 spaces for 26 chips. "event_hit_vec.assign(26,0)"
        TH1F * event_hit_hist[27]; // note : 26 chips + 1 total, the half-ladder
        int chip_N_bin;
        int chip_L_edge;
        int chip_R_edge;
};

Multiplicity::Multiplicity(TString ladder_name, int ROC_index, TString ladder_port, int Felix_ch, int chip_N_bin, int chip_L_edge, int chip_R_edge)
:ladder_name(ladder_name),
ROC_index(ROC_index),
ladder_port(ladder_port),
Felix_ch(Felix_ch),
chip_N_bin(chip_N_bin),
chip_L_edge(chip_L_edge),
chip_R_edge(chip_R_edge)
{
    cout<<"Initialization start"<<endl;

    event_hit_vec.assign(26,0);

    for (int hid = 0; hid < 27; hid++) // note : hid = hist ID, hist[26] is the whole half ladder 
    {
        event_hit_hist[hid] = (hid == 26) ? new TH1F("","", 26 * chip_N_bin, chip_L_edge, 26 * chip_R_edge) : new TH1F("", "", chip_N_bin, chip_L_edge, chip_R_edge);
        
        event_hit_hist[hid] -> GetXaxis() -> SetTitle("N_hit, single event");
        event_hit_hist[hid] -> GetYaxis() -> SetTitle("Entry");
        event_hit_hist[hid] -> SetLineColor( TColor::GetColor(color_code[1]) );
        event_hit_hist[hid] -> SetLineWidth(3);
        event_hit_hist[hid] -> SetStats(0);

        event_hit_hist[hid] -> SetTitle( (hid == 26) ? ("Chip level, all chips") : Form(Form("Chip level, U%i",hid+1)))
    
    }

    cout<<"hist config check, chip unit, Bin width : "<<event_hit_hist[2] -> GetBinWidth(2)<<" range : "<<event_hit_hist[2] -> GetXaxis() -> GetXmin()<<" to "<<event_hit_hist[2] -> GetXaxis() -> GetXmax()<<endl;
    cout<<"hist config check, ALL, Bin width : "<<event_hit_hist[26] -> GetBinWidth(2)<<" range : "<<event_hit_hist[26] -> GetXaxis() -> GetXmin()<<" to "<<event_hit_hist[26] -> GetXaxis() -> GetXmax()<<endl;


    cout<<"Initialization done"<<endl;

}

void Multiplicity::Fill(vector<hit_info> HL_single_event_info)
{
    event_hit_vec.assign(26,0);

    for (int i = 0; i < HL_single_event_info.size(); i++) // todo : we can add some selections here, I think.
    {
        event_hit_vec[ HL_single_event_info[i].chip_id - 1 ] += 1;
    }

    for (int i = 0; i < 26; i++) // note : chip
    {
        event_hit_hist[i] -> Fill( event_hit_vec[i] ); // todo : or if (event_hit_vec[i] != 0) ??
    }

    // todo : of if( HL_single_event_info.size() != 0 ) ??
    event_hit_hist[26] -> Fill( HL_single_event_info.size() ); // note : the ALL hist, number of hits in this event in this half-ladder

    event_hit_vec.assign(26,0);
}

void Multiplicity::Draw_plot(TString plot_folder_dire, TString set_name)
{
    TCanvas * c1 = new TCanvas("c1","c1",800,800);

    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.025);

    c1 -> Print( Form("%s/multiplicity_%s.pdf(", plot_folder_dire.Data(), ladder_name.Data()) );
	for (int i = 0; i < 27; i++) // note : chip + total
    {	
		event_hit_vec[i] -> Draw("hist");
        
        draw_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, Port %s",set_name.Data(),Felix_ch,ladder_port.Data()));

		c1 -> Print( Form("%s/multiplicity_%s.pdf",plot_folder_dire.Data(), ladder_name.Data()) );
        c1 -> Clear();
	}
	c1 -> Print( Form("%s/multiplicity_%s.pdf)",plot_folder_dire.Data(), ladder_name.Data()) );
	c1 -> Clear();
}

void Multiplicity::Output_root(TString plot_folder_dire, TString set_name)
{
    TFile * file_output = new TFile(Form("%s/%s_multiplicity_hist.root", plot_folder_dire.Data(), ladder_name.Data()), "RECREATE");
    file_output -> cd();

    for (int i = 0; i < 27; i++) // note : chip + total
    {	
		event_hit_hist[i] -> Write();
	}

    file_output -> Close();
}

#endif