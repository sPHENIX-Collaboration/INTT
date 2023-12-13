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
std::string i_treename = "status_tree";
std::string i_branchname = "state";

struct params_s {
	TFile* file = nullptr;
	TTree* tree = nullptr;
	char state = 0;
};

int init_params(struct params_s&, int const&);
int det_params(struct params_s&);

int main(int argc, char** argv) {

	std::vector<int> run_nums = {
		20444,
		20447,
		20448,
		20449,
	};

	struct params_s i_pars;
	struct params_s j_pars;

	std::cout << std::endl;

	for(auto itr = run_nums.cbegin(); itr != run_nums.cend(); ++itr) {
		if(init_params(i_pars, *itr)) continue;
		Long64_t good = 0;
		for(Long64_t n = 0; n < i_pars.tree->GetEntriesFast(); ++n) {
			i_pars.tree->GetEntry(n);
			if(i_pars.state) continue;
			++good;
		}

		double ratio = good;
		ratio /= i_pars.tree->GetEntriesFast();
		std::cout << "" << *itr << ": " << ratio << std::endl;
		det_params(i_pars);
	}

	std::cout << std::endl;

	for(auto itr = run_nums.cbegin(); itr != run_nums.cend(); ++itr) {
		if(init_params(i_pars, *itr)) continue;
		for(auto jtr = itr; jtr != run_nums.cend(); ++jtr) {
			if(jtr == itr) continue;
			if(init_params(j_pars, *jtr)) continue;

			Long64_t good_disjunction = 0;
			Long64_t good_conjunction = 0;
			for(Long64_t n = 0; n < i_pars.tree->GetEntriesFast(); ++n) {
				i_pars.tree->GetEntry(n);
				j_pars.tree->GetEntry(n);

				if(i_pars.state == 0 && j_pars.state == 0) ++good_conjunction;
				if(i_pars.state == 0 || j_pars.state == 0) ++good_disjunction;
			}

			double ratio = good_conjunction;
			ratio /= good_disjunction;
			std::cout << "(" << *itr << ", " << *jtr << "): " << ratio << std::endl;

			det_params(j_pars);
		}
		det_params(i_pars);
	}

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

int det_params(
	struct params_s& pars
) {
	if(pars.file)pars.file->Close();
	return 0;
}
