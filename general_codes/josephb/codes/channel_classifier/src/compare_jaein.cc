#include "channel_classifier_helper.h"
#include "channel_classifier/base.h"

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TTree.h>

std::string i_state_format = "/sphenix/user/jbertaux/Data/Debug/beam_intt_classified-%08d.root";
std::string j_state_format = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/HotChannelFinder/event_base_finder/1215/rootfile/normalized/%d_%d_hotlist_clone_0_itr_0.root";

std::string i_treename = "status_tree";
std::string j_treename = "tree";

std::string i_branchname = "state";
std::string j_branchname = "flag";

struct params_s {
	TFile* file = nullptr;
	TTree* tree = nullptr;
	char state = 0;
};

struct jaein_s {
	TFile* file = nullptr;
	TTree* tree = nullptr;
	int state = 0;
	int felix = 0;
	int module = 0;
	int chip_id = 0;
	int chan = 0;
};

int init_params(struct params_s&, int const&);
int init_params(struct jaein_s&, int const&, int const&);
int det_params(struct params_s&);
int det_params(struct jaein_s&);

int main(int argc, char** argv) {
	if(argc < 2) {
		std::cout << "Expected at least one argument" << std::endl;
		std::cout << "\t(the run number)" << std::endl;
		return 1;
	}

	int run_num = -1;
	try {
		run_num = std::stoi(argv[1]);
	} catch (std::invalid_argument& e) {
		std::cout << "Could not cast argument" << std::endl;
		std::cout << "\t" << argv[1] << std::endl;
		std::cout << "As int (expected this positional argument as run number)" <<std::endl;
		std::cout << e.what() << std::endl;
		std::cout << "Exiting" << std::endl;
		return 1;
	} catch (std::exception& e) {
		std::cout << "Other std::stoi failure" << std::endl;
		std::cout << e.what() << std::endl;
		return 1;
	}

	struct params_s i_pars;
	struct jaein_s j_pars;
	struct InttNameSpace::RawData_s raw;

	Long64_t good_conjunction = 0;
	Long64_t good_disjunction = 0;

	if(init_params(i_pars, run_num)) return 1;
	for(int felix_server = 0; felix_server < 8; ++felix_server) {
		if(init_params(j_pars, run_num, felix_server)) continue;

		for(Long64_t n = 0; n < j_pars.tree->GetEntriesFast(); ++n) {
			j_pars.tree->GetEntry(n);

			raw.felix_server = j_pars.felix;
			raw.felix_channel = j_pars.module;
			raw.chip = j_pars.chip_id - 1;
			raw.channel = j_pars.chan;

			int hash = channel_classifier_helper::hash_from_struct(raw);

			i_pars.tree->GetEntry(n);

			if(i_pars.state != 0 && j_pars.state != 0)++good_conjunction;
			if(i_pars.state != 0 || j_pars.state != 0)++good_disjunction;

			if(felix_server == 0 && n < 10) {
				std::cout << "Entry: " << n << std::endl;
				std::cout << "\t" << raw.felix_server << std::endl;
				std::cout << "\t" << raw.felix_channel << std::endl;
				std::cout << "\t" << raw.chip << std::endl;
				std::cout << "\t" << raw.channel << std::endl;
				std::cout << std::endl;
			}
		}
	}

	Double_t ratio = good_conjunction;
	ratio /= good_disjunction;

	std::cout << ratio << std::endl;

	det_params(i_pars);
	det_params(j_pars);

	return 0;
}

int init_params(
	struct params_s& pars,
	int const& run_num
) {
	char buff[256];

	snprintf(buff, sizeof(buff), i_state_format.c_str(), run_num);
	std::string i_filename = buff;
	if(!std::filesystem::exists(i_filename.c_str())) {
		std::cout << "Input file" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		std::cout << "Does not exist" << std::endl;
		return 1;
	}

	pars.file = TFile::Open(i_filename.c_str(), "READ");
	if(!pars.file) {
		std::cout << "Could not open file" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		return 1;
	}

	pars.tree = dynamic_cast<TTree*>(pars.file->Get(i_treename.c_str()));
	if(!pars.tree) {
		std::cout << "Could not open tree" << std::endl;
		std::cout << "\t" << i_treename << std::endl;
		std::cout << "In file:" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		return 1;
	}

	if(!pars.tree->GetBranch(i_branchname.c_str())) {
		std::cout << "Could not get branch" << std::endl;
		std::cout << "\t" << i_branchname << std::endl;
		std::cout << "In tree" << std::endl;
		std::cout << "\t" << i_treename << std::endl;
		std::cout << "In file:" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		return 1;
	}

	pars.tree->SetBranchAddress(i_branchname.c_str(), &pars.state);
	return 0;
}

int init_params(
	struct jaein_s& pars,
	int const& run_num,
	int const& felix
) {
	char buff[256];

	snprintf(buff, sizeof(buff), j_state_format.c_str(), run_num, felix);
	std::string j_filename = buff;
	if(!std::filesystem::exists(j_filename.c_str())) {
		std::cout << "Input file" << std::endl;
		std::cout << "\t" << j_filename << std::endl;
		std::cout << "Does not exist" << std::endl;
		return 1;
	}

	pars.file = TFile::Open(j_filename.c_str(), "READ");
	if(!pars.file) {
		std::cout << "Could not open file" << std::endl;
		std::cout << "\t" << j_filename << std::endl;
		return 1;
	}

	pars.tree = dynamic_cast<TTree*>(pars.file->Get(j_treename.c_str()));
	if(!pars.tree) {
		std::cout << "Could not open tree" << std::endl;
		std::cout << "\t" << j_treename << std::endl;
		std::cout << "In file:" << std::endl;
		std::cout << "\t" << j_filename << std::endl;
		return 1;
	}

	std::map<std::string, int&> branches = {
		{"flag",	pars.state},
		{"felix",	pars.felix},
		{"module",	pars.module},
		{"chip_id",	pars.chip_id},
		{"chan",	pars.chan},
	};

	for(auto& pair : branches) {
		if(pars.tree->GetBranch(pair.first.c_str()))continue;

		std::cout << "Could not get branch" << std::endl;
		std::cout << "\t" << pair.first << std::endl;
		std::cout << "In tree" << std::endl;
		std::cout << "\t" << j_treename << std::endl;
		std::cout << "In file:" << std::endl;
		std::cout << "\t" << j_filename << std::endl;
		return 1;
	}

	for(auto& pair : branches) {
		pars.tree->SetBranchAddress(pair.first.c_str(), &pair.second);
	}
	return 0;
}

int det_params(
	struct params_s& pars
) {
	if(pars.file)pars.file->Close();
	return 0;
}

int det_params(
	struct jaein_s& pars
) {
	if(pars.file)pars.file->Close();
	return 0;
}
