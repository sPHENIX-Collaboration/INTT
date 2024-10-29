#ifndef APPLY_C
#define APPLY_C

#include "defs.C"

#include <tmvahelper/TMVAHelper.h>
R__LOAD_LIBRARY(libtmvahelper.so)

void
apply (
	std::vector<std::string> const& input_files = {
		"outputMinBiasKFParticle_Lc_pKpi_0.root",
	}
) {
	// Helper
	TMVAHelper tmva_helper;
	tmva_helper.read_branches(defs::branches);
	tmva_helper.read_training(defs::training);
	tmva_helper.read_cuts(defs::pT_cuts[0]); // CHANGE ME

	// Initialize reader
	TMVA::Reader* reader = new TMVA::Reader("!Color:!Silent");
	tmva_helper.branch(reader);
	reader->BookMVA (
		defs::method_name,
		(boost::format("dataloader/weights/factory_%s.weights.xml") % defs::method_name.c_str()).str().c_str()
	);

	// Add input files
	for (auto const& input_file : input_files) {
		TTree* tree = tmva_helper.get_tree(defs::data_dir + "/" + input_file, "DecayTree");
		if (!tree) continue;
		if (tmva_helper.branch(tree)) continue;
		for (Long64_t n = 0, N = tree->GetEntriesFast(); n < N; ++n) {
			tree->GetEntry(n);
			if (tmva_helper.eval()) continue;
			tmva_helper.show();
		}
	}

}

#endif//APPLY_C

