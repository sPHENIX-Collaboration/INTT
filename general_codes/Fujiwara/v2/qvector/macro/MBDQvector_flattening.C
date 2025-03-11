#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <TProfile.h>
#include <iostream>
#include <string>
#include <numeric>
#include <TCanvas.h>
#include <TF1.h>

using namespace std;


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
// R__LOAD_LIBRARY(libinttqvector.so)
//R__LOAD_LIBRARY(libinttread.so)

const Int_t nq = 20;
TFile *ifile;
TTree *itree;

TFile *outputFile;
string outputFilename;

double psi_rec,psi_rec_south,psi_rec_north;
double psi_flat,psi_flat_south,psi_flat_north;

TProfile *cosprof;
TProfile *sinprof;

TProfile *cosprof_south;
TProfile *sinprof_south;

TProfile *cosprof_north;
TProfile *sinprof_north;

void overwriteBranch(TTree *tree, const char *branchName);
void setBranches();
void flattening(TTree*itree, int flat_coef);

void CleanProf();

double cal_sin2k_psi(int k,double psi_rec);
double cal_cos2k_psi(int k,double psi_rec);
double cal_sin2k_psi_avg(int k,double psi_rec);
double cal_cos2k_psi_avg(int k,double psi_rec);

void MBDQvector_flattening(
string fname = "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_mbdphi_-20_20_split_full.root"
, int flat_coef = 8 
)
{
    // read ttree
    ifile = new TFile(fname.c_str(), "UPDATE");
    itree = ifile->Get<TTree>("EventTree");
    itree->SetBranchStatus("*", 0);

    flattening(itree,flat_coef);

    itree->SetBranchStatus("*", 1);
    itree->Write("EventTree", TObject::kOverwrite);
    delete itree;

    for(int i=0;i<nq;i++)
    {
      itree = (TTree *)ifile->Get(Form("EventTree_%d", i));
      cout<<"EventTree_"<<i<<endl;
      itree->SetBranchStatus("*", 0);

      flattening(itree,flat_coef);

      itree->SetBranchStatus("*", 1);
      itree->Write(Form("EventTree_%d", i), TObject::kOverwrite);
      delete itree;
    }
    
    ifile->Close();    
}

void setBranches()
{
    itree->SetBranchStatus("*", 0);
    
    itree->SetBranchStatus("Mbd_psi_rec", 1);
    itree->SetBranchAddress("Mbd_psi_rec", &psi_rec);
    
    itree->SetBranchStatus("Mbd_psi_rec_south", 1);
    itree->SetBranchAddress("Mbd_psi_rec_south", &psi_rec_south);
    
    itree->SetBranchStatus("Mbd_psi_rec_north", 1);
    itree->SetBranchAddress("Mbd_psi_rec_north", &psi_rec_north);
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

double cal_sin2k_psi(int k,double psi_rec)
{
    // if(isnan(psi_rec)) return;
    // double sin2k_psi = sin(2 * k * psi_rec);
    return sin(2 * k * psi_rec);
}

double cal_cos2k_psi(int k,double psi_rec)
{
    // if(isnan(psi_rec)) return;
    // double cos2k_psi = cos(2 * k * psi_rec);
    return cos(2 * k * psi_rec);
}

double cal_sin2k_psi_avg(int k,double psi_rec)
{
    vector<double> v;
    Long64_t nEvt = itree->GetEntries();

    for (int i; i < nEvt; i++)
    {
        itree->GetEntry(i);
        if (isnan(psi_rec))
            continue;
        // double sin2k_psi = cal_sin2k_psi(k);
        v.push_back(cal_sin2k_psi(k,psi_rec));
    }
    // cout<<"v size "<<v.size()<<endl;
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double cal_cos2k_psi_avg(int k,double psi_rec)
{
    vector<double> v;
    Long64_t nEvt = itree->GetEntries();

    for (int i; i < nEvt; i++)
    {
        itree->GetEntry(i);
        if (isnan(psi_rec)){
	  //cout<<"Event "<<i<<" ,psi_rec is nan"<<endl;
            continue;
	}
        // double cos2k_psi = cal_cos2k_psi(k);
        v.push_back(cal_cos2k_psi(k,psi_rec));
    }

    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

void flattening(TTree*itree, int flat_coef)
{
  setBranches();
  TBranch *psifb;
  TBranch *psifb_south;
  TBranch *psifb_north;
  
  overwriteBranch(itree, "Mbd_psi_flat");
  psifb = itree->Branch("Mbd_psi_flat", &psi_flat);
  
  overwriteBranch(itree, "Mbd_psi_flat_south");
  psifb_south = itree->Branch("Mbd_psi_flat_south", &psi_flat_south);
  
  overwriteBranch(itree, "Mbd_psi_flat_north");
  psifb_north = itree->Branch("Mbd_psi_flat_north", &psi_flat_north);

  double delta_psi;
  double delta_psi_south;
  double delta_psi_north;
  
  Long64_t nEvt = itree->GetEntries();
  // cout << "nEvt = " << nEvt << endl;
  cosprof = new TProfile("cosavg", "", flat_coef, 0, flat_coef);
  sinprof = new TProfile("sinavg", "", flat_coef, 0, flat_coef);
  
  cosprof_south = new TProfile("cosavg_south", "", flat_coef, 0, flat_coef);
  sinprof_south = new TProfile("sinavg_south", "", flat_coef, 0, flat_coef);
  
  cosprof_north = new TProfile("cosavg_north", "", flat_coef, 0, flat_coef);
  sinprof_north = new TProfile("sinavg_north", "", flat_coef, 0, flat_coef);

  
  for (int k = 1; k < flat_coef; k++)
    {
      for (int i = 0; i < nEvt; i++)
        {
	  itree->GetEntry(i);
	  if (isnan(psi_rec))
	    continue;
	  cosprof->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec));
	  sinprof->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec));
	  
	  if(!isnan(psi_rec_south))
	    {
	      cosprof_south->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec_south));
	      sinprof_south->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec_south));
	    }
	  
	  if(!isnan(psi_rec_north))
	    {
		cosprof_north->Fill(k - 0.5, cal_cos2k_psi(k,psi_rec_north));
		sinprof_north->Fill(k - 0.5, cal_sin2k_psi(k,psi_rec_north));
	    }
        }
    }
  
     // calculate delta_psi and psi_flat
    for (int i = 0; i < nEvt; i++)
    {
        itree->GetEntry(i);

        delta_psi = 0;
        psi_flat = 0;

	delta_psi_south = 0;
        psi_flat_south = 0;

	delta_psi_north = 0;
        psi_flat_north = 0;

        if (isnan(psi_rec)){
	  psi_flat=std::nan("");
	  psi_flat_south=std::nan("");
	  psi_flat_north=std::nan("");
	  //cout<<"Event "<<i<<" ,psi_rec is nan"<<endl;
	  psifb->Fill();
	  psifb_south->Fill();
	  psifb_north->Fill();
	  continue;
	}

        for (int k = 1; k < flat_coef; k++)
        {
  	  double cossin = cosprof->GetBinContent(k) * cal_sin2k_psi(k,psi_rec);
	  double sincos = sinprof->GetBinContent(k) * cal_cos2k_psi(k,psi_rec); 
	  delta_psi += 2 * (cossin - sincos) / k;

	  double cossin_south = cosprof_south->GetBinContent(k) * cal_sin2k_psi(k,psi_rec_south);
	  double sincos_south = sinprof_south->GetBinContent(k) * cal_cos2k_psi(k,psi_rec_south);
	  delta_psi_south += 2 * (cossin_south - sincos_south) / k;
		 
	  double cossin_north = cosprof_north->GetBinContent(k) * cal_sin2k_psi(k,psi_rec_north);
	  double sincos_north = sinprof_north->GetBinContent(k) * cal_cos2k_psi(k,psi_rec_north);
	  delta_psi_north += 2 * (cossin_north - sincos_north) / k;
	  
	}

	psi_flat = psi_rec + delta_psi/2;

	if(isnan(psi_rec_south))
	  {
	    psi_flat_south=std::nan("");
	    psifb_south->Fill();
	  }
	else	  
	  {
	    psi_flat_south = psi_rec_south + delta_psi_south/2;
	    psifb_south->Fill();
	  }
	
	if(isnan(psi_rec_north))
	  {
	    psi_flat_north=std::nan("");
	    psifb_north->Fill();
	  }
	else
	  {
	    psi_flat_north = psi_rec_north + delta_psi_north/2;
	    psifb_north->Fill();
	  }
	
        psifb->Fill();

	if(i<5)
	  {
	    cout<<"psi flat = "<<psi_flat<<endl;
	    cout<<"psi flat south = "<<psi_flat_south<<endl;
	    cout<<"psi flat north = "<<psi_flat_north<<endl;
	  }
    }
    CleanProf();
}

void CleanProf()
{
  delete cosprof;
  delete sinprof;

  delete cosprof_south;
  delete sinprof_south;

  delete cosprof_north;
  delete sinprof_north;
}
