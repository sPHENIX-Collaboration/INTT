void final_results (TString folder_name)
{
	TCanvas * c1 = new TCanvas ("c1","c1",1200,800);
	c1->SetLogy();
	TString list_buffer;
	vector<TString> list_array; list_array.clear();

	ifstream data_list;
	data_list.open(Form("%s/ladder_list.txt",folder_name.Data()));

	while (1)
	{
		data_list >> list_buffer;
		if (!data_list.good())
		{
			break;
		}
		list_array.push_back(list_buffer);
	}
	cout<<"size : "<<list_array.size()<<endl;
	
	int bad_channel;

	TFile *f1;// = TFile::Open(Form("cut_finder_folder/sum_up_all.root"));
	TTree *total_number;

	TH1F * bad_channel_1D = new TH1F ("","ladder bad channel summary",list_array.size(),0,list_array.size());
	bad_channel_1D->GetYaxis()->SetTitleOffset(1.);
	bad_channel_1D->GetXaxis()->SetTitleOffset(1.);
	bad_channel_1D->GetYaxis()->SetTitle("# of bad channels");
	bad_channel_1D->GetXaxis()->SetTitle("ladder ID");
	bad_channel_1D->GetXaxis()->SetNdivisions(-1*int(list_array.size()));
	bad_channel_1D->SetStats(0);


	for (i=1;i<=list_array.size();i++)
	{
		bad_channel_1D->GetXaxis()->SetBinLabel(i,list_array[i-1]);	
	} 
	bad_channel_1D->LabelsOption("v");

	for (int i=0; i<list_array.size(); i++)
	{
		f1 = TFile::Open(Form("%s/ladder_files/%s/bad_channel_summary.root",folder_name.Data(),list_array[i].Data()));
		total_number = (TTree *)f1->Get("total_number");

		total_number->SetBranchAddress("bad_channel",&bad_channel);
		total_number->GetEntry(0);
		cout<<Form("%s, bad channel : ",list_array[i].Data())<<bad_channel<<endl;
		bad_channel_1D->SetBinContent(i+1,bad_channel);
	}

	bad_channel_1D->Draw("hist");
	c1->Print(Form("%s/ladder_bad_channel_final.pdf",folder_name.Data()));

}