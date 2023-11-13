#ifndef TEST_C
#define TEST_C

#include <climits>
#include <cstddef>
#include <cstdint>

#include <set>
#include <random>
#include <vector>

#include <TFile.h>
#include <TTree.h>

#include <intt_josephb/InttHitJb.h>
R__LOAD_LIBRARY(libintt_josephb.so)

void MakeExampleFile();
void ReadExampleFile();

void Test()
{
	//MakeFile();
	//ReadFile();

	//InttHitJb hit;
	//hit.print_size();

	TFile* file = TFile::Open("/sphenix/user/jbertaux/Data/ChannelClassification/beam_intt_combined-00020869-0000.root");
	TTree* tree = (TTree*)file->Get("hits_tree");

	std::vector<InttHitJb>* hits_jb = new std::vector<InttHitJb>;
	tree->SetBranchAddress("hits", &hits_jb);

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
	{
		tree->GetEntry(n);

		std::size_t s = 0;
		for(InttHitJb const& hit : *hits_jb)
		{
			hit.print();

			if(++s > 5)break;
		}
	}
}

void MakeExampleFile()
{
	std::mt19937 gen(1234);
	std::uniform_int_distribution<int> dis(0, INT_MAX);

	TFile* file = TFile::Open("foo.root", "RECREATE");
	TTree* tree = new TTree("foo", "foo");

	std::vector<InttHitJb>* hits = new std::vector<InttHitJb>;
	tree->Branch("hits", &hits);

	for(unsigned int i = 0; i < 2; ++i)
	{
		hits->clear();
		std::set<InttHitJb> old_hits;

		for(unsigned int j = 0; j < 3; ++j)
		{
			InttHitJb hit;

			if(j != 2)
			{
				hit.set_gtm_bco(dis(gen));

				hit.set_flx_bco(dis(gen));
				hit.set_flx_bco(dis(gen));
				hit.set_flx_bco(dis(gen));

				hit.set_chp(dis(gen));
				hit.set_chn(dis(gen));
				hit.set_adc(dis(gen));
			}
			else
			{
				hit = hits->back();
			}

			hit.print();

			if(old_hits.find(hit) != old_hits.end())
			{
				std::cout << "Redundant hit" << std::endl;
				continue;
			}

			old_hits.insert(hit);
			hits->push_back(hit);
		}

		tree->Fill();
	}

	tree->Write();
	file->Write();

	file->Close();
}

void ReadExampleFile()
{
	TFile* file = TFile::Open("foo.root", "READ");
	if(!file)
	{
		std::cout << "no file" << std::endl;
		return;
	}
	TTree* tree = dynamic_cast<TTree*>(file->Get("foo"));
	if(!tree)
	{
		std::cout << "no tree" << std::endl;
		return;
	}
	TBranch* branch = tree->GetBranch("hits");
	if(!branch)
	{
		std::cout << "no branch" << std::endl;
		return;
	}

	std::vector<InttHitJb>* hits = new std::vector<InttHitJb>;
	tree->SetBranchAddress("hits", &hits);

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
	{
		tree->GetEntry(n);

		for(auto hit : *hits)
		{
			hit.print();
		}
	}

	file->Close();
}

#endif
