#include "InttEvent.cc"
#include "InttOfflineEvent.cc"
#include "InttOfflineCluster.cc"
#include "InttFelixMap.cc"
#include "InttMapping.cc"

TH2S *offhist;
TH2S *evthist;
TGraph *cluspoints;
TH2S *diffhist;

TH1F *integraldiffhist = new TH1F("", "", 6, -3, 3);
TCanvas *c = new TCanvas("","");

TText *evnum;

void make_offhithist(int m_layer, int m_lad_phi, int nhit_offevt);
void make_evthithist(int ievent,int m_layer, int m_lad_phi, int nhit_evt);
void make_clusgraph(int m_layer, int m_lad_phi, int nhit_offcls);
void diff_evt_offline(int layer, int lad_phi);

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

char cc[10];

int outpdf = 1; //switching pdf is 0 printed / 1 not printed
string pdfname = "/sphenix/u/mfujiwara/Workspace/tutorials/comp_hitclus/macro/compare_rawhit/histpict/run20869/diff_run20869_box.pdf";

int i=0; //number for make pdf series
int counter=0; //counter of number of event which match condition

// make 1D hist (evt - offline) intergral
void make_dis_diffint()
{
  //gStyle->SetOptStat(0);
    // gROOT->SetStyle("Plain");
    //gStyle->SetPalette(1);
    //const char *fname = "AnaTutorial.root";
  const char *fname ="/sphenix/tg/tg01/commissioning/INTT/work/mfuji/AnaTutorial.root";

    TFile *file = TFile::Open(fname);

    TTree *tree = (TTree *)file->Get("tree");

    tree->SetBranchAddress("event", &inttEvt);
    tree->SetBranchAddress("offevent", &inttOfflineEvt);
    tree->SetBranchAddress("offcluster", &inttOfflineCls);
    cout<<"event number   layer  lad_phi  evt_entries  off_entries"<<endl;

    //for (int ievent = 0; ievent < tree->GetEntries(); ievent++)
    for (int ievent = 0; ievent < 100; ievent++)
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
	      make_offhithist(layer, lad_phi, nhit_offevt);
	      make_evthithist(ievent,layer, lad_phi, nhit_evt);
	      
	      //scalehisthigh(evthist);
	      // make_clusgraph(layer, lad_phi, nhit_offcls);
	      diff_evt_offline(layer, lad_phi);
	      
	      
	      //if(diffhist->Integral() !=0){
	      if(diffhist->GetMaximum() !=0 || diffhist->GetMinimum() !=0){
		//if(layer == 4 && lad_phi ==9){
		cout<<ievent<<"  "<<layer<<"  "<<lad_phi<<"   "<<evthist->GetEntries()<<"  "<<offhist->GetEntries()<<endl;
		counter++;
		
		
		for (int chip = 0; chip < chipbin; chip++)
		    {
		      for (int chan = 0; chan < chanbin; chan++)
			{
			  diffhist->Fill(chip,chan);
			  diffhist->Fill(chip,chan);
			}
		    }
		
		  
		//c->Clear();
		  gStyle->SetPalette(1);
		  c->Divide(3,1);
		  c->cd(1);
		  evthist->Draw("COLZ");
		  c->cd(2);
		  offhist->Draw("COLZ");
		  c->cd(3);
		  diffhist->Draw("COLZ");
		  
		  if(outpdf == 1){
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
	      
	      delete offhist;
	      delete evthist;
	      //delete cluspoints;
	      delete diffhist;
	      
            }
        }
    }
    cout<<"number of event ="<<tree->GetEntries()<<endl;
    cout<<"number of event mach condition = "<<counter<<endl;
    //c->Clear();
    //gStyle->SetOptStat(1);
    //integraldiffhist->Draw();
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
        Intt::RawData_s raw;
        Intt::Offline_s ofl;
        int chip;
        int chan;

        raw.felix_server = Intt::FelixFromPacket(hit->pid);
        raw.felix_channel = hit->module;
        raw.chip = (hit->chip_id + 25) % 26;
        raw.channel = hit->chan_id;
        ofl = Intt::ToOffline(raw);


        if (ofl.layer == m_layer && ofl.ladder_phi == m_lad_phi)
        {
            chip = ofl.strip_y;
            chan = ofl.strip_x;

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
            cluspoints->SetMarkerSize(0.5);
            cluspoints->SetMarkerColor(kBlack);
            cluspoints->SetMarkerStyle(8);
        }

        /*
        else if (layer > m_layer || lad_phi > m_lad_phi)
        {
            break;
        }
        */
    }
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
