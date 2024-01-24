#include <TH1.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <../src/InttOfflineCluster.h>

R__LOAD_LIBRARY(libinttread.so)
using namespace std;

double nclus;
double qx;
double qy;
double psi;

InttOfflineClusterList *inttOfflineCls = nullptr;

void INTTQvector_v2()
{
  string fname ="rootfile/InttQvector_adccut.root";
  const Int_t nq = 20;

  string ofname ="rootfile/Intt_v2prof.root";
  TFile*ofile = new TFile(ofname.c_str(), "recreate");

  TFile *file = TFile::Open(fname.c_str());
    
  TTree *tree_ = (TTree *)file->Get("Qvectortree");

  string pdfname = "pdffile/"+fname.substr(9,18)+to_string(nq)+"v2.pdf";

  TTree *itree = (TTree *)file->Get("InttOfflinetree");

  itree->SetBranchAddress("offcluster", &inttOfflineCls);

  tree_->SetBranchAddress("nclus",&nclus);
  tree_->SetBranchAddress("Qx",&qx);
  tree_->SetBranchAddress("Qy",&qy);
  tree_->SetBranchAddress("psi",&psi);
  
  double nclusmax = tree_->GetMaximum("nclus");
 
  TProfile *prof = new TProfile("v2_multi","v2 vs multiplicity",20,0,100);

  for (int ievent = 0; ievent < itree->GetEntries(); ievent++)
    //for (int ievent = 0; ievent < 10000; ievent++)
    {
      //cout<<"evt number  "<<ievent<<endl;
      itree->GetEntry(ievent);
      tree_->GetEntry(ievent);

      int nhit_offcls = inttOfflineCls->getNClusters();
	
	for (int ihit = 0; ihit < nhit_offcls; ihit++)
	  {
	    InttOfflineCluster*ocls = inttOfflineCls->getCluster(ihit);
	    
	    double phi = atan2(ocls->global[1],ocls->global[0]);
	    double v2 = cos(2*(psi-phi));
	   
	    //cout<<"mult/multmax ="<<nclus/nclusmax*100<<endl;
	    //cout<<"v2 ="<<v2<<endl;
	    
	    prof->Fill(double(nclus/nclusmax*100),v2);
      	  }
    }

  prof->Draw();
  
  ofile->Add(prof);

  ofile->Write();
  ofile->Close();
  

  //prof->Draw();
      
}
