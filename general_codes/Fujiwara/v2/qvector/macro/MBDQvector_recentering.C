#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <TProfile.h>
#include <iostream>
#include <string>

using namespace std;


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
//R__LOAD_LIBRARY(libinttqvector.so)
//R__LOAD_LIBRARY(libinttread.so)

// how many seperate area by multiplicity
const Int_t nq = 20;

TFile *ifile;
TTree *itree;

TFile *outputFile;
string outputFilename;

float Charge,Charge_south,Charge_north;

double qx;
double qy;
double psi;

double qx_rec;
double qy_rec;
double psi_rec;

double nclus_south;
double qx_south;
double qy_south;
double psi_south;

double qx_rec_south;
double qy_rec_south;
double psi_rec_south;

double nclus_north;
double qx_north;
double qy_north;
double psi_north;

double qx_rec_north;
double qy_rec_north;
double psi_rec_north;

int nmax;
int qxmax;
int qymax;
int psimax;

int qxmin;
int qymin;
int psimin;

int qxbin;
int qybin;
int psibin;

// width of each histogram
double nbinwid = 10;
double qxbinwid = 0.01;
double qybinwid = 0.01;
double psibinwid = 0.02;

//Double_t xq[nq];
//Double_t yq[nq];

TH1F *nclush;
TH2S *allqvec;
TH1F *allpsi;

TH1F *nclush_south;
TH2S *allqvec_south;
TH1F *allpsi_south;

TH1F *nclush_north;
TH2S *allqvec_north;
TH1F *allpsi_north;

TH2S *allqvec_rec;
TH1F *allpsi_rec;

TH2S *allqvec_rec_south;
TH1F *allpsi_rec_south;

TH2S *allqvec_rec_north;
TH1F *allpsi_rec_north;

void setBranches(TTree*itree);
void overwriteBranch(TTree *tree, const char *branchName);
void setHistsBinRange(TTree*itree);
void newHists();
void ClearHists();
void newQuantaileHists();
void newProfs();
void cal_rec(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec);
void cal_rec_south(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec);
void cal_rec_north(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec);
void FillQvector(TTree*itree);

void MBDQvector_recentering(  
			      string fname = "Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged_mbdphi_-20_20_split_full.root"
)
{
  TFile *ifile = TFile::Open(fname.c_str(), "UPDATE");

  itree = (TTree *)ifile->Get("EventTree");
  cout<<"EventTree Name "<<itree->GetName()<<endl;
      
  itree->SetBranchStatus("*", 0);
  
  setBranches(itree);	  
  setHistsBinRange(itree);
  FillQvector(itree);  
  
  itree->SetBranchStatus("*", 1);
  itree->Write("EventTree", TObject::kOverwrite);
  delete itree;

  for(int i=0;i<nq;i++)
    {
      itree = (TTree *)ifile->Get(Form("EventTree_%d", i));
      cout<<"EventTree Name "<<itree->GetName()<<endl;
      
      itree->SetBranchStatus("*", 0);
      
      setBranches(itree);	  
      setHistsBinRange(itree);
      FillQvector(itree);

 
      itree->SetBranchStatus("*", 1);
      itree->Write(Form("EventTree_%d", i), TObject::kOverwrite);
      delete itree;
    }

    ifile->Close();

}

void setBranches(TTree*itree)
{
  itree->SetBranchStatus("MBD_charge_sum", 1);
  itree->SetBranchAddress("MBD_charge_sum", &Charge);
  itree->SetBranchStatus("Mbd_Qx", 1);
  itree->SetBranchAddress("Mbd_Qx", &qx);
  itree->SetBranchStatus("Mbd_Qy", 1);
  itree->SetBranchAddress("Mbd_Qy", &qy);
  itree->SetBranchStatus("Mbd_Psi", 1);
  itree->SetBranchAddress("Mbd_Psi", &psi);
  
  itree->SetBranchStatus("Mbd_Qx_south", 1);
  itree->SetBranchAddress("Mbd_Qx_south", &qx_south);
  itree->SetBranchStatus("Mbd_Qy_south", 1);
  itree->SetBranchAddress("Mbd_Qy_south", &qy_south);
  itree->SetBranchStatus("Mbd_Psi_south", 1);
  itree->SetBranchAddress("Mbd_Psi_south", &psi_south);
  itree->SetBranchStatus("MBD_south_charge_sum", 1);
  itree->SetBranchAddress("MBD_south_charge_sum", &Charge_south);
  
  itree->SetBranchStatus("Mbd_Qx_north", 1);
  itree->SetBranchAddress("Mbd_Qx_north", &qx_north);
  itree->SetBranchStatus("Mbd_Qy_north", 1);
  itree->SetBranchAddress("Mbd_Qy_north", &qy_north);
  itree->SetBranchStatus("Mbd_Psi_north", 1);
  itree->SetBranchAddress("Mbd_Psi_north", &psi_north);
  itree->SetBranchStatus("MBD_north_charge_sum", 1);
  itree->SetBranchAddress("MBD_north_charge_sum", &Charge_north);
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

void setHistsBinRange(TTree*itree)
{
    qxmax = 1;
    qxmin = -1;
    cout << "qx range = " << qxmax << "  " << qxmin << endl;

    qymax = 1;
    qymin = -1;
    cout << "qy range = " << qymax << "  " << qymin << endl;

    psimax = 2;
    psimin = -2;
    cout << "psi range = " << psimax << "  " << psimin << endl;

    qxbin = (qxmax + abs(qxmin)) / qxbinwid;
    qybin = (qymax + abs(qymin)) / qybinwid;
    psibin = (psimax + abs(psimin)) / psibinwid;
    cout << "qx qy psibin " << qxbin << "  " << qybin << "  " << psibin << endl;

    if (qxbin != qybin)
    {
        int maxbin = std::max(qxbin, qybin);
        qxmax = qymax = (qxbin > qybin) ? qxmax : qymax;
        qxmin = qymin = (qxbin > qybin) ? qxmin : qymin;
        qxbin = qybin = maxbin;
    }
}

void newHists()
{
    // new Qvector all, qvector recentering all, NClus, psi all, psi recentering all hist
    allqvec = new TH2S("allqvec", "all qvector", qxbin, qxmin, qxmax, qybin, qymin, qymax);
    allqvec_rec = new TH2S("allqvec_rec", "all qvector recentering", 20 / qxbinwid, -10, 10, 20 / qybinwid, -10, 10);

    allpsi = new TH1F("allpsi", "all psi", psibin, psimin, psimax);
    allpsi_rec = (TH1F *)allpsi->Clone("allpsi_rec");

    allqvec_south = new TH2S("allqvec_south", "all qvector", qxbin, qxmin, qxmax, qybin, qymin, qymax);
    allqvec_rec_south = new TH2S("allqvec_rec_south", "all qvector recentering", 20 / qxbinwid, -10, 10, 20 / qybinwid, -10, 10);

    allpsi_south = new TH1F("allpsi_south", "all psi", psibin, psimin, psimax);
    allpsi_rec_south = (TH1F *)allpsi->Clone("allpsi_rec_south");

    allqvec_north = new TH2S("allqvec_north", "all qvector", qxbin, qxmin, qxmax, qybin, qymin, qymax);
    allqvec_rec_north = new TH2S("allqvec_rec_north", "all qvector recentering", 20 / qxbinwid, -10, 10, 20 / qybinwid, -10, 10);

    allpsi_north = new TH1F("allpsi_north", "all psi", psibin, psimin, psimax);
    allpsi_rec_north = (TH1F *)allpsi->Clone("allpsi_rec_north");
}

void cal_rec(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec)
{
    qx_rec = (qx - allqvec->GetMean(1)) / allqvec->GetStdDev(1);
    qy_rec = (qy - allqvec->GetMean(2)) / allqvec->GetStdDev(2);

    allqvec_rec->Fill(qx_rec, qy_rec);

    psi_rec = 0.5 * atan2(qy_rec, qx_rec);

    allpsi_rec->Fill(psi_rec);
}

void cal_rec_south(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec)
{
    qx_rec_south = (qx_south - allqvec->GetMean(1)) / allqvec->GetStdDev(1);
    qy_rec_south = (qy_south - allqvec->GetMean(2)) / allqvec->GetStdDev(2);

    allqvec_rec->Fill(qx_rec_south, qy_rec_south);

    psi_rec_south = 0.5 * atan2(qy_rec_south, qx_rec_south);

    allpsi_rec->Fill(psi_rec_south);
}

void cal_rec_north(TH2S *allqvec, TH2S *allqvec_rec, TH1F *allpsi_rec)
{
    qx_rec_north = (qx_north - allqvec->GetMean(1)) / allqvec->GetStdDev(1);
    qy_rec_north = (qy_north - allqvec->GetMean(2)) / allqvec->GetStdDev(2);

    allqvec_rec->Fill(qx_rec_north, qy_rec_north);

    psi_rec_north = 0.5 * atan2(qy_rec_north, qx_rec_north);

    allpsi_rec->Fill(psi_rec_north);
}

void FillQvector(TTree*itree)
{
    TBranch *qxrb;
    TBranch *qyrb;
    TBranch *psirb;

    TBranch *qxrb_south;
    TBranch *qyrb_south;
    TBranch *psirb_south;

    TBranch *qxrb_north;
    TBranch *qyrb_north;
    TBranch *psirb_north;

    overwriteBranch(itree, "Mbd_Qx_rec");
    overwriteBranch(itree, "Mbd_Qy_rec");
    overwriteBranch(itree, "Mbd_psi_rec");
    
    qxrb = itree->Branch("Mbd_Qx_rec", &qx_rec);
    qyrb = itree->Branch("Mbd_Qy_rec", &qy_rec);
    psirb = itree->Branch("Mbd_psi_rec", &psi_rec);
    
    overwriteBranch(itree, "Mbd_Qx_rec_south");
    overwriteBranch(itree, "Mbd_Qy_rec_south");
    overwriteBranch(itree, "Mbd_psi_rec_south");
    
    qxrb_south = itree->Branch("Mbd_Qx_rec_south", &qx_rec_south);
    qyrb_south = itree->Branch("Mbd_Qy_rec_south", &qy_rec_south);
    psirb_south = itree->Branch("Mbd_psi_rec_south", &psi_rec_south);

    overwriteBranch(itree, "Mbd_Qx_rec_north");
    overwriteBranch(itree, "Mbd_Qy_rec_north");
    overwriteBranch(itree, "Mbd_psi_rec_north");
	  
    qxrb_north = itree->Branch("Mbd_Qx_rec_north", &qx_rec_north);
    qyrb_north = itree->Branch("Mbd_Qy_rec_north", &qy_rec_north);
    psirb_north = itree->Branch("Mbd_psi_rec_north", &psi_rec_north);

    newHists();

    // Fill all Qvector, NClus, Psi, QxQyProf
    int Entries = itree->GetEntries();

    for (int i = 0; i < Entries; i++)
    {
        itree->GetEntry(i);

	if (Charge != 0){
        allpsi->Fill(psi);
        allqvec->Fill(qx, qy);
	}
	
	if (Charge_south != 0){
        allpsi_south->Fill(psi_south);
        allqvec_south->Fill(qx_south, qy_south);
	}
	
	if (Charge_north != 0){
        allpsi_north->Fill(psi_north);
        allqvec_north->Fill(qx_north, qy_north);
	}
    }


    // recentering qx,qy using all event
    for (int i = 0; i < Entries; i++)
    {
        itree->GetEntry(i);

	 if (Charge == 0)
        {
            qx_rec = std::nan("");
            qy_rec = std::nan("");
            psi_rec = std::nan("");
	    
	    qxrb->Fill();
	    qyrb->Fill();
	    psirb->Fill();	    
        }

	 if (Charge_south == 0)
        {
            qx_rec_south = std::nan("");
            qy_rec_south = std::nan("");
            psi_rec_south = std::nan("");
	    
	    qxrb_south->Fill();
	    qyrb_south->Fill();
	    psirb_south->Fill();	    
	    
        }

	 if (Charge_north == 0)
        {
            qx_rec_north = std::nan("");
            qy_rec_north = std::nan("");
            psi_rec_north = std::nan("");
	    
	    qxrb_north->Fill();
	    qyrb_north->Fill();
	    psirb_north->Fill();	    
	    
        }


	 if(Charge!=0)
	   {
	     cal_rec(allqvec, allqvec_rec, allpsi_rec);
	     qxrb->Fill();
	     qyrb->Fill();
	     psirb->Fill();
	   }
	 
	 if(Charge_south!=0)
	   {
	     cal_rec_south(allqvec_south, allqvec_rec_south, allpsi_rec_south);
	     qxrb_south->Fill();
	     qyrb_south->Fill();
	     psirb_south->Fill();
	   }

	 if(Charge_north!=0)
	   {
	     cal_rec_north(allqvec_north, allqvec_rec_north, allpsi_rec_north);
	     qxrb_north->Fill();
	     qyrb_north->Fill();
	     psirb_north->Fill();
	   }
    }

    ClearHists();
}

void ClearHists()
{
  delete allqvec;
  delete allpsi;

  delete allqvec_south;
  delete allpsi_south;

  delete allqvec_north;
  delete allpsi_north;

  delete allqvec_rec;
  delete allpsi_rec;
  
  delete allqvec_rec_south;
  delete allpsi_rec_south;
  
  delete allqvec_rec_north;
  delete allpsi_rec_north;
}
