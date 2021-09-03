#include <vector>
/**/
int GetMaxBinContent(vector < TH2D* >& hists)
{
	vector < int > contents;
	for (int i = 0; i < hists.size(); i++)
	{
		
		for (int j = 0; j < hists[i]->GetNbinsX(); j++)
		{
			for (int k = 0; k < hists[i]->GetNbinsY(); k++)
			{
				contents.push_back(hists[i]->GetBinContent(j, k));
				cout << contents[contents.size() - 1] << endl;
			}
		}
	}
	return *max_element(contents.begin(), contents.end());
}

int analysis_cosmic( string data = "fphx_raw_20200727-1917_0.root" )
{
	string data_root_dir = "/Users/sphenix/Documents/INTT_testbench/data/";

	if (data.find(data_root_dir.c_str()) == string::npos)
		data = data_root_dir + data;

	TFile* tf = new TFile( data.c_str(), "READ" );

	TTree* tr = (TTree*)tf->Get("tree");
	tr->Print();

	// branches: adc, ampl, chip_id, fpga_id,module, chan_id, fem_id, bco, bco_full, event
	// chip_id: 1-26
	// chan_id: 0-127


	//TCanvas* c = new TCanvas("name", "title", 2400, 400);
	TCanvas* c = new TCanvas("name", "title", 800, 800);
	int event_num = tr->GetEntries();

//	stringstream expression
	tr->Draw("chan_id:event>>(3e3, 0, 4e4, 128, 0, 127)", "", "colz");
	
	/*
	c->Divide(13, 2);
	//	tr->Draw("chan_id", "", "colz");
	// tr->Draw("ampl:chan_id", "chip_id==1", "colz");
	//	tr->Draw("ampl", "chip_id == 1 && chan_id == 0", "");

	int event_num = tr->GetEntries();
	int event_bin_num = 50;

	// loop over ch to make hists
//	vector < TH2D* > hists;
//	TH2D* hists[26];
	for (int i = 26; i > -1; i--)
	{
		stringstream hist_name, hist_title;
		hist_name << "ChipID" << i;
		hist_title << hist_name.str() << ";#event;Channel ID";

		TH2D* hist = new TH2D(hist_name.str().c_str(), hist_title.str().c_str(),
			event_bin_num, 0, event_num - 1,
			128/10, 0, 127);

		stringstream expression, cut;
		expression << "chan_id:event>>" << hist_name.str();
		cut << "chip_id==" << i;

		c->cd(26 - i + 1);
		tr->Draw(expression.str().c_str(), cut.str().c_str(), "colz");
		//hists.push_back( (TH2D*)hist->Clone() );
		//hists[26 - i] = hist;
	}
	*/
//	cout << hists[0]->GetName();
//	int zaxis_max = GetMaxBinContent(hists);
	gPad->Update();
	return 0;
}