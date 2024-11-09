#ifndef CONFIG_C
#define CONFIG_C

#include <sPhenixStyle.C>
#include <boost/format.hpp>

namespace config {

	// gen
	std::string const channel            = "Lc_pKpi";
	std::string const pythia_config_file = "steering_cards/pythia8_Lc_pKpi.cfg";
	std::string const evtgen_config_file = "dec_files/Lc_pKpi.DEC";
	std::string const decay_descriptor   = "[Lambda_c+ -> proton^+ K^- pi^+]cc";
	int         const particle_trigger   = 4122;

	// fit
	Double_t const min_mass = 2.10;
	Double_t const max_mass = 2.60;

	Double_t mean = 2.27653e+00;
	Double_t sig  = 2.00000e-01;
	Int_t num_bins = 100;

	// training/application
	// Double_t const ratio = 0.015;
	Double_t const ratio = 2.0E-3;
	std::string const factory_file_name = "factory/factory.root";

	TMVA::Types::EMVA const method_type = TMVA::Types::kBDT;
	std::string       const method_name = "BDT";
	std::string       const method_options =
		"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20";
	// cut: 0.312493 significance: 0.00769632
	Double_t cut_val =  0.312493;

	// TMVA::Types::EMVA const method_type = TMVA::Types::kMLP;
	// std::string       const method_name = "MLP";
	// std::string       const method_options =
	// 	"!H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator";
	// Double_t cut_val = 0.0;

	std::string const particle_name = "Lambda_cplus";
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
		"track_3_pT", "track_3_pTErr",

		"primary_vertex_volume",
		"primary_vertex_chi2",
		"primary_vertex_nDoF/U",
	};

	std::vector<std::string> const prod_branches = {
		mass_branch, particle_name + "_pT",

		particle_name + "_vertex_volume",
		particle_name + "_IP",
		particle_name + "_chi2",
		particle_name + "_nDoF/U",
		particle_name + "_DIRA",

		particle_name + "_decayLength", particle_name + "_decayLengthErr",

		particle_name + "_pseudorapidity",

		"track_1_pT", "track_1_pTErr", "track_1_p",
		"track_2_pT", "track_2_pTErr", "track_2_p",
		"track_3_pT", "track_3_pTErr", "track_3_p",

		"track_1_dEdx",
		"track_2_dEdx",
		"track_3_dEdx",

		"track_1_PDG_ID/I",
		"track_2_PDG_ID/I",
		"track_3_PDG_ID/I",

		"primary_vertex_volume",
		"primary_vertex_chi2",
		"primary_vertex_nDoF/U",

		"primary_vertex_z",

		"track_1_IP_xy",
		"track_2_IP_xy",
		"track_3_IP_xy",

		"track_1_MVTX_nStates/U", "track_1_INTT_nHits/U", "track_1_TPC_nStates/U",
		"track_2_MVTX_nStates/U", "track_2_INTT_nHits/U", "track_2_TPC_nStates/U",
		"track_3_MVTX_nStates/U", "track_3_INTT_nHits/U", "track_3_TPC_nStates/U",

		"track_1_bunch_crossing/I",
		"track_2_bunch_crossing/I",
		"track_3_bunch_crossing/I",

		"track_1_pseudorapidity",
		"track_2_pseudorapidity",
		"track_3_pseudorapidity",

		"track_1_track_2_DCA_xy",
		"track_1_track_3_DCA_xy",
		"track_2_track_3_DCA_xy",
	};

	std::vector<std::string> const prod_cuts = {
		// mother
		std::string{"1.0 < "} + particle_name + "_pT",
		std::string{"fabs("} + particle_name + "_pseudorapidity < 1.1)",
		std::string{"5.0 < fabs("} + particle_name + "_decayLength / "
			+ particle_name + "_decayLengthErr)",
		particle_name + "_decayLength > 0.004",
		particle_name + "_decayLength < 0.05",
		particle_name + "_DIRA > 0.98",
		std::string{"fabs("} + particle_name + "_chi2) < 15",
		particle_name + "_IP < 0.1",

		// vertex
		"fabs(primary_vertex_z) < 10.0",

		// ipxy
		"fabs(track_1_IP_xy) < 0.1",
		"fabs(track_2_IP_xy) < 0.1",
		"fabs(track_3_IP_xy) < 0.1",

		// states/hits
		"2 < track_1_MVTX_nStates",
		"2 < track_2_MVTX_nStates",
		"2 < track_3_MVTX_nStates",
		"2 == track_1_INTT_nHits",
		"2 == track_2_INTT_nHits",
		"2 == track_3_INTT_nHits",
		"25 < track_1_TPC_nStates",
		"25 < track_2_TPC_nStates",
		"25 < track_3_TPC_nStates",

		// bunchcrossing
		"track_1_bunch_crossing == track_2_bunch_crossing",
		"track_2_bunch_crossing == track_3_bunch_crossing",
		"0 < track_1_bunch_crossing && track_1_bunch_crossing < 350",
		"0 < track_2_bunch_crossing && track_2_bunch_crossing < 350",
		"0 < track_3_bunch_crossing && track_3_bunch_crossing < 350",

		// kinematics
		"0.2 < track_1_pT",
		"0.2 < track_2_pT",
		"0.2 < track_3_pT",
		"fabs(track_1_pseudorapidity) < 1.1",
		"fabs(track_2_pseudorapidity) < 1.1",
		"fabs(track_3_pseudorapidity) < 1.1",

		"fabs(track_1_track_2_DCA_xy) < 0.004",
		"fabs(track_1_track_3_DCA_xy) < 0.004",
		"fabs(track_2_track_3_DCA_xy) < 0.004",
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
		std::string{"track_3_pT_sig:=(track_3_pT / track_3_pTErr)"},

	};
	
	std::vector<std::string> const cuts = {
		// ...
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

