#ifndef TEST_C
#define TEST_C

#include <climits>
#include <cstddef>
#include <cstdint>

#include <array>
#include <memory>
#include <map>
#include <random>
#include <vector>

#include <TFile.h>
#include <TTree.h>

R__LOAD_LIBRARY(libintt_jb.so)
#include <intt_jb/InttEvtJb.h>
#include <intt_jb/InttEvtJbVer1.h>
#include <intt_jb/InttHitJb.h>
#include <intt_jb/InttHitJbVer1.h>

void Compare();
void WriteFile();
void ReadFile();

void Test()
{
	Compare();
}

void Compare() {
	std::string file_name_j = "/sphenix/user/jbertaux/Data/ChannelClassification/beam_intt_combined-00020869-0000.root";
	std::string file_name_o = "/sphenix/user/jbertaux/Data/Debug/beam_intt_combined-00020869-0000.root";

	TFile* file_j = TFile::Open(file_name_j.c_str());
	if(!file_j) {
		std::cout << "Could not get file:" << std::endl;
		std::cout << "\"" << file_name_j << "\"" << std::endl;

		return;
	}
	TTree* tree_j = (TTree*)file_j->Get("hits_tree");
	if(!tree_j) {
		std::cout << "Could not get tree:" << std::endl;
		std::cout << "\"" << "hits_tree" << "\"" << std::endl;

		return;
	}

	InttEvtJb* evt = new InttEvtJbVer1;
	tree_j->SetBranchAddress("hits", &evt);

	for(Long64_t n = 0; n < tree_j->GetEntriesFast(); ++n) {
		tree_j->GetEntry(n);

		std::cout << "Event: " << n << std::endl;
		auto hit = evt->begin();
		(*hit)->print();
		++hit;
		(*hit)->print();
		hit = evt->end();
		--hit;
		(*hit)->print();
	}
	file_j->Close();
	delete evt;

	std::cout << "----------------------------------" << std::endl;

	TFile* file_o = TFile::Open(file_name_o.c_str());
	if(!file_o) {
		std::cout << "Could not get file:" << std::endl;
		std::cout << "\"" << file_name_o << "\"" << std::endl;

		return;
	}
	TTree* tree_o = (TTree*)file_o->Get("prdf_tree");
	if(!tree_o) {
		std::cout << "Could not get tree:" << std::endl;
		std::cout << "\"" << "prdf_tree" << "\"" << std::endl;

		return;
	}
	std::vector<std::pair<std::string, std::vector<int>*>> branches = {
		{"flx_bco",	nullptr},
		{"flx_svr",	nullptr},
		{"flx_chn",	nullptr},
		{"chp",		nullptr},
		{"chn",		nullptr},
		{"adc",		nullptr},
	};
	for(auto& itm : branches) {
		itm.second = new std::vector<int>();
		tree_o->SetBranchAddress(itm.first.c_str(), &itm.second);
	}

	for(Long64_t n = 0; n < tree_o->GetEntriesFast(); ++n) {
		tree_o->GetEntry(n);

		std::cout << "Event: " << n << std::endl;
		for(auto itm : branches) {
			printf("%12s%3u\n", itm.first.c_str(), itm.second->operator[](0));
		}
		std::cout << std::endl;
		for(auto itm : branches) {
			printf("%12s%3u\n", itm.first.c_str(), itm.second->operator[](1));
		}
		std::cout << std::endl;
		for(auto itm : branches) {
			printf("%12s%3u\n", itm.first.c_str(), itm.second->operator[](itm.second->size()-1));
		}
	}

	file_o->Close();
	for(auto itm : branches) {
		delete itm.second;
	}
}

void WriteFile() {

	TFile* file = TFile::Open("foo.root", "RECREATE");
	TTree* tree = new TTree("foo", "foo");

	InttEvtJb* evt = new InttEvtJbVer1();
	tree->Branch("evt", &evt);

	for(Long64_t n_evt = 0; n_evt < 3; ++n_evt) {
		std::cout << "evt: " << n_evt << std::endl;
		evt->set_gtm_bco(n_evt);

		evt->clear();
		for(Long64_t n_hit = 0; n_hit < 1; ++n_hit) {
			InttHitJb* hit = new InttHitJbVer1();

			hit->set_gtm_bco(0xa);
			hit->set_flx_bco(0xb);

			hit->set_flx_svr(n_evt * 6 + n_hit + 1);
			hit->set_flx_chn(n_evt * 6 + n_hit + 2);
			hit->set_chp(n_evt * 6 + n_hit + 3);
			hit->set_chn(n_evt * 6 + n_hit + 4);

			hit->set_adc(n_evt * 6 + n_hit + 5);

			hit->print();

			evt->append_owned(hit);
		}
		tree->Fill();
	}

	tree->Write();
	file->Write();
	file->Close();
}

void ReadFile() {

	TFile* file = TFile::Open("foo.root", "READ");
	TTree* tree = (TTree*)file->Get("foo");

	InttEvtJb* evt = new InttEvtJbVer1();
	tree->SetBranchAddress("evt", &evt);

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		std::cout << "evt: " << n << std::endl;
		tree->GetEntry(n);

		for(auto hit = evt->begin(); hit != evt->end(); ++hit) {
			(*hit)->print();
		}
	}

	file->Close();
}
#endif
