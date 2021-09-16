void ladder_summary (TString folder_name)
{
	//=================cut variable=================
	 //TString folder_name = "/home/5202011/INTT_cal/INTT_cal_test/ladder_cali/02-0028";

	ifstream cut_value;
	cut_value.open(Form("%s/output_cut_value.txt", folder_name.Data()));
	TString criteria_name, imtermediate_name;
	double cut_value_low, cut_value_high;

	vector<double> cut_array_low; cut_array_low.clear();
	vector<double> cut_array_high; cut_array_high.clear();


	while (1)
	{
		cut_value >> criteria_name >> cut_value_low >> imtermediate_name >> cut_value_high;
		cut_array_low.push_back(cut_value_low);
		cut_array_high.push_back(cut_value_high);
		if (!cut_value.good())
		{
			break;
		}
	}



	int all_in_one [2][26][128];
	int decoder=0x7FF;
	const int number_of_selection = 10;
	int file_of_each_ladder = 1; 






	//1
	double width_noise_cut_high = cut_array_high[0];
	double width_noise_cut_low = cut_array_low[0]; 

	//10
	double entry_cut_high = cut_array_high[1];
	double entry_cut_low = cut_array_low[1];

	//100
	double ampl_entry_cut_high = cut_array_high[2];
	double ampl_entry_cut_low = cut_array_low[2];

	// //1000 (not use now)
	// double adc0_cut_high = cut_array_high[3];
	// double adc0_cut_low  = cut_array_low[3];

	//1000 (numeric)
	double adc0_cut_highN = cut_array_high[3];
	double adc0_cut_lowN  = cut_array_low[3];
	
	//1 0000
	double unbonded_cut = 4;
	
	//10 0000
	double slope_pvalue_cut = cut_array_high[6];//(5 sigma)
 	
 	//100 0000
	double width_pvalue_cut = cut_array_high[5];//(5 sigma)

	//1000 0000
	double TP_cut_high = cut_array_high[4];
	double TP_cut_low  = cut_array_low[4];
	
	//1 0000 0000
	double slope_correction = (cut_array_low[10]+cut_array_low[11])/2.;
	double slope_cut_high = cut_array_high[7];
	double slope_cut_low = cut_array_low[7];

	//10 0000 0000
	double width_noise_cut_high02 = cut_array_high[8];
	double width_noise_cut_low02 = cut_array_low[8]; 

	double width_noise_cut_high13 = cut_array_high[9];
	double width_noise_cut_low13 = cut_array_low[9];

	cout<<"width noise all  : "<<width_noise_cut_low<<"	"<<width_noise_cut_high<<endl;
	cout<<"entry cut        : "<<entry_cut_low<<"	"<<entry_cut_high<<endl;
	cout<<"entry ampl cut   : "<<ampl_entry_cut_low<<"	"<<ampl_entry_cut_high<<endl;
	//cout<<"adc0             : "<<adc0_cut_low<<"	"<<adc0_cut_high<<endl;
	cout<<"adc0 numeric     : "<<adc0_cut_lowN<<"	"<<adc0_cut_highN<<endl;
	cout<<"threshold position "<<TP_cut_low<<"	"<<TP_cut_high<<endl;
	cout<<"width pvalue     : "<<width_pvalue_cut<<endl;
	cout<<"slope pvalue     : "<<slope_pvalue_cut<<endl;
	cout<<" slope value     : "<<slope_cut_low<<"	"<<slope_cut_high<<endl;
	cout<<"widht noise 02   : "<<width_noise_cut_low02<<"	"<<width_noise_cut_high02<<endl;
	cout<<"widht noise 13   : "<<width_noise_cut_low13<<"	"<<width_noise_cut_high13<<endl;
	cout<<"slope correction : "<<slope_correction<<endl;




	// //1
	// double width_noise_cut_high = 3.94959;
	// double width_noise_cut_low = 2.29972; 

	// //10
	// double entry_cut_high = 376.583;
	// double entry_cut_low = 265.11;

	// //100
	// double ampl_entry_cut_high = 200;
	// double ampl_entry_cut_low = 170;

	// //1000
	// double adc0_cut_high = 41.1099;
	// double adc0_cut_low  = 30.076;
	
	// //1 0000
	// double unbonded_cut = 4;
	
	// //10 0000
	// double slope_pvalue_cut = 0.0000006;//(5 sigma)
 	
 // 	//100 0000
	// double width_pvalue_cut = 0.0000006;//(5 sigma)

	// //1000 0000
	// double TP_cut_high = 37.9863;
	// double TP_cut_low  = 26.2654;
	
	// //1 0000 0000
	// double slope_correction = (0.022+0.023)/2.;
	// double slope_cut_high = 8.4377;
	// double slope_cut_low = 5.44815;

	// //10 0000 0000
	// double width_noise_cut_high02 = 4.56993;
	// double width_noise_cut_low02 = 2.03665; 

	// double width_noise_cut_high13 = 4.5427;
	// double width_noise_cut_low13 = 2.04647; 
	

	//         1 : 0x01
	//        10 : 0x02
	//       100 : 0x04
	//      1000 : 0x08
	//    1 0000 : 0x10
	//   10 0000 : 0x20
	//  100 0000 : 0x40
	// 1000 0000 : 0x80
   //1 0000 0000 : 0x100
  //10 0000 0000 : 0x200	

	//=================cut variable=================

	TString list_buffer;
	vector<TString> list_array;
	list_array.clear();

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

	ofstream bad_channel_detail;
	bad_channel_detail.open(Form("%s/bad_channel_detail.txt", folder_name.Data()), ios::out); 
	if (!bad_channel_detail)
	{ 
		cout << "Fail to open file: " << endl;
	}


	TFile *f1;// = TFile::Open(Form("cut_finder_folder/sum_up_all.root"));

	TTree *chan_gaus_width;// = (TTree *)f1->Get("chan_gaus_width"); //value_W
	TTree *chan_entry;// = (TTree *)f1->Get("chan_entry"); //value_E
	TTree *ampl_channel_entries;// = (TTree *)f1->Get("ampl_channel_entries"); //value_EA
	TTree *chan_thre_position;// = (TTree *)f1->Get("chan_thre_position"); //value_TP
	TTree *chan_adc_study;// = (TTree *)f1->Get("chan_adc_study"); //value_adc_0
	TTree *chan_study ;// = (TTree *)f1->Get("chan_study"); //value_slope_chi, value_width_chi


	int chip_W, chan_W;
	double value_W, value_W02, value_W13;

	int chip_E, chan_E;
	int value_E;

	int chip_EA, chan_EA;
	int value_EA;

	int chip_TP, chan_TP;
	double value_TP;

	int chip_adc, chan_adc;
	double value_adc_0;
	double value_adc_0N;

	int chip_chi, chan_chi;
	double value_slope_chi, value_width_chi;
	double value_slope_value, value_width_value;

	int bad_chan_0=0;
	int bad_chan_1=0;

	// vector<int> bad_chip_W; bad_chip_W.clear();
	// vector<int> bad_chan_W; bad_chan_W.clear();
	// vector<int> bad_value_W; bad_value_W.clear(); // width -> 1

	// vector<int> bad_chip_E; bad_chip_E.clear();
	// vector<int> bad_chan_E; bad_chan_E.clear();
	// vector<int> bad_value_E; bad_value_E.clear(); // entry ->10

	// vector<int> bad_chip_EA; bad_chip_EA.clear();
	// vector<int> bad_chan_EA; bad_chan_EA.clear();
	// vector<int> bad_value_EA; bad_value_EA.clear(); // entry after ampl cut ->100

	// vector<int> bad_chip_adc0; bad_chip_adc0.clear();
	// vector<int> bad_chan_adc0; bad_chan_adc0.clear();
	// vector<int> bad_value_adc0; bad_value_adc0.clear(); // mean of adc0 ->1000

	// vector<int> bad_chip_UB; bad_chip_UB.clear();
	// vector<int> bad_chan_UB; bad_chan_UB.clear();
	// vector<int> bad_value_UB; bad_value_UB.clear(); //unbonded channel ->10000

	// vector<int> bad_chip_SC; bad_chip_SC.clear();
	// vector<int> bad_chan_SC; bad_chan_SC.clear();
	// vector<int> bad_value_SC; bad_value_SC.clear(); //slope chi2 ->100000

	// vector<int> bad_chip_WC; bad_chip_WC.clear();
	// vector<int> bad_chan_WC; bad_chan_WC.clear();
	// vector<int> bad_value_WC; bad_value_WC.clear(); //width chi2 ->1000000

	int file_ID = 0; 
	
	for (int i=0; i<2; i++)
	{
		for (int i1=0; i1<26; i1++)
		{
			for (int i2=0; i2<128; i2++)
			{
				all_in_one[i][i1][i2]=decoder;
			}
		}
	}



	for (int i=0; i<2; i++)
	{
		

		for (int i1=0; i1<file_of_each_ladder; i1++)
		{
			//cout<<"test : "<<file_ID<<endl;
			f1 = TFile::Open(Form("%s/folder_%s/%s_summary.root",folder_name.Data(),list_array[file_ID].Data(),list_array[file_ID].Data()));

			chan_gaus_width = (TTree *)f1->Get("chan_gaus_width"); //value_W
			chan_entry = (TTree *)f1->Get("chan_entry"); //value_E
			ampl_channel_entries = (TTree *)f1->Get("ampl_channel_entries"); //value_EA
			chan_thre_position = (TTree *)f1->Get("chan_thre_position"); //value_TP
			chan_adc_study = (TTree *)f1->Get("chan_adc_study"); //value_adc_0
			chan_study = (TTree *)f1->Get("chan_study"); //value_slope_chi, value_width_chi

			chan_gaus_width->SetBranchAddress("chip_id", &chip_W);
			chan_gaus_width->SetBranchAddress("chan_id", &chan_W);
			chan_gaus_width->SetBranchAddress("Gaus_width", &value_W);
			chan_gaus_width->SetBranchAddress("Gaus_width02", &value_W02);
			chan_gaus_width->SetBranchAddress("Gaus_width13", &value_W13);


			chan_entry->SetBranchAddress("chip_id",&chip_E);
			chan_entry->SetBranchAddress("chan_id",&chan_E);
			chan_entry->SetBranchAddress("chan_entry",&value_E);

			ampl_channel_entries->SetBranchAddress("chip_id",&chip_EA);
			ampl_channel_entries->SetBranchAddress("chan_id",&chan_EA);
			ampl_channel_entries->SetBranchAddress("chan_entry",&value_EA);

			chan_thre_position->SetBranchAddress("chip_id",&chip_TP);
			chan_thre_position->SetBranchAddress("chan_id",&chan_TP);
			chan_thre_position->SetBranchAddress("TP_turnon",&value_TP);

			chan_adc_study->SetBranchAddress("chip_id",&chip_adc);
			chan_adc_study->SetBranchAddress("chan_id",&chan_adc);
			chan_adc_study->SetBranchAddress("adc0",&value_adc_0);
			chan_adc_study->SetBranchAddress("adc0N",&value_adc_0N);


			chan_study->SetBranchAddress("chip_id",&chip_chi);
			chan_study->SetBranchAddress("chan_id",&chan_chi);
			chan_study->SetBranchAddress("slope_value",&value_slope_value);
			chan_study->SetBranchAddress("slope_rchi2",&value_slope_chi);
			chan_study->SetBranchAddress("width_value",&value_width_value);
			chan_study->SetBranchAddress("width_rchi2",&value_width_chi);

			int total_size_W = chan_gaus_width->GetEntriesFast();
			int total_size_E = chan_entry->GetEntriesFast();
			int total_size_EA = ampl_channel_entries->GetEntriesFast();
			int total_size_TP = chan_thre_position->GetEntriesFast();
			int total_size_adc = chan_adc_study->GetEntriesFast();
			int total_size_chi = chan_study->GetEntriesFast();

			cout<<"width :              "<<total_size_W<<endl;
			cout<<"entry :              "<<total_size_E<<endl;
			cout<<"ampl entry :         "<<total_size_EA<<endl;
			cout<<"threshold position : "<<total_size_TP<<endl;
			cout<<"adc :                "<<total_size_adc<<endl;
			cout<<"rchi2 :              "<<total_size_chi<<endl;
			cout<<" "<<endl;

			// if (i1==0)
			// {
			// 	for (int i2=0; i2<total_size_chi; i2++)
			// 	{
			// 		chan_gaus_width->GetEntry(i2);
			// 		// chan_entry->GetEntry(i2);
			// 		// ampl_channel_entries->GetEntry(i2);
			// 		// chan_thre_position->GetEntry(i2);
			// 		// chan_adc_study->GetEntry(i2);
			// 		// chan_study->GetEntry(i2);
					
			// 		if (value_W<unbonded_cut && value_W !=0 )
			// 		{
			// 			all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x10;
			// 		}

			// 	}
			// }

			if (1==1) 
			{				
				for (int i2=0; i2<total_size_chi; i2++)
				{
					chan_gaus_width->GetEntry(i2);
					chan_entry->GetEntry(i2);
					ampl_channel_entries->GetEntry(i2);
					chan_thre_position->GetEntry(i2);
					chan_adc_study->GetEntry(i2);
					chan_study->GetEntry(i2);

					// if (value_W<width_noise_cut_low || value_W>width_noise_cut_high)//1
					// {
					// 	all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x01;
					// }
					if (value_E<entry_cut_low || value_E>entry_cut_high )
					{
						all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x02;
					}

					if (value_EA<ampl_entry_cut_low || value_EA>ampl_entry_cut_high )
					{
						all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x04;
					}

					if (value_adc_0N<adc0_cut_lowN || value_adc_0N>adc0_cut_highN )
					{
						all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x08;
					}

					if (TMath::Prob(value_slope_chi, 2)<slope_pvalue_cut)
					{
						all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x020;
					}

					if (TMath::Prob(value_width_chi, 3)<width_pvalue_cut)
					{
						all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x40;
					}
					
					if (value_TP < TP_cut_low|| value_TP > TP_cut_high)
					{
						all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x80;	
					}

					if ( (value_slope_value-(slope_correction*chip_chi)) < slope_cut_low|| (value_slope_value-(slope_correction*chip_chi)) > slope_cut_high)
					{
						all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x100;	
					}

					if (value_W02<width_noise_cut_low02 || value_W02>width_noise_cut_high02	)
					{
						all_in_one[i][chip_W-1][chan_W]=all_in_one[i][chip_W-1][chan_W]^0x200;
					}


				}		
			}

			file_ID +=1;

		}
	}
	
	int ladder_B, chip_B, chan_B, decoder_B;
	int bad_channel;

	TFile *file_output = new TFile(Form("%s/bad_channel_summary.root", folder_name.Data()), "RECREATE");
	TTree *tree_output1 = new TTree("bad_channel_detail", Form("the detail of bad channel"));
	TTree *tree_output2 = new TTree("total_number", Form("total number of bad channels of this ladder"));

	tree_output1->Branch("ladder_id", &ladder_B);
	tree_output1->Branch("chip_id", &chip_B);
	tree_output1->Branch("chan_id", &chan_B);
	tree_output1->Branch("decoder", &decoder_B);

	tree_output2->Branch("bad_channel",&bad_channel);

	TCanvas * c1 = new TCanvas ("c1","c1",1200,800);
	c1->cd();

	TH2F * bad_channel_map = new TH2F ("","bad channel map",52,1,53,128,0,128);
	bad_channel_map->GetXaxis()->SetTitle("Chip ID");
	bad_channel_map->GetYaxis()->SetTitle("Channel ID");
	bad_channel_map->SetStats(0);

	for (int i1=0; i1<26; i1++)
	{
		for (int i2=0; i2<128; i2++)
		{	
			if (all_in_one[0][i1][i2]!= decoder)
			{
				cout<<"bad channel in ladder 0, chip : "<<i1+1<<"	channel : "<<i2<<"	decode : "<<std::bitset<number_of_selection>(all_in_one[0][i1][i2])<<endl;
				bad_chan_0 +=1;

				ladder_B = 0;
				chip_B = i1+1;
				chan_B = i2;
				decoder_B = all_in_one[0][i1][i2];
				tree_output1->Fill();
				bad_channel_map->Fill(i1+1,i2);

				bad_channel_detail<< 0 <<"	"<<i1+1<<"	"<<i2<<"	"<<std::bitset<number_of_selection>(all_in_one[0][i1][i2])<<"\r"<<endl;
			}
		}
	}	
	
	cout<<" "<<endl;
	cout<<"================================================================================="<<endl;
	cout<<"================================================================================="<<endl;
	cout<<"================================================================================="<<endl;
	cout<<"================================================================================="<<endl;
	cout<<" "<<endl;
	for (int i1=0; i1<26; i1++)
	{
		for (int i2=0; i2<128; i2++)
		{
			if (all_in_one[1][i1][i2]!= decoder)
			{
				cout<<"bad channel in ladder 1, chip : "<<i1+1<<"	channel : "<<i2<<"	decode : "<<std::bitset<number_of_selection>(all_in_one[1][i1][i2])<<endl;
				bad_chan_1 +=1;

				ladder_B = 1;
				chip_B = i1+1;
				chan_B = i2;
				decoder_B = all_in_one[1][i1][i2];
				tree_output1->Fill();
				bad_channel_map->Fill(i1+1+26,i2);

				bad_channel_detail<< 1 <<"	"<<i1+1<<"	"<<i2<<"	"<<std::bitset<number_of_selection>(all_in_one[1][i1][i2])<<"\r"<<endl;
			}
		}
	}	
	
	bad_channel_map->Draw("colz0");
	c1->Print(Form("%s/bad_channel_map.pdf",folder_name.Data()));

	cout<<" "<<endl;
	cout<<"////////////////////////////"<<endl;
	cout<<"////  final results   //////"<<endl;
	cout<<"////////////////////////////"<<endl;	
	cout<<" "<<endl;
	cout<<"Ladder0 bad channel : "<<bad_chan_0<<" ladder1 bad channel : "<<bad_chan_1<<endl;
	cout<<"Total bad channel   : "<<bad_chan_1+bad_chan_0<<" total performance : "<<Form("%.5f",((6656.-double(bad_chan_1+bad_chan_0))/6656.))<<endl;
	

	bad_channel = bad_chan_1+bad_chan_0;
	tree_output2->Fill();

	tree_output1->Write("", TObject::kOverwrite);
	tree_output2->Write("", TObject::kOverwrite);

	bad_channel_detail<<"\r"<<endl;	
	bad_channel_detail<<"total bad channel : "<<bad_chan_1+bad_chan_0<<"\r"<<endl;

	file_output->Close();
	bad_channel_detail.close();


}



