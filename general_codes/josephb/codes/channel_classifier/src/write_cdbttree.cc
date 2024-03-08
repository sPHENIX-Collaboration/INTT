#include "channel_classifier_helper.h"
#include "channel_classifier/base.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <type_traits>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TTree.h>

#include <intt/InttMapping.h>
#include <cdbobjects/CDBTTree.h>

struct params_s {
	TFile* file = nullptr;
	TTree* tree = nullptr;
	CDBTTree* cdbttree = nullptr;
	InttNameSpace::RawData_s raw;
	char state = 0;
};

int init_params(struct params_s&, int const&);
int det_params(struct params_s&);

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

	struct params_s pars;
	if(init_params(pars, run_num)) return 1;

	int size = 0;
	for(Long64_t n = 0; n < pars.tree->GetEntriesFast(); ++n) {
		pars.tree->GetEntry(n);
		if(!pars.state) continue;

		pars.cdbttree->SetIntValue(size, "felix_server", pars.raw.felix_server);
		pars.cdbttree->SetIntValue(size, "felix_channel", pars.raw.felix_channel);
		pars.cdbttree->SetIntValue(size, "chip", pars.raw.chip);
		pars.cdbttree->SetIntValue(size, "channel", pars.raw.channel);
		++size;

		// printf("n:%3ld\n", n);
		// printf("\t%16s%3d\n", "felix_server:", pars.raw.felix_server);
		// printf("\t%16s%3d\n", "felix_channel:", pars.raw.felix_channel);
		// printf("\t%16s%3d\n", "chip:", pars.raw.chip);
		// printf("\t%16s%3d\n", "channel:", pars.raw.channel);
		// printf("\t%16s%3u\n", "state", pars.state);
		// printf("\n");
		// if(size > 5)break;
	}
	pars.cdbttree->SetSingleIntValue("size", size);

	det_params(pars);

	return 0;
}

int init_params(
	struct params_s& pars,
	int const& run_num
) {
	std::string STATE_FORMAT = "/sphenix/user/jbertaux/Data/Debug/beam_intt_classified-%08d.root";
	std::string CDB_FORMAT = "/sphenix/user/jbertaux/Data/Debug/intt_cdb-%08d.root";
	std::string TREENAME = "status_tree";
	std::string STATE_BRANCHNAME = "state";
	std::map<std::string, int*> BRANCHNAMES = {
		{"flx_svr", &(pars.raw.felix_server)},
		{"flx_chn", &(pars.raw.felix_channel)},
		{"chp", &(pars.raw.chip)},
		{"chn", &(pars.raw.channel)},
	};
	// 	"felix_server",
	// 	"felix_channel",
	// 	"chip",
	// 	"channel",

	char buff[256];
	std::string FILENAME;

	snprintf(buff, sizeof(buff), STATE_FORMAT.c_str(), run_num);
	FILENAME = buff;
	if(!std::filesystem::exists(FILENAME.c_str())) {
		std::cout << "Input file" << std::endl;
		std::cout << "\t" << FILENAME << std::endl;
		std::cout << "Does not exist" << std::endl;
		return 1;
	}

	pars.file = TFile::Open(FILENAME.c_str(), "READ");
	if(!pars.file) {
		std::cout << "Could not open file" << std::endl;
		std::cout << "\t" << FILENAME << std::endl;
		return 1;
	}

	pars.tree = dynamic_cast<TTree*>(pars.file->Get(TREENAME.c_str()));
	if(!pars.tree) {
		std::cout << "Could not open tree" << std::endl;
		std::cout << "\t" << TREENAME << std::endl;
		std::cout << "In file:" << std::endl;
		std::cout << "\t" << FILENAME << std::endl;
		return 1;
	}

	if(!pars.tree->GetBranch(STATE_BRANCHNAME.c_str())) {
		std::cout << "Could not get branch" << std::endl;
		std::cout << "\t" << STATE_BRANCHNAME << std::endl;
		std::cout << "In tree" << std::endl;
		std::cout << "\t" << TREENAME << std::endl;
		std::cout << "In file:" << std::endl;
		std::cout << "\t" << FILENAME << std::endl;
		return 1;
	}
	pars.tree->SetBranchAddress(STATE_BRANCHNAME.c_str(), &pars.state);

	bool b = false;
	for(auto& p : BRANCHNAMES) {
		if(!pars.tree->GetBranch(p.first.c_str())) {
			std::cout << "Could not get branch" << std::endl;
			std::cout << "\t" << p.first << std::endl;
			std::cout << "In tree" << std::endl;
			std::cout << "\t" << TREENAME << std::endl;
			std::cout << "In file:" << std::endl;
			std::cout << "\t" << FILENAME << std::endl;

			b = true;
			continue;
		}

		pars.tree->SetBranchAddress(p.first.c_str(), p.second);
	}

	if(b) {
		return 1;
	}

	snprintf(buff, sizeof(buff), CDB_FORMAT.c_str(), run_num);
	FILENAME = buff;
	if(pars.cdbttree)delete pars.cdbttree;
	pars.cdbttree = new CDBTTree(FILENAME);

	return 0;
}

int det_params(
	struct params_s& pars
) {
	if(pars.cdbttree)pars.cdbttree->Commit();
	if(pars.cdbttree)pars.cdbttree->CommitSingle();
	if(pars.cdbttree)pars.cdbttree->WriteCDBTTree();
	if(pars.cdbttree)delete pars.cdbttree;
	pars.cdbttree = nullptr;
	if(pars.file)pars.file->Close();
	return 0;
}
