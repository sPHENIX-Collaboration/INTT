#ifndef CONFIG_C
#define CONFIG_C

#include <boost/format.hpp>

namespace config {

	// gen
	std::string const channel            = "D0_Kpi"; // "Lc_pKpi";
	std::string const pythia_config_file = "steering_cards/pythia8_D0_Kpi.cfg";
	std::string const evtgen_config_file = "dec_files/D0_Kpi.DEC";
	std::string const decay_descriptor   = "[D0 -> K^- pi^+]cc";
	int         const particle_trigger   = 421;

	// fit
	Double_t const min_mass = 1.5;
	Double_t const max_mass = 2.2;

	Double_t mean = 1.85834e+00;
	Double_t sig =  1.43871e-02;
	Int_t num_bins = 100;

	// training/application
	Double_t const ratio = 2E-3;
	std::string const factory_file_name = "factory/factory.root";

	TMVA::Types::EMVA const method_type = TMVA::Types::kBDT;
	std::string       const method_name = "BDT";
	std::string       const method_options =
		"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20";
	Double_t cut_val = 0.0520139;

	// TMVA::Types::EMVA const method_type = TMVA::Types::kMLP;
	// std::string       const method_name = "MLP";
	// std::string       const method_options =
	// 	"!H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator";
	// Double_t cut_val = 0.0;

	std::string const particle_name = "D0"; // "Lambda_cplus";
	std::string const mass_branch = particle_name + "_mass";

	std::vector<std::string> const branches = {
		mass_branch, particle_name + "_pT",

		particle_name + "_vertex_volume",
		particle_name + "_chi2",
		particle_name + "_nDoF/I",
		particle_name + "_DIRA",

		particle_name + "_decayLength", particle_name + "_decayLengthErr",

		"track_1_pT", "track_1_pTErr",
		"track_2_pT", "track_2_pTErr",
		// "track_3_pT", "track_3_pTErr",

		"primary_vertex_volume",
		"primary_vertex_chi2",
		"primary_vertex_nDoF/I",

		// "track_1_PDG_ID/I",
		// "track_2_PDG_ID/I",
		// "track_3_PDG_ID/I",
	};
	
	std::vector<std::string> const training = {
		// decay_length_significance
		std::string{"decay_len_sig:=log(abs("} + particle_name + "_decayLength / " + particle_name + "_decayLengthErr" + "))",

		// pointing angle
		std::string{"alpha:="} + "acos(" + particle_name + "_DIRA" + ")",

		// vertex volume
		std::string{"vertex_quality:="} + particle_name + "_chi2 / " + particle_name + "_nDoF",

		std::string{"track_1_pT_sig:=(track_1_pT / track_1_pTErr)"},
		std::string{"track_2_pT_sig:=(track_2_pT / track_2_pTErr)"},
		// std::string{"track_3_pT_sig:=(track_3_pT / track_3_pTErr)"},
	};
	
	std::vector<std::string> const cuts = {
		// "2.5 < " + particle_name + "_pT && " + particle_name + "_pT < 3.5",
	};

	TCut get_sideband_cut () {
		std::string cut = std::string{"("}
			+ std::to_string(mean - 6.0 * sig) + " < " + mass_branch + " && " + mass_branch + " < " + std::to_string(mean - 3.0 * sig)
			+ ") || ("
			+ std::to_string(mean + 3.0 * sig) + " < " + mass_branch + " && " + mass_branch + " < " + std::to_string(mean + 6.0 * sig)
			+ ")";
		return TCut(cut.c_str());
	}

	Double_t get_bin_width (
		Double_t num
	) {
		Double_t bin_width = 3.49 * sig / pow(num, 0.3333); // Freedman-Diaconis rule
		return (max_mass - min_mass) / bin_width;
	}

};

#endif//CONFIG_C

