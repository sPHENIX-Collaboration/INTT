#include <TH1.h>
#include <TTree.h>
#include <TFile.h>
#include <math.h>

#include <../src/InttOfflineCluster.h>

R__LOAD_LIBRARY(libinttread.so)
using namespace std;

double nclus;
double qx,qy;
double psi;
InttOfflineClusterList *inttOfflineCls = nullptr;

void INTTQvector_AdcCut()
{
  string ifname ="rootfile/InttQvector20869_20240115.root";

  TFile *ifile = TFile::Open(ifname.c_str());
    
  TTree *itree = (TTree *)ifile->Get("InttOfflinetree");

  itree->SetBranchAddress("offcluster", &inttOfflineCls);

  string ofname = "rootfile/InttQvector_adccut.root";

  TFile*ofile = new TFile(ofname.c_str(), "recreate");
  
  TTree*otree = new TTree("Qvectortree", "Qvector tree");

  otree->Branch("nclus",&nclus);
  otree->Branch("Qx",&qx);
  otree->Branch("Qy",&qy);
  otree->Branch("psi",&psi);

  TH1F*psidis = new TH1F("psidis","psi all",120,-3,3);
  TH2S*qvecdis = new TH2S("qvecdis","q all",1000,-5,5,1000,-5,5);

  TH1F*adcdis = new TH1F("adcdis","adc all",4000,0,20000);

  cout << itree->GetEntries()<<endl;

  for (int ievent = 0; ievent < itree->GetEntries(); ievent++)
    //for (int ievent = 0; ievent < 10000; ievent++)
    {
      //cout<<"evt number  "<<ievent<<endl;
      itree->GetEntry(ievent);

        int nhit_offcls = inttOfflineCls->getNClusters();
	double phi;
	double count=0;
	double sumx = 0;
	double sumy = 0; 

	for (int ihit = 0; ihit < nhit_offcls; ihit++)
	  {
	    InttOfflineCluster*ocls = inttOfflineCls->getCluster(ihit);
	    int Adc = ocls->adc;
	    
	    if(Adc>45)
	      {
		//int cluskey = ocls->hitsetkey;
		//int cluster = ocls->clusterid;

		phi = atan2(ocls->global[1],ocls->global[0]);
		sumx = sumx + cos(2*phi);
		sumy = sumy + sin(2*phi);
		count++;

		adcdis->Fill(Adc);
	      }
	  }
	
	qx = sumx/count;
	qy = sumy/count;
	nclus = count;
	psi = 0.5*atan2(qx,qy);
	otree->Fill();
	psidis->Fill(psi);
	qvecdis->Fill(qx,qy);
    }

   if(ofile!=nullptr){
    ofile->Write();
    ofile->Close();
  }
   
}
