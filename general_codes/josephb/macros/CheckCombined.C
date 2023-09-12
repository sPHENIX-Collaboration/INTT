#ifndef CHECK_COMBINED_C
#define CHECK_COMBINED_C

#include <iostream>
#include <string>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH1D.h>

std::string check_format="/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt%d-%08d-0000.root";
std::string combined_format="/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt_combined-%08d-0000.root";
std::string combined_1_format="/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt_combined-%08d-0000_1.root";

bool DEBUG = true;
int DEBUG_WIDTH = 12;
int DEBUG_MULTIPLICITY = 10;
int DEBUG_ENTRY = 5;

void CheckCombined()
{
	int run_num = 20445;
	int server = 4;

	//File retrieval, guard clauses
	std::string name;
	name = Form(check_format.c_str(), server, run_num);
	TFile* check_file = TFile::Open(name.c_str(), "READ");
	if(!check_file)
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
	TTree* check_tree = (TTree*)check_file->Get("prdf_tree");
	if(!check_tree)
	{
		std::cout << std::endl;
		std::cout << "Could not get tree \"prdf_tree\" from file:" << std::endl;
		std::cout << "\t" << name << std::endl;
		std::cout << "(Contains:)" << std::endl;
		check_file->ls();
		std::cout << "Exiting" << std::endl;
		std::cout << std::endl;

		exit(1);
	}

	name = Form(combined_format.c_str(), run_num);
	TFile* combined_file = TFile::Open(name.c_str(), "READ");
	if(!combined_file)
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
	TTree* combined_tree = (TTree*)combined_file->Get("prdf_tree");
	if(!combined_tree)
	{
		std::cout << std::endl;
		std::cout << "Could not get tree \"prdf_tree\" from file:" << std::endl;
		std::cout << "\t" << name << std::endl;
		std::cout << "(Contains:)" << std::endl;
		combined_file->ls();
		std::cout << "Exiting" << std::endl;
		std::cout << std::endl;

		exit(1);
	}
	name = Form(combined_1_format.c_str(), run_num);
	TFile* combined_1_file = TFile::Open(name.c_str(), "READ");
	if(!combined_1_file)
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
	TTree* combined_1_tree = (TTree*)combined_file->Get("prdf_tree");
	if(!combined_1_tree)
	{
		std::cout << std::endl;
		std::cout << "Could not get tree \"prdf_tree\" from file:" << std::endl;
		std::cout << "\t" << name << std::endl;
		std::cout << "(Contains:)" << std::endl;
		combined_1_file->ls();
		std::cout << "Exiting" << std::endl;
		std::cout << std::endl;

		exit(1);
	}
	//Branch addressing/branch addressing verification
	Int_t check_hits = 0;
	Int_t max_hits = 0;
	check_tree->SetBranchAddress("num_hits", &check_hits);
	for(Long64_t n = 0; n < check_tree->GetEntriesFast(); ++n)
	{
		check_tree->GetEntry(n);
		if(check_hits > max_hits)max_hits = check_hits;
	}

	std::map<std::string, Int_t*> check_branches =
	{
		{"flx_chn",	new Int_t[max_hits]},
		{"lyr",		new Int_t[max_hits]},
		{"ldr",		new Int_t[max_hits]},
		{"arm",		new Int_t[max_hits]},
		{"chp",		new Int_t[max_hits]},
		{"chn",		new Int_t[max_hits]},
	};

	for(auto& itr: check_branches)
	{
		check_tree->SetBranchAddress(itr.first.c_str(), itr.second);
	}

	if(DEBUG)
	{
		for(Long64_t n = 0; n < check_tree->GetEntriesFast(); ++n)
		{
			check_tree->GetEntry(n);

			printf("Entry: %08lld (multiplicity %08d)\n", n, check_hits);
			for(auto& itr: check_branches)
			{
				printf("%*s", DEBUG_WIDTH, itr.first.c_str());
			}
			printf("\n");
			for(int s = 0; s < check_hits; ++s)
			{
				for(auto& itr: check_branches)
				{
					printf("%*d", DEBUG_WIDTH, itr.second[s]);
				}
				printf("\n");

				if(s > DEBUG_MULTIPLICITY)break;
			}
			printf("\n");

			if(n > DEBUG_ENTRY)break;
		}

		//check_tree->Scan();
	}

	std::map<std::string, std::vector<Int_t>*> combined_branches =
	{
		{"flx_svr",	new std::vector<Int_t>()},
		{"flx_chn",	new std::vector<Int_t>()},
		{"lyr",		new std::vector<Int_t>()},
		{"ldr",		new std::vector<Int_t>()},
		{"arm",		new std::vector<Int_t>()},
		{"chp",		new std::vector<Int_t>()},
		{"chn",		new std::vector<Int_t>()},
	};

	for(auto& itr: combined_branches)
	{
		combined_tree->SetBranchAddress(itr.first.c_str(), &(itr.second));
	}

	if(DEBUG)
	{
		for(Long64_t n = 0; n < combined_tree->GetEntriesFast(); ++n)
		{
			combined_tree->GetEntry(n);

			printf("Entry: %08lld (multiplicity %08zu)\n", n, combined_branches.begin()->second->size());
			for(auto& itr: combined_branches)
			{
				printf("%*s", DEBUG_WIDTH, itr.first.c_str());
			}
			printf("\n");
			for(std::size_t s = 0; s < combined_branches.begin()->second->size(); ++s)
			{
				for(auto& itr: combined_branches)
				{
					printf("%*d", DEBUG_WIDTH, (*itr.second)[s]);
				}
				printf("\n");

				if(s > DEBUG_MULTIPLICITY)break;
			}
			printf("\n");

			if(n > DEBUG_ENTRY)break;
		}

		//combined_tree->Scan();
	}


	//Histogram filling
	TH1* check_chp_dis = new TH1D("check_chp_dis", "check_chp_dis", 26, -0.5, 25.5);
	TH1* check_chn_dis = new TH1D("check_chn_dis", "check_chn_dis", 128, -0.5, 127.5);

	for(Long64_t n = 0; n < check_tree->GetEntriesFast(); ++n)
	{
		check_tree->GetEntry(n);

		for(int s = 0; s < check_hits; ++s)
		{
			//Do things
			check_chp_dis->Fill(check_branches["chp"][s]);
			check_chn_dis->Fill(check_branches["chn"][s]);
		}
	}

	TH1* combined_chp_dis = new TH1D("combined_chp_dis", "combined_chp_dis", 26, -0.5, 25.5);
	TH1* combined_chn_dis = new TH1D("combined_chn_dis", "combined_chn_dis", 128, -0.5, 127.5);

	for(Long64_t n = 0; n < combined_tree->GetEntriesFast(); ++n)
	{
		combined_tree->GetEntry(n);

		for(std::size_t s = 0; s < combined_branches.begin()->second->size(); ++s)
		{
			//Do things
			if((*combined_branches["flx_svr"])[s] != server)continue;
			combined_chp_dis->Fill((*combined_branches["chp"])[s]);
			combined_chn_dis->Fill((*combined_branches["chn"])[s]);
		}
	}

	if(!combined_1_tree)goto JUMP_POINT;
	for(auto& itr: combined_branches)
	{
		combined_1_tree->SetBranchAddress(itr.first.c_str(), &(itr.second));
	}
	for(Long64_t n = 0; n < combined_1_tree->GetEntriesFast(); ++n)
	{
		combined_1_tree->GetEntry(n);

		for(std::size_t s = 0; s < combined_branches.begin()->second->size(); ++s)
		{
			//Do things
			if((*combined_branches["flx_svr"])[s] != server)continue;
			combined_chp_dis->Fill((*combined_branches["chp"])[s]);
			combined_chn_dis->Fill((*combined_branches["chn"])[s]);
		}
	}
	JUMP_POINT:

	TCanvas* check_chp_cnvs = new TCanvas("check_chp_cnvs", "check_chp_cnvs");
	check_chp_cnvs->cd();
	check_chp_dis->Draw();
	//...

	TCanvas* check_chn_cnvs = new TCanvas("check_chn_cnvs", "check_chn_cnvs");
	check_chn_cnvs->cd();
	check_chn_dis->Draw();
	//...


	TCanvas* combined_chp_cnvs = new TCanvas("combined_chp_cnvs", "combined_chp_cnvs");
	combined_chp_cnvs->cd();
	combined_chp_dis->Draw();
	//...

	TCanvas* combined_chn_cnvs = new TCanvas("combined_chn_cnvs", "combined_chn_cnvs");
	combined_chn_cnvs->cd();
	combined_chn_dis->Draw();
	//...


	std::cout << std::endl;
}

#endif//CHECK_COMBINED_C
