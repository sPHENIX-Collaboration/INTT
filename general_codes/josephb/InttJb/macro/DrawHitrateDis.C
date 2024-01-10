#ifndef DRAW_HITRATE_DIS_C
#define DRAW_HITRATE_DIS_C

#include <climits>
#include <cstddef>
#include <cstdint>

#include <array>
#include <memory>
#include <map>
#include <random>
#include <vector>

#include <TFile.h>
#include <TTree.h>

R__LOAD_LIBRARY(libintt_jb.so)
#include <intt_jb/InttEvtJb.h>
#include <intt_jb/InttEvtJbVer1.h>
#include <intt_jb/InttHitJb.h>
#include <intt_jb/InttHitJbVer1.h>

int NUM_SIG = 5;
int NUM_BINS = 100;
void Compare(int const&);

void DrawHitrateDis()
{
	// 20441
	// ...
	// 20449
	// 20869
	Compare(20869);
}

void Compare(int const& run_num) {
	std::string file_name_format =
		"/sphenix/user/jbertaux/Data/ChannelClassification/beam_intt_combined-%08d-0000.root";
	std::string cnvs_name_format =
		"cnvs_run_%08d.png";
	std::string hitrate_tree_name =
		"hitrates_tree";

	char buff[256];
	snprintf(buff, sizeof(buff), file_name_format.c_str(), run_num);

	TFile* file = TFile::Open(buff);
	if(!file) {
		std::cout << "Could not get file:" << std::endl;
		std::cout << "\"" << buff << "\"" << std::endl;

		return;
	}
	TTree* tree = (TTree*)file->Get(hitrate_tree_name.c_str());
	if(!tree) {
		std::cout << "Could not get tree:" << std::endl;
		std::cout << "\"" << hitrate_tree_name << "\"" << std::endl;

		return;
	}

	InttHitJb* hit_info = new InttHitJbVer1;
	Double_t hit_rate{};
	Double_t adc_dis[8] = {};

	tree->SetBranchAddress("hit", &hit_info);
	tree->SetBranchAddress("hitrate", &hit_rate);
	tree->SetBranchAddress("adc_dis", adc_dis);

	Double_t min = INT_MAX;
	Double_t max = 0;

	Double_t avg = 0.0;
	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		avg += hit_rate;
		if(hit_rate < min)min = hit_rate;
		if(hit_rate > max)max = hit_rate;
	}
	avg /= tree->GetEntriesFast();

	Double_t sig = 0.0;
	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		sig += (hit_rate - avg) * (hit_rate - avg);
	}
	sig /= tree->GetEntriesFast();
	sig = sqrt(sig);

	if(min < avg - NUM_SIG * sig)min = avg - NUM_SIG * sig;
	if(max > avg + NUM_SIG * sig)max = avg + NUM_SIG * sig;

	std::cout << avg << std::endl;
	std::cout << sig << std::endl;

	// snprintf(buff, sizeof(buff), "cnvs_run_%08d", run_num);
	// TCanvas* cnvs = new TCanvas(buff, buff);
	// cnvs->cd();

	TH1* hist = new TH1D (
		"hitrates",
		"hitrates",
		NUM_BINS,
		min,// - 0.5 * (max - min) / NUM_BINS,
		max// + 0.5 * (max - min) / NUM_BINS
	);

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		if(hit_rate < min)hit_rate = min;
		if(hit_rate > max)hit_rate = max;
		hist->Fill(hit_rate);
	}

	hist->Draw();
	// cnvs->Draw();

	// snprintf(buff, sizeof(buff), cnvs_name_format.c_str(), run_num);
	// cnvs->SaveAs();

	// std::cout << std::endl;
	// std::cout << "\t" << buff << std::endl;
	// std::cout << std::endl;

	// file->Close();
}

#endif//DRAW_HITRATE_DIS
