#ifndef TRAIN_C
#define TRAIN_C

#include "config.C"

#include <tmvahelper/TmvaHelper.h>
R__LOAD_LIBRARY(libtmvahelper.so)

#include <filesystem>

void
train (
) {
	// Helper
	TmvaHelper tmva_helper;
	tmva_helper.read_branches(config::branches);
	tmva_helper.read_training(config::training);
	tmva_helper.read_cuts(config::cuts);

	// Initialize factory and dataloader
	TFile* factory_file = TFile::Open(config::factory_file_name.c_str(), "RECREATE");
	if (!factory_file) {
		std::cerr << "file: " << config::factory_file_name << std::endl;
		return;
	}

	TMVA::Factory* factory = new TMVA::Factory (
		"factory", factory_file,
		"!V:!Silent:AnalysisType=Classification"
	);
	TMVA::DataLoader* dataloader = new TMVA::DataLoader("dataloader");

	// Add variables and no-NaN cuts
	tmva_helper.branch(dataloader);

	TTree* signal_tree = TmvaHelper::get_tree("signal.root", "DecayTree");
	if (!signal_tree) {
		std::cerr << "expected file 'signal.root' not present" << std::endl;
		return;
	}
	dataloader->AddSignalTree(signal_tree);

	TTree* background_tree = TmvaHelper::get_tree("background.root", "DecayTree");
	if (!background_tree) {
		std::cerr << "expected file 'background.root' not present" << std::endl;
		return;
	}
	dataloader->AddBackgroundTree(background_tree);

	// Train
	factory->BookMethod(dataloader, config::method_type, config::method_name.c_str(), config::method_options.c_str());
	factory->TrainAllMethods();
	factory->TestAllMethods();
	factory->EvaluateAllMethods();

	// Get Cut value from the training
	Double_t sig_cut, max_sig;
	auto method = dynamic_cast<TMVA::MethodBase*>(factory->GetMethod("dataloader", config::method_name.c_str()));
	sig_cut = method->GetMaximumSignificance(config::ratio, 1.0, max_sig);
	std::cout << "sig_cut: " << sig_cut << std::endl;
	std::cout << "max_sig: " << max_sig << std::endl;

	// Cleanup
	factory_file->Close();
	delete dataloader;
	delete factory;
}

#endif//TRAIN_C

