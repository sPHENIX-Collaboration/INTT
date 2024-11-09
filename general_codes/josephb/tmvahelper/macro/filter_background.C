#ifndef FILTER_BACKGROUND_C
#define FILTER_BACKGROUND_C

#include "config.C"

#include <tmvahelper/TmvaHelper.h>
#include <tmvahelper/DedxBrancher.h>
R__LOAD_LIBRARY(libtmvahelper.so)

#include <filesystem>
#include <boost/format.hpp>

void
filter_background (
	std::string const& data_dir,
	std::string const& fit_path =
		"/sphenix/tg/tg01/hf/jbertaux/dEdx_fits/dedx_fitparam.root"
) {
	// Get sideband cut from signal roofit
	TFile* file = TFile::Open("signal_fit.root", "READ");
	if (!file) {std::cerr << __LINE__ << std::endl; return;}

	RooWorkspace* w = dynamic_cast<RooWorkspace*>(file->Get("w"));
	if (!w) {std::cerr << __LINE__ << std::endl; return;}
	w->Print();
	RooArgSet args = w->allVars();

	Float_t sigma_eff{0};
	Float_t total{0};
	Float_t mean = dynamic_cast<RooRealVar&>(args["mean"]).getValV();
	for (int i = 0; i < 2; ++i) {
		std::string name;

		name = (boost::format("sigma_%d") % i).str();
		Float_t sigma = dynamic_cast<RooRealVar&>(args[name.c_str()]).getValV();
		std::cout << name << " " << sigma << std::endl;

		name = (boost::format("coeff_%d") % i).str();
		Float_t coeff = dynamic_cast<RooRealVar&>(args[name.c_str()]).getValV();
		std::cout << name << " " << coeff << std::endl;

		sigma_eff += sigma * sigma * coeff;
		total += coeff;
	}
	sigma_eff /= total;
	sigma_eff = sqrt(sigma_eff);
	std::cout << sigma_eff << std::endl;

	std::string sideband_cut = (
		boost::format("(%f < %s && %s < %f) || (%f < %s && %s < %f)")
		% (mean - 6.0 * sigma_eff) % config::mass_branch % config::mass_branch % (mean - 3.0 * sigma_eff)
		% (mean + 3.0 * sigma_eff) % config::mass_branch % config::mass_branch % (mean + 6.0 * sigma_eff)
	).str();

	std::vector<std::string> background_cuts = config::cuts;
	background_cuts.push_back(sideband_cut);
	std::cout << sideband_cut << std::endl;

	// Output
	TFile* background_file = TFile::Open("background.root", "RECREATE");
	TTree* background_tree = new TTree("DecayTree", "DecayTree");
	background_tree->SetDirectory(background_file);

	// TmvaHelpers
	TmvaHelper tmva_helper;
	tmva_helper.read_branches(config::branches);
	tmva_helper.read_training(config::training);
	tmva_helper.read_cuts(background_cuts);
	tmva_helper.make_branches(background_tree);

	DedxBrancher dedx_brancher;
	dedx_brancher.set_num_daughters(3);
	if (dedx_brancher.get_dedx_fits(fit_path)) return;
	if (dedx_brancher.make_branch(background_tree)) return;

	Long64_t counts{0};
	Long64_t files{0};
	for (auto const& entry : std::filesystem::directory_iterator{data_dir}) {
		if (!entry.is_regular_file()) continue;

		std::string filename = entry.path().filename();
		if (filename.find(config::channel) == std::string::npos) continue;
		if (filename.find("bak_KFP") == std::string::npos) continue;

		TTree* tree = tmva_helper.get_tree(entry.path().string(), "DecayTree");
		if (!tree || tmva_helper.branch(tree) || dedx_brancher.branch(tmva_helper)) {
			std::cerr << entry.path().c_str() << std::endl;
			return;
		}
		++files;

		for (Long64_t n = 0, N = tree->GetEntriesFast(); n < N; ++n) {
			tree->GetEntry(n);

			if (tmva_helper.eval()) continue;
			if (dedx_brancher.eval()) continue;

			background_tree->Fill();
			++counts;
		}
	}

	background_file->cd();
	background_tree->Write();
	background_file->Write();
	background_file->Close();

	std::cout << "finished" << std::endl;
	std::cout << "counts: " << counts << std::endl;
	std::cout << "files:  " << files << std::endl;
}

#endif//FILTER_BACKGROUND_C
