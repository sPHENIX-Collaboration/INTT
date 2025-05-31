#ifndef ARBORIST_CHECK_C
#define ARBORIST_CHECK_C

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllServer.h>
R__LOAD_LIBRARY(libfun4all.so)

#include <intt/InttArborist.h>
R__LOAD_LIBRARY(libintt.so)

void
arborist_check (
	int n_events = 0,
	std::string const& dst_in_path = "dstout.root"
) {
	// /// Run InttArborist with Fun4All
	gSystem->Load("libg4dst");

	Fun4AllServer *se = Fun4AllServer::instance();
	se->Verbosity(1);

	Fun4AllDstInputManager *in = new Fun4AllDstInputManager("DSTIN");
	in->fileopen(dst_in_path);
	se->registerInputManager(in);

	InttArborist* intt_arborist = new InttArborist;
	intt_arborist->CreateOutputFile("arborist.root");
	se->registerSubsystem(intt_arborist);

	se->run(n_events);
	se->End();

	/// Check the produced TTree after Fun4All
	TFile* arborist_file = TFile::Open("arborist.root", "READ");
	if (!arborist_file) {
		std::cout << "issue opening arborist.root" << std::endl;
		return;
	}

	TTree* arborist_tree = dynamic_cast<TTree*>(arborist_file->Get("intt_tree"));
	if (!arborist_tree) {
		std::cout << "issue with tree from arborist.root" << std::endl;
		arborist_file->Close();
		return;
	}

	std::map<std::string, std::vector<InttArborist::small_t>*>
	small_branches = {
		{"adc", new std::vector<InttArborist::small_t>},
		{"amp", new std::vector<InttArborist::small_t>},
		{"bco", new std::vector<InttArborist::small_t>},
		{"chn", new std::vector<InttArborist::small_t>},
		{"chp", new std::vector<InttArborist::small_t>},
		{"fee", new std::vector<InttArborist::small_t>},
		{"pid", new std::vector<InttArborist::small_t>},
	};
	
	std::map<std::string, InttArborist::large_t>
	large_branches = {
		{"gtm", 0},
	};

	for (auto& [name, vec_ptr] : small_branches) {
		arborist_tree->SetBranchAddress(name.c_str(), &vec_ptr);
	}
	for (auto& [name, val] : large_branches) {
		arborist_tree->SetBranchAddress(name.c_str(), &val);
	}

	for (Long64_t n = 0, N = arborist_tree->GetEntriesFast(); n < N; ++n) {
		arborist_tree->GetEntry(n);
		std::cout << n << ":" << std::endl;
		for (auto const& [name, val] : large_branches) {
			std::cout << "\t" << name << ": " << val << std::endl;
		}
		for (auto const& [name, vec_ptr] : small_branches) {
			std::cout << "\t" << name << ": ";
			for (auto const& val : *vec_ptr) {
				std::cout << val << ", ";
			}
			std::cout << std::endl;
		}
	}
}

#endif//ARBORIST_CHECK_C
