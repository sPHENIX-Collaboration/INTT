/// C++ includes
#include <cassert>
#include <sstream>
#include <string>
#include <typeinfo>
#include <iostream>
#include <numeric>

/// ROOT includes
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>

using namespace std;

//string fname = "../../F4AInttAna/macro/Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root";

//string fname = "Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root";

TFile *file;

TTree *EventTree;
//std::vector<float> *ClusX = nullptr;
//std::vector<float> *ClusY = nullptr;
//std::vector<float> *ClusZ = nullptr;
//std::vector<float> *ClusAdc = nullptr;
std::vector<float> *mbd_phi = nullptr;
std::vector<float> *mbd_pmt_q = nullptr;

double Qx,Qx_south,Qx_north;
double Qy,Qy_south,Qy_north;
//int NClus,NClus_south,NClus_north;
float Charge,Charge_south,Charge_north;
double Psi,Psi_south,Psi_north;
//double AdcClus,AdcClus_south,AdcClus_north;

//int AdcCut = 45;

void setupBranches(TTree *tree);
double calculateMean(const std::vector<float> &values);
void overwriteBranch(TTree *tree, const char *branchName);

void MbdQvector_rawQvector(string fname = //"Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_mbdphi_-20_20_split_full.root"
			   "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_00000.root"
			   )
{
  TFile *file = TFile::Open(fname.c_str(), "UPDATE");
  TTree *EventTree = (TTree *)file->Get("EventTree");

    setupBranches(EventTree);

    overwriteBranch(EventTree, "Mbd_Qx");
    overwriteBranch(EventTree, "Mbd_Qy");
    overwriteBranch(EventTree, "Mbd_Psi");

    overwriteBranch(EventTree, "Mbd_Qx_south");
    overwriteBranch(EventTree, "Mbd_Qy_south");
    overwriteBranch(EventTree, "Mbd_Psi_south");

    overwriteBranch(EventTree, "Mbd_Qx_north");
    overwriteBranch(EventTree, "Mbd_Qy_north");
    overwriteBranch(EventTree, "Mbd_Psi_north");

    TBranch *branchQx = EventTree->Branch("Mbd_Qx", &Qx);
    TBranch *branchQy = EventTree->Branch("Mbd_Qy", &Qy);
    TBranch *branchPsi = EventTree->Branch("Mbd_Psi", &Psi);

    TBranch *branchQx_south = EventTree->Branch("Mbd_Qx_south", &Qx_south);
    TBranch *branchQy_south = EventTree->Branch("Mbd_Qy_south", &Qy_south);
    TBranch *branchPsi_south = EventTree->Branch("Mbd_Psi_south", &Psi_south);

    TBranch *branchQx_north = EventTree->Branch("Mbd_Qx_north", &Qx_north);
    TBranch *branchQy_north = EventTree->Branch("Mbd_Qy_north", &Qy_north);
    TBranch *branchPsi_north = EventTree->Branch("Mbd_Psi_north", &Psi_north);

    Long64_t nEntries = EventTree->GetEntries();

    for (Long64_t i = 0; i < nEntries; ++i)
    {
        EventTree->GetEntry(i);

        std::vector<float> ClusCosPhi;
        std::vector<float> ClusSinPhi;

	std::vector<float> ClusCosPhi_south;
        std::vector<float> ClusSinPhi_south;
	
	std::vector<float> ClusCosPhi_north;
        std::vector<float> ClusSinPhi_north;

        for (size_t j = 0; j < mbd_phi->size(); ++j)
        {
	  double phi = (*mbd_phi)[j];
	  double q = (*mbd_pmt_q)[j];
	  float cosPhi = cos(2 * phi);
	  float sinPhi = sin(2 * phi);
	  
	  for(int k=0;k<q;k++){
	    ClusCosPhi.push_back(cosPhi);
	    ClusSinPhi.push_back(sinPhi);
	    if(q==0)
	      cout<<"q = 0"<<endl;
	  }
	  	  
	  if(j<64)
	    {
	      for(int k=0;k<q;k++){
		ClusCosPhi_south.push_back(cosPhi);
		ClusSinPhi_south.push_back(sinPhi);
	      }
	    }

	  if(j>=64)
	    {
	      for(int k=0;k<q;k++){
		ClusCosPhi_north.push_back(cosPhi);
		ClusSinPhi_north.push_back(sinPhi);
	      }
	    }
        }

        Qx = calculateMean(ClusCosPhi);
        Qy = calculateMean(ClusSinPhi);
        Psi = 0.5*atan2(Qy,Qx);

	Qx_south = calculateMean(ClusCosPhi_south);
        Qy_south = calculateMean(ClusSinPhi_south);
        Psi_south = 0.5*atan2(Qy_south,Qx_south);

	Qx_north = calculateMean(ClusCosPhi_north);
        Qy_north = calculateMean(ClusSinPhi_north);
        Psi_north = 0.5*atan2(Qy_north,Qx_north);

	if(Charge!=0)
	  {
	    branchQx->Fill();
	    branchQy->Fill();
	    branchPsi->Fill();
	  }
	else if(Charge==0)
	  {
	    Qx = std::nan("");
	    Qy = std::nan("");
	    Psi = std::nan("");
	    branchQx->Fill();
	    branchQy->Fill();
	    branchPsi->Fill();
	    if(i<100)
	      cout<<"Event "<<i<<" is no entry"<<endl;
	  }
	
	if(Charge_south!=0){
	  branchQx_south->Fill();
	  branchQy_south->Fill();
	  branchPsi_south->Fill();
	}
	else if(Charge_south==0)
	  {
	    Qx_south = std::nan("");
	    Qy_south = std::nan("");
	    Psi_south = std::nan("");
	    branchQx_south->Fill();
	    branchQy_south->Fill();
	    branchPsi_south->Fill();
	  }
	
	if(Charge_north!=0)
	  {
	    branchQx_north->Fill();
	    branchQy_north->Fill();
	    branchPsi_north->Fill();
	  }
	else if(Charge_north==0)
	  {
	    Qx_north = std::nan("");
	    Qy_north = std::nan("");
	    Psi_north = std::nan("");
	    branchQx_north->Fill();
	    branchQy_north->Fill();
	    branchPsi_north->Fill();
	  }

    }

    file->cd();
    EventTree->SetBranchStatus("*", 1);
    EventTree->Write("", TObject::kOverwrite);
    file->Close();
}

void setupBranches(TTree *tree)
{
  tree->SetBranchStatus("*", 0);
  tree->SetBranchStatus("MBD_phi", 1);
  tree->SetBranchAddress("MBD_phi", &mbd_phi);
  tree->SetBranchStatus("MBD_pmt_q", 1);
  tree->SetBranchAddress("MBD_pmt_q", &mbd_pmt_q);
  tree->SetBranchStatus("MBD_south_charge_sum", 1);
  tree->SetBranchAddress("MBD_south_charge_sum", &Charge_south);
  tree->SetBranchStatus("MBD_north_charge_sum", 1);
  tree->SetBranchAddress("MBD_north_charge_sum", &Charge_north);
  tree->SetBranchStatus("MBD_charge_sum", 1);
  tree->SetBranchAddress("MBD_charge_sum", &Charge);
  
  return;
}

double calculateMean(const std::vector<float> &values)
{
    if (values.empty())
        return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

void overwriteBranch(TTree *tree, const char *branchName)
{
    TBranch *branch = tree->GetBranch(branchName);
    if (branch)
    {
        tree->GetListOfBranches()->Remove(branch);
        delete branch;
    }
}
