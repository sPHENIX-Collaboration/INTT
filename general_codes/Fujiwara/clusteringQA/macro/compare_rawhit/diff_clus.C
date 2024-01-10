#include "InttEvent.cc"
#include "InttOfflineEvent.cc"
#include "InttOfflineCluster.cc"
//#include "InttFelixMap.cc"
//#include "InttMapping.cc"
#include <intt/InttFelixMap.h>
#include <intt/InttMapping.h>

#include <cmath>
#include <limits>
#include <iostream>

using namespace std;

R__LOAD_LIBRARY(libinttread.so)

TH2S *offhist;
TH2S *evthist;
TGraph *cluspoints;
TGraph *evtcluspoints;
TH2S *diffhist;

TH1F * h1 = new TH1F("", "minimum distance", 6000, -3, 3);
TCanvas *c = new TCanvas("","");

TText *evnum;

void make_offhithist(int m_layer, int m_lad_phi, int nhit_offevt);
void make_evthithist(int ievent,int m_layer, int m_lad_phi, int nhit_evt);
void make_clusgraph(int m_layer, int m_lad_phi, int nhit_offcls);
void make_evtclusgraph(int ievent, int m_layer, int m_lad_phi, int nhit_evt);
void diff_evt_offline(int ievent,int layer, int lad_phi, int nhit_evt);

void cal_dis_mini(); //calculate minimal distance of my cluster and offline cluster 

void scalehisthigh(TH2S *hist);

int getlad_phimax(int layer);

InttEvent *inttEvt = nullptr;
InttOfflineEvent *inttOfflineEvt = nullptr;
InttOfflineClusterList *inttOfflineCls = nullptr;

int chipbin = 26;
int chipmin = 0;
int chipmax = 26;

int chanbin = 256;
int chanmin = 0;
int chanmax = 256;

int numcls = 0;

char cc[10];

float chsum = 0;  //channel sum of next to
float clsize=0;  //nummber of channel next to
float clusch; //channel of cluster

int ev = 10; //number of event you need to scan if 0 all event scan

vector<int> veccls;

int outpdf = 1; //switching pdf is 0 printed / 1 not printed
string pdfname = "/sphenix/u/mfujiwara/Workspace/tutorials/comp_hitclus/macro/compare_rawhit/histpict/run20869/diff_run20869_myclustering.pdf";

int i=0; //number for make pdf series
int counter=0; //counter of number of event which match condition

// make 1D hist (evt - offline) intergral
void diff_clus()
{
  gStyle->SetOptStat(0);
    // gROOT->SetStyle("Plain");
    //gStyle->SetPalette(1);
    //const char *fname = "AnaTutorial.root";
  const char *fname ="/sphenix/tg/tg01/commissioning/INTT/work/mfuji/AnaTutorial.root";

    TFile *file = TFile::Open(fname);

    TTree *tree = (TTree *)file->Get("tree");

    tree->SetBranchAddress("event", &inttEvt);
    tree->SetBranchAddress("offevent", &inttOfflineEvt);
    tree->SetBranchAddress("offcluster", &inttOfflineCls);
    //cout<<"event number   layer  lad_phi  evt_entries  off_entries"<<endl;

    if(ev == 0 || ev > tree->GetEntries())
      {
	ev = tree->GetEntries();
      }

    for (int ievent = 0; ievent < ev; ievent++)
    {
        tree->GetEntry(ievent);

        int nhit_evt = inttEvt->getNHits();
        int nhit_offevt = inttOfflineEvt->getNHits();
        int nhit_offcls = inttOfflineCls->getNClusters();

        for (int layer = 3; layer < 7; layer++)
        {
            int lad_phimax = getlad_phimax(layer);

            for (int lad_phi = 0; lad_phi < lad_phimax; lad_phi++)
            {
	      
	      if(layer == 4 && lad_phi == 9){
		continue;
	      }
	      

	      make_offhithist(layer, lad_phi, nhit_offevt);
	      make_evthithist(ievent,layer, lad_phi, nhit_evt);
	      
	      make_clusgraph(layer, lad_phi, nhit_offcls);
	      make_evtclusgraph(ievent, layer, lad_phi, nhit_evt);

	      cal_dis_mini();
	      //cout<<"my clusters "<<evtcluspoints->GetN() <<"  offline clusters "<< cluspoints->GetN()<<endl;

	      //h1->Fill(evtcluspoints->GetN() - cluspoints->GetN());

	      /*
	      if(evtcluspoints->GetN() != cluspoints->GetN())
	      //if(h1->Integral(32,60) != 0)
		{
		  //diff_evt_offline(layer, lad_phi);
		  gStyle->SetPalette(1);
		  c->Divide(2,1);
		  c->cd(1);
		  evthist->Draw("COLZ");
		  evtcluspoints->Draw("P");
		  
		  c->cd(2);
		  offhist->Draw("COLZ");
		  cluspoints->Draw("P");
		  
		  if(outpdf==1){
		    c->Modified();
		    c->Update();
		    
		    cin >> cc;
		    cout << cc << endl;
		    c->Clear();
		    
		  }
		  
		  if(i==0 && outpdf == 0){
		    c->Print((pdfname+"(").c_str());
		    c->Clear();
		    //delete evnum;
		    i++;
		  }else if(outpdf == 0){
		    c->Print(pdfname.c_str());
		    c->Clear();
		    //delete evnum;
		  }
		}
	      */
	      delete offhist;
	      delete evthist;
	      delete cluspoints;
	      delete diffhist;
	      delete evtcluspoints;
	    }	   
	}
    }
    
    cout<<"number of event = "<< ev <<endl;
    cout<<"number of cluster = "<<numcls<<endl;
    //cout<<"number of event mach condition = "<<counter<<endl;

    gStyle->SetOptStat(1);
    h1->Draw();

    if(outpdf == 0){
      c->Print((pdfname+")").c_str());
    }
}

// make offline hit 2D hist chan vs chip
void make_offhithist(int m_layer, int m_lad_phi, int nhit_offevt)
{
  offhist = new TH2S(Form("offhist%d", m_lad_phi), Form("offline hits l%dphi%d", m_layer, m_lad_phi), chipbin, chipmin, chipmax, chanbin, chanmin, chanmax);
    for (int ihit = 0; ihit < nhit_offevt; ihit++)
    {
        InttOfflineHit *ohit = inttOfflineEvt->getHit(ihit);
        int layer = (ohit->hitsetkey >> 16) & 0xFF;
        int sensor = (ohit->hitsetkey >> 14) & 0x3;
        int lad_phi = (ohit->hitsetkey >> 10) & 0xF;
        int bco = (ohit->hitsetkey) & 0x3FF;
        int chip = (ohit->hitkey >> 16) & 0xFFFF;
        int chan = (ohit->hitkey) & 0xFFFF;

        if (layer == m_layer && lad_phi == m_lad_phi)
        {
            if (sensor == 0)
            {
                chip = chip + 5;
            }
            else if (sensor == 2)
            {
                chip = chip + 13;
            }
            else if (sensor == 3)
            {
                chip = chip + 21;
            }
            offhist->Fill(chip, chan);
        }
        /*
        else if (layer > m_layer || lad_phi > m_lad_phi)
        {
            break;
        }
        */
    }
}

// make evt 2D hist chan vs chip
void make_evthithist(int ievent,int m_layer, int m_lad_phi, int nhit_evt)
{
  evthist = new TH2S(Form("evthist%d", m_lad_phi), Form("event%d evt hits l%dphi%d",ievent, m_layer, m_lad_phi), chipbin, chipmin, chipmax, chanbin, chanmin, chanmax);

    for (int ihit = 0; ihit < nhit_evt; ihit++)
    {
        InttHit *hit = inttEvt->getHit(ihit);
        InttNameSpace::RawData_s raw;
        InttNameSpace::Offline_s ofl;
        int chip;
        int chan;

        raw.felix_server = InttNameSpace::FelixFromPacket(hit->pid);
        raw.felix_channel = hit->module;
        raw.chip = (hit->chip_id + 25) % 26;
        raw.channel = hit->chan_id;
        ofl = InttNameSpace::ToOffline(raw);


        if (ofl.layer == m_layer && ofl.ladder_phi == m_lad_phi)
        {
            chip = ofl.strip_y;
            chan = -ofl.strip_x + 255;

            if (ofl.ladder_z == 0)
            {
                chip = ofl.strip_y + 5;
            }
            else if (ofl.ladder_z == 2)
            {
                chip = ofl.strip_y + 13;
            }
            else if (ofl.ladder_z == 3)
            {
                chip = ofl.strip_y + 21;
            }
	    if(evthist->GetBinContent(chip+1,chan+1)==0){
            evthist->Fill(chip, chan);
	    }
        }
        /*
        else if (layer > m_layer || lad_phi > m_lad_phi)
        {
            break;
        }
        */
    }
}

// make cluster graoh chan vs chip
void make_clusgraph(int m_layer, int m_lad_phi, int nhit_offcls)
{
  cluspoints = new TGraph();
  cluspoints->SetMarkerSize(0.7);
  cluspoints->SetMarkerColor(kBlack);
  cluspoints->SetMarkerStyle(20);

    for (int ihit = 0; ihit < nhit_offcls; ihit++)
    {
        InttOfflineCluster *ocls = inttOfflineCls->getCluster(ihit);
        int layer = (ocls->hitsetkey >> 16) & 0xFF;
        int sensor = (ocls->hitsetkey >> 14) & 0x3;
        int lad_phi = (ocls->hitsetkey >> 10) & 0xF;
        int bco = (ocls->hitsetkey) & 0x3FF;
        float lx = ocls->local[0];
        float ly = ocls->local[1];
        float clus_chip;
        float clus_chan;

        if (layer == m_layer && lad_phi == m_lad_phi)
        {
            if (sensor == 0)
            {
                clus_chip = ly / 1.6 + 9;
            }
            else if (sensor == 1)
            {
                clus_chip = ly / 2.0 + 2.5;
            }
            else if (sensor == 2)
            {
                clus_chip = ly / 1.6 + 17;
            }
            else if (sensor == 3)
            {
                clus_chip = ly / 2.0 + 23.5;
            }

            clus_chan = lx / 0.0078;
            clus_chan = clus_chan + 128.0;

            cluspoints->SetPoint(cluspoints->GetN(), clus_chip, clus_chan);
	    //cout<<"layer "<<layer<<" ladphi "<<lad_phi<<" chip "<<clus_chip<<"  chan "<<clus_chan<<endl;
	   
        }

        /*
        else if (layer > m_layer || lad_phi > m_lad_phi)
        {
            break;
        }
        */
    }
    //cout<<endl;
}

// make 2D hist (evt - offline)
void diff_evt_offline(int layer, int lad_phi)
{
    diffhist = new TH2S(Form("diffhist%d", lad_phi), Form("evt - off l%dphi%d", layer, lad_phi), chipbin, chipmin, chipmax, chanbin, chanmin, chanmax);
    diffhist->Add(evthist, offhist, 1, -1);
}

// input layer, return lad_phi max in this layer
int getlad_phimax(int m_layer)
{
    int lad_phi;
    if (m_layer == 3 || m_layer == 4)
    {
        lad_phi = 12;
    }
    else if (m_layer == 5 || m_layer == 6)
    {
        lad_phi = 16;
    }

    return lad_phi;
}

// scale higher of hist bin =1
void scalehisthigh(TH2S *hist)
{
    for (int chip = 0; chip < chipbin; chip++)
    {
        for (int chan = 0; chan < chanbin; chan++)
        {
            int bin = hist->GetBinContent(chip + 1, chan + 1);
            if (bin > 1)
            {
                hist->SetBinContent(chip + 1, chan + 1, 1);
                //cout<<"chip "<<chip<<" chan "<<chan<<" evthist bin "<<hist->GetBinContent(chip+1,chan+1)<<endl;
            }
        }
    }
}

void make_evtclusgraph(int ievent, int m_layer, int m_lad_phi, int nhit_evt){
  evtcluspoints = new TGraph();
  evtcluspoints->SetMarkerSize(0.7);
  evtcluspoints->SetMarkerColor(kBlack);
  evtcluspoints->SetMarkerStyle(20);

  if(evthist==nullptr){
    make_evthithist(ievent,m_layer, m_lad_phi, nhit_evt);
  }
  
  for(int chip=0;chip<chipmax;chip++)
    {
      for(int chan=0;chan<chanmax;chan++)
	{
	  chipbin = chip+1;
	  chanbin = chan+1;
	  if(evthist->GetBinContent(chipbin,chanbin)!=0)
	    {
	      //cout<<"chan "<<chan<<endl;
	      chsum = chsum + chan;
	      clsize++;
	    }

	  if((evthist->GetBinContent(chipbin,chanbin)!=0 && evthist->GetBinContent(chipbin,chanbin+1)==0) || (chan == chanmax-1 && chsum !=0))
	    {
	      clusch = chsum/clsize;
	      //cout<<"sum of chan "<<chsum<<" number of chan next to "<<clsize<<endl;	      
	      evtcluspoints->SetPoint(evtcluspoints->GetN(), chip+0.5, clusch+0.5);
	      //cout<<"layer "<<m_layer<<" ladphi "<<m_lad_phi<<" chip "<<chip<<"  chan "<<clusch<<endl;

	      chsum = 0;
	      clsize = 0;
	    }
	}
    }  
}

void cal_dis_mini()
{
  float dis_mini;
  float offline_x;
  float offline_y;
  float evtclus_x;
  float evtclus_y;

  numcls = numcls + cluspoints->GetN();

  for(int i=0;i<cluspoints->GetN();i++)
    {
      //float dis_mini=10;
      offline_x = cluspoints->GetPointX(i);
      offline_y = cluspoints->GetPointY(i);

      for(int j=0;j<evtcluspoints->GetN();j++)
	{
	  evtclus_x = evtcluspoints->GetPointX(j);
	  evtclus_y = evtcluspoints->GetPointY(j);

	  if(j == 0||dis_mini>hypot(offline_x-evtclus_x,offline_y-evtclus_y))
	    {
	      dis_mini = hypot(offline_x-evtclus_x,offline_y-evtclus_y);
	    }
	}

      h1->Fill(dis_mini);
      if(dis_mini !=0){
	cout<<dis_mini<<endl;
      }
    }
}
