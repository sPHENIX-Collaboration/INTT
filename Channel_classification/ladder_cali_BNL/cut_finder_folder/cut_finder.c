void cut_finder ()
{
	TString folder_name = "/home/cwshih/INTT_cal/INTT_cal_test/ladder_cali/cut_finder_folder/";
	int cut_range = 5; //sigma

	TFile *f1 = TFile::Open(Form("%s/sum_up_all.root",folder_name.Data()));

	TTree *chan_gaus_width = (TTree *)f1->Get("chan_gaus_width"); //value_W
	TTree *chan_entry = (TTree *)f1->Get("chan_entry"); //value_E
	TTree *ampl_channel_entries = (TTree *)f1->Get("ampl_channel_entries"); //value_EA
	TTree *chan_thre_position = (TTree *)f1->Get("chan_thre_position"); //value_TP
	TTree *chan_adc_study = (TTree *)f1->Get("chan_adc_study"); //value_adc_0
	TTree *chan_study = (TTree *)f1->Get("chan_study"); //value_slope_chi, value_width_chi

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

	TCanvas * c1 = new TCanvas ("c1","c1",1800,1800);
	c1->SetLogy();

	TCanvas * c2 = new TCanvas ("c2","c2",1600,800);
	// c2->SetLogy();

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

	gStyle->SetOptStat(111111);

	TH1F * width_1D = new TH1F ("","offseted width of each channel",50,0,6);
	// width_1D->SetLogy();
	width_1D->GetXaxis()->SetTitle("Width (unit : ampl)");

	TH1F * width_1D02 = new TH1F ("","offseted width of each channel (0, 2)",50,0,6);
	// width_1D->SetLogy();
	width_1D02->GetXaxis()->SetTitle("Width (unit : ampl)");

	TH1F * width_1D13 = new TH1F ("","offseted width of each channel (1, 3)",50,0,6);
	// width_1D->SetLogy();
	width_1D13->GetXaxis()->SetTitle("Width (unit : ampl)");


	TH1F * entry_1D = new TH1F ("","entry of each channel",50,0,400);
	// entry_1D->SetLogy();
	entry_1D->GetXaxis()->SetTitle("# of events");

	TH1F * entry_ampl_1D = new TH1F ("","entry of each channel w/ ampl cut",50,150,200);
	// entry_ampl_1D->SetLogy();
	entry_ampl_1D->GetXaxis()->SetTitle("# of events");

	TH1F * TP_1D = new TH1F ("","(error function) threshold position of each channel",50,10,50);
	// TP_1D->SetLogy();
	TP_1D->GetXaxis()->SetTitle("unit : ampl");	

	TH1F * adc_0_1D = new TH1F ("","mean of adc0 distribution of each channel",50,10,50);
	// adc_0_1D->SetLogy();
	adc_0_1D->GetXaxis()->SetTitle("unit : ampl");	

	TH1F * adc_0_1DN = new TH1F ("","numeric mean of adc0 distribution of each channel",50,10,50);
	// adc_0_1D->SetLogy();
	adc_0_1DN->GetXaxis()->SetTitle("unit : ampl");		

	TH1F * width_rchi2_1D = new TH1F("","width reduced #chi^{2} of each channel ",50,0,10);
	// width_rchi2_1D->SetLogy();
	width_rchi2_1D->GetXaxis()->SetTitle("unit : #chi^{2}/NDF");	

	TH1F * width_value_1D = new TH1F("","width value of each channel",50,1,6);
	// width_value_1D->SetLogy();
	width_value_1D->GetXaxis()->SetTitle("unit : ampl");	

	TH1F * width_pvalue_1D = new TH1F("","width p-value of each channel",50,0,1);
	width_pvalue_1D->GetXaxis()->SetTitle("p-value");
	// slope_value_1D->SetLogy();
	//slope_value_1D->GetXaxis()->SetTitle("unit : ampl/adc");



	TH1F * slope_rchi2_1D = new TH1F("","slope reduced #chi^{2} of each channel",50,0,10);
	// slope_rchi2_1D->SetLogy();
	slope_rchi2_1D->GetXaxis()->SetTitle("unit : #chi^{2}/NDF");	

	TH1F * slope_value_1D = new TH1F("","slope value of each channel",50,4,10);
	// slope_value_1D->SetLogy();
	slope_value_1D->GetXaxis()->SetTitle("unit : ampl/adc");

	TH1F * slope_pvalue_1D = new TH1F("","slope p-value of each channel",50,0,1);
	slope_pvalue_1D->GetXaxis()->SetTitle("p-value");
	// slope_value_1D->SetLogy();
	//slope_value_1D->GetXaxis()->SetTitle("unit : ampl/adc");

	TH2F * slopevalue_chip_2D = new TH2F("","slope vs chip ID",30,0,30,30,6,9);
	slopevalue_chip_2D->GetXaxis()->SetTitle("chip ID");
	slopevalue_chip_2D->GetYaxis()->SetTitle("ampl-ADC slope");

	TH2F * slopevalue_chip_2D_F = new TH2F("","slope vs chip ID after correction",30,0,30,30,6,9);
	slopevalue_chip_2D_F->GetXaxis()->SetTitle("chip ID");
	slopevalue_chip_2D_F->GetYaxis()->SetTitle("ampl-ADC slope");

	TH1F * slope_value_1D_F = new TH1F("","slope value of each channel after correction",50,4,10);
	// slope_value_1D->SetLogy();
	slope_value_1D_F->GetXaxis()->SetTitle("unit : ampl/adc");




	TF1 * adc_0_fit = new TF1 ("adc_0_fit","gaus",10,50);

	TF1 * adc_0_fitN = new TF1 ("adc_0_fitN","gaus",10,50);

	TF1 * entry_fit = new TF1 ("entry_fit","gaus",0,400);

	TF1 * entry_ampl_fit = new TF1 ("entry_ampl_fit","gaus",150,200);

	TF1 * TP_fit = new TF1 ("TP_fit","gaus",10,50);

	TF1 * slope_value_fit_B = new TF1 ("slope_value_fit_B","gaus",4,10);

	TF1 * slope_value_fit = new TF1 ("slope_value_fit","gaus",4,10);

	TF1 * width_offset_fit = new TF1 ("width_offset_fit","gaus",0,6);

	TF1 * width_offset_fit02 = new TF1 ("width_offset_fit02","gaus",0,6);

	TF1 * width_offset_fit13 = new TF1 ("width_offset_fit13","gaus",0,6);
	
	TF1 * pol1_13 = new TF1 ("pol1_13","pol1",1,14);
	TF1 * pol1_26 = new TF1 ("pol1_26","pol1",14,27);


	


	for (int i = 0; i < total_size_W; i++)
	{
		chan_gaus_width->GetEntry(i);
		chan_entry->GetEntry(i);
		ampl_channel_entries->GetEntry(i);
		chan_thre_position->GetEntry(i);
		chan_adc_study->GetEntry(i);
		chan_study->GetEntry(i);

		width_1D->Fill(value_W);
		width_1D02->Fill(value_W02);
		width_1D13->Fill(value_W13);

		entry_1D->Fill(value_E);
		entry_ampl_1D->Fill(value_EA);
		TP_1D->Fill(value_TP);
		adc_0_1D->Fill(value_adc_0);
		adc_0_1DN->Fill(value_adc_0N);
		width_rchi2_1D->Fill(value_width_chi);
		width_value_1D->Fill(value_width_value);
		slope_rchi2_1D->Fill(value_slope_chi);
		slope_value_1D->Fill(value_slope_value);

		slopevalue_chip_2D->Fill(chip_chi,value_slope_value);

		width_pvalue_1D->Fill(TMath::Prob(value_width_chi, 3));
		if (TMath::Prob(value_width_chi, 3) < 0.0000006)
		{
			cout<<"width : "<<i<<" "<<chip_chi<<" "<<chan_chi<<" "<<Form("%.10f",TMath::Prob(value_width_chi, 3))<<endl;
		}
		
		slope_pvalue_1D->Fill(TMath::Prob(value_slope_chi, 2));
		if (TMath::Prob(value_slope_chi, 2) < 0.0000006)
		{
			cout<<"slope : "<<i<<" "<<chip_chi<<" "<<chan_chi<<" "<<Form("%.10f",TMath::Prob(value_slope_chi, 2))<<endl;
		}
		
	}


	TLatex *tex11 = new TLatex();
	tex11 -> SetNDC       ();
	tex11 -> SetTextFont (42);
	tex11 -> SetTextSize  (0.028);
	tex11 -> SetTextAlign (13);
	

	c1->cd();
	width_1D->Draw("hist");
	width_1D->Fit("width_offset_fit","NQ");
	width_offset_fit->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", width_offset_fit->GetParameter(2), width_offset_fit->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", width_offset_fit->GetParameter(1), width_offset_fit->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",width_offset_fit->GetChisquare(),width_offset_fit->GetNDF(),width_offset_fit->GetChisquare()/width_offset_fit->GetNDF() ));

	c1->Print(Form("%s/width_offset_1D.pdf",folder_name.Data()));
	c1->Clear();




	c1->cd();
	width_1D02->Draw("hist");
	width_1D02->Fit("width_offset_fit02","NQ");
	width_offset_fit02->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", width_offset_fit02->GetParameter(2), width_offset_fit02->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", width_offset_fit02->GetParameter(1), width_offset_fit02->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",width_offset_fit02->GetChisquare(),width_offset_fit02->GetNDF(),width_offset_fit02->GetChisquare()/width_offset_fit02->GetNDF() ));

	c1->Print(Form("%s/width_offset_1D_02.pdf",folder_name.Data()));
	c1->Clear();




	c1->cd();
	width_1D13->Draw("hist");
	width_1D13->Fit("width_offset_fit13","NQ");
	width_offset_fit13->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", width_offset_fit13->GetParameter(2), width_offset_fit13->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", width_offset_fit13->GetParameter(1), width_offset_fit13->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",width_offset_fit13->GetChisquare(),width_offset_fit13->GetNDF(),width_offset_fit13->GetChisquare()/width_offset_fit13->GetNDF() ));

	c1->Print(Form("%s/width_offset_1D_13.pdf",folder_name.Data()));
	c1->Clear();




	c1->cd();
	entry_1D->Draw("hist");
	entry_1D->Fit("entry_fit","NQ");
	entry_fit->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", entry_fit->GetParameter(2), entry_fit->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", entry_fit->GetParameter(1), entry_fit->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",entry_fit->GetChisquare(),entry_fit->GetNDF(),entry_fit->GetChisquare()/entry_fit->GetNDF() ));

	c1->Print(Form("%s/entry_1D.pdf",folder_name.Data()));
	c1->Clear();



	c1->cd();
	entry_ampl_1D->Draw("hist");
	// entry_ampl_1D->Fit("entry_ampl_fit","NQ");
	// entry_ampl_fit->Draw("lsame");
	c1->Print(Form("%s/entry_ampl_1D.pdf",folder_name.Data()));
	c1->Clear();



	c1->cd();
	TP_1D->Draw("hist");
	TP_1D->Fit("TP_fit","NQ");
	TP_fit->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", TP_fit->GetParameter(2), TP_fit->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", TP_fit->GetParameter(1), TP_fit->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",TP_fit->GetChisquare(),TP_fit->GetNDF(),TP_fit->GetChisquare()/TP_fit->GetNDF() ));

	c1->Print(Form("%s/TP_1D.pdf",folder_name.Data()));
	c1->Clear();



	c1->cd();
	adc_0_1D->Draw("hist");
	adc_0_1D->Fit("adc_0_fit","NQ");
	adc_0_fit->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", adc_0_fit->GetParameter(2), adc_0_fit->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", adc_0_fit->GetParameter(1), adc_0_fit->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",adc_0_fit->GetChisquare(),adc_0_fit->GetNDF(),adc_0_fit->GetChisquare()/adc_0_fit->GetNDF() ));

	c1->Print(Form("%s/adc_0_1D.pdf",folder_name.Data()));
	c1->Clear();




	c1->cd();
	adc_0_1DN->Draw("hist");
	adc_0_1DN->Fit("adc_0_fitN","NQ");
	adc_0_fitN->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", adc_0_fitN->GetParameter(2), adc_0_fitN->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", adc_0_fitN->GetParameter(1), adc_0_fitN->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",adc_0_fitN->GetChisquare(),adc_0_fitN->GetNDF(),adc_0_fitN->GetChisquare()/adc_0_fitN->GetNDF() ));

	c1->Print(Form("%s/adc_0_1D_numeric.pdf",folder_name.Data()));
	c1->Clear();



	c1->cd();
	width_rchi2_1D->Draw("hist");
	c1->Print(Form("%s/width_rchi2_1D.pdf",folder_name.Data()));
	c1->Clear();

	c1->cd();
	width_value_1D->Draw("hist");
	c1->Print(Form("%s/width_value_1D.pdf",folder_name.Data()));
	c1->Clear();

	c1->cd();
	slope_rchi2_1D->Draw("hist");
	c1->Print(Form("%s/slope_rchi2_1D.pdf",folder_name.Data()));
	c1->Clear();


	c1->cd();
	slope_value_1D->Draw("hist");
	slope_value_1D->Fit("slope_value_fit_B","NQ");
	slope_value_fit_B->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", slope_value_fit_B->GetParameter(2), slope_value_fit_B->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", slope_value_fit_B->GetParameter(1), slope_value_fit_B->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",slope_value_fit_B->GetChisquare(),slope_value_fit_B->GetNDF(),slope_value_fit_B->GetChisquare()/slope_value_fit_B->GetNDF() ));

	c1->Print(Form("%s/slope_value_1D_B.pdf",folder_name.Data()));
	c1->Clear();



	c1->cd();
	slope_pvalue_1D->Draw("hist");
	c1->Print(Form("%s/slope_pvalue_1D.pdf",folder_name.Data()));
	c1->Clear();

	c1->cd();
	width_pvalue_1D->Draw("hist");
	c1->Print(Form("%s/width_pvalue_1D.pdf",folder_name.Data()));
	c1->Clear();

	c2->cd();
	slopevalue_chip_2D->Draw("colz0");
	slopevalue_chip_2D->Fit (pol1_13 , "L0", "", 1, 14);
	slopevalue_chip_2D->Fit (pol1_26, "L0", "", 14, 27);
	pol1_13->Draw("lsame");
	pol1_26->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{1 ~ 13 Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet  Y =  %.3f X + %.3f", pol1_13->GetParameter(1), pol1_13->GetParError(0)));
	tex11 -> DrawLatex (0.12, 0.740, Form("#bf{1 ~ 13 Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",pol1_13->GetChisquare(),pol1_13->GetNDF(),pol1_13->GetChisquare()/pol1_13->GetNDF() ));

	tex11 -> DrawLatex (0.35, 0.850, Form("#bf{14 ~ 26 Fit parameters}"));
	tex11 -> DrawLatex (0.35, 0.810, Form("#bullet  Y =  %.3f X + %.3f", pol1_26->GetParameter(1), pol1_26->GetParError(0)));
	tex11 -> DrawLatex (0.35, 0.740, Form("#bf{14 ~ 26 Fit quality}"));
	tex11 -> DrawLatex (0.35, 0.700, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",pol1_26->GetChisquare(),pol1_26->GetNDF(),pol1_26->GetChisquare()/pol1_26->GetNDF() ));

	c2->Print(Form("%s/slope_chip_2D_B.pdf",folder_name.Data()));
	c2->Clear();


	for (int i = 0; i < total_size_W; i++)
	{
		chan_gaus_width->GetEntry(i);
		chan_entry->GetEntry(i);
		ampl_channel_entries->GetEntry(i);
		chan_thre_position->GetEntry(i);
		chan_adc_study->GetEntry(i);
		chan_study->GetEntry(i);

		slope_value_1D_F->Fill(value_slope_value-    ((pol1_13->GetParameter(1)+pol1_26->GetParameter(1))/2. * chip_chi)   );

		slopevalue_chip_2D_F->Fill(chip_chi,value_slope_value - ((pol1_13->GetParameter(1)+pol1_26->GetParameter(1))/2. * chip_chi));
	}

	c1->cd();
	slope_value_1D_F->Draw("hist");
	slope_value_1D_F->Fit("slope_value_fit","NQ");
	slope_value_fit->Draw("lsame");

	tex11 -> DrawLatex (0.12, 0.850, Form("#bf{Fit parameters}"));
	tex11 -> DrawLatex (0.12, 0.810, Form("#bullet Width = %.3f #pm %.3f", slope_value_fit->GetParameter(2), slope_value_fit->GetParError(2)));
	tex11 -> DrawLatex (0.12, 0.770, Form("#bullet Mean = %.3f #pm %.3f", slope_value_fit->GetParameter(1), slope_value_fit->GetParError(1)));
	tex11 -> DrawLatex (0.12, 0.700, Form("#bf{Fit quality}"));
	tex11 -> DrawLatex (0.12, 0.660, Form("#bullet #chi^{2}/ndf = %.3f/%d = %.3f",slope_value_fit->GetChisquare(),slope_value_fit->GetNDF(),slope_value_fit->GetChisquare()/slope_value_fit->GetNDF() ));

	c1->Print(Form("%s/slope_value_1D_correction.pdf",folder_name.Data()));
	c1->Clear();


	c2->cd();
	slopevalue_chip_2D_F->Draw("colz0");
	c2->Print(Form("%s/slope_chip_2D_correction.pdf",folder_name.Data()));
	c2->Clear();

	ofstream output_txt;
	output_txt.open(Form("%s/output_cut_value.txt", folder_name.Data()), ios::out); //¶}±ÒÀÉ®×
	if (!output_txt)
	{ //¦pªG¶}±ÒÀÉ®×¥¢±Ñ¡Afp¬°0¡F¦¨¥\¡Afp¬°«D0
		cout << "Fail to open file: " << endl;
	}

	cout<<" "<<endl;
	cout<<"/////////////////////////////////////////////"<<endl;
	cout<<"///////////  cut finding  result  ///////////"<<endl;
	cout<<Form("///////////      %d-sigma cut      ///////////",cut_range)<<endl;
	cout<<"/////////////////////////////////////////////"<<endl;	
	cout<<" "<<endl;
	cout<<"1. offseted width     : "<<width_offset_fit->GetParameter(1)-cut_range*(width_offset_fit->GetParameter(2))<<"	to "<<width_offset_fit->GetParameter(1)+cut_range*(width_offset_fit->GetParameter(2))<<endl;
	cout<<"2. entry              : "<<entry_fit->GetParameter(1)-cut_range*(entry_fit->GetParameter(2))<<"	to "<<entry_fit->GetParameter(1)+cut_range*(entry_fit->GetParameter(2))<<endl;
	cout<<"3. entry ampl cut     : "<<170<<"	to "<<200<<" (P.S. No fitting)"<<endl;
	cout<<"4. mean adc 0         : "<<adc_0_fit->GetParameter(1)-cut_range*(adc_0_fit->GetParameter(2))<<"	to "<<adc_0_fit->GetParameter(1)+cut_range*(adc_0_fit->GetParameter(2))<<endl;
	cout<<"5. mean adc 0 numeric : "<<adc_0_fitN->GetParameter(1)-cut_range*(adc_0_fitN->GetParameter(2))<<"	to "<<adc_0_fitN->GetParameter(1)+cut_range*(adc_0_fitN->GetParameter(2))<<endl;
	cout<<"6. threshold position : "<<TP_fit->GetParameter(1)-cut_range*(TP_fit->GetParameter(2))<<"	to "<<TP_fit->GetParameter(1)+cut_range*(TP_fit->GetParameter(2))<<endl;
	cout<<"7. width rchi2 p-value: "<<Form("%d-sigma -> 0.0000006 (P.S. No fitting)",cut_range)<<endl;
	cout<<"8. slope rchi2 p-value: "<<Form("%d-sigma -> 0.0000006 (P.S. No fitting)",cut_range)<<endl;
	cout<<"9. slope value correct: "<<slope_value_fit->GetParameter(1)-cut_range*(slope_value_fit->GetParameter(2))<<"	to "<<slope_value_fit->GetParameter(1)+cut_range*(slope_value_fit->GetParameter(2))<<endl;
	cout<<"10. offseted width 02  : "<<width_offset_fit02->GetParameter(1)-cut_range*(width_offset_fit02->GetParameter(2))<<"	to "<<width_offset_fit02->GetParameter(1)+cut_range*(width_offset_fit02->GetParameter(2))<<endl;
	cout<<"11.offseted width 13  : "<<width_offset_fit13->GetParameter(1)-cut_range*(width_offset_fit13->GetParameter(2))<<"	to "<<width_offset_fit13->GetParameter(1)+cut_range*(width_offset_fit13->GetParameter(2))<<endl;



	cout<<" "<<endl;
	cout<<"/////slope correct///// "<<endl;
	cout<<Form("Fit : chip 1 ~ 13, Y = %.3f X + %.3f",pol1_13->GetParameter(1), pol1_13->GetParError(0))<<endl;
	cout<<Form("Fit : chip 14~ 26, Y = %.3f X + %.3f",pol1_26->GetParameter(1), pol1_26->GetParError(0))<<endl;


	output_txt<<"offseted_width	"<<width_offset_fit->GetParameter(1)-cut_range*(width_offset_fit->GetParameter(2))<<"	to "<<width_offset_fit->GetParameter(1)+cut_range*(width_offset_fit->GetParameter(2))<<"\r"<<endl;
	output_txt<<"entry	"<<entry_fit->GetParameter(1)-cut_range*(entry_fit->GetParameter(2))<<"	to "<<entry_fit->GetParameter(1)+cut_range*(entry_fit->GetParameter(2))<<"\r"<<endl;
	output_txt<<"entry_ampl_cut	"<<170<<"	to "<<200<<endl;
	output_txt<<"mean_adc_0_numeric	"<<adc_0_fitN->GetParameter(1)-cut_range*(adc_0_fitN->GetParameter(2))<<"	to "<<adc_0_fitN->GetParameter(1)+cut_range*(adc_0_fitN->GetParameter(2))<<"\r"<<endl;
	output_txt<<"threshold_position	"<<TP_fit->GetParameter(1)-cut_range*(TP_fit->GetParameter(2))<<"	to "<<TP_fit->GetParameter(1)+cut_range*(TP_fit->GetParameter(2))<<"\r"<<endl;
	output_txt<<"width_rchi2_p-value	"<<0<<"	to"<<Form(" 0.0000006")<<"\r"<<endl;
	output_txt<<"slope_rchi2_p-value	"<<0<<"	to"<<Form(" 0.0000006")<<"\r"<<endl;
	output_txt<<"slope_value_correct	"<<slope_value_fit->GetParameter(1)-cut_range*(slope_value_fit->GetParameter(2))<<"	to "<<slope_value_fit->GetParameter(1)+cut_range*(slope_value_fit->GetParameter(2))<<"\r"<<endl;
	output_txt<<"offseted_width_02	"<<width_offset_fit02->GetParameter(1)-cut_range*(width_offset_fit02->GetParameter(2))<<"	to "<<width_offset_fit02->GetParameter(1)+cut_range*(width_offset_fit02->GetParameter(2))<<"\r"<<endl;
	output_txt<<"offseted_width_13	"<<width_offset_fit13->GetParameter(1)-cut_range*(width_offset_fit13->GetParameter(2))<<"	to "<<width_offset_fit13->GetParameter(1)+cut_range*(width_offset_fit13->GetParameter(2))<<"\r"<<endl;
	output_txt<<"1_13_slope	"<<Form("%.4f	and %.4f",pol1_13->GetParameter(1), pol1_13->GetParError(0))<<"\r"<<endl;
	output_txt<<"14_26_slope	"<<Form("%.4f	and %.4f",pol1_26->GetParameter(1), pol1_26->GetParError(0))<<"\r"<<endl;
	output_txt.close();
}




