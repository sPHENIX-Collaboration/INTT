#ifndef DRAW_PLOTS_C
#define DRAW_PLOTS_C

void
draw_plots (
	int run_num
) {
	std::string format = "dat/run_%08d_intt%01d.root";
	char buff[256];

	for(int i = 0; i < 8; ++i) {
		snprintf(buff, sizeof(buff), format.c_str(), run_num, i);
		std::cout << buff << std::endl;

		TFile* file = TFile::Open(buff, "READ");
		if(!file) {
			std::cerr << "file " << buff << std::endl;
			continue;
		}

		char buff[256];
		for(int fee = 0; fee < 14; ++fee) {
			snprintf(buff, sizeof(buff), "hitmap_felix_channel_%02d", fee);
			TH1* hist = (TH1*)file->Get(buff);
			if(!hist) {
				std::cerr << "hist " << buff << std::endl;
				continue;
			}

			gStyle->SetOptStat(0);
			snprintf(buff, sizeof(buff), "Run %08d intt%01d felix%02d hitmap", run_num, i, fee);
			TCanvas* cnvs = new TCanvas(buff, buff, -1, 0, 1920, 1080);

			cnvs->cd();
			hist->Draw("COLZ");

			cnvs->Update();
			snprintf(buff, sizeof(buff), "run%08d_intt%01d_fee%02d.png", run_num, i, fee);
			cnvs->SaveAs(buff);

			delete cnvs;
		}
	}
}

#endif//DRAW_PLOTS_C
