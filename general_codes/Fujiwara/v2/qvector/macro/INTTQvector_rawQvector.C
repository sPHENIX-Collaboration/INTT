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
std::vector<float> *ClusX = nullptr;
std::vector<float> *ClusY = nullptr;
std::vector<float> *ClusZ = nullptr;
std::vector<float> *ClusAdc = nullptr;

double Qx,Qx_south,Qx_north;
double Qy,Qy_south,Qy_north;
int NClus,NClus_south,NClus_north;
double Psi,Psi_south,Psi_north;
double AdcClus,AdcClus_south,AdcClus_north;

int AdcCut = 45;

void setupBranches(TTree *tree);
double calculateMean(const std::vector<float> &values);
void overwriteBranch(TTree *tree, const char *branchName);

void INTTQvector_rawQvector(string fname = "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_mbdphi_-20_20_split_full.root"
			   //"Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_mbdphi_-20_20_split.root"
			   //"Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_00000.root"
			   //"Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_100k.root"
)
{
  TFile *file = TFile::Open(fname.c_str(), "UPDATE");
  TTree *EventTree = (TTree *)file->Get("EventTree");

    setupBranches(EventTree);
    TH1F *histClusAdc = new TH1F("histClusAdc", "Cluster ADC after cut", 5000, 0, 10000);
    TH1F *histClusAdc1 = new TH1F("histClusAdc1", "Cluster ADC before cut", 5000, 0, 10000);

    overwriteBranch(EventTree, "Intt_Qx");
    overwriteBranch(EventTree, "Intt_Qy");
    overwriteBranch(EventTree, "Intt_Psi");
    overwriteBranch(EventTree, "Adcfiltered_NClus");

    overwriteBranch(EventTree, "Intt_Qx_south");
    overwriteBranch(EventTree, "Intt_Qy_south");
    overwriteBranch(EventTree, "Intt_Psi_south");
    overwriteBranch(EventTree, "Adcfiltered_NClus_south");

    overwriteBranch(EventTree, "Intt_Qx_north");
    overwriteBranch(EventTree, "Intt_Qy_north");
    overwriteBranch(EventTree, "Intt_Psi_north");
    overwriteBranch(EventTree, "Adcfiltered_NClus_north");

    TBranch *branchQx = EventTree->Branch("Intt_Qx", &Qx);
    TBranch *branchQy = EventTree->Branch("Intt_Qy", &Qy);
    TBranch *branchPsi = EventTree->Branch("Intt_Psi", &Psi);
    TBranch *brancInttNClus = EventTree->Branch("Adcfiltered_NClus", &AdcClus);

    TBranch *branchQx_south = EventTree->Branch("Intt_Qx_south", &Qx_south);
    TBranch *branchQy_south = EventTree->Branch("Intt_Qy_south", &Qy_south);
    TBranch *branchPsi_south = EventTree->Branch("Intt_Psi_south", &Psi_south);
    TBranch *brancInttNClus_south = EventTree->Branch("Adcfiltered_NClus_south", &AdcClus_south);

    TBranch *branchQx_north = EventTree->Branch("Intt_Qx_north", &Qx_north);
    TBranch *branchQy_north = EventTree->Branch("Intt_Qy_north", &Qy_north);
    TBranch *branchPsi_north = EventTree->Branch("Intt_Psi_north", &Psi_north);
    TBranch *brancInttNClus_north = EventTree->Branch("Adcfiltered_NClus_north", &AdcClus_north);

    Long64_t nEntries = EventTree->GetEntries();

    cout<<"Cut off Clusters Adc value lower "<<AdcCut<<endl;
    for (Long64_t i = 0; i < nEntries; ++i)
    {
        EventTree->GetEntry(i);
	if(i<10)
	  cout << "NClus =" << NClus << endl;

        std::vector<float> ClusCosPhi;
        std::vector<float> ClusSinPhi;

	std::vector<float> ClusCosPhi_south;
        std::vector<float> ClusSinPhi_south;
	
	std::vector<float> ClusCosPhi_north;
        std::vector<float> ClusSinPhi_north;

        for (size_t j = 0; j < ClusX->size(); ++j)
        {
	  histClusAdc1->Fill((*ClusAdc)[j]);
            if ((*ClusAdc)[j] > AdcCut)
            {
                double phi = atan2((*ClusY)[j], (*ClusX)[j]);

                float cosPhi = cos(2 * phi);
                float sinPhi = sin(2 * phi);
                ClusCosPhi.push_back(cosPhi);
                ClusSinPhi.push_back(sinPhi);

		if((*ClusZ)[j]<0)
		  {
		    ClusCosPhi_south.push_back(cosPhi);
		    ClusSinPhi_south.push_back(sinPhi);
		  }
		else if((*ClusZ)[j]>0)
		  {
		    ClusCosPhi_north.push_back(cosPhi);
		    ClusSinPhi_north.push_back(sinPhi);
		  }

                histClusAdc->Fill((*ClusAdc)[j]);
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

        AdcClus = ClusCosPhi.size();
	AdcClus_south = ClusCosPhi_south.size();
	AdcClus_north = ClusCosPhi_north.size();

        // cout << "Qx = " << Qx << endl;
        // cout << "Qy = " << Qy << endl;
	
	if(AdcClus!=0)
	  {
	    branchQx->Fill();
	    branchQy->Fill();
	    branchPsi->Fill();
	    brancInttNClus->Fill();
	  }
	else if(AdcClus==0)
	  {
	    Qx = std::nan("");
	    Qy = std::nan("");
	    Psi = std::nan("");
	    branchQx->Fill();
	    branchQy->Fill();
	    branchPsi->Fill();
	    brancInttNClus->Fill();
	    if(i<100)
	      cout<<"Event "<<i<<" is no entry"<<endl;
	  }
	
	if(AdcClus_south!=0){
	  branchQx_south->Fill();
	  branchQy_south->Fill();
	  branchPsi_south->Fill();
	  brancInttNClus_south->Fill();
	}
	else if(AdcClus_south==0)
	  {
	    Qx_south = std::nan("");
	    Qy_south = std::nan("");
	    Psi_south = std::nan("");
	    branchQx_south->Fill();
	    branchQy_south->Fill();
	    branchPsi_south->Fill();
	    brancInttNClus_south->Fill();
	  }
	
	if(AdcClus_north!=0)
	  {
	    branchQx_north->Fill();
	    branchQy_north->Fill();
	    branchPsi_north->Fill();
	    brancInttNClus_north->Fill();
	  }
	else if(AdcClus_north==0)
	  {
	    Qx_north = std::nan("");
	    Qy_north = std::nan("");
	    Psi_north = std::nan("");
	    branchQx_north->Fill();
	    branchQy_north->Fill();
	    branchPsi_north->Fill();
	    brancInttNClus_north->Fill();	  
	  }

	//cout<<"Event Number = "<<i<<"  Adcfiletered NClus = "<<AdcClus<<endl;
	//if(AdcClus != 0)
	//EventTree->Fill();
    }

    file->cd();
    histClusAdc->Write();
    histClusAdc1->Write();
    EventTree->SetBranchStatus("*", 1);
    EventTree->Write("", TObject::kOverwrite);
    file->Close();
}

void setupBranches(TTree *tree)
{
  tree->SetBranchStatus("*", 1);
  tree->SetBranchStatus("NClus", 1);
  tree->SetBranchAddress("NClus", &NClus);
  tree->SetBranchStatus("ClusX", 1);
  tree->SetBranchAddress("ClusX", &ClusX);
  tree->SetBranchStatus("ClusY", 1);
  tree->SetBranchAddress("ClusY", &ClusY);
  tree->SetBranchStatus("ClusZ", 1);
  tree->SetBranchAddress("ClusZ", &ClusZ);
  tree->SetBranchStatus("ClusAdc", 1);
  tree->SetBranchAddress("ClusAdc", &ClusAdc);
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
