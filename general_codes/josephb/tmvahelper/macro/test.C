#include "config.C"

#include <tmvahelper/TMVAHelper.h>
R__LOAD_LIBRARY(libtmvahelper.so)

#include <filesystem>

void
test (
	std::string const& data_dir
) {
	Int_t track_1_PDG_ID{};
	Int_t track_2_PDG_ID{};
	Int_t track_3_PDG_ID{};
	std::vector<Int_t>* track_1_true_track_history_PDG_ID = new std::vector<int>{};
	std::vector<Int_t>* track_2_true_track_history_PDG_ID = new std::vector<int>{};
	std::vector<Int_t>* track_3_true_track_history_PDG_ID = new std::vector<int>{};

	for (auto const& entry : std::filesystem::directory_iterator{data_dir}) {
		if (!entry.is_regular_file()) continue;

		std::string filename = entry.path().filename();
		if (filename.find(config::channel) == std::string::npos) continue;
		if (filename.find("sig_KFP") == std::string::npos) continue;

		TTree* tree = TMVAHelper::get_tree(entry.path().string(), "DecayTree");
		if (!tree) {
			std::cerr << entry.path().c_str() << std::endl;
			continue;
		}

		tree->SetBranchAddress("track_1_PDG_ID", &track_1_PDG_ID);
		tree->SetBranchAddress("track_2_PDG_ID", &track_2_PDG_ID);
		tree->SetBranchAddress("track_3_PDG_ID", &track_3_PDG_ID);
		tree->SetBranchAddress("track_1_true_track_history_PDG_ID", &track_1_true_track_history_PDG_ID);
		tree->SetBranchAddress("track_2_true_track_history_PDG_ID", &track_2_true_track_history_PDG_ID);
		tree->SetBranchAddress("track_3_true_track_history_PDG_ID", &track_3_true_track_history_PDG_ID);

		std::cout << "Got 'DecayTree' from file " << filename << std::endl;
		for (Long64_t n = 0, N = tree->GetEntriesFast(); n < N; ++n) {
			tree->GetEntry(n);

			if (
				track_1_true_track_history_PDG_ID->size() != 0 &&
				track_1_true_track_history_PDG_ID->back() != track_1_PDG_ID
			) {
				std::cout
					<< "track_1_true_track_history_PDG_ID->back(): " << track_1_true_track_history_PDG_ID->back() << "\t"
					<< "track_1_PDG_ID: " << track_1_PDG_ID << "\n"
					<< std::flush;
			}

			if (
				track_2_true_track_history_PDG_ID->size() != 0 &&
				track_2_true_track_history_PDG_ID->back() != track_2_PDG_ID
			) {
				std::cout
					<< "track_2_true_track_history_PDG_ID->back(): " << track_2_true_track_history_PDG_ID->back() << "\t"
					<< "track_2_PDG_ID: " << track_2_PDG_ID << "\n"
					<< std::flush;
			}

			if (
				track_3_true_track_history_PDG_ID->size() != 0 &&
				track_3_true_track_history_PDG_ID->back() != track_3_PDG_ID
			) {
				std::cout
					<< "track_3_true_track_history_PDG_ID->back(): " << track_3_true_track_history_PDG_ID->back() << "\t"
					<< "track_3_PDG_ID: " << track_3_PDG_ID << "\n"
					<< std::flush;
			}

			if (10 < n) break;
		}
	}
}
