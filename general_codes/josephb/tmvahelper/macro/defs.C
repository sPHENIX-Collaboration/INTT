#ifndef DEFS_C
#define DEFS_C

#include <boost/format.hpp>

namespace defs {
	std::string const data_dir =          "/sphenix/user/jbertaux/Repositories/analysis/HF-Particle/KFParticle_sPHENIX/hf_generator/dat";
	std::string const factory_file_name = "factory/factory.root";

	Double_t ratio = 0.1;
	Double_t mean =  2.27977e+00;
	Double_t sigma = 1.30624e-02;

	TMVA::Types::EMVA const method_type = TMVA::Types::kBDT;
	std::string       const method_name = "BDT";
	std::string       const method_options =
		"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20";

	// TMVA::Types::EMVA const method_type = TMVA::Types::kMLP;
	// std::string       const method_name = "MLP";
	// std::string       const method_options =
	// 	"!H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator";
	
	std::vector<std::string> const branches = {
		"Lambda_cplus_mass",
		"Lambda_cplus_pT",
		"Lambda_cplus_decayLength",
		"Lambda_cplus_decayLengthErr",
		"Lambda_cplus_DIRA",
		"Lambda_cplus_FDchi2",
		"Lambda_cplus_IP",
		"Lambda_cplus_IPchi2",
		"Lambda_cplus_IPErr",
		"Lambda_cplus_IP_xy",
	};
	
	std::vector<std::string> const training = {
		"LogDLS := log(Lambda_cplus_decayLength / Lambda_cplus_decayLengthErr)",
		"Lambda_cplus_DIRA",
		"Lambda_cplus_FDchi2",
		"Lambda_cplus_IP",
		"Lambda_cplus_IPchi2",
		"Lambda_cplus_IPErr",
		"Lambda_cplus_IP_xy",
	};
	
	std::vector<std::vector<std::string>> const pT_cuts = {
		{"2.0 < Lambda_cplus_pT", "Lambda_cplus_pT < 5.0"},
	};

	// std::vector<std::vector<std::string>> const cent_cuts = {
	// 	{"2.0 < Lambda_cplus_pT", "Lambda_cplus_pT < 5.0"},
	// };

	TCut get_sideband_cut () {
		std::string mass = "Lambda_cplus_mass";
		boost::format cut("(%f < %s && %s < %f) || (%f < %s && %s < %f)");
		return TCut((cut
				% (mean - 6.0 * sigma) % mass.c_str() % mass.c_str() % (mean - 3.0 * sigma)
				% (mean + 3.0 * sigma) % mass.c_str() % mass.c_str() % (mean + 6.0 * sigma)).str().c_str());
	}
	//...
};

#endif//DEFS_C

