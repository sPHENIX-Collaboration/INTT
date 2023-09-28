#ifndef DEBUG_COMBINED_C
#define DEBUG_COMBINED_C

#include <cstddef>
#include <cstdint>

#include <iostream>
#include <string>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH1D.h>

const int BUFF_SIZE = 256;

void DebugCombined(std::string i_format, std::string o_format, int run_num)
{
	char buff[BUFF_SIZE];

	snprintf(buff, BUFF_SIZE, i_format.c_str(), run_num);
	std::string name = buff;
	TFile* file = TFile::Open(name.c_str(), "READ");
	if(!file)
	{
		std::cout << std::endl;
		std::cout << "Could not open file:" << std::endl;
		std::cout << "\t" << name << std::endl;
		std::cout << "Exiting" << std::endl;
		std::cout << std::endl;

		exit(1);
	}

	std::cout << std::endl;
	std::cout << "Got file:" << std::endl;
	std::cout << "\t" << name << std::endl;
	TTree* tree = (TTree*)file->Get("prdf_tree");
	if(!tree)
	{
		std::cout << std::endl;
		std::cout << "Could not get tree \"prdf_tree\" from file:" << std::endl;
		std::cout << "\t" << name << std::endl;
		std::cout << "(Contains:)" << std::endl;
		file->ls();
		std::cout << "Exiting" << std::endl;
		std::cout << std::endl;

		exit(1);
	}
	std::cout << std::endl;
	std::cout << "Got tree:" << std::endl;
	std::cout << "\t" << "\"prdf_tree\"" << std::endl;

	std::map<std::string, std::vector<Int_t>*> branches =
	{
		//{"flx_svr",	new std::vector<Int_t>()},
		//{"flx_chn",	new std::vector<Int_t>()},
		{"lyr",		new std::vector<Int_t>()},
		//{"ldr",	new std::vector<Int_t>()},
		//{"arm",	new std::vector<Int_t>()},
		//{"chp",	new std::vector<Int_t>()},
		//{"chn",	new std::vector<Int_t>()},
	};


	{
		bool b = false;
		for(auto& itr: branches)
		{
			if(!tree->GetBranch(itr.first.c_str()))
			{
				std::cout << "\tCould not get branch:" << std::endl;
				std::cout << "\t\t" << itr.first << std::endl;
				b = true;

				continue;
			}
			tree->SetBranchAddress(itr.first.c_str(), &(itr.second));
		}
		if(b)
		{
			std::cout << "Failed to get all branches from tree" << std::endl;
			std::cout << "\t\"prdf_tree\"" << std::endl;
			std::cout << "From file:" << std::endl;
			std::cout << "\t" << name << std::endl;
			std::cout << "Exiting" << std::endl;

			exit(1);
		}
	}

	int min_multiplicity = INT_MAX;
	int max_multiplicity = 0;

	snprintf(buff, BUFF_SIZE, o_format.c_str(), run_num);
	std::string o_name = buff;
	TFile* o_file = TFile::Open(o_name.c_str(), "RECREATE");
	TTree* o_tree = new TTree("correlation_tree", "correlation_tree");
	std::map<std::string, int> o_branches =
	{
		{"inner_hits", 0},
		{"outer_hits", 0},
	};
	for(auto& itr: o_branches)
	{
		o_tree->Branch(itr.first.c_str(), &(itr.second));
	}

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
	{
		tree->GetEntry(n);

		o_branches["inner_hits"] = 0;
		o_branches["outer_hits"] = 0;
		for(std::size_t s = 0; s < branches.begin()->second->size(); ++s)
		{
			if(branches["lyr"]->operator[](s) < 2)	++o_branches["inner_hits"];
			else					++o_branches["outer_hits"];
		}
		o_tree->Fill();

		//for(auto itr: o_branches)
		//{
		//	if(itr.second < min_multiplicity)min_multiplicity = itr.second;
		//	if(itr.second > max_multiplicity)max_multiplicity = itr.second;
		//}
	}

	TCanvas* cnvs = new TCanvas(
		Form("INTT_Multiplicity_Correlation_Run_%08d", run_num), 
		Form("INTT_Multiplicity_Correlation_Run_%08d", run_num));

	cnvs->cd();
	o_tree->Draw("outer_hits:inner_hits");
	cnvs->SaveAs(Form("INTT_Multiplicity_Correlation_Run_%08d.pdf", run_num));

	o_tree->Write();
	o_file->Write();
	o_file->Close();
}

#endif//DEBUG_COMBINED_C
