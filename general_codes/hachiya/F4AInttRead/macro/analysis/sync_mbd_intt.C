#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TVector3.h>
#include <TNtuple.h>

#include "mbdtree.C"
#include "inttcls.C"

#include <iostream>
#include <iomanip>

struct cluster {
    int      evt;
    uint64_t bco;

    double x,y,z;
    double adc;
    double size;
    int    layer;

    void set(int Evt, uint64_t Bco, double X, double Y, double Z, double Adc, double Size, int Layer){
        evt    = Evt;
        bco    = Bco;
        x      = X;
        y      = Y;
        z      = Z;
        adc    = Adc;
        size   = Size;
        layer  = Layer;
    }
};

struct clustEvent {
    int      evt;
    uint64_t bco;
    vector<cluster> vclus;
};

struct ClustInfo {
    int layer;
    double adc;
    TVector3 pos;
};


TH1*  h_zvtxseed_ = nullptr;

double calculateZvertex(
       clustEvent& vCluster,
       double   xbeam=0.0,  double  ybeam=0.0, 
       double*  zcenter=NULL, double* zrms=NULL, double* zmean=NULL,
       TNtuple* ntp=NULL)
{
    vector<ClustInfo> clusters[8][2];

    // vertex calculation
    // sort cluster into inner layer group and outer layer group
    int nclus2=0;
    for(auto itrC=vCluster.vclus.begin(); itrC!=vCluster.vclus.end(); ++itrC){
      //cluster* cls = itrC;    
      ClustInfo info;
      info.layer   = itrC->layer;
      info.adc     = itrC->adc;
      info.pos = TVector3(itrC->x, itrC->y, itrC->z);

      double phi = atan2(itrC->y, itrC->x);
      //int iphi = (phi + TMath::Pi())/TMath::PiOver4();
      int iphi = 0;
      if(iphi<0)  iphi+=8;
      iphi%=8;
      //cout<<"phi : "<<phi<<" "<<iphi<<endl;

      int inout = (info.layer<2) ? 0 : 1;

      clusters[iphi][inout].push_back(info);

      if(itrC->adc>40) nclus2++;
    }


    int nTotal = vCluster.vclus.size();
    float ntpval2[20];
    TVector3 beamspot   = TVector3(xbeam,ybeam,0);
    vector<double> vz_array;

    for(int iphi=0; iphi<8; iphi++){
        for(auto c1=clusters[iphi][0].begin(); c1!=clusters[iphi][0].end(); ++c1) // inner
        {
            for(auto c2=clusters[iphi][1].begin(); c2!=clusters[iphi][1].end(); ++c2) // outer
            {
                if(c1->adc<40 || c2->adc<40) continue;

                TVector3 p1 = c1->pos - beamspot;
                TVector3 p2 = c2->pos - beamspot;
                // skip bad compbination
                double p1_ang = atan2(p1.y(), p1.x());
                double p2_ang = atan2(p2.y(), p2.x());
                double d_ang = p2_ang - p1_ang;

                //if(fabs(d_ang)>0.2 ) continue;

                TVector3 u = p2 - p1;
                double unorm = sqrt(u.x()*u.x()+u.y()*u.y());
                if(unorm<0.00001) continue;



                // unit vector in 2D
                double ux = u.x()/unorm;
                double uy = u.y()/unorm;
                double uz = u.z()/unorm; // same normalization factor(2D) is multiplied

                TVector3 p0   = beamspot - p1;

                double dca_p0 = p0.x()*uy - p0.y()*ux; // cross product of p0 x u
                double len_p0 = p0.x()*ux + p0.y()*uy; // dot product of p0 . u


                // beam center in X-Y plane
                double vx = len_p0*ux + p1.x();
                double vy = len_p0*uy + p1.y();

                double vz = len_p0*uz + p1.z();

                //if(unorm>4.5||d_ang<-0.005*3.1415||0.005*3.14
                //if(unorm>4.5||d_ang<-0.25*3.1415||0.25*3.1415<d_ang)
                //if(fabs(d_ang)<0.1 && fabs(dca_p0) < 1.0)
                if(fabs(d_ang)<0.05 && fabs(dca_p0) < 1.0)
                {
                    h_zvtxseed_->Fill(vz);
                    vz_array.push_back(vz);
                }

                //h_dca2d_zero->Fill(dca_p0);
                //h2_dca2d_zero->Fill(dca_p0, nCluster);
                //h2_dca2d_len->Fill(dca_p0, len_p0);
                //cout<<"dca : "<<dca_p0<<endl;
                //
                if(nTotal<300) 
                {
                    ntpval2[0]  = nTotal;
                    ntpval2[1]  = nclus2;
                    ntpval2[2]  = vCluster.bco&0xFFFF;
                    ntpval2[3]  = vCluster.evt;
                    ntpval2[4]  = p1_ang;
                    ntpval2[5]  = p2_ang;
                    ntpval2[6]  = p1.z();
                    ntpval2[7]  = p2.z();
                    ntpval2[8]  = c1->adc;
                    ntpval2[9]  = c2->adc;
                    ntpval2[10] = dca_p0;
                    ntpval2[11] = len_p0;
                    ntpval2[12] = unorm;
                    ntpval2[13] = c1->layer;
                    ntpval2[14] = c2->layer;
                    ntpval2[15] = vx;
                    ntpval2[16] = vy;
                    ntpval2[17] = vz;
                    ntpval2[18] = -999.; //mbdt.bz;
                    if(ntp!=NULL) ntp->Fill(ntpval2);
                }
            }
        }
    }

    // calculate trancated mean of DCA~Z histogram as Z-vertex position
    double zvtx=-9999.;

    double zcenter1 = -9999.;
    double zmean1   = -9999.;
    double zrms1    = -9999.;
    if(vz_array.size()>3){
        double zbin    = h_zvtxseed_->GetMaximumBin();
        zcenter1       = h_zvtxseed_->GetBinCenter(zbin);
        zmean1         = h_zvtxseed_->GetMean();
        zrms1          = h_zvtxseed_->GetRMS();
        if(zrms1<20) zrms1 = 20;

        double zmax = zcenter1 + zrms1; // 1 sigma
        double zmin = zcenter1 - zrms1; // 1 sigma

        double zsum=0.;
        int    zcount=0;
        for(auto iz=vz_array.begin(); iz!=vz_array.end(); ++iz){
            double vz = (*iz);
            if(zmin<vz&&vz<zmax){
                zsum+=vz;
                zcount++;
            }
        }
        if(zcount>0) zvtx = zsum/zcount;

        cout<<"ZVTX: "<<zvtx<<" "<<zcenter1<<" "<<zmean1<<" "<<zrms1<<" "<<zbin<<endl; //" "<<mbdt.bz<<endl;
    }

    if(zcenter!=NULL) *zcenter = zcenter1;
    if(zrms   !=NULL) *zrms    = zrms1;
    if(zmean  !=NULL) *zmean   = zmean1;

    return zvtx;
}


void sync_mbd_intt()
{

  double xbeam_ = 0.0;
  double ybeam_ = 0.0;


  TDirectory* gDir = gDirectory;
  ///////////
  //

  //TFile *f_mbd  = TFile::Open("/sphenix/user/chiu/sphenix_bbc/run2023/beam_mbd-00020708-0000_mbd.root");
  TFile *f_mbd  = TFile::Open("/sphenix/user/chiu/sphenix_bbc/run2023/goodruns/beam_seb18-00020869-0000_mbd.root");
  gDirectory=gDir;

  TTree *t_mbd = (TTree*)f_mbd->Get("t");
  cout<<" "<<t_mbd<<endl;
  if(!t_mbd) return ;

  mbdtree mbdt(t_mbd);

  ///////////////////
  //TFile *f_intt = TFile::Open("/gpfs/mnt/gpfs02/sphenix/user/hachiya/tutorial/tutorials/AnaTutorial/macro/AnaTutorial.root");
  //TFile *f_intt = TFile::Open("/gpfs/mnt/gpfs02/sphenix/user/hachiya/tutorial/tutorials/AnaTutorial/macro/AnaTutorial_run20708_100k.root");
  TFile *f_intt = TFile::Open("/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/F4AInttRead/macro2/AnaTutorial_run20869.root");
  gDirectory=gDir;
  TTree *t_intt = (TTree*)f_intt->Get("ntp_clus");
  cout<<" "<<t_intt<<endl;
  if(!t_intt) return ;

  inttcls inttcl(t_intt);


  cout<<t_mbd->GetEntries()<<" "<<t_intt->GetEntries()<<endl;
  //cout<<t_mbd->GetEntries()<<endl;
  //
  
  gDirectory = gDir;

  TFile *froot = new TFile("sync_run20869.root", "recreate");

  TH2F *h_qmbd_nintt = new TH2F("h_qmbd_nintt", "Intt N vs BbcQ ", 100, 0, 2000, 100, 0, 10000);

  TNtuple* h_ntp_cluspair = new TNtuple("ntp_cluspair", "Cluster Pair Ntuple", 
     "nclus:nclus2:bco_full:evt:ang1:ang2:z1:z2:adc1:adc2:dca2d:len:unorm:l1:l2:vx:vy:vz:bz");
  TH1* h_zvtx        = new TH1F("h_zvtx", "Zvertex", 400, -40, 40);
  TH2* h_zvtx_bz     = new TH2F("h_zvtx_bz", "Zvertex (MBD) vs INTT", 100, -40, 40, 100, -40, 40);


  TNtuple* h_ntp_evt = new TNtuple("ntp_evt", "Event Ntuple", 
     "nclus:nclus2:bco_full:evt:zv:zvs:zvm:zvc:bz:bqn:bqs:bfemclk");


  // histogram to calculate a peak
  h_zvtxseed_ = new TH1F("h_zvtxseed", "Zvertex Seed histogram", 200, -50, 50);


  int        prev_mbdclk = 0;
  ULong64_t  prev_bco = 0;
  
  bool found_firstevt=false;
  int mbd_evt_offset = 0;
  int intt_evt_offset = 2;

  bool intt_first_done=false;

  int iIntt=0;
  for(int i=0; i<t_mbd->GetEntries(); i++){
  //for(int i=0; i<15000; i++){
    mbdt.LoadTree(i+mbd_evt_offset);
    mbdt.GetEntry(i+mbd_evt_offset);

    clustEvent vCluster;


    for(; iIntt<t_intt->GetEntries(); iIntt++){
      inttcl.LoadTree(iIntt);
      inttcl.GetEntry(iIntt);
      if(!intt_first_done){
        if(inttcl.evt==0){
          cout<<"first done"<<endl;
          intt_first_done=true;
        }
        else {
          continue;
        }
      }

      if(mbdt.evt==inttcl.evt+intt_evt_offset){
        vCluster.evt = inttcl.evt;
        vCluster.bco = (int)inttcl.bco_full;
        cluster c; c.set(inttcl.evt, inttcl.bco_full, inttcl.x, inttcl.y, inttcl.z, inttcl.adc, inttcl.size, inttcl.lay);
        vCluster.vclus.push_back(c);
        //cout<<"bco : "<<vCluster.evt<<" "<<hex<<vCluster.bco<<dec<<endl;
      }
      else if(mbdt.evt>inttcl.evt+intt_evt_offset){
        continue;
      }
      else if(mbdt.evt<inttcl.evt+intt_evt_offset){
        break;
      }
    }

    if((i%100)==0)
    {
      cout<<i<<" mbd: " <<mbdt.evt<<" "<<hex<<mbdt.clk<<" "<<mbdt.femclk<<dec<<" "<<mbdt.bns<<" "<<mbdt.bnn
          <<" "<<mbdt.bqs<<" "<<mbdt.bqn<<" "<<mbdt.bz<<endl;
      cout<<"intt: "<<vCluster.evt<<" "<<hex<<(vCluster.bco&0xFFFF)<<dec<<" "<<vCluster.vclus.size()<<endl;
      cout<<endl;
    }


    h_zvtxseed_->Reset();

    if(vCluster.vclus.size()<500)
    {

       double zcenter=-999,zrms=-999,zmean=-999;
       double zvtx = calculateZvertex(vCluster, 
                                      xbeam_, ybeam_, 
                                      &zcenter, &zrms, &zmean, h_ntp_cluspair);

/*
        float ntpval2[20];
        TVector3 beamspot   = TVector3(xbeam_,ybeam_,0);
        vector<double> vz_array;
        for(auto c1=clusters[0].begin(); c1!=clusters[0].end(); ++c1) // inner
        {
            for(auto c2=clusters[1].begin(); c2!=clusters[1].end(); ++c2) // outer
            {
                if(c1->adc<40 || c2->adc<40) continue;

                TVector3 p1 = c1->pos - beamspot;
                TVector3 p2 = c2->pos - beamspot;
                // skip bad compbination
                double p1_ang = atan2(p1.y(), p1.x());
                double p2_ang = atan2(p2.y(), p2.x());
                double d_ang = p2_ang - p1_ang;

                if(fabs(d_ang)>0.2 ) continue;

                TVector3 u = p2 - p1;
                double unorm = sqrt(u.x()*u.x()+u.y()*u.y());
                if(unorm<0.00001) continue;



                // unit vector in 2D
                double ux = u.x()/unorm;
                double uy = u.y()/unorm;
                double uz = u.z()/unorm; // same normalization factor(2D) is multiplied

                TVector3 p0   = beamspot - p1;

                double dca_p0 = p0.x()*uy - p0.y()*ux; // cross product of p0 x u
                double len_p0 = p0.x()*ux + p0.y()*uy; // dot product of p0 . u


                // beam center in X-Y plane
                double vx = len_p0*ux + p1.x();
                double vy = len_p0*uy + p1.y();

                double vz = len_p0*uz + p1.z();

                //if(unorm>4.5||d_ang<-0.005*3.1415||0.005*3.14
                //if(unorm>4.5||d_ang<-0.25*3.1415||0.25*3.1415<d_ang)
                //if(fabs(d_ang)<0.1 && fabs(dca_p0) < 1.0)
                if(fabs(d_ang)<0.05 && fabs(dca_p0) < 1.0)
                {
                    h_zvtxseed_->Fill(vz);
                    vz_array.push_back(vz);
                }

                //h_dca2d_zero->Fill(dca_p0);
                //h2_dca2d_zero->Fill(dca_p0, nCluster);
                //h2_dca2d_len->Fill(dca_p0, len_p0);
                //cout<<"dca : "<<dca_p0<<endl;
                //
                if(vCluster.vclus.size()<300) 
                {
                    ntpval2[0]  = vCluster.vclus.size();
                    ntpval2[1]  = nclus2;
                    ntpval2[2]  = vCluster.bco&0xFFFF;
                    ntpval2[3]  = vCluster.evt;
                    ntpval2[4]  = p1_ang;
                    ntpval2[5]  = p2_ang;
                    ntpval2[6]  = p1.z();
                    ntpval2[7]  = p2.z();
                    ntpval2[8]  = c1->adc;
                    ntpval2[9]  = c2->adc;
                    ntpval2[10]  = dca_p0;
                    ntpval2[11]  = len_p0;
                    ntpval2[12] = unorm;
                    ntpval2[13] = c1->layer;
                    ntpval2[14] = c2->layer;
                    ntpval2[15] = vx;
                    ntpval2[16] = vy;
                    ntpval2[17] = vz;
                    ntpval2[18] = mbdt.bz;
                    h_ntp_cluspair->Fill(ntpval2);
                }
            }
        }

        // calculate trancated mean of DCA~Z histogram as Z-vertex position
        double zvtx=-9999.;

        double zcenter = -9999.;
        double zmean   = -9999.;
        double zrms    = -9999.;
        if(vz_array.size()>3){
            double zbin    = h_zvtxseed_->GetMaximumBin();
                   zcenter = h_zvtxseed_->GetBinCenter(zbin);
                   zmean   = h_zvtxseed_->GetMean();
                   zrms    = h_zvtxseed_->GetRMS();
            if(zrms<20) zrms = 20;

            double zmax = zcenter + zrms; // 1 sigma
            double zmin = zcenter - zrms; // 1 sigma

            double zsum=0.;
            int    zcount=0;
            for(auto iz=vz_array.begin(); iz!=vz_array.end(); ++iz){
                double vz = (*iz);
                if(zmin<vz&&vz<zmax){
                    zsum+=vz;
                    zcount++;
                }
            }
            if(zcount>0) zvtx = zsum/zcount;

            cout<<"ZVTX: "<<zvtx<<" "<<zcenter<<" "<<zmean<<" "<<zrms<<" "<<zbin<<" "<<mbdt.bz<<endl;
        }
*/

        h_zvtx->Fill(zvtx);
        h_zvtx_bz->Fill(zvtx, mbdt.bz-12);

        float ntpval3[20];
        ntpval3[ 0]  = vCluster.vclus.size();
        ntpval3[ 1]  = 0; //nclus2;
        ntpval3[ 2]  = vCluster.bco;
        ntpval3[ 3]  = vCluster.evt;
        ntpval3[ 4]  = zvtx;
        ntpval3[ 5]  = zrms;
        ntpval3[ 6]  = zmean;
        ntpval3[ 7]  = zcenter;
        ntpval3[ 8]  = mbdt.bz;
        ntpval3[ 9]  = mbdt.bqn;
        ntpval3[10]  = mbdt.bqs;
        ntpval3[11]  = mbdt.femclk;

            h_ntp_evt->Fill(ntpval3); 
           //"nclus:nclus2:bco_full:evt:zv:zvs:zvm:zvc:bz:bqn:bqs");
    }
         

    float bbcq  = mbdt.bqn+mbdt.bqs;
    float nintt = vCluster.vclus.size();

    unsigned short mbdclk = mbdt.femclk;
    ULong64_t bco         = vCluster.bco;
    ULong64_t bco16       = bco&0xFFFF;

    int       mbd_prvdif  = (mbdclk-prev_mbdclk)&0xFFFF;
    ULong64_t intt_prvdif = bco-prev_bco;

    cout<<i<<" "<<hex<<setw(6)<<mbdclk<<" "<<setw(6)<<bco16<<" (mbd-intt)"<<setw(6)<<((mbdclk-bco16)&0xFFFF)
        <<"      (femclk-prev)"<<setw(6)<<mbd_prvdif<<" (bco-prev)"<<setw(6)<<intt_prvdif<<dec<<endl;

    h_qmbd_nintt->Fill(bbcq, nintt);


    prev_mbdclk = mbdclk;
    prev_bco    = bco;

    //if(i>99990) break;
    if(i>50000) break;
    //if(i>10000) break;
    //if(i>5000) break;
    //if(i>1000) break;
    //if(i>100) break;
  }
  
  froot->Write();
  froot->Close();
}
