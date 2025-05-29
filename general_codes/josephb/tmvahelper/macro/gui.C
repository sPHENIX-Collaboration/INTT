#ifndef GUI_C
#define GUI_C

#include "config.C"

#include <tmvahelper/TmvaHelper.h>
R__LOAD_LIBRARY(libtmvahelper.so)

#include <boost/format.hpp>

Double_t background_ratio = 10E+5;

void
gui (
) {
	TmvaHelper tmva_helper;
	tmva_helper.read_branches(config::branches);
	tmva_helper.read_training(config::training);
	tmva_helper.read_cuts(config::cuts);

	TMVA::Reader* reader = new TMVA::Reader("!Color:!Silent");
	tmva_helper.branch(reader);
	auto method = reader->BookMVA (
		config::method_name,
		(boost::format("dataloader/weights/factory_%s.weights.xml") % config::method_name.c_str()).str().c_str()
	);

	auto method_cuts = dynamic_cast<TMVA::MethodCuts*>(method);
	if (method_cuts) {

		Double_t best_signal_efficiency = 0.0;
		Double_t best_significance = 0.0;
		Int_t N = 10;
		for (int n = 0; n < N; ++n) {
			Float_t background_efficiency = (n + 0.5) / N;
			std::string why_is_root_like_this = (boost::format("Efficiency:%0.2f") % background_efficiency).str();
			std::cout << why_is_root_like_this << std::endl;
			Float_t signal_efficiency = method_cuts->GetTrainingEfficiency(why_is_root_like_this);

			std::cout << background_efficiency << " " << signal_efficiency << std::endl;
			method_cuts->PrintCuts(signal_efficiency);

			// std::cout << efficiency << " ";
			// method_cuts->GetCuts(efficiency, lower, upper);
			// std::cout << efficiency << " " << std::endl;
		}

		std::vector<Double_t> lower, upper;
	}

	// if (!gROOT->IsBatch()) TMVA::TMVAGui(config::factory_file_name.c_str());
}

#endif//GUI_C
