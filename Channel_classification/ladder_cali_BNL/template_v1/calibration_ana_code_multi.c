//INTT calibration_ana_code
//Cheng-Wei Shih, NCU, Taiwan

#include <stdio.h>
#include <iostream>
#include "TRandom.h"
#include "TMath.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TProfile.h"
#include "TRandom.h"
#include "TTree.h"
#include <TFile.h>
#include <TH1.h>
#include <TH2D.h>
#include <TH3D.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TLatex.h>
#include "TGraphErrors.h"
#include <TTree.h>
#include <TLine.h>
#include <TAxis.h>
#include <TPad.h>
#include <TColor.h>
#include "Riostream.h"
#include <vector>
#include <fstream>
#include <string.h> //to add string with the tree name
#include <TSystem.h>
#include <TGraphAsymmErrors.h>
#include <TH1D.h>
#include <TF1.h>
#include <TLorentzVector.h>
//#include <iomanip>
//#include "untuplizer.h"
//#include "sigmaEff.h"

double RC_eq1(double *t, double *par)
{
	return par[0] * t[0] + par[1];
}


double shifted_hist(TH1F* h_org, TH1F* h_new, float offset = 0)
{
  for(int i =0; i<h_org->GetNbinsX(); i++) {
    float x = h_org->GetBinCenter(i+1);
    int bincontent = h_org->GetBinContent(i+1);

    float x_new = x - offset;  // new bin center                                                                                                                                                           

    int new_bin_id = h_new->FindBin(x_new);
    h_new->SetBinContent(new_bin_id, bincontent);    
  }

  return 0;
}





//void name with "copy" is correct
void calibration_ana_code_multi_copy(TString folder_name, int module_number, bool run_option, bool assembly_check, int noise_level_check, bool new_check, bool unbound_check, bool noise_channel_check, bool multi_run)
{
	//===============criteria variable===================

	int entry_cut_l=280; //all entry cut lower
	int entry_cut_h=400; //all entry cut high

	double Gaus_width_cut = 4.; //the cut of fit gaussian width

	int threshold_cut = 45; //the threshold cut of ampl
	int threshold_cut_hist_l=0; 
	int threshold_cut_hist_h=300;

	//===============criteria variable===================


	TString list_buffer;
	vector<TString> list_array;
	list_array.clear();
	TString the_name;

	ifstream data_list;
	data_list.open(Form("%s/total_file.txt", folder_name.Data()));

	while (1)
	{
		data_list >> list_buffer;
		list_array.push_back(list_buffer);
		if (!data_list.good())
		{
			break;
		}
	}

	cout << "test list size : " << list_array.size() << endl;
	int run_file = data_index; // data index is not declared, it has to be changed by .sh file
	the_name = list_array[run_file];

	cout << Form(" !!! The input data name : %s", the_name.Data()) << endl;
	system(Form("mkdir -p %s/folder_%s", folder_name.Data(), the_name.Data()));
	TFile *f1 = TFile::Open(Form("%s/%s", folder_name.Data(), the_name.Data()));
	TTree *Gettree = (TTree *)f1->Get("tree");

	TString data_buffer_a, data_buffer_b, data_buffer_c, data_buffer_d, data_buffer_e;
	vector<TString> data_a;
	data_a.clear();
	vector<TString> data_b;
	data_b.clear();
	vector<TString> data_c;
	data_c.clear();
	vector<TString> data_d;
	data_d.clear();
	vector<TString> data_e;
	data_e.clear();

	// TString DB_NB, DB_ND;
	// double DB_NA, DB_NC, DB_NE;
	// vector<double>D_NA; D_NA.clear();
	// vector<TString>D_NB; D_NB.clear();
	// vector<double>D_NC; D_NC.clear();
	// vector<TString>D_ND; D_ND.clear();
	// vector<double>D_NE; D_NE.clear();

	// TString DB_EB, DB_ED;
	// double DB_EA, DB_EC, DB_EE;
	// vector<double>D_EA; D_EA.clear();
	// vector<TString>D_EB; D_EB.clear();
	// vector<double>D_EC; D_EC.clear();
	// vector<TString>D_ED; D_ED.clear();
	// vector<double>D_EE; D_EE.clear();


	int chip_ER, chan_ER;
	int chip_ERM, chan_ERM;

	
	int chip_N, chan_N;
	int chip_E, chan_E, level_E;
	int chip_U, chan_U;
	int chip_W, chan_W;
	int chip_CE, chan_CE, level_CE;
	int chip_CE_ampl, chan_CE_ampl, level_CE_ampl;
	int chip_TP, chan_TP;
	int chip_rchi2, chan_rchi2;
	int chip_adc_root, chan_adc_root;
	double slope_value, slope_rchi2, width_value, width_rchi2;
	double root_adc_0, root_adc_1, root_adc_2, root_adc_3, root_adc_4;
	double root_adc_0N, root_adc_1N, root_adc_2N, root_adc_3N, root_adc_4N;


	double level_N, level_U, level_W, level_W02, level_W13, level_ADC0, level_ADC1, level_ADC2, level_ADC3, level_pvalue, mean_ADC0, mean_ADC1, mean_ADC2, mean_ADC3;
	//TString source_name;

	double entries_mean, entries_width;
	double entries_mean_ampl, entries_width_ampl;

	double TP_turnon, TP_width, TP_height;

	TFile *file_output = new TFile(Form("%s/folder_%s/%s_summary.root", folder_name.Data(), the_name.Data(), the_name.Data()), "RECREATE");
	//file_output->cd();
	TTree *tree_output1 = new TTree("Noise_check", Form("With bias 100V, noise level > %1.f",Gaus_width_cut));
	TTree *tree_output2 = new TTree("Entries_check", Form("With bias 100V, Entries > %d or entries < %d",entry_cut_h,entry_cut_l));
	TTree *tree_output3 = new TTree("Unbonded_check", Form("Without bias, Noise level < %1.f",Gaus_width_cut));
	TTree *tree_output4 = new TTree("chan_gaus_width", "gaus width of each channel");
	TTree *tree_output5 = new TTree("Total_entries", "Total entries distribution");
	TTree *tree_output6 = new TTree("chan_entry", "entry of each channel");
	TTree *tree_output7 = new TTree("ampl_total_entries", "Total entries distribution after ampl cut");
	TTree *tree_output8 = new TTree("ampl_channel_entries", "entry of each channel after ampl cut");
	TTree *tree_output9 = new TTree("chan_thre_position", "threshold_position of each channel");
	TTree *tree_output10 = new TTree("chan_study", "slop, width, chi2 of each channel");
	TTree *tree_output11 = new TTree("chan_adc_study", "mean of each adc of each channel");
	TTree *tree_output12 = new TTree("chan_err_width", "err width of each channel");
	TTree *tree_output13 = new TTree("chan_err_mean", "err mean of each channel");




	tree_output12->Branch("source_id", &run_file);
	tree_output12->Branch("chip_id", &chip_ER);
	tree_output12->Branch("chan_id", &chan_ER);
	tree_output12->Branch("err_width0", &level_ADC0);  // the ADC0 "err fit width" of each channel (3328 channels)
	tree_output12->Branch("err_width1", &level_ADC1); // the ADC1 "err fit width" of each channel (3328 channels)
	tree_output12->Branch("err_width2", &level_ADC2); // the ADC2 "err fit width" of each channel (3328 channels)
	tree_output12->Branch("err_width3", &level_ADC3); // the ADC3 "err fit width" of each channel (3328 channels)
	tree_output12->Branch("err_pvalue", &level_pvalue); 

	
	tree_output13->Branch("source_id", &run_file);
	tree_output13->Branch("chip_id", &chip_ERM);
	tree_output13->Branch("chan_id", &chan_ERM);
	tree_output13->Branch("err_mean0", &mean_ADC0);  // the ADC0 "err fit mean" of each channel (3328 channels)
	tree_output13->Branch("err_mean1", &mean_ADC1); // the ADC1 "err fit mean" of each channel (3328 channels)
	tree_output13->Branch("err_mean2", &mean_ADC2); // the ADC2 "err fit mean" of each channel (3328 channels)
	tree_output13->Branch("err_mean3", &mean_ADC3); // the ADC3 "err fit mean" of each channel (3328 channels)

	

	tree_output1->Branch("source_id", &run_file);
	tree_output1->Branch("chip_id", &chip_N);
	tree_output1->Branch("chan_id", &chan_N);
	tree_output1->Branch("noise_level", &level_N); // the channel whose fit gaussian width is > 4 

	tree_output2->Branch("source_id", &run_file);
	tree_output2->Branch("chip_id", &chip_E);
	tree_output2->Branch("chan_id", &chan_E);
	tree_output2->Branch("entries", &level_E); // the channel whose # of entry > 400 or entry < 280 (with no ampl cut)

	tree_output3->Branch("source_id", &run_file);
	tree_output3->Branch("chip_id", &chip_U);
	tree_output3->Branch("chan_id", &chan_U);
	tree_output3->Branch("noise_level", &level_U); // the channel whose fit gaussian width is < 4

	tree_output4->Branch("source_id", &run_file);
	tree_output4->Branch("chip_id", &chip_W);
	tree_output4->Branch("chan_id", &chan_W);
	tree_output4->Branch("Gaus_width", &level_W); // the "gaussian fit width" of each channel (3328 channels)
	tree_output4->Branch("Gaus_width02", &level_W02); // the ADC0 and ADC2 "gaussian fit width" of each channel (3328 channels)
	tree_output4->Branch("Gaus_width13", &level_W13); // the ADC1 and ADC3 "gaussian fit width" of each channel (3328 channels)


	tree_output5->Branch("source_id", &run_file);
	tree_output5->Branch("entries_mean", &entries_mean);
	tree_output5->Branch("entries_width", &entries_width); // the "mean and width " of channel entry from 1D hist (all channels in hist) 

	tree_output6->Branch("source_id", &run_file);
	tree_output6->Branch("chip_id", &chip_CE);
	tree_output6->Branch("chan_id", &chan_CE);     //entry of each channel without ampl cut
	tree_output6->Branch("chan_entry", &level_CE); //the # of event of each channel (3328 channels)

	tree_output7->Branch("source_id", &run_file);
	tree_output7->Branch("entries_mean", &entries_mean_ampl);
	tree_output7->Branch("entries_width", &entries_width_ampl); // the "mean and width " of channel entry from 1D hist (all channels in hist) 

	tree_output8->Branch("source_id", &run_file);
	tree_output8->Branch("chip_id", &chip_CE_ampl);
	tree_output8->Branch("chan_id", &chan_CE_ampl);     //entry of each channel after ampl cut
	tree_output8->Branch("chan_entry", &level_CE_ampl); //the # of event of each channel (3328 channels)

	tree_output9->Branch("source_id", &run_file);
	tree_output9->Branch("chip_id", &chip_TP);
	tree_output9->Branch("chan_id", &chan_TP);
	tree_output9->Branch("TP_turnon", &TP_turnon); 
	tree_output9->Branch("TP_width", &TP_width);   // threshold_position of each channel (3328 channels)
	tree_output9->Branch("TP_height", &TP_height); // chan_ampl_1D all channels

	tree_output10->Branch("source_id", &run_file);
	tree_output10->Branch("chip_id", &chip_rchi2);
	tree_output10->Branch("chan_id", &chan_rchi2);
	tree_output10->Branch("slope_value", &slope_value); //linearity study, slope value of each channel (3328 channels)
	tree_output10->Branch("slope_rchi2", &slope_rchi2); //linearity study, reduced chi2 of slope of each channel (3328 channels)
	tree_output10->Branch("width_value", &width_value); //"width consistence" study, average width value of each channel (3328 channels)
	tree_output10->Branch("width_rchi2", &width_rchi2); //"width consistence" study, reduced chi2 of pol1 fit of each channel. (3328 channels)

	tree_output11->Branch("source_id", &run_file);
	tree_output11->Branch("chip_id", &chip_adc_root);
	tree_output11->Branch("chan_id", &chan_adc_root);
	tree_output11->Branch("adc0", &root_adc_0);  // mean of ADC0 distribution of each channel (3328 channels)
	tree_output11->Branch("adc1", &root_adc_1);
	tree_output11->Branch("adc2", &root_adc_2);
	tree_output11->Branch("adc3", &root_adc_3);
	tree_output11->Branch("adc4", &root_adc_4);
	tree_output11->Branch("adc0N", &root_adc_0N);  // numeric mean of ADC0 distribution of each channel (3328 channels)
	tree_output11->Branch("adc1N", &root_adc_1N);
	tree_output11->Branch("adc2N", &root_adc_2N);
	tree_output11->Branch("adc3N", &root_adc_3N);
	tree_output11->Branch("adc4N", &root_adc_4N);



	if (multi_run == true)
	{
		ifstream input_data;
		input_data.open(Form("%s/multi_run_status.txt", folder_name.Data()));

		while (1)
		{
			input_data >> data_buffer_a >> data_buffer_b >> data_buffer_c >> data_buffer_d >> data_buffer_e;
			data_a.push_back(data_buffer_a);
			data_b.push_back(data_buffer_b);
			data_c.push_back(data_buffer_c);
			data_d.push_back(data_buffer_d);
			data_e.push_back(data_buffer_e);

			if (!input_data.good())
			{
				break;
			}
			//the_test_event.push_back(n+1);
		}

		// ifstream input_data_N;
		// input_data_N.open(Form("%s/multi_run_noise.txt"));

		// while ( 1 )
		// {
		//   input_data_N >> DB_NA >> DB_NB >> DB_NC >> DB_ND >> DB_NE ;
		//   D_NA.push_back(DB_NA);
		//   D_NB.push_back(DB_NB);
		//   D_NC.push_back(DB_NC);
		//   D_ND.push_back(DB_ND);
		//   D_NE.push_back(DB_NE);

		//   if ( !input_data_N.good() )
		//     {
		//       break;
		// 	}
		//   //the_test_event.push_back(n+1);
		// }

		// ifstream input_data_E;
		// input_data_E.open(Form("%s/multi_run_entries.txt"));

		// while ( 1 )
		// {
		//   input_data_E >> DB_EA >> DB_EB >> DB_EC >> DB_ED >> DB_EE ;
		//   D_EA.push_back(DB_EA);
		//   D_EB.push_back(DB_EB);
		//   D_EC.push_back(DB_EC);
		//   D_ED.push_back(DB_ED);
		//   D_EE.push_back(DB_EE);

		//   if ( !input_data_E.good() )
		//     {
		//       break;
		// 	}
		//   //the_test_event.push_back(n+1);
		// }
	}

	ofstream multi_txt;
	if (multi_run == true)
	{

		multi_txt.open(Form("%s/multi_run_status.txt", folder_name.Data()), ios::out); //¶}±ÒÀÉ®×
		if (!multi_txt)
		{ //¦pªG¶}±ÒÀÉ®×¥¢±Ñ¡Afp¬°0¡F¦¨¥\¡Afp¬°«D0
			cout << "Fail to open file: " << endl;
		}

		// fstream multi_txt_noise;
		// multi_txt_noise.open(Form("%s/multi_run_noise.txt"), ios::out);//¶}±ÒÀÉ®×
		// if(!multi_txt_noise)
		// { //¦pªG¶}±ÒÀÉ®×¥¢±Ñ¡Afp¬°0¡F¦¨¥\¡Afp¬°«D0
		// cout<<"Fail to open file: "<<endl;
		// }

		// fstream multi_txt_entries;
		// multi_txt_entries.open(Form("%s/multi_run_entries.txt"), ios::out);//¶}±ÒÀÉ®×
		// if(!multi_txt_entries)
		// { //¦pªG¶}±ÒÀÉ®×¥¢±Ñ¡Afp¬°0¡F¦¨¥\¡Afp¬°«D0
		// cout<<"Fail to open file: "<<endl;
		// }
	}

	// fstream fp;
	//   fp.open(Form("%s/%s/slope_offset_ChiNDF.txt",the_name.Data()  ), ios::out);//¶}±ÒÀÉ®×
	//   if(!fp)
	//   { //¦pªG¶}±ÒÀÉ®×¥¢±Ñ¡Afp¬°0¡F¦¨¥\¡Afp¬°«D0
	//     cout<<"Fail to open file: "<<endl;
	//   }

	// fstream fpall;
	//   fpall.open(Form("%s/%s/All_slope_offset_ChiNDF.txt",the_name.Data()  ), ios::out);//¶}±ÒÀÉ®×
	//   if(!fpall)
	//   { //¦pªG¶}±ÒÀÉ®×¥¢±Ñ¡Afp¬°0¡F¦¨¥\¡Afp¬°«D0
	//     cout<<"Fail to open file: "<<endl;
	//   }

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

	vector<int> adc_memory;
	adc_memory.clear();
	vector<int> ampl_memory;
	ampl_memory.clear();
	vector<int> chip_id_memory;
	chip_id_memory.clear();
	vector<int> fpga_id_memory;
	fpga_id_memory.clear();
	vector<int> module_memory;
	module_memory.clear();
	vector<int> chan_id_memory;
	chan_id_memory.clear();
	vector<int> fem_id_memory;
	fem_id_memory.clear();
	vector<int> bco_memory;
	bco_memory.clear();
	vector<int> bco_full_memory;
	bco_full_memory.clear();
	vector<int> event_memory;
	event_memory.clear();

	vector<int> chip_adc[26][128];	//chip_adc.clear();
	vector<int> chip_ampl[26][128]; //chip_ampl.clear();
	vector<double> ampl_adc_slope[26];
	vector<double> ampl_adc_offset[26];
	vector<double> ampl_adc_chiNDF[26];

	vector<int> unbound_chip;
	unbound_chip.clear();
	vector<int> unbound_channel;
	unbound_channel.clear();
	vector<double> unbound_width;
	unbound_width.clear();

	vector<int> noise_chip;
	noise_chip.clear();
	vector<int> noise_channel;
	noise_channel.clear();
	vector<double> noise_width;
	noise_width.clear();

	vector<int> entries_chip;
	entries_chip.clear();
	vector<int> entries_channel;
	entries_channel.clear();
	vector<int> entries_entries;
	entries_entries.clear();

	vector<int> Centries_chip;
	Centries_chip.clear();
	vector<int> Centries_channel;
	Centries_channel.clear();
	vector<int> Centries_entries;
	Centries_entries.clear();

	for (int i = 0; i < 26; i++)
	{
		ampl_adc_slope[i].clear();
		ampl_adc_offset[i].clear();
		ampl_adc_chiNDF[i].clear();
		for (int i1 = 0; i1 < 128; i1++)
		{
			chip_adc[i][i1].clear();
			chip_ampl[i][i1].clear();
		}
	}

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
	cout << " total event : " << total_size << endl;

	cout << " the weird event :  event : ampl : adc : chip_id : chan_id " << endl;
	for (int i = 0; i < total_size; i++)
	{
		Gettree->GetEntry(i);
		//if (chan_id == 127)cout << event <<" : "<< ampl <<" : "<< adc <<" : "<< chip_id <<" : "<< chan_id<<endl;
		if (chip_id > 0 && chip_id < 27 && chan_id > -1 && chan_id < 128 && module == module_number)
		{
			chip_ampl[chip_id - 1][chan_id].push_back(ampl);
			chip_adc[chip_id - 1][chan_id].push_back(adc);
		}
		else
		{
			if (module == module_number)
			{
				cout << " the weird event : " << event << " : " << ampl << " : " << adc << " : " << chip_id << " : " << chan_id << endl;
			}
		}

		if (fpga_id != 0)
		{
			cout << " !! fpga_id event : " << event << " : " << ampl << " : " << adc << " : " << chip_id << " : " << chan_id << " fpga_id : " << fpga_id << " module : " << module << endl;
		}
		//if ( module != 8 ) {cout <<" !! module event : "<< event <<" : "<< ampl <<" : "<< adc <<" : "<< chip_id <<" : "<< chan_id<<" fpga_id : "<<fpga_id<<" module : "<<module<<endl;}
		// adc_memory.push_back(adc);
		// ampl_memory.push_back(ampl);
		// chip_id_memory.push_back(chip_id);
		// chan_id_memory.push_back(chan_id);
		// event_memory.push_back(event);

		// if (chip_id == )
		//   {
		//   	cout << event <<" : "<< ampl <<" : "<< adc <<" : "<< chip_id <<" : "<< chan_id<<endl;
		//   }
	}
	TCanvas *c1 = new TCanvas("", "", 1200, 1200);
	//TCanvas *c2=new TCanvas("","",1200,1200);
	TCanvas *c4 = new TCanvas("c4", "channel to ampl", 1625, 250);
	c4->Divide(13, 2);

	TCanvas *c5 = new TCanvas("c5", "channel to adc", 1625, 250);
	c5->Divide(13, 2);

	TF1 *slope = new TF1("slope", RC_eq1, 0, 70, 2);
	TH2F *ampl_adc[26];
	TLatex *tex111 = new TLatex();
	tex111->SetNDC();
	//tex11 -> SetTextFont (42);
	tex111->SetTextSize(0.028);
	TH1F *noise_level = new TH1F("", Form("noise_level : %d", noise_level_check), 70, 0, 70);
	TF1 *gaus_function = new TF1("gaus_function", "gaus", 0, 70);
	gaus_function->SetLineColor(2);

	TF1 *gaus_fit = new TF1("gaus_fit", "gaus", 0.0, 0.18);

	TF1 *ef_fit = new TF1("ef_fit","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);
	TF1 *ef_fit_adc0 = new TF1("ef_fit_adc0","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);
	TF1 *ef_fit_adc1 = new TF1("ef_fit_adc1","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);
	TF1 *ef_fit_adc2 = new TF1("ef_fit_adc2","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);
	TF1 *ef_fit_adc3 = new TF1("ef_fit_adc3","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);
	TF1 *ef_fit_adc4 = new TF1("ef_fit_adc4","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);
	TF1 *ef_fit_adc_acum = new TF1("ef_fit_adc_acum","[2] * 0.5* (1.0 + TMath::Erf((x - [0]) / [1] / TMath::Sqrt2()))",0, 70);

	

	vector<double> chip_slope;
	chip_slope.clear();
	vector<double> chip_index;
	chip_index.clear();

	int threshold_cut_array[26][128];

	for(int i111=0; i111<26; i111++)
	{
		for(int i112=0; i112<128; i112++)
		{
			threshold_cut_array[i111][i112]=0;
		}	
	}

	TH1F * threshold_cut_hist = new TH1F ("","# of event after ampl cut",50,threshold_cut_hist_l,threshold_cut_hist_h);
	threshold_cut_hist->GetXaxis()->SetTitle("channel event");
	threshold_cut_hist->GetYaxis()->SetTitle("entr");

	cout << "==============fitting data of 128_combine_ampladc ==========" << endl;
	for (int i4 = 0; i4 < 26; i4++)
	{
		c4->cd(i4 + 1);
		ampl_adc[i4] = new TH2F("", Form("chip_id=%d", i4 + 1), 70, 0, 70, 8, 0, 8);
		ampl_adc[i4]->SetStats(0);

		for (int i2 = 0; i2 < 128; i2++)
		{
			for (int i3 = 0; i3 < chip_ampl[i4][i2].size(); i3++)
			{
				if (chip_ampl[i4][i2][i3] > 0)
				{
					ampl_adc[i4]->Fill(chip_ampl[i4][i2][i3], chip_adc[i4][i2][i3]);
					if (chip_adc[i4][i2][i3] == noise_level_check)
					{
						noise_level->Fill(chip_ampl[i4][i2][i3]);
					}
					if (chip_ampl[i4][i2][i3]>threshold_cut)
					{
						threshold_cut_array[i4][i2]+=1;
					}
				}
			}
		}
		ampl_adc[i4]->Draw("COLZ0");
		ampl_adc[i4]->Fit("slope", "NQ");

		//cout<<Form("chip_id=%d",i4+1)<<" slope : "<< slope->GetParameter(0)<<" offset : "<<  slope->GetParameter(1)<<" chi2/NDF "<<slope->GetChisquare()/slope->GetNDF()<<endl;
		//cout<<"TESTTESTTEST"<<endl;
		//fpall<<i4+1<<" "<< slope->GetParameter(0)<<" "<<  slope->GetParameter(1)<<" "<<slope->GetChisquare()/slope->GetNDF()<<"\r"<<endl;
		//cout<<"TESTTESTTEST"<<endl;
		slope->Draw("lsame");
		chip_slope.push_back(slope->GetParameter(0));
		chip_index.push_back(i4 + 1);
		//cout<<"TESTTESTTEST"<<endl;
		//c1 -> cd(); tex111 -> DrawLatex (0.12, 0.80-(i4*0.03), Form("chip_id=%d,  slope : %.4f,  offset : %.3f,  chi2/NDF : %.2f",i4+1,slope->GetParameter(0),slope->GetParameter(1),slope->GetChisquare()/slope->GetNDF()));
		//cout<<"TESTTESTTEST"<<endl;
		//ampl_adc->Reset("ICESM");
	}
	c4->Print(Form("%s/folder_%s/128_combine_ampladc.pdf", folder_name.Data(), the_name.Data()));
	//c1->Print(Form("%s/%s/128_combine_ampladc_detail.pdf",the_name.Data()));
	cout << "==============fitting data of 128_combine_ampladc ==========" << endl;
	c4->Clear();
	c1->Clear();
	//fpall.close();
	//cout<<"\r\n"<<"#the file fpall rewriting is finish#"<<"\r\n"<<endl;

	for (int i5 = 0; i5 < 26; i5++)
		ampl_adc[i5]->Reset("ICESM");

	cout << " " << endl;
	cout << " " << endl;
	cout << " " << endl;

	c1->cd();
	noise_level->Draw("hist");
	noise_level->Fit("gaus_function", "NQ");
	TLatex *texnoise_level = new TLatex();
	texnoise_level->SetNDC();
	//tex11 -> SetTextFont (42);
	texnoise_level->SetTextSize(0.04);
	texnoise_level->DrawLatex(0.12, 0.750, Form("Gaus width : %.4f", gaus_function->GetParameter(2)));

	gaus_function->Draw("lsame");
	c1->Print(Form("%s/folder_%s/128_combine_noise_level_check_%d.pdf", folder_name.Data(), the_name.Data(), noise_level_check));
	c1->Clear();

	TGraph *slop_chip_TG = new TGraph(26, &chip_index[0], &chip_slope[0]);
	slop_chip_TG->GetXaxis()->SetTitle("chip_id");
	slop_chip_TG->GetYaxis()->SetTitle("slope");
	//slop_chip_TG->GetYaxis()->SetRangeUser(-0.01,0.01);
	slop_chip_TG->GetYaxis()->SetTitleOffset(1.8);
	slop_chip_TG->SetTitle("");

	slop_chip_TG->SetMarkerStyle(20);
	slop_chip_TG->SetMarkerSize(1.5);
	slop_chip_TG->SetMarkerColor(2);
	TCanvas *c3 = new TCanvas("c3", "c3", 1200, 1200);
	//c1->SetRightMargin(0 );

	c3->SetLeftMargin(0.120);
	c3->SetRightMargin(0.075);
	c3->cd();
	slop_chip_TG->Draw("apl");
	c3->Print(Form("%s/folder_%s/chip_slope_TG.pdf", folder_name.Data(), the_name.Data()));

	cout << "==============information of 128_combine_chanampl ==========" << endl;
	TH2F *chan_ampl[26];
	TH2F *chan_adc[26];
	c4->Divide(13, 2);
	for (int i4 = 0; i4 < 26; i4++)
	{

		chan_ampl[i4] = new TH2F("", Form("chip_id=%d", i4 + 1), 140, 0, 140, 70, 0, 70);
		chan_ampl[i4]->SetStats(0);
		chan_ampl[i4]->SetMaximum(20);
		chan_ampl[i4]->SetMinimum(0);

		chan_adc[i4] = new TH2F("", Form("chip_id=%d", i4 + 1), 140, 0, 140, 8, 0, 8);
		chan_adc[i4]->SetStats(0);
		//chan_adc[i4]->SetMaximum(20);
		chan_adc[i4]->SetMinimum(0);

		for (int i2 = 0; i2 < 128; i2++)
		{
			if (chip_ampl[i4][i2].size() > 700 || chip_ampl[i4][i2].size() < 200)
				cout << " Need to check, the entries of each channel, chip : " << i4 + 1 << " channel : " << i2 << " " << chip_ampl[i4][i2].size() << endl;
			for (int i3 = 0; i3 < chip_ampl[i4][i2].size(); i3++)
			{
				if (chip_ampl[i4][i2][i3] > 0)
				{
					//if (i4+1==16 && i2==0) cout<<i2<<" "<<chip_ampl[i4][i2][i3]<<" "<<chip_adc[i4][i2][i3]<<endl;
					chan_ampl[i4]->Fill(i2, chip_ampl[i4][i2][i3]);
					chan_adc[i4]->Fill(i2, chip_adc[i4][i2][i3]);
				}
			}
		}
		c4->cd(i4 + 1);
		chan_ampl[i4]->Draw("COLZ0");

		c5->cd(i4 + 1);
		chan_adc[i4]->Draw("COLZ0");

		//ampl_adc->Reset("ICESM");
	}
	c4->Print(Form("%s/folder_%s/128_combine_chanampl.pdf", folder_name.Data(), the_name.Data()));
	cout << "==============information of 128_combine_chanampl ==========" << endl;

	c5->Print(Form("%s/folder_%s/128_combine_chanadc.pdf", folder_name.Data(), the_name.Data()));
	cout << "==============information of 128_combine_chanadc ==========" << endl;
	c4->Clear();
	c5->Clear();
	for (int i5 = 0; i5 < 26; i5++)
		chan_ampl[i5]->Reset("ICESM");
	cout << " " << endl;
	cout << " " << endl;
	cout << " " << endl;

	cout << "==============information of ampladc_detail ==========" << endl;

	TLatex *tex11 = new TLatex();
	tex11->SetNDC();
	//tex11 -> SetTextFont (42);
	tex11->SetTextSize(0.028);

	TLatex *tex02 = new TLatex();
	tex02->SetNDC();
	//tex02 -> SetTextFont (42);
	tex02->SetTextSize(0.028);

	TLatex *tex13 = new TLatex();
	tex13->SetNDC();
	//tex13 -> SetTextFont (42);
	tex13->SetTextSize(0.028);

	TLatex *tex_chan_ampl_adc0 = new TLatex();
	tex_chan_ampl_adc0->SetNDC();
	tex_chan_ampl_adc0->SetTextSize(0.028);
	TLatex *tex_chan_ampl_adc1 = new TLatex();
	tex_chan_ampl_adc1->SetNDC();
	tex_chan_ampl_adc1->SetTextSize(0.028);
	TLatex *tex_chan_ampl_adc2 = new TLatex();
	tex_chan_ampl_adc2->SetNDC();
	tex_chan_ampl_adc2->SetTextSize(0.028);
	TLatex *tex_chan_ampl_adc3 = new TLatex();
	tex_chan_ampl_adc3->SetNDC();
	tex_chan_ampl_adc3->SetTextSize(0.028);
	TLatex *tex_chan_ampl_adc4 = new TLatex();
	tex_chan_ampl_adc4->SetNDC();
	tex_chan_ampl_adc4->SetTextSize(0.028);

	TLatex *tex_adc_width_compactor = new TLatex();
	tex_adc_width_compactor->SetNDC();
	tex_adc_width_compactor->SetTextSize(0.028);

	
	TLatex *tex_chan_ampl_adc_acum = new TLatex();
	tex_chan_ampl_adc_acum->SetNDC();
	tex_chan_ampl_adc_acum->SetTextSize(0.028);

	
	TLatex *tex_chan_ampl_1D = new TLatex();
	tex_chan_ampl_1D->SetNDC();
	tex_chan_ampl_1D->SetTextSize(0.028);

	TLatex *tex_adc_width = new TLatex();
	tex_adc_width->SetNDC();
	tex_adc_width->SetTextSize(0.028);

	TLatex *tex_adc_ampl = new TLatex();
	tex_adc_ampl->SetNDC();
	tex_adc_ampl->SetTextSize(0.028);

	
	TH2F *ampladc_detail = new TH2F("", "", 70, 0, 70, 8, 0, 8);

	TH1F *check_new = new TH1F("", "", 70, 0, 70);
	check_new->GetXaxis()->SetTitle("offset_ampl");
	check_new->GetYaxis()->SetTitle("entries");


	TH1F *err_width = new TH1F("", "", 60, 0, 6);
	err_width->GetXaxis()->SetTitle("offset_ampl");
	err_width->GetYaxis()->SetTitle("entries");

	
	TH1F * offset_width_1D_02 = new TH1F("", "", 70, 0, 70);
	offset_width_1D_02->GetXaxis()->SetTitle("offset_ampl");
	offset_width_1D_02->GetYaxis()->SetTitle("entries");

	TH1F * offset_width_1D_13 = new TH1F("", "", 70, 0, 70);
	offset_width_1D_13->GetXaxis()->SetTitle("offset_ampl");
	offset_width_1D_13->GetYaxis()->SetTitle("entries");

	TH1F *chan_ampl_1D = new TH1F("", "", 70, 0, 70);
	chan_ampl_1D->GetXaxis()->SetTitle("ampl");
	chan_ampl_1D->GetYaxis()->SetTitle("entries");	

	TF1 *gaus_fit_new = new TF1("gaus_fit_new", "gaus", 0, 70);
	TF1 *gaus_fit_width = new TF1("gaus_fit_width", "gaus", 0, 6);

	TF1 *gaus_fit_new_02 = new TF1("gaus_fit_new_02", "gaus", 0, 70);
	TF1 *gaus_fit_new_13 = new TF1("gaus_fit_new_13", "gaus", 0, 70);
	
	vector<double> response_width[26];
	vector<double> response_width02[26];
	vector<double> response_width13[26];

	vector<double> response_width0[26];
	vector<double> response_width1[26];
	vector<double> response_width2[26];
	vector<double> response_width3[26];
	vector<double> response_pvalue[26];

	vector<double> response_mean0[26];
	vector<double> response_mean1[26];
	vector<double> response_mean2[26];
	vector<double> response_mean3[26];

	
	TH1F *channel_entries_check = new TH1F("", "# of event each channel", 50, 0, 450);
	channel_entries_check->GetXaxis()->SetTitle("channel event");
	channel_entries_check->GetYaxis()->SetTitle("entries");

	TH2F *slope_TH2 = new TH2F("", "", 128, 0, 128, 128, 0.04, 0.15);
	slope_TH2->SetStats(0);
	TH2F *offset_TH2 = new TH2F("", "", 128, 0, 128, 128, 0.04, 0.15);
	offset_TH2->SetStats(0);
	TH2F *ChiNDF_TH2 = new TH2F("", "", 128, 0, 128, 128, 0.04, 0.15);
	ChiNDF_TH2->SetStats(0);

	TH1F * width_pvalue_1D = new TH1F("","width p-value",50,0,1);
        width_pvalue_1D->GetXaxis()->SetTitle("p-value");


	
	TCanvas *c2 = new TCanvas("c2", "c2", 1200, 1200);
	TH1F *slop_detail_TH = new TH1F("", "", 100, 0.08, 0.18);
	slop_detail_TH->GetYaxis()->SetTitle("entries");
	slop_detail_TH->GetXaxis()->SetTitle("fit slope ");
	slop_detail_TH->GetYaxis()->SetTitleOffset(1.6);
	c2->SetLeftMargin(0.120);
	c2->SetRightMargin(0.075);
	gStyle->SetOptStat(111111);

	double average_adc[8];
	// double average_adc1;
	// double average_adc2;
	// double average_adc3;
	// double average_adc4;
	// double average_adc5;
	// double average_adc6;
	// double average_adc7;

	double sum_adc[8];
	// double sum_adc1;
	// double sum_adc2;
	// double sum_adc3;
	// double sum_adc4;
	// double sum_adc5;
	// double sum_adc6;
	// double sum_adc7;

	double count_adc[8];
	// double count_adc1;
	// double count_adc2;
	// double count_adc3;
	// double count_adc4;
	// double count_adc5;
	// double count_adc6;
	// double count_adc7;
	//ampladc_detail->SetStats(0);
	TCanvas *c6 = new TCanvas("c6", "c6", 1200, 1200);
	TCanvas *c7 = new TCanvas("c7", "c7", 1200, 1200);
	TCanvas *c8 = new TCanvas("c8", "c8", 1200, 1200);
	TCanvas *c9 = new TCanvas("c9", "c9", 1200, 1200);
	TCanvas *c10 = new TCanvas("c10", "c10", 1200, 1200);
	TCanvas *c11 = new TCanvas("c11", "c11", 1200, 1200);
	TCanvas *c12 = new TCanvas("c12", "c12", 1200, 1200);
	
	TCanvas *c13 = new TCanvas("c13", "c13", 1200, 1200);
	TCanvas *c14 = new TCanvas("c14", "c14", 1200, 1200);
	TCanvas *c15 = new TCanvas("c15", "c15", 1200, 1200);
	TCanvas *c16 = new TCanvas("c16", "c16", 1200, 1200);
	TCanvas *c17 = new TCanvas("c17", "c17", 1200, 1200);
	TCanvas *c18 = new TCanvas("c18", "c18", 1200, 1200);
	TCanvas *c19 = new TCanvas("c19", "c19", 1200, 1200);
	TCanvas *c20 = new TCanvas("c20", "c20", 1200, 1200);
	//TCanvas *c21 = new TCanvas("c21", "c21", 1200, 1200);

	int channel_entries_outsider = 0;

	TH1F * TP_turnon_1D = new TH1F ("","threshold position distribution",50,25,40);
	TP_turnon_1D->GetXaxis()->SetTitle("ampl");
	TP_turnon_1D->GetYaxis()->SetTitle("entry");

	TH1F * width_rchi2_1D = new TH1F ("","Reduced #chi^{2} of width - ADC plot ",50,0,5);
	width_rchi2_1D->GetXaxis()->SetTitle("Reduced #chi^{2} of pol0 fit");
	width_rchi2_1D->GetYaxis()->SetTitle("entry");

	TH1F * slope_rchi2_1D = new TH1F ("","Reduced #chi^{2} of slope - ADC plot ",50,0,5);
	slope_rchi2_1D->GetXaxis()->SetTitle("Reduced #chi^{2} of pol1 fit");
	slope_rchi2_1D->GetYaxis()->SetTitle("entry");


	TH1F*channel_ADC_0_on = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_1_on = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_2_on = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_3_on = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_4_on = new TH1F ("","",70,0,70);

	TH1F*channel_ADC_0_on_clone = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_1_on_clone = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_2_on_clone = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_3_on_clone = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_4_on_clone = new TH1F ("","",70,0,70);

	
	TH1F*channel_ADC_0 = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_1 = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_2 = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_3 = new TH1F ("","",70,0,70);
	TH1F*channel_ADC_4 = new TH1F ("","",70,0,70);


	channel_ADC_0_on->GetXaxis()->SetTitle("ampl");
	channel_ADC_0_on->GetYaxis()->SetTitle("entry");
	channel_ADC_0_on->GetYaxis()->SetRangeUser(0,13);
	channel_ADC_1_on->GetXaxis()->SetTitle("ampl");
	channel_ADC_1_on->GetYaxis()->SetTitle("entry");
	channel_ADC_1_on->GetYaxis()->SetRangeUser(0,13);
	channel_ADC_2_on->GetXaxis()->SetTitle("ampl");
	channel_ADC_2_on->GetYaxis()->SetTitle("entry");
	channel_ADC_2_on->GetYaxis()->SetRangeUser(0,13);
	channel_ADC_3_on->GetXaxis()->SetTitle("ampl");
	channel_ADC_3_on->GetYaxis()->SetTitle("entry");
	channel_ADC_3_on->GetYaxis()->SetRangeUser(0,13);
	channel_ADC_4_on->GetXaxis()->SetTitle("ampl");
	channel_ADC_4_on->GetYaxis()->SetTitle("entry");
	channel_ADC_4_on->GetYaxis()->SetRangeUser(0,13);




	channel_ADC_0_on_clone->GetXaxis()->SetTitle("ampl");
	channel_ADC_0_on_clone->GetYaxis()->SetTitle("entry");
	channel_ADC_0_on_clone->GetYaxis()->SetRangeUser(0,13);
	channel_ADC_1_on_clone->GetXaxis()->SetTitle("ampl");
	channel_ADC_1_on_clone->GetYaxis()->SetTitle("entry");
	channel_ADC_1_on_clone->GetYaxis()->SetRangeUser(0,13);
	channel_ADC_2_on_clone->GetXaxis()->SetTitle("ampl");
	channel_ADC_2_on_clone->GetYaxis()->SetTitle("entry");
	channel_ADC_2_on_clone->GetYaxis()->SetRangeUser(0,13);
	channel_ADC_3_on_clone->GetXaxis()->SetTitle("ampl");
	channel_ADC_3_on_clone->GetYaxis()->SetTitle("entry");
	channel_ADC_3_on_clone->GetYaxis()->SetRangeUser(0,13);
	channel_ADC_4_on_clone->GetXaxis()->SetTitle("ampl");
	channel_ADC_4_on_clone->GetYaxis()->SetTitle("entry");
	channel_ADC_4_on_clone->GetYaxis()->SetRangeUser(0,13);



	
	TH1F*channel_ADC_all = new TH1F ("","",70,0,70);
	channel_ADC_all->GetXaxis()->SetTitle("ampl");
	channel_ADC_all->GetYaxis()->SetTitle("entry");
	channel_ADC_all->GetYaxis()->SetRangeUser(0,13);
	
	channel_ADC_0->SetStats(0);	
	channel_ADC_1->SetStats(0);	
	channel_ADC_2->SetStats(0);	
	channel_ADC_3->SetStats(0);	
	channel_ADC_4->SetStats(0);	

	channel_ADC_0_on->SetStats(0);	
	channel_ADC_1_on->SetStats(0);	
	channel_ADC_2_on->SetStats(0);	
	channel_ADC_3_on->SetStats(0);	
	channel_ADC_4_on->SetStats(0);	
	
	channel_ADC_0_on->Sumw2();	
	channel_ADC_1_on->Sumw2();	
	channel_ADC_2_on->Sumw2();	
	channel_ADC_3_on->Sumw2();	
	channel_ADC_4_on->Sumw2();	


	channel_ADC_0_on_clone->SetStats(0);	
	channel_ADC_1_on_clone->SetStats(0);	
	channel_ADC_2_on_clone->SetStats(0);	
	channel_ADC_3_on_clone->SetStats(0);	
	channel_ADC_4_on_clone->SetStats(0);	
	
	channel_ADC_0_on_clone->Sumw2();	
	channel_ADC_1_on_clone->Sumw2();	
	channel_ADC_2_on_clone->Sumw2();	
	channel_ADC_3_on_clone->Sumw2();	
	channel_ADC_4_on_clone->Sumw2();	

	
	THStack * adc_stack;


	TLatex *tex_channel_adc = new TLatex();
	tex_channel_adc->SetNDC();
	tex_channel_adc->SetTextSize(0.028);


	
	TF1 *chan_ADC_fit = new TF1("chan_ADC_fit", "gaus", 0, 70);
	TF1 *chan_ADC0_fit = new TF1("chan_ADC0_fit", "gaus", 0, 70);
	TF1 *chan_ADC1_fit = new TF1("chan_ADC1_fit", "gaus", 0, 70);
	TF1 *chan_ADC2_fit = new TF1("chan_ADC2_fit", "gaus", 0, 70);
	TF1 *chan_ADC3_fit = new TF1("chan_ADC3_fit", "gaus", 0, 70);
	TF1 *chan_ADC4_fit = new TF1("chan_ADC4_fit", "gaus", 0, 70);

	chan_ADC0_fit->SetLineColor(4);
	chan_ADC2_fit->SetLineColor(3);
	chan_ADC3_fit->SetLineColor(5);
	chan_ADC4_fit->SetLineColor(6);


	vector<double> chan_ADC_mean; chan_ADC_mean.clear();
	vector<double> chan_ADC_mean_error; chan_ADC_mean_error.clear();
	vector<double> chan_ADC_width; chan_ADC_width.clear();
	vector<double> chan_ADC_width_error; chan_ADC_width_error.clear();

	double YADC [5]= {0,1,2,3,4};
	double YADCerror [5]= {0.5,0.5,0.5,0.5,0.5};

	double XADC [5]= {0,1,2,3,4};
	double XADCerror [5]= {0,0,0,0,0};

	TF1 * polynomial0 = new TF1 ("polynomial0","pol0",-1,10);
	TF1 * polynomial0_adc = new TF1 ("polynomial0_adc","pol0",-1,10);

	TF1 * polynomial1 = new TF1 ("polynomial1","pol1",0,70);



	for (int i4 = 0; i4 < 26; i4++)
	{
		c9->cd();
		c9->Print(Form("%s/folder_%s/chip%d_detail_adcamplmean.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));
		
		c10->cd();
		c10->Print(Form("%s/folder_%s/chip%d_detail_widthvariation.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		c8->cd();
		c8->Print(Form("%s/folder_%s/chip%d_detail_adcwidth.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		c11->cd();
		c11->Print(Form("%s/folder_%s/chip%d_detail_adcwidthall.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		c12->cd();
		c12->Print(Form("%s/folder_%s/chip%d_detail_adcwidthall_stack.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));
		
		c13->cd();
		c13->Print(Form("%s/folder_%s/chip%d_detail_offsetwidth02.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		c14->cd();
		c14->Print(Form("%s/folder_%s/chip%d_detail_offsetwidth13.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		c15->cd();
		c15->Print(Form("%s/folder_%s/chip%d_detail_adcerr.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		c16->cd();
		c16->Print(Form("%s/folder_%s/chip%d_detail_acumerr.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		c17->cd();
		c17->Print(Form("%s/folder_%s/chip%d_detail_adcerr_acum.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		c18->cd();
		c18->Print(Form("%s/folder_%s/chip%d_detail_adc_compactor.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));

		if (run_option == true)
		{
			c1->cd();
			c1->Print(Form("%s/folder_%s/chip%d_detail_ampladc.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));
		}
		if (new_check == true)
		{
			c6->cd();
			c6->Print(Form("%s/folder_%s/chip%d_detail_amploffset.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));
			c7->cd();
			c7->Print(Form("%s/folder_%s/chip%d_detail_ampl1D.pdf(", folder_name.Data(), the_name.Data(), i4 + 1));						
		}
		for (int i2 = 0; i2 < 128; i2++)
		{	

			ampladc_detail->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));
			check_new->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));
			chan_ampl_1D->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));
			offset_width_1D_02 ->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));
			offset_width_1D_13 ->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));



			channel_ADC_0_on->SetTitle(Form("chip_id=%d, chan_id = %d, ADC0", i4 + 1, i2));
			channel_ADC_1_on->SetTitle(Form("chip_id=%d, chan_id = %d, ADC1", i4 + 1, i2));
			channel_ADC_2_on->SetTitle(Form("chip_id=%d, chan_id = %d, ADC2", i4 + 1, i2));
			channel_ADC_3_on->SetTitle(Form("chip_id=%d, chan_id = %d, ADC3", i4 + 1, i2));
			channel_ADC_4_on->SetTitle(Form("chip_id=%d, chan_id = %d, ADC4", i4 + 1, i2));

			
			channel_ADC_0->SetTitle(Form("chip_id=%d, chan_id = %d, ADC0", i4 + 1, i2));
			channel_ADC_1->SetTitle(Form("chip_id=%d, chan_id = %d, ADC1", i4 + 1, i2));
			channel_ADC_2->SetTitle(Form("chip_id=%d, chan_id = %d, ADC2", i4 + 1, i2));
			channel_ADC_3->SetTitle(Form("chip_id=%d, chan_id = %d, ADC3", i4 + 1, i2));
			channel_ADC_4->SetTitle(Form("chip_id=%d, chan_id = %d, ADC4", i4 + 1, i2));

		
			
			channel_ADC_all->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));
			
			adc_stack = new THStack("","ADC stack");
			adc_stack->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));

			//if (chip_ampl[i4][i2].size()>700 || chip_ampl[i4][i2].size()<200) cout<<" Need to check, the entries of each channel, chip : "<<i4+1<<" channel : "<<i2<<" " <<chip_ampl[i4][i2].size()<<endl;
			for (int i3 = 0; i3 < chip_ampl[i4][i2].size(); i3++)
			{
				if (chip_ampl[i4][i2][i3] > 0)
				{
					//if (i4+1==16 && i2==0) cout<<i2<<" "<<chip_ampl[i4][i2][i3]<<" "<<chip_adc[i4][i2][i3]<<endl;
					ampladc_detail->Fill(chip_ampl[i4][i2][i3], chip_adc[i4][i2][i3]);
					chan_ampl_1D->Fill(chip_ampl[i4][i2][i3]);

					if (chip_adc[i4][i2][i3] == 0)
					{
						channel_ADC_0->Fill(chip_ampl[i4][i2][i3]);
												
					}

					if (chip_adc[i4][i2][i3] == 1)
					{
						channel_ADC_1->Fill(chip_ampl[i4][i2][i3]);
					}

					if (chip_adc[i4][i2][i3] == 2)
					{
						channel_ADC_2->Fill(chip_ampl[i4][i2][i3]);
					}

					if (chip_adc[i4][i2][i3] == 3)
					{
						channel_ADC_3->Fill(chip_ampl[i4][i2][i3]);
					}

					if (chip_adc[i4][i2][i3] == 4)
					{
						channel_ADC_4->Fill(chip_ampl[i4][i2][i3]);
					}
					//-----------------------------------------------

					
					if (chip_adc[i4][i2][i3] == 0 || chip_adc[i4][i2][i3] == 1 || chip_adc[i4][i2][i3] == 2 || chip_adc[i4][i2][i3] == 3 || chip_adc[i4][i2][i3] == 4 || chip_adc[i4][i2][i3] == 5 || chip_adc[i4][i2][i3] == 6 || chip_adc[i4][i2][i3] == 7)
					{
					  
					  channel_ADC_0_on->Fill(chip_ampl[i4][i2][i3]);
					  channel_ADC_0_on_clone->Fill(chip_ampl[i4][i2][i3]);
					}
					
					
					if (chip_adc[i4][i2][i3] == 1 || chip_adc[i4][i2][i3] == 2 || chip_adc[i4][i2][i3] == 3 || chip_adc[i4][i2][i3] == 4 || chip_adc[i4][i2][i3] == 5 || chip_adc[i4][i2][i3] == 6 || chip_adc[i4][i2][i3] == 7)
					{
					  channel_ADC_1_on->Fill(chip_ampl[i4][i2][i3]);
					  channel_ADC_1_on_clone->Fill(chip_ampl[i4][i2][i3]);
					}

					if (chip_adc[i4][i2][i3] == 2 || chip_adc[i4][i2][i3] == 3 || chip_adc[i4][i2][i3] == 4 || chip_adc[i4][i2][i3] == 5 || chip_adc[i4][i2][i3] == 6 || chip_adc[i4][i2][i3] == 7)
					{
						channel_ADC_2_on->Fill(chip_ampl[i4][i2][i3]);
						channel_ADC_2_on_clone->Fill(chip_ampl[i4][i2][i3]);
					}

					if (chip_adc[i4][i2][i3] == 3 || chip_adc[i4][i2][i3] == 4 || chip_adc[i4][i2][i3] == 5 || chip_adc[i4][i2][i3] == 6 || chip_adc[i4][i2][i3] == 7)
					{
						channel_ADC_3_on->Fill(chip_ampl[i4][i2][i3]);
						channel_ADC_3_on_clone->Fill(chip_ampl[i4][i2][i3]);
					}

					if (chip_adc[i4][i2][i3] == 4 || chip_adc[i4][i2][i3] == 5 || chip_adc[i4][i2][i3] == 6 || chip_adc[i4][i2][i3] == 7)
					{
						channel_ADC_4_on->Fill(chip_ampl[i4][i2][i3]);
						channel_ADC_4_on_clone->Fill(chip_ampl[i4][i2][i3]);
					}

					
					sum_adc[chip_adc[i4][i2][i3]] += chip_ampl[i4][i2][i3];
					count_adc[chip_adc[i4][i2][i3]] += 1;
				}
			}



		
			
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



			c15->cd();
			//channel_ADC_0_on->SetFillColorAlpha(4,0.25);
			channel_ADC_0_on->Draw("hist");
			//chan_ADC0_fit_on->Draw("lsame");
			
			if (channel_ADC_0_on->GetEntries() ==0)
			  {
			    ef_fit_adc0->SetParameter(0,10); //turn on
			    ef_fit_adc0->SetParameter(1,0); //slope
			    ef_fit_adc0->SetParameter(2,1);//height
			  }
			else 
			  {
			    ef_fit_adc0->SetParameters(30, 2, 12); 
			    ef_fit_adc0->SetParLimits(2,9.9,12);
			    ef_fit_adc0->SetParLimits(1,0.2,10);
				// [0] turn_on
				// [1] slope
				// [2] height 
			    channel_ADC_0_on->Fit("ef_fit_adc0", "NQ");	
			  }
			c15->cd();
			ef_fit_adc0->Draw("lsame");
			c15->cd();
			tex_chan_ampl_adc0->DrawLatex(0.12, 0.750, Form("turn on : %.4f", ef_fit_adc0->GetParameter(0)));
			c15->cd();
			tex_chan_ampl_adc0->DrawLatex(0.12, 0.720, Form("width   : %.3f", ef_fit_adc0->GetParameter(1)));
			c15->cd();
			tex_chan_ampl_adc0->DrawLatex(0.12, 0.690, Form("height  : %.3f", ef_fit_adc0->GetParameter(2)));
			c15->Print(Form("%s/folder_%s/chip%d_detail_adcerr.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c15->Clear();



			c15->cd();
			channel_ADC_1_on->Draw("hist");
			
			if (channel_ADC_1_on->GetEntries() ==0)
			  {
			    ef_fit_adc1->SetParameter(0,10); //turn on
			    ef_fit_adc1->SetParameter(1,0); //slope
			    ef_fit_adc1->SetParameter(2,1);//height
			  }
			else 
			  {
			    ef_fit_adc1->SetParameters(35, 2, 12); 
			    ef_fit_adc1->SetParLimits(2,9.9,12);
			    ef_fit_adc1->SetParLimits(1,0.2,10);
				// [0] turn_on
				// [1] slope
				// [2] height 
			    channel_ADC_1_on->Fit("ef_fit_adc1", "NQ");	
			  }
			c15->cd();
			ef_fit_adc1->Draw("lsame");
			c15->cd();
			tex_chan_ampl_adc1->DrawLatex(0.12, 0.750, Form("turn on : %.4f", ef_fit_adc1->GetParameter(0)));
			c15->cd();
			tex_chan_ampl_adc1->DrawLatex(0.12, 0.720, Form("width   : %.3f", ef_fit_adc1->GetParameter(1)));
			c15->cd();
			tex_chan_ampl_adc1->DrawLatex(0.12, 0.690, Form("height  : %.3f", ef_fit_adc1->GetParameter(2)));
			c15->Print(Form("%s/folder_%s/chip%d_detail_adcerr.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c15->Clear();
	

			
			c15->cd();
			channel_ADC_2_on->Draw("hist");
			
			if (channel_ADC_2_on->GetEntries() ==0)
			  {
			    ef_fit_adc2->SetParameter(0,10); //turn on
			    ef_fit_adc2->SetParameter(1,0); //slope
			    ef_fit_adc2->SetParameter(2,1);//height
			  }
			else 
			  {
			    ef_fit_adc2->SetParameters(40, 2, 12); 
			    ef_fit_adc2->SetParLimits(2,9.9,12);
			    ef_fit_adc2->SetParLimits(1,0.2,10);
				// [0] turn_on
				// [1] slope
				// [2] height 
			    channel_ADC_2_on->Fit("ef_fit_adc2", "NQ");	
			  }
			c15->cd();
			ef_fit_adc2->Draw("lsame");
			c15->cd();
			tex_chan_ampl_adc2->DrawLatex(0.12, 0.750, Form("turn on : %.4f", ef_fit_adc2->GetParameter(0)));
			c15->cd();
			tex_chan_ampl_adc2->DrawLatex(0.12, 0.720, Form("width   : %.3f", ef_fit_adc2->GetParameter(1)));
			c15->cd();
			tex_chan_ampl_adc2->DrawLatex(0.12, 0.690, Form("height  : %.3f", ef_fit_adc2->GetParameter(2)));
			c15->Print(Form("%s/folder_%s/chip%d_detail_adcerr.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c15->Clear();
	


			c15->cd();
			channel_ADC_3_on->Draw("hist");
			
			if (channel_ADC_3_on->GetEntries() ==0)
			  {
			    ef_fit_adc3->SetParameter(0,10); //turn on
			    ef_fit_adc3->SetParameter(1,0); //slope
			    ef_fit_adc3->SetParameter(2,1);//height
			  }
			else 
			  {
			    ef_fit_adc3->SetParameters(45, 2, 12); 
			    ef_fit_adc3->SetParLimits(2,9.9,12);
			    ef_fit_adc3->SetParLimits(1,0.2,10);
				// [0] turn_on
				// [1] slope
				// [2] height 
			    channel_ADC_3_on->Fit("ef_fit_adc3", "NQ");	
			  }
			c15->cd();
			ef_fit_adc3->Draw("lsame");
			c15->cd();
			tex_chan_ampl_adc3->DrawLatex(0.12, 0.750, Form("turn on : %.4f", ef_fit_adc3->GetParameter(0)));
			c15->cd();
			tex_chan_ampl_adc3->DrawLatex(0.12, 0.720, Form("width   : %.3f", ef_fit_adc3->GetParameter(1)));
			c15->cd();
			tex_chan_ampl_adc3->DrawLatex(0.12, 0.690, Form("height  : %.3f", ef_fit_adc3->GetParameter(2)));
			c15->Print(Form("%s/folder_%s/chip%d_detail_adcerr.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c15->Clear();
	


			c15->cd();
			channel_ADC_4_on->Draw("hist");
			
			if (channel_ADC_4_on->GetEntries() ==0)
			  {
			    ef_fit_adc4->SetParameter(0,10); //turn on
			    ef_fit_adc4->SetParameter(1,0); //slope
			    ef_fit_adc4->SetParameter(2,1);//height
			  }
			else 
			  {
			    ef_fit_adc4->SetParameters(50, 2, 12); 
			    ef_fit_adc4->SetParLimits(2,9.9,12);
			    ef_fit_adc4->SetParLimits(1,0.2,10);
				// [0] turn_on
				// [1] slope
				// [2] height 
			    channel_ADC_4_on->Fit("ef_fit_adc4", "NQ");	
			  }
			c15->cd();
			ef_fit_adc4->Draw("lsame");
			c15->cd();
			tex_chan_ampl_adc4->DrawLatex(0.12, 0.750, Form("turn on : %.4f", ef_fit_adc4->GetParameter(0)));
			c15->cd();
			tex_chan_ampl_adc4->DrawLatex(0.12, 0.720, Form("width   : %.3f", ef_fit_adc4->GetParameter(1)));
			c15->cd();
			tex_chan_ampl_adc4->DrawLatex(0.12, 0.690, Form("height  : %.3f", ef_fit_adc4->GetParameter(2)));   			
			c15->Print(Form("%s/folder_%s/chip%d_detail_adcerr.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c15->Clear();


			c16->cd();
			
			
			// after shift                                                                                                                                                                                           
			double offset1 = (ef_fit_adc1->GetParameter(0))-(ef_fit_adc0->GetParameter(0));
			double offset2 = (ef_fit_adc2->GetParameter(0))-(ef_fit_adc0->GetParameter(0));
			double offset3 = (ef_fit_adc3->GetParameter(0))-(ef_fit_adc0->GetParameter(0));
		
			TH1F* hnew1 = new TH1F("hnew1", "", 70, 0, 70);
			shifted_hist(channel_ADC_1_on_clone, hnew1, offset1);
			
			TH1F* hnew2 = new TH1F("hnew2", "", 70, 0, 70);
			shifted_hist(channel_ADC_2_on_clone, hnew2, offset2);
			
			TH1F* hnew3 = new TH1F("hnew3", "", 70, 0, 70);
			shifted_hist(channel_ADC_3_on_clone, hnew3, offset3);


			TH1F* h_ADC_0 = (TH1F*)channel_ADC_0_on_clone->Clone("h_ADC_0");
			// add up the two histogram                                                                                                                                                       
			channel_ADC_0_on_clone->Add(hnew1);
			channel_ADC_0_on_clone->Add(hnew2);
			channel_ADC_0_on_clone->Add(hnew3);


	
			// plot                                                                                                                                                                           
			channel_ADC_0_on_clone->SetLineColor(4);
			channel_ADC_0_on_clone->Draw("hist");
			h_ADC_0->SetLineColor(1);
			h_ADC_0->Draw("hist same");
			hnew1->SetLineColor(2);
			hnew1->Draw("hist same");
			hnew2->SetLineColor(3);
			hnew2->Draw("hist same");
			hnew3->SetLineColor(5);
			hnew3->Draw("hist same");
		

			c16->Print(Form("%s/folder_%s/chip%d_detail_acumerr.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c16->Clear();
			
			//-----------------------

			response_width0[i4].push_back(ef_fit_adc0->GetParameter(1));
			response_width1[i4].push_back(ef_fit_adc1->GetParameter(1));
			response_width2[i4].push_back(ef_fit_adc2->GetParameter(1));
			response_width3[i4].push_back(ef_fit_adc3->GetParameter(1));
			
			response_mean0[i4].push_back(ef_fit_adc0->GetParameter(0));
			response_mean1[i4].push_back(ef_fit_adc1->GetParameter(0));
			response_mean2[i4].push_back(ef_fit_adc2->GetParameter(0));
			response_mean3[i4].push_back(ef_fit_adc3->GetParameter(0));
	
			
			err_width->Fill(ef_fit_adc0->GetParameter(1));
			err_width->Fill(ef_fit_adc1->GetParameter(1));
			err_width->Fill(ef_fit_adc2->GetParameter(1));
			err_width->Fill(ef_fit_adc3->GetParameter(1));
			


			c17->cd();
			//channel_ADC_0_on->SetFillColorAlpha(4,0.25);
			channel_ADC_0_on_clone->Draw("hist");
			//chan_ADC0_fit_on->Draw("lsame");
			
			if (channel_ADC_0_on_clone->GetEntries() ==0)
			  {
			    ef_fit_adc_acum->SetParameter(0,10); //turn on
			    ef_fit_adc_acum->SetParameter(1,0); //slope
			    ef_fit_adc_acum->SetParameter(2,1);//height
			  }
			else 
			  {
			    ef_fit_adc_acum->SetParameters(30, 2, 20); 
			    ef_fit_adc_acum->SetParLimits(2,40,50);
			    ef_fit_adc_acum->SetParLimits(1,0.2,10);
				// [0] turn_on
				// [1] slope
				// [2] height 
			    channel_ADC_0_on_clone->Fit("ef_fit_adc_acum","NQ");	
			  }
			c17->cd();
			ef_fit_adc_acum->Draw("lsame");
			c17->cd();
			tex_chan_ampl_adc_acum->DrawLatex(0.12, 0.750, Form("turn on : %.4f", ef_fit_adc_acum->GetParameter(0)));
			c17->cd();
			tex_chan_ampl_adc_acum->DrawLatex(0.12, 0.720, Form("width   : %.3f", ef_fit_adc_acum->GetParameter(1)));
			c17->cd();
			tex_chan_ampl_adc_acum->DrawLatex(0.12, 0.690, Form("height  : %.3f", ef_fit_adc_acum->GetParameter(2)));
			c17->Print(Form("%s/folder_%s/chip%d_detail_adcerr_acum.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c17->Clear();


			

	
			const Int_t n = 4;
			Double_t x[n]  = {0, 1, 2, 3};
			Double_t y[n]  = { ef_fit_adc0->GetParameter(1), ef_fit_adc1->GetParameter(1), ef_fit_adc2->GetParameter(1), ef_fit_adc3->GetParameter(1) };
			Double_t ex[n] = {0, 0, 0, 0};
			Double_t ey[n] = { ef_fit_adc0->GetParError(1), ef_fit_adc1->GetParError(1), ef_fit_adc2->GetParError(1), ef_fit_adc3->GetParError(1)};
		
			TGraphErrors * compactor_width = new TGraphErrors(n,x,y,ex,ey);			
			compactor_width->GetXaxis()->SetTitle("ADC");
			compactor_width->GetYaxis()->SetTitle("width");
			compactor_width->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));
			compactor_width->SetMarkerStyle(20);
			compactor_width->SetMarkerSize(2);
			compactor_width->SetMarkerColor(1);
			compactor_width->GetXaxis()->SetLimits(-0.5,3.5);

			c18->cd();
			compactor_width->Draw("apl");

			
			compactor_width->Fit("polynomial0_adc","NQ");	
			c18->cd();
			polynomial0_adc->Draw("lsame");
			
			tex_adc_width_compactor->DrawLatex(0.12, 0.750, Form("fit : %.3f", polynomial0_adc->GetParameter(0)));
			c18->cd();
			tex_adc_width_compactor->DrawLatex(0.12, 0.720, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.2f", polynomial0_adc->GetChisquare(),polynomial0_adc->GetNDF(),polynomial0_adc->GetChisquare()/polynomial0_adc->GetNDF()));	
					
			c18->Print(Form("%s/folder_%s/chip%d_detail_adc_compactor.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c18->Clear();


			width_pvalue_1D->Fill(TMath::Prob(polynomial0_adc->GetChisquare(), polynomial0_adc->GetNDF()));
			response_pvalue[i4].push_back(TMath::Prob(polynomial0_adc->GetChisquare(), polynomial0_adc->GetNDF()));
					


			
			
			//

			
			c8->cd();
			channel_ADC_0->Draw("hist");
			if (channel_ADC_0->GetEntries()==0.)
			{
				chan_ADC_fit->SetParameter(2,0);
				chan_ADC_fit->SetParameter(1,0);

				chan_ADC0_fit->SetParameter(2,0);
				chan_ADC0_fit->SetParameter(1,0);
			}
			else 
			{
				channel_ADC_0->Fit("chan_ADC_fit","NQ");
				channel_ADC_0->Fit("chan_ADC0_fit","NQ");
			}
			c8->cd();
			chan_ADC_fit->Draw("lsame");
			chan_ADC_mean.push_back(chan_ADC_fit->GetParameter(1));
			chan_ADC_width.push_back(chan_ADC_fit->GetParameter(2));
			chan_ADC_width_error.push_back(chan_ADC_fit->GetParError(2));
			chan_ADC_mean_error.push_back(chan_ADC_fit->GetParError(1));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.750, Form("gaus width : %.4f", chan_ADC_fit->GetParameter(2)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.720, Form("gaus size  : %.2f", chan_ADC_fit->GetParameter(0)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.690, Form("gaus mean  : %.2f", chan_ADC_fit->GetParameter(1)));
			c8->Print(Form("%s/folder_%s/chip%d_detail_adcwidth.pdf", folder_name.Data(), the_name.Data(), i4 + 1));	
			c8->Clear();

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			c8->cd();
			channel_ADC_1->Draw("hist");
			if (channel_ADC_1->GetEntries()==0.)
			{
				chan_ADC_fit->SetParameter(2,0);
				chan_ADC_fit->SetParameter(1,0);

				chan_ADC1_fit->SetParameter(2,0);
				chan_ADC1_fit->SetParameter(1,0);
			}
			else 
			{
				channel_ADC_1->Fit("chan_ADC_fit","NQ");
				channel_ADC_1->Fit("chan_ADC1_fit","NQ");
			}
			c8->cd();
			chan_ADC_fit->Draw("lsame");
			chan_ADC_mean.push_back(chan_ADC_fit->GetParameter(1));
			chan_ADC_width.push_back(chan_ADC_fit->GetParameter(2));
			chan_ADC_width_error.push_back(chan_ADC_fit->GetParError(2));
			chan_ADC_mean_error.push_back(chan_ADC_fit->GetParError(1));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.750, Form("gaus width : %.4f", chan_ADC_fit->GetParameter(2)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.720, Form("gaus size  : %.2f", chan_ADC_fit->GetParameter(0)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.690, Form("gaus mean  : %.2f", chan_ADC_fit->GetParameter(1)));
			c8->Print(Form("%s/folder_%s/chip%d_detail_adcwidth.pdf", folder_name.Data(), the_name.Data(), i4 + 1));	
			c8->Clear();

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			c8->cd();
			channel_ADC_2->Draw("hist");
			if (channel_ADC_2->GetEntries()==0.)
			{
				chan_ADC_fit->SetParameter(2,0);
				chan_ADC_fit->SetParameter(1,0);

				chan_ADC2_fit->SetParameter(2,0);
				chan_ADC2_fit->SetParameter(1,0);
			}
			else 
			{
				channel_ADC_2->Fit("chan_ADC_fit","NQ");
				channel_ADC_2->Fit("chan_ADC2_fit","NQ");
			}
			c8->cd();
			chan_ADC_fit->Draw("lsame");
			chan_ADC_mean.push_back(chan_ADC_fit->GetParameter(1));
			chan_ADC_width.push_back(chan_ADC_fit->GetParameter(2));
			chan_ADC_width_error.push_back(chan_ADC_fit->GetParError(2));
			chan_ADC_mean_error.push_back(chan_ADC_fit->GetParError(1));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.750, Form("gaus width : %.4f", chan_ADC_fit->GetParameter(2)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.720, Form("gaus size  : %.2f", chan_ADC_fit->GetParameter(0)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.690, Form("gaus mean  : %.2f", chan_ADC_fit->GetParameter(1)));
			c8->Print(Form("%s/folder_%s/chip%d_detail_adcwidth.pdf", folder_name.Data(), the_name.Data(), i4 + 1));	
			c8->Clear();

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			c8->cd();
			channel_ADC_3->Draw("hist");
			if (channel_ADC_3->GetEntries()==0.)
			{
				chan_ADC_fit->SetParameter(2,0);
				chan_ADC_fit->SetParameter(1,0);

				chan_ADC3_fit->SetParameter(2,0);
				chan_ADC3_fit->SetParameter(1,0);

			}
			else 
			{
				channel_ADC_3->Fit("chan_ADC_fit","NQ");
				channel_ADC_3->Fit("chan_ADC3_fit","NQ");
			}
			c8->cd();
			chan_ADC_fit->Draw("lsame");
			chan_ADC_mean.push_back(chan_ADC_fit->GetParameter(1));
			chan_ADC_width.push_back(chan_ADC_fit->GetParameter(2));
			chan_ADC_width_error.push_back(chan_ADC_fit->GetParError(2));
			chan_ADC_mean_error.push_back(chan_ADC_fit->GetParError(1));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.750, Form("gaus width : %.4f", chan_ADC_fit->GetParameter(2)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.720, Form("gaus size  : %.2f", chan_ADC_fit->GetParameter(0)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.690, Form("gaus mean  : %.2f", chan_ADC_fit->GetParameter(1)));
			c8->Print(Form("%s/folder_%s/chip%d_detail_adcwidth.pdf", folder_name.Data(), the_name.Data(), i4 + 1));	
			c8->Clear();

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			c8->cd();
			channel_ADC_4->Draw("hist");
			if (channel_ADC_4->GetEntries()==0.)
			{
				chan_ADC_fit->SetParameter(2,0);
				chan_ADC_fit->SetParameter(1,0);

				chan_ADC4_fit->SetParameter(2,0);
				chan_ADC4_fit->SetParameter(1,0);

			}
			else 
			{
				channel_ADC_4->Fit("chan_ADC_fit","NQ");
				channel_ADC_4->Fit("chan_ADC4_fit","NQ");
			}

			c8->cd();
			chan_ADC_fit->Draw("lsame");
			chan_ADC_mean.push_back(chan_ADC_fit->GetParameter(1));
			chan_ADC_width.push_back(chan_ADC_fit->GetParameter(2));
			chan_ADC_width_error.push_back(chan_ADC_fit->GetParError(2));
			chan_ADC_mean_error.push_back(chan_ADC_fit->GetParError(1));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.750, Form("gaus width : %.4f", chan_ADC_fit->GetParameter(2)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.720, Form("gaus size  : %.2f", chan_ADC_fit->GetParameter(0)));
			c8->cd();
			tex_channel_adc->DrawLatex(0.12, 0.690, Form("gaus mean  : %.2f", chan_ADC_fit->GetParameter(1)));
			c8->Print(Form("%s/folder_%s/chip%d_detail_adcwidth.pdf", folder_name.Data(), the_name.Data(), i4 + 1));	
			c8->Clear();

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~






			//-----------------------------------------------------------------




			
			chip_adc_root = i4+1;
			chan_adc_root = i2;
			root_adc_0 = chan_ADC0_fit->GetParameter(1);
			root_adc_1 = chan_ADC1_fit->GetParameter(1);
			root_adc_2 = chan_ADC2_fit->GetParameter(1);
			root_adc_3 = chan_ADC3_fit->GetParameter(1);
			root_adc_4 = chan_ADC4_fit->GetParameter(1);

			root_adc_0N = channel_ADC_0->GetMean();
			root_adc_1N = channel_ADC_1->GetMean();
			root_adc_2N = channel_ADC_2->GetMean();
			root_adc_3N = channel_ADC_3->GetMean();
			root_adc_4N = channel_ADC_4->GetMean();

			tree_output11->Fill();

			c11->cd();
			channel_ADC_all->SetStats(0);
			channel_ADC_all->Draw("hist");
			channel_ADC_0->SetFillColorAlpha(4,0.25);
			channel_ADC_0->Draw("hist same");
			chan_ADC0_fit->Draw("lsame");

			channel_ADC_1->SetLineColor(2);
			channel_ADC_1->SetFillColorAlpha(2,0.25);
			channel_ADC_1->Draw("hist same");
			chan_ADC1_fit->Draw("lsame");
			
			channel_ADC_2->SetLineColor(3);
			channel_ADC_2->SetFillColorAlpha(3,0.25);
			channel_ADC_2->Draw("hist same");
			chan_ADC2_fit->Draw("lsame");
			
			channel_ADC_3->SetLineColor(5);
			channel_ADC_3->SetFillColorAlpha(5,0.25);
			channel_ADC_3->Draw("hist same");
			chan_ADC3_fit->Draw("lsame");

			channel_ADC_4->SetLineColor(6);
			channel_ADC_4->SetFillColorAlpha(6,0.25);
			channel_ADC_4->Draw("hist same");
			chan_ADC4_fit->Draw("lsame");


			
			
			c11->Print(Form("%s/folder_%s/chip%d_detail_adcwidthall.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c11->Clear();



			
			TGraphErrors * chan_ADC_ampl_plot;
			if (channel_ADC_1->GetEntries() !=0)
			{
				chan_ADC_ampl_plot = new TGraphErrors (4,XADC,&chan_ADC_mean[0],XADCerror,&chan_ADC_mean_error[0]);
			}
			else 
			{
				chan_ADC_ampl_plot = new TGraphErrors (4,YADC,XADCerror,XADCerror,XADCerror);
			}
			chan_ADC_ampl_plot->GetXaxis()->SetTitle("ADC");
			chan_ADC_ampl_plot->GetYaxis()->SetTitle("mean of ampl");
			chan_ADC_ampl_plot->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));
			chan_ADC_ampl_plot->SetMarkerStyle(20);
			chan_ADC_ampl_plot->SetMarkerSize(2);
			chan_ADC_ampl_plot->SetMarkerColor(1);
			chan_ADC_ampl_plot->GetXaxis()->SetLimits(-0.5,3.5);

			c9->cd();
			chan_ADC_ampl_plot->Draw("apl");
			if (channel_ADC_1->GetEntries() !=0)
			{
				chan_ADC_ampl_plot->Fit("polynomial1","NQ");	
			}
			else 
			{
				polynomial1->SetParameter(1,0);
				polynomial1->SetChisquare(1000000);
				polynomial1->SetNDF(1);
			}
			c9->cd();
			polynomial1->Draw("lsame");
			
			tex_adc_ampl->DrawLatex(0.12, 0.750, Form("fit :   %.3f * x + %.3f", polynomial1->GetParameter(1), polynomial1->GetParameter(0)));
			c9->cd();
			tex_adc_ampl->DrawLatex(0.12, 0.720, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.2f", polynomial1->GetChisquare(),polynomial1->GetNDF(),polynomial1->GetChisquare()/polynomial1->GetNDF()));			
			c9->Print(Form("%s/folder_%s/chip%d_detail_adcamplmean.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c9->Clear();





			TGraphErrors * chan_ADC_width_plot;
			if (channel_ADC_1->GetEntries() !=0)
			{
				chan_ADC_width_plot = new TGraphErrors (4,XADC,&chan_ADC_width[0],XADCerror,&chan_ADC_width_error[0]);
			}
			else
			{
				chan_ADC_width_plot = new TGraphErrors (4,XADC,XADCerror,XADCerror,XADCerror);
			}
			chan_ADC_width_plot->GetXaxis()->SetTitle("ADC");
			chan_ADC_width_plot->GetYaxis()->SetTitle("gaus width of ADC ");	
			chan_ADC_width_plot->SetTitle(Form("chip_id=%d, chan_id = %d", i4 + 1, i2));
			chan_ADC_width_plot->SetMarkerStyle(20);
			chan_ADC_width_plot->SetMarkerSize(2);
			chan_ADC_width_plot->SetMarkerColor(1);
			chan_ADC_width_plot->GetXaxis()->SetLimits(-0.5,3.5);

			c10->cd();
			chan_ADC_width_plot->Draw("apl");
			if (channel_ADC_1->GetEntries() !=0)
			{
				chan_ADC_width_plot->Fit("polynomial0","NQ");	
			}
			else 
			{
				polynomial0->SetParameter(0,0);
				polynomial0->SetChisquare(1000000);
				polynomial0->SetNDF(1);
			}
			c10->cd();
			polynomial0->Draw("lsame");
			tex_adc_width->DrawLatex(0.12, 0.750, Form("p0 : %.4f", polynomial0->GetParameter(0)));
			c10->cd();
			tex_adc_width->DrawLatex(0.12, 0.720, Form("#chi^{2} : %.2f, NDF : %d, #chi^{2}/NDF : %.3f", polynomial0->GetChisquare(),polynomial0->GetNDF(),polynomial0->GetChisquare()/polynomial0->GetNDF()));
			c10->Print(Form("%s/folder_%s/chip%d_detail_widthvariation.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c10->Clear();


			chip_rchi2 = i4+1;
			chan_rchi2 =  i2;
			slope_value = polynomial1->GetParameter(1);
			slope_rchi2 = polynomial1->GetChisquare()/polynomial1->GetNDF();
			width_value = polynomial0->GetParameter(0);
			width_rchi2 = polynomial0->GetChisquare()/polynomial0->GetNDF();
			tree_output10->Fill();

			width_rchi2_1D->Fill(polynomial0->GetChisquare()/polynomial0->GetNDF());
			slope_rchi2_1D->Fill(polynomial1->GetChisquare()/polynomial1->GetNDF());



			for (int i7 = 0; i7 < 8; i7++)
			{
				if (count_adc[i7] == 0)
				{
					average_adc[i7] = sum_adc[i7] / 1.;
				}
				else
				{
					average_adc[i7] = sum_adc[i7] / count_adc[i7];
				}
			}

			for (int i13 = 0; i13 < chip_ampl[i4][i2].size(); i13++)
			{
				if (chip_ampl[i4][i2][i13] > 0)
				{
					//cout<<"line : 427 check "<<endl;
					check_new->Fill(chip_ampl[i4][i2][i13] - (average_adc[chip_adc[i4][i2][i13]] - average_adc[0]));
					
					if (chip_adc[i4][i2][i13] == 0 || chip_adc[i4][i2][i13] == 2 || chip_adc[i4][i2][i13] == 4 || chip_adc[i4][i2][i13] == 6 )
					{
						offset_width_1D_02->Fill(chip_ampl[i4][i2][i13] - (average_adc[chip_adc[i4][i2][i13]] - average_adc[0]));
					}

					if (chip_adc[i4][i2][i13] == 1 || chip_adc[i4][i2][i13] == 3 || chip_adc[i4][i2][i13] == 5 || chip_adc[i4][i2][i13] == 7)
					{
						offset_width_1D_13->Fill(chip_ampl[i4][i2][i13] - (average_adc[chip_adc[i4][i2][i13]] - average_adc[0]));
					}

				}


			}

			c1->cd();
			ampladc_detail->Draw("COLZ0");
			ampladc_detail->Fit("slope", "NQ");
			c1->cd();
			slope->Draw("lsame");
			slope_TH2->Fill(i2, slope->GetParameter(0));
			ampl_adc_slope[i4].push_back(slope->GetParameter(0));
			ampl_adc_offset[i4].push_back(slope->GetParameter(1));
			//ampl_adc_chiNDF[i4].push_back(slope->GetChisquare()/slope->GetNDF());

			//fp<<slope->GetParameter(0)<<" "<<slope->GetParameter(1)<<" "<<slope->GetChisquare()/slope->GetNDF()<<"\r"<<endl;
			slop_detail_TH->Fill(slope->GetParameter(0));
			if (assembly_check == true)
			{
				if (i4 == 0 && i2 == 0)
					cout << " =========== pad performance check =========" << endl;
				if (slope->GetParameter(0) > 0.115)
				{
					//cout<<" the error pads check, chip_id : "<<i4+1<<" channel : "<<128-(i2/2)<<" slope : "<< slope->GetParameter(0)<<endl;

					if (i4 < 13)
					{
						if (i2 % 2 == 0)
						{
							cout << "1_the error pads check, chip_id : " << i4 + 1 << " channel : " << 128 - (i2 / 2) << " slope : " << slope->GetParameter(0) << endl;
						}
						else
						{
							cout << "2_the error pads check, chip_id : " << i4 + 1 << " channel : " << 64 - ((i2 / 2) - 1) << " slope : " << slope->GetParameter(0) << endl;
						}
					}
					else if (i4 < 26)
					{
						if (i2 % 2 == 0)
						{
							cout << "4_the error pads check, chip_id : " << i4 + 1 << " channel : " << 65 + ((i2 / 2)) << " slope : " << slope->GetParameter(0) << endl;
						}
						else
						{
							cout << "3_the error pads check, chip_id : " << i4 + 1 << " channel : " << 1 + (i2 / 2) << " slope : " << slope->GetParameter(0) << endl;
						}
					}
				}
			}

			c1->cd();
			tex11->DrawLatex(0.12, 0.750, Form("slope : %.4f", slope->GetParameter(0)));
			c1->cd();
			tex11->DrawLatex(0.12, 0.720, Form("offset : %.2f", slope->GetParameter(1)));
			//c1->cd(); tex11 -> DrawLatex (0.12, 0.690, Form("chi2/NDF : %.2f", slope->GetChisquare()/slope->GetNDF()));

			if (run_option == true)
			{
				c1->cd();
				c1->Print(Form("%s/folder_%s/chip%d_detail_ampladc.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			}

			c6->cd();
			check_new->Draw("hist");
			if (check_new->GetEntries() ==0)
			{
				gaus_fit_new->SetParameter(2,0);
			}
			else 
			{
				check_new->Fit("gaus_fit_new", "NQ");	
			}
			
			
			c6->cd();
			gaus_fit_new->Draw("lsame");
			c6->cd();
			tex11->DrawLatex(0.12, 0.750, Form("gaus width : %.4f", gaus_fit_new->GetParameter(2)));
			c6->cd();
			tex11->DrawLatex(0.12, 0.720, Form("gaus size  : %.2f", gaus_fit_new->GetParameter(0)));
			c6->cd();
			tex11->DrawLatex(0.12, 0.690, Form("gaus mean  : %.2f", gaus_fit_new->GetParameter(1)));




			c13->cd();
			offset_width_1D_02->Draw("hist");
			if (offset_width_1D_02->GetEntries() ==0)
			{
				gaus_fit_new_02->SetParameter(2,0);
			}
			else 
			{
				offset_width_1D_02->Fit("gaus_fit_new_02", "NQ");	
			}
			c13->cd();
			gaus_fit_new_02->Draw("lsame");
			c13->cd();
			tex02->DrawLatex(0.12, 0.750, Form("gaus width : %.4f", gaus_fit_new_02->GetParameter(2)));
			c13->cd();
			tex02->DrawLatex(0.12, 0.720, Form("gaus size  : %.2f", gaus_fit_new_02->GetParameter(0)));
			c13->cd();
			tex02->DrawLatex(0.12, 0.690, Form("gaus mean  : %.2f", gaus_fit_new_02->GetParameter(1)));




			c14->cd();
			offset_width_1D_13->Draw("hist");
			if (offset_width_1D_13->GetEntries() ==0)
			{
				gaus_fit_new_13->SetParameter(2,0);
			}
			else 
			{
				offset_width_1D_13->Fit("gaus_fit_new_13", "NQ");	
			}
			c14->cd();
			gaus_fit_new_13->Draw("lsame");
			c14->cd();
			tex13->DrawLatex(0.12, 0.750, Form("gaus width : %.4f", gaus_fit_new_13->GetParameter(2)));
			c14->cd();
			tex13->DrawLatex(0.12, 0.720, Form("gaus size  : %.2f", gaus_fit_new_13->GetParameter(0)));
			c14->cd();
			tex13->DrawLatex(0.12, 0.690, Form("gaus mean  : %.2f", gaus_fit_new_13->GetParameter(1)));



			if (chan_ampl_1D->GetEntries() ==0)
			{
				ef_fit->SetParameter(0,10); //turn on
				ef_fit->SetParameter(1,0); //slope
				ef_fit->SetParameter(2,1);//height
			}
			else 
			{
				ef_fit->SetParameters(30,2, 10); 
				ef_fit->SetParLimits(2,9.9,10.5);
				ef_fit->SetParLimits(1,0.2,10.5);
				// [0] turn_on
				// [1] slope
				// [2] height 
				chan_ampl_1D->Fit("ef_fit", "NQ");	
			}

			c12->cd();		
			adc_stack->Add(channel_ADC_0);
			adc_stack->Add(channel_ADC_1);
			adc_stack->Add(channel_ADC_2);
			adc_stack->Add(channel_ADC_3);
			adc_stack->Add(channel_ADC_4);
			adc_stack->Draw("");
			ef_fit->Draw("lsame");
			c12->Print(Form("%s/folder_%s/chip%d_detail_adcwidthall_stack.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
			c12->Clear();
			adc_stack->Delete();
			
			c7->cd();
			chan_ampl_1D->Draw("hist");
			c7->cd();
			ef_fit->Draw("lsame");
			c7->cd();
			tex_chan_ampl_1D->DrawLatex(0.12, 0.750, Form("turn on : %.4f", ef_fit->GetParameter(0)));
			c7->cd();
			tex_chan_ampl_1D->DrawLatex(0.12, 0.720, Form("width   : %.3f", ef_fit->GetParameter(1)));
			c7->cd();
			tex_chan_ampl_1D->DrawLatex(0.12, 0.690, Form("height  : %.3f", ef_fit->GetParameter(2)));

			TP_turnon_1D->Fill(ef_fit->GetParameter(0));

			chip_TP = i4+1;
			chan_TP = i2;
			TP_turnon = ef_fit->GetParameter(0); 
			TP_width = ef_fit->GetParameter(1);
			TP_height = ef_fit->GetParameter(2);
			tree_output9->Fill();

			response_width[i4].push_back(gaus_fit_new->GetParameter(2));
			response_width02[i4].push_back(gaus_fit_new_02->GetParameter(2));
			response_width13[i4].push_back(gaus_fit_new_13->GetParameter(2));
			channel_entries_check->Fill(check_new->GetEntries());

			Centries_chip.push_back(i4 + 1);
			Centries_channel.push_back(i2);
			Centries_entries.push_back(check_new->GetEntries());

			if (check_new->GetEntries() < entry_cut_l || check_new->GetEntries() > entry_cut_h)
			{
				//cout<<"Outsider entries, please check : "<<Form("chip_id=%d, chan_id = %d",i4+1,i2)<<" Entries : "<<check_new->GetEntries()<<endl;
				entries_chip.push_back(i4 + 1);
				entries_channel.push_back(i2);
				entries_entries.push_back(check_new->GetEntries());
				channel_entries_outsider += 1;
			}

			if (new_check == true)
			{
				c6->cd();
				c6->Print(Form("%s/folder_%s/chip%d_detail_amploffset.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
				c7->cd();
				c7->Print(Form("%s/folder_%s/chip%d_detail_ampl1D.pdf", folder_name.Data(), the_name.Data(), i4 + 1));	

				c13->cd();
				c13->Print(Form("%s/folder_%s/chip%d_detail_offsetwidth02.pdf", folder_name.Data(), the_name.Data(), i4 + 1));
				c14->cd();
				c14->Print(Form("%s/folder_%s/chip%d_detail_offsetwidth13.pdf", folder_name.Data(), the_name.Data(), i4 + 1));

			}
			ampladc_detail->Reset("ICESM");
			check_new->Reset("ICESM");
			chan_ampl_1D->Reset("ICESM");
			offset_width_1D_02->Reset("ICESM");
			offset_width_1D_13->Reset("ICESM");

			channel_ADC_0->Reset("ICESM");
			channel_ADC_1->Reset("ICESM");
			channel_ADC_2->Reset("ICESM");
			channel_ADC_3->Reset("ICESM");
			channel_ADC_4->Reset("ICESM");

			channel_ADC_0_on->Reset("ICESM");
			channel_ADC_1_on->Reset("ICESM");
			channel_ADC_2_on->Reset("ICESM");
			channel_ADC_3_on->Reset("ICESM");
			channel_ADC_4_on->Reset("ICESM");

			channel_ADC_0_on_clone->Reset("ICESM");
			channel_ADC_1_on_clone->Reset("ICESM");
			channel_ADC_2_on_clone->Reset("ICESM");
			channel_ADC_3_on_clone->Reset("ICESM");
			channel_ADC_4_on_clone->Reset("ICESM");

			
			hnew1->Reset("ICESM");
			hnew2->Reset("ICESM");
			hnew3->Reset("ICESM");

			h_ADC_0->Reset("ICESM");
			  
			c1->Clear();
			c6->Clear();
			c7->Clear();
			c13->Clear();
			c14->Clear();
			if (assembly_check == true && i4 == 25 && i2 == 127)
				cout << " =========== pad performance check done =========" << endl;
			for (int i8 = 0; i8 < 8; i8++)
			{
				sum_adc[i8] = 0;
				count_adc[i8] = 0;
				average_adc[i8] = 0;
			}
			chan_ADC_mean.clear();
			chan_ADC_width.clear();
			chan_ADC_width_error.clear();
			chan_ADC_mean_error.clear();

		} // channel 128


		
		cout << " " << endl;
		if (run_option == true)
		{
			c1->cd();
			c1->Print(Form("%s/folder_%s/chip%d_detail_ampladc.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
		}
		if (new_check == true)
		{
			c6->cd();
			c6->Print(Form("%s/folder_%s/chip%d_detail_amploffset.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c6->Clear();

			c7->cd();
			c7->Print(Form("%s/folder_%s/chip%d_detail_ampl1D.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));						
			c7->Clear();

			c8->cd();
			c8->Print(Form("%s/folder_%s/chip%d_detail_adcwidth.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));	
			c8->Clear();

			c9->cd();
			c9->Print(Form("%s/folder_%s/chip%d_detail_adcamplmean.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c9->Clear();

			c10->cd();
			c10->Print(Form("%s/folder_%s/chip%d_detail_widthvariation.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c10->Clear();

			c11->cd();
			c11->Print(Form("%s/folder_%s/chip%d_detail_adcwidthall.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c11->Clear();

			c12->cd();
			c12->Print(Form("%s/folder_%s/chip%d_detail_adcwidthall_stack.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c12->Clear();

			c13->cd();
			c13->Print(Form("%s/folder_%s/chip%d_detail_offsetwidth02.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c13->Clear();

			c14->cd();
			c14->Print(Form("%s/folder_%s/chip%d_detail_offsetwidth13.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c14->Clear();


			c15->cd();
			c15->Print(Form("%s/folder_%s/chip%d_detail_adcerr.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c15->Clear();

			c16->cd();
			c16->Print(Form("%s/folder_%s/chip%d_detail_acumerr.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c16->Clear();

			c17->cd();
			c17->Print(Form("%s/folder_%s/chip%d_detail_adcerr_acum.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c17->Clear();

			c18->cd();
			c18->Print(Form("%s/folder_%s/chip%d_detail_adc_compactor.pdf)", folder_name.Data(), the_name.Data(), i4 + 1));
			c18->Clear();

		}
	} // chip 26 end



	c19->cd();
	err_width->Draw("hist");
	err_width->Fit("gaus_fit_width","NQ");
	gaus_fit_width->Draw("lsame");
	TLatex *tex_acum_gaus_width = new TLatex();
	tex_acum_gaus_width->SetNDC();
	tex_acum_gaus_width->SetTextFont(42);
	tex_acum_gaus_width->SetTextSize(0.032);
	tex_acum_gaus_width->DrawLatex(0.2, 0.840, Form("Gaus Mean : %.4f", gaus_fit_width->GetParameter(1)));
	tex_acum_gaus_width->DrawLatex(0.2, 0.80, Form("Gaus Sigma : %.4f", gaus_fit_width->GetParameter(2)));

	c19->Print(Form("%s/folder_%s/err_width_distri.pdf", folder_name.Data(), the_name.Data()));
	c19->Clear();

	c20->cd();
	width_pvalue_1D->Draw("hist");
	c20->Print(Form("%s/folder_%s/err_width_pvalue.pdf", folder_name.Data(), the_name.Data()));
	c20->Clear();

	
	
	cout << "==============information of ampladc_detail ==========" << endl;
	c1->Clear();
	c1->cd();
	slope_TH2->Draw("COLZ0");
	c1->Print(Form("%s/folder_%s/ampl_adc_slope_TH2.pdf", folder_name.Data(), the_name.Data()));
	c1->Clear();
	cout << " TH2 plot " << endl;
	cout << " " << endl;
	cout << " " << endl;
	slop_detail_TH->Fit("gaus_fit", "NQ");
	c2->cd();
	slop_detail_TH->Draw("hist");
	TLatex *tex2 = new TLatex();
	tex2->SetNDC();
	tex2->SetTextFont(42);
	tex2->SetTextSize(0.032);
	tex2->DrawLatex(0.2, 0.840, Form("Gaus Mean : %.4f", gaus_fit->GetParameter(1)));
	tex2->DrawLatex(0.2, 0.80, Form("Gaus Sigma : %.4f", gaus_fit->GetParameter(2)));
	gaus_fit->Draw("lsame");
	c2->Print(Form("%s/folder_%s/slop_detail_TH.pdf", folder_name.Data(), the_name.Data()));
	c2->Clear();

	cout << "==============information of rest plots ==========" << endl;

	//int xaxis_plot[128];

	vector<double> xaxis_plot;
	xaxis_plot.clear();
	for (int i6 = 0; i6 < 128; i6++)
		xaxis_plot.push_back(i6);
	c1->Print(Form("%s/folder_%s/ampl_adc_slope_detail.pdf(", folder_name.Data(), the_name.Data()));
	cout << " the size of file : " << ampl_adc_slope[0].size() << endl;

	TGraph *grr[26];

	c4->Clear();
	c4->Divide(13, 2);
	for (int i4 = 0; i4 < 26; i4++)
	{
		c4->cd(i4 + 1);

		grr[i4] = new TGraph(128, &xaxis_plot[0], &ampl_adc_slope[i4][0]);
		grr[i4]->GetYaxis()->SetRangeUser(0.04, 0.15);
		grr[i4]->SetTitle(Form("S_chip_id=%d", i4 + 1));
		grr[i4]->GetXaxis()->SetLimits(-5, 135);
		grr[i4]->SetMarkerStyle(20);
		grr[i4]->SetMarkerColor(2);
		grr[i4]->SetMarkerSize(1.5);
		grr[i4]->Draw("al");

		c1->cd();
		grr[i4]->Draw("apl");
		c1->Print(Form("%s/folder_%s/ampl_adc_slope_detail.pdf", folder_name.Data(), the_name.Data()));

		//ampl_adc->Reset("ICESM");
	}
	c4->Print(Form("%s/folder_%s/ampl_adc_slope.pdf", folder_name.Data(), the_name.Data()));
	c4->Clear();
	for (int i6 = 0; i6 < 26; i6++)
		delete grr[i6];
	c1->Print(Form("%s/folder_%s/ampl_adc_slope_detail.pdf)", folder_name.Data(), the_name.Data()));
	c1->Clear();

	c6->Clear();
	c6->Print(Form("%s/folder_%s/ampl_adc_width_detial.pdf(", folder_name.Data(), the_name.Data()));
	TGraph *grr_new[26];
	TH2F *width_TH2 = new TH2F("", "", 128, 0, 128, 128, 0.00, 13);
	width_TH2->SetStats(0);
	for (int i9 = 0; i9 < 26; i9++)
	{
		grr_new[i9] = new TGraph(128, &xaxis_plot[0], &response_width[i9][0]);
		grr_new[i9]->GetYaxis()->SetRangeUser(0.00, 13);
		grr_new[i9]->SetTitle(Form("S_chip_id=%d", i9 + 1));
		grr_new[i9]->GetXaxis()->SetLimits(-5, 135);
		grr_new[i9]->SetMarkerStyle(20);
		grr_new[i9]->SetMarkerColor(2);
		grr_new[i9]->SetMarkerSize(1.5);
		for (int i10 = 0; i10 < response_width[i9].size(); i10++)
		{
			width_TH2->Fill(xaxis_plot[i10], response_width[i9][i10]);
			//cout<<"TESTTEST : "<<response_width[i9].size()<<endl;
			if (true == true)
			{
				chip_W = i9 + 1;
				chan_W = i10;
				level_W = response_width[i9][i10];
				level_W02 = response_width02[i9][i10];
				level_W13 = response_width13[i9][i10];
				tree_output4->Fill();



				chip_ER = i9 + 1;
				chan_ER = i10;
				level_ADC0 = response_width0[i9][i10];
				level_ADC1 = response_width1[i9][i10];
				level_ADC2 = response_width2[i9][i10];
				level_ADC3 = response_width3[i9][i10];
				level_pvalue = response_pvalue[i9][i10];
				tree_output12->Fill();


				chip_ERM = i9 + 1;
				chan_ERM = i10;
				mean_ADC0 = response_mean0[i9][i10];
				mean_ADC1 = response_mean1[i9][i10];
				mean_ADC2 = response_mean2[i9][i10];
				mean_ADC3 = response_mean3[i9][i10];
				tree_output13->Fill();


				
				if (response_width[i9][i10] < Gaus_width_cut && response_width[i9][i10] !=0)
				{
					unbound_chip.push_back(i9 + 1);
					unbound_channel.push_back(i10);
					unbound_width.push_back(response_width[i9][i10]);
				}
				else /* (response_width[i9][i10] > Gaus_width_cut)*/
				{
					noise_chip.push_back(i9 + 1);
					noise_channel.push_back(i10);
					noise_width.push_back(response_width[i9][i10]);
				}
			}
		}
		c6->cd();
		grr_new[i9]->Draw("apl");

		c6->Print(Form("%s/folder_%s/ampl_adc_width_detial.pdf", folder_name.Data(), the_name.Data()));
	}
	c6->Print(Form("%s/folder_%s/ampl_adc_width_detial.pdf)", folder_name.Data(), the_name.Data()));
	c6->Clear();
	c6->cd();
	width_TH2->Draw("COLZ0");
	c6->Print(Form("%s/folder_%s/ampl_adc_width_detial_TH2.pdf", folder_name.Data(), the_name.Data()));
	c6->Clear();

	c1->Print(Form("%s/folder_%s/ampl_adc_offset_detail.pdf(", folder_name.Data(), the_name.Data()));
	//TGraph * grr[26];

	c4->Divide(13, 2);
	for (int i4 = 0; i4 < 26; i4++)
	{
		c4->cd(i4 + 1);

		grr[i4] = new TGraph(128, &xaxis_plot[0], &ampl_adc_offset[i4][0]);
		grr[i4]->SetTitle(Form("O_chip_id=%d", i4 + 1));
		grr[i4]->GetXaxis()->SetLimits(-5, 135);
		grr[i4]->SetMarkerStyle(20);
		grr[i4]->SetMarkerColor(2);
		grr[i4]->SetMarkerSize(1.5);
		grr[i4]->Draw("al");

		c1->cd();
		grr[i4]->Draw("apl");
		c1->Print(Form("%s/folder_%s/ampl_adc_offset_detail.pdf", folder_name.Data(), the_name.Data()));

		//ampl_adc->Reset("ICESM");
	}
	c4->Print(Form("%s/folder_%s/ampl_adc_offset.pdf", folder_name.Data(), the_name.Data()));
	c4->Clear();
	c1->Print(Form("%s/folder_%s/ampl_adc_offset_detail.pdf)", folder_name.Data(), the_name.Data()));
	c1->Clear();
	for (int i6 = 0; i6 < 26; i6++)
		delete grr[i6];

	// c1->Print(Form("%s/%s/ampl_adc_ChiNDF_detail.pdf(",the_name.Data()));
	// TGraph * grr[26];

	// c4->Divide(13,2);
	//   for (int i4=0; i4<26; i4++)
	//    {
	//    	 c4->cd(i4+1);

	//    	 grr[i4] = new TGraph (128,&xaxis_plot[0],&ampl_adc_chiNDF[i4][0]);

	//    	 grr[i4]->SetTitle(Form("C_chip_id=%d",i4+1));
	//    	 grr[i4]->GetXaxis()->SetLimits(-5,135);
	//    	 grr[i4]->SetMarkerStyle(20);
	//    	 grr[i4]->SetMarkerColor(2);
	//    	 grr[i4]->SetMarkerSize(1.5);
	//    	 grr[i4]->Draw("al");
	//    	 c1->cd (); grr[i4]->Draw("apl");
	//   	 c1->Print(Form("%s/%s/ampl_adc_ChiNDF_detail.pdf",the_name.Data()));

	//   	 //ampl_adc->Reset("ICESM");
	//    }
	// c4->Print(Form("%s/%s/ampl_adc_chiNDF.pdf",the_name.Data()));
	// c4 ->Clear();
	// for (int i6=0; i6<26; i6++) delete grr[i6];

	// cout<<"==============information of rest plots =========="<<endl;
	// c4 ->Clear();
	// c1->Print(Form("%s/%s/ampl_adc_ChiNDF_detail.pdf)",the_name.Data()));
	// c1 ->Clear();

	c4->Clear();
	c1->Clear();

	//fp.close();//Ãö³¬ÀÉ®×

	for (int i11 = 0; i11 < noise_channel.size(); i11++)
	{
		//source_name=Form("%s",the_name.Data());
		chip_N = noise_chip[i11];
		chan_N = noise_channel[i11];
		level_N = noise_width[i11];
		tree_output1->Fill();
	}
	for (int i11 = 0; i11 < entries_entries.size(); i11++)
	{
		//source_name=Form("%s",the_name.Data());
		chip_E = entries_chip[i11];
		chan_E = entries_channel[i11];
		level_E = entries_entries[i11];
		tree_output2->Fill();
	}

	for (int i11 = 0; i11 < unbound_chip.size(); i11++)
	{
		//source_name=Form("%s",the_name.Data());
		chip_U = unbound_chip[i11];
		chan_U = unbound_channel[i11];
		level_U = unbound_width[i11];
		tree_output3->Fill();
	}

	for (int i11 = 0; i11 < Centries_chip.size(); i11++)
	{
		//source_name=Form("%s",the_name.Data());
		chip_CE = Centries_chip[i11];
		chan_CE = Centries_channel[i11];
		level_CE = Centries_entries[i11];
		tree_output6->Fill();
	}

	if (unbound_check == true)
	{
		cout << "======unbound_check======" << endl;
		cout << "number of unbounded channels : " << unbound_chip.size() << endl;
		cout << "chip : channel " << endl;
		for (int i11 = 0; i11 < unbound_chip.size(); i11++)
		{
			cout << unbound_chip[i11] << " : " << unbound_channel[i11] << endl;
		}
	}

	if (noise_channel_check == true)
	{
		cout << "======noise_check======" << endl;
		cout << "number of noise channels : " << noise_channel.size() << endl;
		cout << "chip : channel " << endl;
		for (int i11 = 0; i11 < noise_channel.size(); i11++)
		{
			cout << noise_chip[i11] << " : " << noise_channel[i11] << " width : " << noise_width[i11] << endl;
		}
	}

	c1->cd();
	channel_entries_check->Draw("hist");
	c1->Print(Form("%s/folder_%s/channel_entries.pdf", folder_name.Data(), the_name.Data()));
	c1->Clear();

	for(int i111=0; i111<26; i111++)
	{
		for(int i112=0; i112<128; i112++)
		{
			threshold_cut_hist->Fill(threshold_cut_array[i111][i112]);
			chip_CE_ampl = i111+1;
			chan_CE_ampl = i112;
			level_CE_ampl = threshold_cut_array[i111][i112];
			tree_output8->Fill();

		}	
	}

	c1->cd();
	TLatex *tex1111 = new TLatex();
	tex1111->SetNDC();
	//tex11 -> SetTextFont (42);
	tex1111->SetTextSize(0.028);
	threshold_cut_hist->Draw("hist");
	tex1111->DrawLatex(0.12, 0.750, Form("ampl cut : %.0f", double(threshold_cut)));
	c1->Print(Form("%s/folder_%s/threshold_channel_entries.pdf", folder_name.Data(), the_name.Data()));
	c1->Clear();

	c1->cd();
	TP_turnon_1D->Draw("hist");
	c1->Print(Form("%s/folder_%s/TP_turnon_1D.pdf", folder_name.Data(), the_name.Data()));
	c1->Clear();

	c1->cd();
	width_rchi2_1D->Draw("hist");
	c1->Print(Form("%s/folder_%s/width_rchi2_1D.pdf", folder_name.Data(), the_name.Data()));
	c1->Clear();	

	c1->cd();
	slope_rchi2_1D->Draw("hist");
	c1->Print(Form("%s/folder_%s/slope_rchi2_1D.pdf", folder_name.Data(), the_name.Data()));
	c1->Clear();

	//##################################################################

	entries_mean = channel_entries_check->GetMean();
	entries_width = channel_entries_check->GetStdDev();
	tree_output5->Fill();

	entries_mean_ampl = threshold_cut_hist->GetMean();
	entries_width_ampl = threshold_cut_hist->GetStdDev();
	tree_output7->Fill();

	//##################################################################


	cout << "Number of channel entries outsider : " << channel_entries_outsider << endl;
	cout << "Entries outsider list : " << endl;
	for (int i11 = 0; i11 < entries_entries.size(); i11++)
	{
		cout << entries_chip[i11] << " : " << entries_channel[i11] << " entries : " << entries_entries[i11] << endl;
	}

	if (multi_run == true)
	{
		for (int i12 = 0; i12 < data_a.size(); i12++)
		{
			multi_txt << data_a[i12] << " " << data_b[i12] << " " << data_c[i12] << " " << data_d[i12] << " " << data_e[i12] << "\r" << endl;
		}
		multi_txt << Form("data name : %s.root .", the_name.Data()) << "\r" << endl;
		multi_txt << Form("noise channel number : ") << noise_channel.size() << "\r" << endl;
		for (int i11 = 0; i11 < noise_channel.size(); i11++)
		{
			multi_txt << noise_chip[i11] << " : " << noise_channel[i11] << " : " << noise_width[i11] << "\r" << endl;
		}
		multi_txt << Form("entries_outsider channel number : ") << channel_entries_outsider << "\r" << endl;
		for (int i11 = 0; i11 < entries_entries.size(); i11++)
		{
			multi_txt << entries_chip[i11] << " : " << entries_channel[i11] << " : " << entries_entries[i11] << endl;
		}
		multi_txt << "~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~"<< "\r" << endl;
		multi_txt.close();

		// for (int i11=0; i11<DNA.size(); i11++)
		// {
		// 	multi_txt_noise<<D_NA[i11]<<" "<<D_NB[i11]<<" "<<D_NC[i11]<<" "<<D_ND[i11]<<" "<<D_NE[i11]<<"\r"<<endl;
		// }
	}

	tree_output1->Write("", TObject::kOverwrite);
	tree_output2->Write("", TObject::kOverwrite);
	tree_output3->Write("", TObject::kOverwrite);
	tree_output4->Write("", TObject::kOverwrite);
	tree_output5->Write("", TObject::kOverwrite);
	tree_output6->Write("", TObject::kOverwrite);
	tree_output7->Write("", TObject::kOverwrite);
	tree_output8->Write("", TObject::kOverwrite);
	tree_output9->Write("", TObject::kOverwrite);
	tree_output10->Write("", TObject::kOverwrite);
	tree_output11->Write("", TObject::kOverwrite);
	tree_output12->Write("", TObject::kOverwrite);
	tree_output13->Write("", TObject::kOverwrite);
	// delete tree_output1;
	// delete tree_output2;

	//delete tree_output3;
	//delete file_output;
	file_output->Close();

	cout << "\r\n"<< "#the file rewriting is finish#"<< "\r\n"<< endl;
}
