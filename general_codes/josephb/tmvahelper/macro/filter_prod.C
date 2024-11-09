#ifndef FILTER_PROD_C
#define FILTER_PROD_C

#include "config.C"

#include <tmvahelper/TMVAHelper.h>
R__LOAD_LIBRARY(libtmvahelper.so)

#include <filesystem>
#include <limits>
#include <boost/format.hpp>

void
filter_again (
	std::string const& data_path =
		"/sphenix/tg/tg01/hf/cdean/QM25_productions_full_data/pKpi_reco",
	std::string const& fit_path =
		"/sphenix/tg/tg01/hf/jbertaux/dEdx_fits/dedx_fitparam.root"
) {
	// Helper
	TMVAHelper tmva_helper;
	tmva_helper.read_branches(config::prod_branches);
	tmva_helper.read_cuts(config::prod_cuts);
	tmva_helper.read_training(config::training);

	TFile* prod_file = TFile::Open("prod.root", "RECREATE");
	TTree* prod_tree = new TTree("DecayTree", "DecayTree");
	prod_tree->SetDirectory(prod_file);
	tmva_helper.make_branches(prod_tree);

	// dEdx cuts
	TFile* fit_file = TFile::Open(fit_path.c_str(), "READ");
	if (!fit_file || !fit_file->IsOpen()) {
		std::cerr
			<< "\t" << fit_path << "\n"
			<< std::flush;
		return;
	}

	std::map<int, TF1*> fit_map;
	fit_map.insert({  211, dynamic_cast<TF1*>(fit_file->Get("f_piband"))});
	fit_map.insert({ -211, dynamic_cast<TF1*>(fit_file->Get("f_piminus_band"))});
	fit_map.insert({  321, dynamic_cast<TF1*>(fit_file->Get("f_Kband"))});
	fit_map.insert({ -321, dynamic_cast<TF1*>(fit_file->Get("f_Kminus_band"))});
	fit_map.insert({ 2212, dynamic_cast<TF1*>(fit_file->Get("f_pband"))});
	fit_map.insert({-2212, dynamic_cast<TF1*>(fit_file->Get("f_pbar_band"))});
	for (auto const& [pid, func_ptr] : fit_map) {
		if (func_ptr) continue;
		std::cerr
			<< "\t" << pid << "\n"
			<< std::flush;
		return;
	}

	Int_t* track_id[3];
	Float_t* track_p[3];
	Float_t* track_dEdx[3];

	for (auto const& entry : std::filesystem::directory_iterator{data_dir}) {
		TTree* tree = tmva_helper.get_tree(data_path, "DecayTree");
		if (!tree || tmva_helper.branch(tree)) {
			std::cerr
				<< "\t" << data_path << "\n"
				<< std::flush;
			return;
		}

		for (int i = 0; i < 3; ++i) {
			std::string name;

			name = (boost::format("track_%d_PDG_ID") % (i + 1)).str();
			track_id[i] = static_cast<Int_t*>(tmva_helper.get_branch(name));
			if (!track_id[i]) {
				std::cerr
					<< "\t" << name << "\n"
					<< std::flush;
				return;
			}

			name = (boost::format("track_%d_p") % (i + 1)).str();
			track_p[i] = static_cast<Float_t*>(tmva_helper.get_branch(name));
			if (!track_p[i]) {
				std::cerr
					<< "\t" << name << "\n"
					<< std::flush;
				return;
			}

			name = (boost::format("track_%d_dEdx") % (i + 1)).str();
			track_dEdx[i] = static_cast<Float_t*>(tmva_helper.get_branch(name));
			if (!track_dEdx[i]) {
				std::cerr
					<< "\t" << name << "\n"
					<< std::flush;
				return;
			}
		}
	
		for (Long64_t n = 0, N = tree->GetEntriesFast(); n < N; ++n) {
			tree->GetEntry(n);
			if (tmva_helper.eval()) continue;
	
			bool should_continue = false;
			for (int i = 0; i < 3; ++i) {
				auto itr = fit_map.find(abs(*track_id[i]));
				if (itr == fit_map.end()) {
					should_continue = true;
					break;
				}
	
				float dEdx = itr->second->Eval(abs(*track_p[i]));
				if (dEdx * 0.7 < *track_dEdx[i] && *track_dEdx[i] < dEdx * 1.3) continue;
				should_continue = true;
				break;
			}
			if (should_continue) continue;
		
			prod_tree->Fill();
		}
	}

	prod_file->cd();
	prod_tree->Write();
	prod_file->Write();
	prod_file->Close();
}

#endif//FILTER_PROD_C
