#ifndef APPLY_C
#define APPLY_C

#include "config.C"

#include <tmvahelper/TMVAHelper.h>
R__LOAD_LIBRARY(libtmvahelper.so)

#include <filesystem>

void
apply (
	std::string const& data_dir
) {

	// Helper
	TMVAHelper tmva_helper;
	tmva_helper.read_branches(config::branches);
	tmva_helper.read_training(config::training);
	tmva_helper.read_cuts(config::cuts);

	TFile* selection_file = TFile::Open("selection.root", "RECREATE");
	selection_file->cd();
	TTree* selection_tree = new TTree("DecayTree", "DecayTree");
	selection_tree->SetDirectory(selection_file);
	tmva_helper.make_branches(selection_tree);

	// Initialize reader
	TMVA::Reader* reader = new TMVA::Reader("!Color:!Silent");
	tmva_helper.branch(reader);
	reader->BookMVA (
		config::method_name,
		(boost::format("dataloader/weights/factory_%s.weights.xml") % config::method_name.c_str()).str().c_str()
	);

	TH1D* reference_hist = new TH1D (
		"reference_hist", "reference_hist",
		config::num_bins, config::min_mass, config::max_mass
	);

	TH1D* selection_hist = new TH1D (
		"selection_hist", "selection_hist",
		config::num_bins, config::min_mass, config::max_mass
	);

	// Add input files
	int num_files{0};
	for (auto const& entry : std::filesystem::directory_iterator{data_dir}) {
		if (!entry.is_regular_file()) continue;

		std::string filename = entry.path().filename();
		// if (filename.find(config::channel) == std::string::npos) continue;
		// if (filename.find("bak_KFP") == std::string::npos) continue; // change or ommit
		if (filename.find("outputKFParticle_pKpi_reco") == std::string::npos) continue;

		TTree* tree = tmva_helper.get_tree(entry.path().string(), "DecayTree");
		if (!tree || tmva_helper.branch(tree)) {
			std::cerr << "file: " << entry.path() << std::endl;
			continue;
		}

		Float_t* mass = static_cast<Float_t*>(tmva_helper.get_branch(config::mass_branch));
		for (Int_t n = 0, N = tree->GetEntriesFast(); n < N; ++n) {
			tree->GetEntry(n);

			if (tmva_helper.eval()) continue;
			reference_hist->Fill(*mass);

			if (reader->EvaluateMVA(config::method_name.c_str()) < config::cut_val) continue;
			selection_hist->Fill(*mass);
			selection_tree->Fill();
		}

		if (++num_files > 100) break;
	}

	SetsPhenixStyle();
	TCanvas* cnvs = new TCanvas (
		"cnvs", "cnvs", 800, 600
	);

	cnvs->cd();
	TPad* reference_pad = new TPad (
		"reference_pad", "reference_pad",
		0.0, 0.0,
		0.5, 1.0
	);
	reference_pad->SetFillStyle(4000);
	reference_pad->Draw();
	reference_pad->cd();
	reference_hist->Draw();

	cnvs->cd();
	TPad* selection_pad = new TPad (
		"selection_pad", "selection_pad",
		0.5, 0.0,
		1.0, 1.0
	);
	selection_pad->SetFillStyle(4000);
	selection_pad->Draw();
	selection_pad->cd();
	selection_hist->Draw();

	cnvs->Update();
	cnvs->SaveAs("cnvs.png");
	// delete cnvs;

	selection_file->cd();
	selection_tree->Write();
	selection_file->Write();
	selection_file->Close();

}

#endif//APPLY_C

