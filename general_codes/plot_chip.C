void plot_chip(const char *fname = "fphx.root") {

	gStyle->SetOptStat(0);
	TFile *file = TFile::Open(fname);

	TCanvas *c1 = new TCanvas("c1", "histgram1", 800, 450);
	TH2D *h1 = new TH2D("h1", "amplitule vs chip", 30, 0, 30, 70, 0, 70);
	
	tree->Draw("ampl:chip_id>>h1", "ampl<64&&chip_id<27", "colz");

/*	TCanvas *c2 = new TCanvas("c2", "histgram1", 1800, 1500);
	c2->Divide(16, 8);

	for (int i = 0; i < 128; i++) {
		c2->cd(i + 1);
		tree->Draw("ampl:chip_id", Form("ampl<64&&chip_id<27&&chan_id==%d", i), "colz");
		tree->Draw("ampl:chip_id", Form("module==8&&fem_id==4&&ampl<64&&chip_id<27&&chan_id==%d",i), "colz");
		cout << "chan_id==" << i << endl;
	}
*/	
}