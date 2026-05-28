
#include <vector>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TArc.h>
#include <TBox.h>

//
// Variables for Event Display
//

string idatafile;   //filename of the input data file
int NmaxEvent;      // # of events in the data file
int idata_Type;     // data type. 1=Hachiya 2=Jingyu


//Global variables
//
// imode==1 : run AnalyzeEvent()
// imode==2 : Event Display
int imode = 2;   

// display frame;
static int iframe=-1; //0: All view 1: Si View
static TH1F *frame1 = nullptr;
static TH1F *frame2 = nullptr;
static TCanvas *c1=nullptr;
static TCanvas *c2=nullptr;

static TArc *Circle2 = nullptr;
static TArc *Circle4 = nullptr;
static TArc *Circle8 = nullptr;
static TArc *Circle10 = nullptr;

//pointer to the TTree for the data.0
TTree *evtTree;
TTree *caloTree;  //EMCal clusters
TTree *topoTree;  //topoClusters (EMC+iHCal+oHCal)
TTree *SiClusTree;
TTree *SiClusAllTree;
TTree *trackTree;

// data in evtTree
Int_t evt;
float xvtx;  //SiSeed X
float yvtx;  //SiSeed Y
float zvtx;  //SiSeed Z
float gxvtx;  //Global vertex X
float gyvtx;  //Global vertex Y
float gzvtx;  //Global vertex Z

// data in caloTree
vector<float> *energy=nullptr;
vector<float> *emc_phi=nullptr;
vector<float> *emc_r=nullptr;
vector<float> *emc_z  =nullptr;

// data in topoTree
vector<float> *top_e  =nullptr;
vector<float> *top_x  =nullptr;
vector<float> *top_y  =nullptr;
vector<float> *top_z  =nullptr;
vector<float> *top_r  =nullptr;
vector<float> *top_phi=nullptr;
vector<float> *emc_e  =nullptr;
vector<float> *ihc_e  =nullptr;
vector<float> *ohc_e  =nullptr;

// data in SiClustTree
vector<int> *layer=nullptr;
vector<int> *trkid=nullptr;
vector<float> *si_x=nullptr;
vector<float> *si_y=nullptr;
vector<float> *si_z=nullptr;

// data in SiClustAllTree
vector<int> *a_layer=nullptr;
vector<int> *a_trkid=nullptr;
vector<float> *a_si_x=nullptr;
vector<float> *a_si_y=nullptr;
vector<float> *a_si_z=nullptr;

// data in trackTree
vector<float> *eta0=nullptr;
vector<float> *phi0=nullptr;
vector<float> *pt0=nullptr;
vector<float> *vx0=nullptr;
vector<float> *vy0=nullptr;
vector<float> *vz0=nullptr;

// buffer for hits to display
const int nMax=4000;
float x[nMax], y[nMax];
TBox *hits[nMax];
TBox *hitsRZ[nMax];
static int nhits = 0;
static int nhitsRZ = 0;


// ZVTX maker
TArrow *ZvtxMarker = nullptr;

// buffer for tracks to display
const int nMaxTrk=300;
TArrow *seedtrk[nMaxTrk];
TArrow *seedtrkRZ[nMaxTrk];
static int ntrks = 0;
static int ntrksRZ = 0;

// constants
const float pt0min=0.5; // GeV/c. minimum pT0 for tracks
const float EMScale=0.14;  //EMC hit scaling factor

// RZ slice angle
static float phiSlice = 3.141592/2;
const float DphiSlice = 0.15;
static float Rmax = 12.0;
static TArc *dphiArc = nullptr;

//
// variables for EMC-INTT tracking
//
struct EmcINTTtriplet {
  int iEmc;       //index in vEMr, vEMphi, vEMz
  int iINTT0;     //index in vINTT0.r, vINTT0.phi, vINTT0z
  int iINTT1;     //index in vINTT1.r, vINTT1.phi, vINTT1z
};

struct EmcINTT_Mvtx {
  int iEmc;       //index in vEMr, vEMphi, vEMz
  int iINTT0;     //index in vINTT0.r, vINTT0.phi, vINTT0z
  int iINTT1;     //index in vINTT1.r, vINTT1.phi, vINTT1z
  vector<int> iMVTX;
};

vector<EmcINTTtriplet> vEmcINTT;

vector<float> vEMr;
vector<float> vEMphi;
vector<float> vEMz;
vector<float> vEMe;

vector<float> vINTT0r;
vector<float> vINTT0phi;
vector<float> vINTT0z;

vector<float> vINTT1r;
vector<float> vINTT1phi;
vector<float> vINTT1z;

vector<float> vMVTXr;
vector<float> vMVTXphi;
vector<float> vMVTXz;

void LoadData(string filename);
void GetEvent(int ievent);
void EventSelection(int&);
void EventDisplay(void);

void AnalyzeInit(void);
void AnalyzeEvent(void);
void AnalyzeEnd(void);
void LoadTrackerData(void);
void FindEmcINTTtriplet(void);
void INTT_zvtx_Finder(float&);
void FindEmcINTT_Mvtx(void);

void display_data(void){
  int ievent = 0;
  const int MaxLoop = 100;


  // open data file and load the data
  //  LoadData("data/52077/ana_52077_00346.root");//zero field and very old
  //  LoadData("data/53018/ana_53018_00089.root");//field ON but old format

  //input data file
  idatafile ="run2pp/53876/ana_53876_00102.root";
  // idatafile ="JYtaskonRIKEN/ana_53879_100kevt_topo_eh_default.root";

  // type of the data
  idata_Type = 1;  // Hachiya nDST
  // idata_Type = 2;  // Jingy's new nDST
    
  LoadData(idatafile.c_str());  //field ON and new
  NmaxEvent = evtTree->GetEntries();
  
  cout <<"Data file:"<<idatafile.c_str()<<endl;
  cout <<"# of events in the file:"<<NmaxEvent<<endl;
  
  cout << "Select Mode: " <<endl;
  cout << " 1: run AnalyzeEvent" << endl;
  cout << " 2: event display" <<endl;
  cin >> imode;
  if(imode == 1) {
    int istart = 0;
    int nevent=0;
    
    cout << "enter istart (first event)"<<endl;
    cin >> istart;
    if(istart<0) istart=0;
    
    cout << "how many event:"<<endl;
    cin >> nevent;
    if(nevent<0) nevent=0;
    if(istart+nevent > NmaxEvent) {
      cout <<" Exceeds the # of events in the file" <<endl;
      nevent = NmaxEvent - istart;
    }
    cout << "analyze "<< nevent << " events" <<endl;

    AnalyzeInit();
    for(int i=istart;i<istart+nevent;i++) {
      //      cout << "event = "<<i << endl;
      GetEvent(i);
      AnalyzeEvent();
      if(i%1000 == 0) cout << i << endl;
    }//for
    AnalyzeEnd();
  } else if(imode == 2 ) { // Event Display
    gStyle->SetCanvasPreferGL();

    // select frame. This is asked only once at the beginning
    if(iframe==-1) {
      cout << "select frame (0 or 1)" << endl;
      cin >> iframe;
      if(iframe != 0) iframe=1;
      cout << "iframe="<<iframe<<endl;
    }
    AnalyzeInit();
    for(int i=0;i<MaxLoop;i++) {
      EventSelection( ievent);
      if(ievent<0) break;
      GetEvent(ievent);
      AnalyzeEvent();  // do additional analysis
      EventDisplay();
    }//for
  }//if
}

void EventSelection( int& ievent) {
  // Menu
  int imenu;
  cout <<" <0: quit"<<endl;
  cout <<"  0: display current data"<<endl;
  cout <<"  1: display next data"<<endl;
  cout <<"  2: display specified event"<<endl;
  cin >> imenu;
  if(imenu<0) {
    cout <<" quit "<<endl;
    ievent = -1;
    return;
  }
  if(imenu==0) {
    cout <<"Event "<<ievent<<endl;
  }
  if(imenu==1) {
    ievent ++;
    cout <<"Event "<<ievent<<endl;
  }
  if(imenu==2) {
    cout <<"Enter event number to display (negative to quit) "<<endl;
    cin>>ievent;
    if(ievent<0 || ievent >40000) {
      ievent = -1;
      return;
    }
  }
  if(imenu>2) {
    cout <<" out of range. Keep current event"<<endl;
  }
  
  cout<<" enter phiSlice"<<endl;
  cin>>phiSlice;
  cout << "phiSlice="<<phiSlice<<endl;
}

bool Is_Hot_EMC(int iemc) {
  float phiEMC = emc_phi->at(iemc);
  float zEMC = emc_z->at(iemc);
  if( (2.4 < phiEMC && phiEMC < 2.7) && (zEMC < 0)) return true;
  else return false;
}

void LoadData(string filename) {
  TFile *file = TFile::Open(filename.c_str(),"READ");
  if(!file || file->IsZombie()) return;

  // connect to the TTrees in the nDST file
  evtTree       = dynamic_cast<TTree*>(file->Get("evtTree"));
  caloTree      = dynamic_cast<TTree*>(file->Get("caloTree"));
  SiClusTree    = dynamic_cast<TTree*>(file->Get("SiClusTree"));
  SiClusAllTree = dynamic_cast<TTree*>(file->Get("SiClusAllTree"));
  trackTree     = dynamic_cast<TTree*>(file->Get("trackTree"));
  topoTree      = dynamic_cast<TTree*>(file->Get("TopoClusTree"));

  //data in evtTree  
  evtTree->SetBranchAddress("evt",&evt);
  evtTree->SetBranchAddress("xgvtx",&xvtx);
  evtTree->SetBranchAddress("ygvtx",&yvtx);
  if(idata_Type == 1) {
    evtTree->SetBranchAddress("zgvtx",&zvtx);
  } else if(idata_Type == 2) {
    evtTree->SetBranchAddress("zvtx",&zvtx);
  }

  //data in caloTree
  if(idata_Type == 1) {// data by Hachiya
    caloTree->SetBranchAddress("energy",&energy);
    caloTree->SetBranchAddress("phi",&emc_phi);
    caloTree->SetBranchAddress("r",&emc_r);
    caloTree->SetBranchAddress("z",&emc_z);
  } else if(idata_Type ==2) {// data by Jingyu. This includes topoTree
    caloTree->SetBranchAddress("Calo_Clus_energy",&energy);
    caloTree->SetBranchAddress("Calo_Clus_phi",&emc_phi);
    caloTree->SetBranchAddress("Calo_Clus_r",&emc_r);
    caloTree->SetBranchAddress("Calo_Clus_z",&emc_z);

    topoTree->SetBranchAddress("clus_e",&top_e);
    topoTree->SetBranchAddress("clus_x",&top_x);
    topoTree->SetBranchAddress("clus_y",&top_y);
    topoTree->SetBranchAddress("clus_z",&top_z);
    topoTree->SetBranchAddress("clus_r",&top_r);
    topoTree->SetBranchAddress("clus_z",&top_z);
    topoTree->SetBranchAddress("clus_phi",&top_phi);
    topoTree->SetBranchAddress("clus_emcal_e",&emc_e);
    topoTree->SetBranchAddress("clus_ihcal_e",&ihc_e);
    topoTree->SetBranchAddress("clus_ohcal_e",&ohc_e);
  }

  //data in SiClusTree
  SiClusTree->SetBranchAddress("Siclus_layer",&layer);
  SiClusTree->SetBranchAddress("Siclus_trackid",&trkid);
  SiClusTree->SetBranchAddress("Siclus_x",&si_x);
  SiClusTree->SetBranchAddress("Siclus_y",&si_y);
  SiClusTree->SetBranchAddress("Siclus_z",&si_z);

  //data in SiClusAllTree
  SiClusAllTree->SetBranchAddress("Siclus_layer",&a_layer);
  SiClusAllTree->SetBranchAddress("Siclus_trackid",&a_trkid);
  SiClusAllTree->SetBranchAddress("Siclus_x",&a_si_x);
  SiClusAllTree->SetBranchAddress("Siclus_y",&a_si_y);
  SiClusAllTree->SetBranchAddress("Siclus_z",&a_si_z);
  
  //data in trackTree
  trackTree->SetBranchAddress("eta0",&eta0);
  trackTree->SetBranchAddress("phi0",&phi0);
  trackTree->SetBranchAddress("pt0",&pt0);
  trackTree->SetBranchAddress("x0",&vx0);
  trackTree->SetBranchAddress("y0",&vy0);
  trackTree->SetBranchAddress("z0",&vz0);
}

void GetEvent(int i) {
  evtTree->GetEntry(i);
  caloTree->GetEntry(i);
  trackTree->GetEntry(i);
  SiClusTree->GetEntry(i);
  SiClusAllTree->GetEntry(i);
}

void EventDisplay(void){   

  int nEmcClust=energy->size();
  int ntrack=pt0->size();
  int nSiClust=si_x->size();
  int nSiClustA=a_si_x->size();
  
  if(nEmcClust>nMax) nEmcClust=nMax;

  //  cout << "nhits="<<nhits<<endl;
  if(nhits>0) {
    for(int ihit=0;ihit<nhits;ihit++) {
      hits[ihit]->Delete();
    }
    nhits=0;
  }

  if(nhitsRZ>0) {
    for(int ihitRZ=0;ihitRZ<nhitsRZ;ihitRZ++) {
      hitsRZ[ihitRZ]->Delete();
    }
    nhitsRZ=0;
  }

  //  cout << "ntrks="<<ntrks<<endl;
  if(ntrks>0) {
    for(int itrk=0;itrk<ntrks;itrk++) {
      seedtrk[itrk]->Delete();
    }
    ntrks=0;
  }
  if(ntrksRZ>0){
    for(int itrkRZ=0;itrkRZ<ntrksRZ;itrkRZ++) {
      seedtrkRZ[itrkRZ]->Delete();
    }
    ntrksRZ=0;
  }
  
  float s0;    //size of EMC hit
  float s1;    //size of Silicon hit
  float s2;    //size of SiliconAll hit
  const float sLA=0.8;  //half length of strip. type-A
  const float sLB=1.0;  //half lenght of strip. type-B
  if(iframe==0) {
    s0=1.0;
    s1=0.5;
    s2=0.2;
  } else {
    s0=0.3;
    s1=0.2;
    s2=0.1;
  }

  /*  
  cout <<evt<< ":   nEmcClust="<<nEmcClust;
  cout <<" nSiClust="<<nSiClust;
  cout <<" nSiClustA="<<nSiClustA;
  cout <<" ntrack="<<ntrack;
  cout <<" zvtx="<<zvtx;
  if(abs(zvtx)>50) {
    zvtx = 0;
    cout << "--> "<<zvtx;
  }
  cout <<endl;
  */

  // Create ZVTX marker
  if(ZvtxMarker !=nullptr) ZvtxMarker->Delete();
  ZvtxMarker= new TArrow(zvtx,2.0,zvtx,0.0,0.01);
  ZvtxMarker->SetLineWidth(2);
  ZvtxMarker->SetLineColor(kRed);
  
  //  Create EM hits
  for(int j=0;j<nEmcClust;j++){
    if(!Is_Hot_EMC(j)) {
      x[j]= (emc_r->at(j))*cos(emc_phi->at(j));
      y[j]= (emc_r->at(j))*sin(emc_phi->at(j));


      float sE = sqrt(s0*energy->at(j));
      // rescale EMC hits if iframe==1
      if(iframe == 1) {
	x[j]=EMScale*x[j];
	y[j]=EMScale*y[j];
      }
      hits[nhits]=new TBox(x[j]-sE,y[j]-sE,x[j]+sE,y[j]+sE);
      hits[nhits]->SetFillColor(kBlue);
      nhits++;

      if(abs(emc_phi->at(j)-phiSlice)<DphiSlice) {
	float Remc = sqrt(x[j]*x[j]+y[j]*y[j]);
	float Zemc = emc_z->at(j);
	if(iframe == 1) Zemc = EMScale*Zemc;
	hitsRZ[nhitsRZ]=new TBox(Zemc-sE,Remc-sE,Zemc+sE,Remc+sE);
	hitsRZ[nhitsRZ]->SetFillColor(kBlue);
	nhitsRZ++;
      }//if(!Is_Hot_EMC)
    }//for
  }

  // Creates Silicon hits
  for(int jj=0;jj<nSiClust;jj++) {
    x[jj]=si_x->at(jj);
    y[jj]=si_y->at(jj);
    hits[nhits]=new TBox(x[jj]-s1,y[jj]-s1,x[jj]+s1,y[jj]+s1);
    if(layer->at(jj) < 3) hits[nhits]->SetFillColor(kGreen);
    else          hits[nhits]->SetFillColor(kRed);
    nhits++;

    float phiHit=atan2(y[jj],x[jj]);
    if(abs(phiHit-phiSlice)<DphiSlice) {
      float Rhit =  sqrt(x[jj]*x[jj]+y[jj]*y[jj]);
      float Zhit =  si_z->at(jj);

      if(layer->at(jj) < 3) { //MVTX hit
	hitsRZ[nhitsRZ]=new TBox(Zhit-s1,Rhit-s1,Zhit+s1,Rhit+s1);
	hitsRZ[nhitsRZ]->SetFillColor(kGreen);
      } else { //INTT hit
	if(abs(Zhit)<12.8) { // type-A strip
	  hitsRZ[nhitsRZ]=new TBox(Zhit-sLA,Rhit-s1,Zhit+sLA,Rhit+s1);
	} else {  // type-B strip
	  hitsRZ[nhitsRZ]=new TBox(Zhit-sLB,Rhit-s1,Zhit+sLB,Rhit+s1);
	}
	hitsRZ[nhitsRZ]->SetFillColor(kRed);
      }
      nhitsRZ++;
    }
  }

  // Creates SiliconAll hits
  for(int jj=0;jj<nSiClustA;jj++) {
    x[jj]=a_si_x->at(jj);
    y[jj]=a_si_y->at(jj);
    hits[nhits]=new TBox(x[jj]-s2,y[jj]-s2,x[jj]+s2,y[jj]+s2);
    if(a_layer->at(jj) < 3) hits[nhits]->SetFillColor(kBlack);
    else          hits[nhits]->SetFillColor(kBlack);
    nhits++;

    float phiHit=atan2(y[jj],x[jj]);
    if(abs(phiHit-phiSlice)<DphiSlice) {
      float Rhit =  sqrt(x[jj]*x[jj]+y[jj]*y[jj]);
      float Zhit =  a_si_z->at(jj);

      if(a_layer->at(jj) < 3) { // MVTX hit
	hitsRZ[nhitsRZ]=new TBox(Zhit-s2,Rhit-s2,Zhit+s2,Rhit+s2);
	hitsRZ[nhitsRZ]->SetFillColor(kBlack);
      } else {// INTT hit
	if(abs(Zhit)<12.8) { // type-A strip
	  hitsRZ[nhitsRZ]=new TBox(Zhit-sLA,Rhit-s2,Zhit+sLA,Rhit+s2);
	} else { // type-B strip
	  hitsRZ[nhitsRZ]=new TBox(Zhit-sLB,Rhit-s2,Zhit+sLB,Rhit+s2);
	}
	hitsRZ[nhitsRZ]->SetFillColor(kBlack);
      }
      nhitsRZ++;
    }
  }
  
  // Creates SiSeed tracklets (arrow)
  float Rtrk;
  const float Asize = 0.02;
  if(iframe==1) Rtrk=2.0; else Rtrk=80;
  if(iframe==1) Rmax=12.0; else Rmax=80;
  for(int k=0;k<ntrack;k++) {
    if(pt0->at(k)>pt0min) {
      //  cout<<k<<":"<<eta0->at(k)<<"  "<<phi0->at(k)<<"  "<<pt0->at(k)<<endl;
      float dx=Rtrk*cos(phi0->at(k));
      float dy=Rtrk*sin(phi0->at(k));
      seedtrk[ntrks]=new TArrow(vx0->at(k),vy0->at(k),vx0->at(k)+dx,vy0->at(k)+dy,Asize);
      ntrks++;

      if(abs(phi0->at(k)-phiSlice)<DphiSlice) {
	float r0=sqrt((vx0->at(k))*(vx0->at(k))+(vy0->at(k))*(vy0->at(k)));
	float eta = eta0->at(k);
	float theta = 2*atan(exp(-eta));
	float Zend = (Rmax-r0)/tan(theta)+vz0->at(k);

	seedtrkRZ[ntrksRZ]=new TArrow(vz0->at(k),r0,Zend,Rmax,Asize);
	ntrksRZ++;
	// if iframe==1, also plot "scaled" RZ track
	if(iframe==1){
	  float Rmax2=80;
	  float Zend2 = (Rmax2-r0)/tan(theta)+vz0->at(k);
	  seedtrkRZ[ntrksRZ]=new TArrow(EMScale*(vz0->at(k)),
					EMScale*r0,
					EMScale*Zend2,
					EMScale*Rmax2,Asize);
	  seedtrkRZ[ntrksRZ]->SetLineStyle(2);
	  ntrksRZ++;
	}
      }
    }
  }

  // Draw hits (EMC and Silicon) and seedtrk in c1
  // create or select c1
  if(c1==nullptr){
    c1 = new TCanvas("c1","XY View",800.,800.);
    if(iframe==0) {
      frame1 = c1->DrawFrame(-160.,-160.,160.,160.);
    } else if(iframe==1) {
      frame1 = c1->DrawFrame(-16.,-16.,16.,16.);
    }
  } else {
    c1->cd();
  }
  // Draw X-Y view
  for(int kk=0;kk<nhits;kk++){
    hits[kk]->Draw();
  }
  for(int itk=0;itk<ntrks;itk++) {
    seedtrk[itk]->Draw();
  }
  // Draw phi slice region
  if(dphiArc != nullptr) dphiArc->Delete();
  float phimin=(phiSlice-DphiSlice)*180./3.141592;
  float phimax=(phiSlice+DphiSlice)*180./3.141592;
  dphiArc = new TArc(0.,0.,Rmax,phimin,phimax);
  dphiArc->SetLineColorAlpha(0,0.1);
  dphiArc->SetFillColorAlpha(kBlue,0.1);
  dphiArc->Draw();

  // Draw Circle for reference size of INTT and MVTX
  if(Circle2==nullptr) {
    Circle2=new TArc(0.,0.,2.3,0,360);
    Circle2->SetFillStyle(0);
  }
  if(Circle4==nullptr) {
    Circle4=new TArc(0.,0.,4.0,0,360);
    Circle4->SetFillStyle(0);
  }
  if(Circle8==nullptr) {
    Circle8=new TArc(0.,0.,7.6,0.,360);
    Circle8->SetFillStyle(0);
  }
  if(Circle10==nullptr) {
    Circle10=new TArc(0.,0.,10.2,0.,360);
    Circle10->SetFillStyle(0);
  }
  Circle2->Draw();
  Circle4->Draw();
  Circle8->Draw();
  Circle10->Draw();
  
  c1->Draw();
  c1->Modified();
  c1->Update();
  cout<<endl;

  // Draw vertical R-Z slice
  // create or select c2
  if(c2==nullptr) {//set up frame
    c2= new TCanvas("c2","RZ Slice",800,270);
    if(iframe==0) {
      frame2 = c2->DrawFrame(-240.,0.,240.,160.);
    } else if(iframe==1) {
      frame2 = c2->DrawFrame(-24.,0.,24.,16.);
    }
  } else {
    c2->cd();
  }

  //Draw ZvtxMarker in c2
  ZvtxMarker->Draw();
  
  //Draw RZ slice
  for(int kk=0;kk<nhitsRZ;kk++){
    hitsRZ[kk]->Draw();
  }
  for(int itk=0;itk<ntrksRZ;itk++) {
    seedtrkRZ[itk]->Draw();
  }
  c2->Modified();
  c2->Update();
}

//
TH1F* hdphi1;
TH1F* hdphi0;
TH1F* hdeta1;
TH1F* hdeta0;
TH1F* hdphi1_0;
TH1F* hdphi1t;
TH1F* hdeta1t;

TH1F* hEMe;
TH1F* hEMeM;
TH1F* hEMeMt;
TH2F* hEMC;
TH2F* hEMCt;
TH2F* hdphiM;
TH2F* hdphiM2;
TH2F* hdphiM3;

TH2F* hdphi10_MVTX;

TH1F* hzvtx;
TH1F* hzvtxINTT;

void AnalyzeInit(void) {
  cout << "AnalyzeInit()"<<endl;
  hdphi1=new TH1F("hdphi1","dphi1",100,-0.2,0.2);
  hdphi0=new TH1F("hdphi0","dphi0",100,-0.2,0.2);
  hdeta1=new TH1F("hdeta1","eta1-EMCeta",150,-0.03,0.03);
  hdeta0=new TH1F("hdeta0","eta1-EMCeta",150,-0.03,0.03);
  hdphi1_0=new TH1F("hdphi1_0","phi1-phi0",100,-0.1,0.1);
  hdphi1t=new TH1F("hdphi1t","dphi1 (tight)",100,-0.2,0.2);
  hdeta1t=new TH1F("hdeta1t","eta1-EMCeta (tight)",150,-0.03,0.03);
  
  hEMe = new TH1F("hEMe","EMC energy (inclusive)",250,0.,5.0);
  hEMeM = new TH1F("hEMeM","MEC energy (matched)",250,0.,5.0);
  hEMeMt = new TH1F("hEMeMt","MEC energy (tight matched)",250,0.,5.0);
  hEMC= new TH2F("hEMC","EMz vs EMphi matched w INTT",100,-150.,150.,100,-3.5,3.5);
  hEMCt= new TH2F("hEMCt","EMz vs EMphi tight matched w INTT",100,-150.,150.,100,-3.5,3.5);
  hdphiM = new TH2F("hdphiM","EMphi-INTT1phi vs INTT1phi-INTT0phi",100,-0.2,0.2,100,-0.025,0.025);
  hdphiM2 = new TH2F("hdphiM2","EMphi-INTT1phi vs INTT1phi-INTT0phi Rotate",100,-0.2,0.2,100,-0.025,0.025);
  hdphiM3 = new TH2F("hdphiM3","EMphi-INTT1phi vs INTT1phi-INTT0phi Rot, Tight",100,-0.2,0.2,100,-0.025,0.025);

  hdphi10_MVTX = new TH2F("hdphi10_MVTX","INTT0phi-MVTXphi vs INTT1phi-INTT0phi",100,-0.02,0.02,100,-0.02,0.02);
  
  hzvtx     = new TH1F("hzvtx","zvtx in evtTree",41,-20.5,20.5);
  hzvtxINTT = new TH1F("hzvtxINTT","zvtx by INTT",41,-20.5,20.5);
  
}

void AnalyzeEvent(void) {
  float zvtxINTT;
  INTT_zvtx_Finder(zvtxINTT);
  LoadTrackerData();
  FindEmcINTTtriplet();
  FindEmcINTT_Mvtx();
  
  hzvtxINTT->Fill(zvtxINTT);
}

void AnalyzeEnd(void) {
  gStyle->SetOptFit(1111);
  TF1* f1= new TF1("f1","gaus(0)+pol0(3)",-0.03,0.03);
  
  TCanvas *c3=new TCanvas("c3");
  hdphi1->SetMinimum(0);
  hdphi1->Draw();

  TCanvas *c4=new TCanvas("c4");
  hdphi0->SetMinimum(0);
  hdphi0->Draw();

  TCanvas *c5=new TCanvas("c5");
  hdeta1->SetMinimum(0);
  hdeta1->Draw();
  
  TCanvas *c6=new TCanvas("c6");
  hdeta0->SetMinimum(0);
  hdeta0->Draw();

  TCanvas *c7=new TCanvas("c7");
  f1->SetParameters(300,0.,0.01,0.);
  hdphi1_0->Draw();
  hdphi1_0->Fit("f1");
  
  TCanvas *c8=new TCanvas("c8");
  hEMC->Draw();

  TCanvas *c9=new TCanvas("c9");
  hdphiM->Draw();
  
  TCanvas *c10=new TCanvas("c10");
  hdphiM2->Draw();

  TCanvas *c11=new TCanvas("c11");
  hdphiM3->Draw();

  TCanvas *c12=new TCanvas("c12");
  hEMCt->Draw();

  TCanvas *c13=new TCanvas("c13");
  hdphi1t->SetMinimum(0);
  hdphi1t->Draw();

  TCanvas *c14=new TCanvas("c14");
  hdeta1t->SetMinimum(0);
  hdeta1t->Draw();

  TCanvas *c15=new TCanvas("c15");
  hdphi10_MVTX->Draw();


  TFile *file = new TFile("AnaEvent.root","RECREATE");
  hdphi1->Write();
  hdphi0->Write();
  hdeta1->Write();
  hdeta0->Write();
  hdphi1_0->Write();
  hdphi1t->Write();
  hdeta1t->Write();
  hEMC->Write();
  hEMCt->Write();
  hEMe->Write();
  hEMeM->Write();
  hEMeMt->Write();
  hdphiM->Write();
  hdphiM2->Write();
  hdphiM3->Write();
  hdphi10_MVTX->Write();
  hzvtx->Write();
  hzvtxINTT->Write();
  
  file->Close();
}

void LoadTrackerData(void) {

  int nEmc = energy->size();
  int nSiA  = a_layer->size();

  //  cout << "nEmc = "<<nEmc <<endl;
  //  cout << "nSiA = "<<nSiA <<endl;

  vEMr.clear();
  vEMphi.clear();
  vEMz.clear();
  vEMe.clear();

  vMVTXr.clear();
  vMVTXphi.clear();
  vMVTXz.clear();

  vINTT0r.clear();
  vINTT0phi.clear();
  vINTT0z.clear();
  
  vINTT1r.clear();
  vINTT1phi.clear();
  vINTT1z.clear();

  //Fill vector of EMC data
  for(int iemc=0;iemc<nEmc;iemc++) {
    if(!Is_Hot_EMC(iemc)){
      vEMr.push_back(emc_r->at(iemc));
      vEMphi.push_back(emc_phi->at(iemc));
      vEMz.push_back(emc_z->at(iemc));
      vEMe.push_back(energy->at(iemc));
    }
  }

  //Fill vector of MVTX and INTT data
  for(int i=0;i<nSiA;i++) {
    float xhit = a_si_x->at(i);
    float yhit = a_si_y->at(i);
    float zhit = a_si_z->at(i);
    float phi_hit = atan2(yhit,xhit);
    float rhit = sqrt(xhit*xhit+yhit*yhit);
    if(a_layer->at(i)<3) {       // MVTX
      vMVTXr.push_back(rhit);
      vMVTXphi.push_back(phi_hit);
      vMVTXz.push_back(zhit);
    }
    else if(a_layer->at(i)< 5) { //INTT0
      vINTT0r.push_back(rhit);
      vINTT0phi.push_back(phi_hit);
      vINTT0z.push_back(zhit);
    }
    else {                       //INTT1
      vINTT1r.push_back(rhit);
      vINTT1phi.push_back(phi_hit);
      vINTT1z.push_back(zhit);
    } //if(a_layer)
  }// for(i)
  //  cout << " nMVTX="<<vMVTXr.size();
  //  cout << " nINTT0=" <<vINTT0r.size()<< " nINTT1="<<vINTT1r.size()<<endl;

}

float rz2eta(float r,float z) {
  if(z>=0) {
    return log((sqrt(r*r+z*z)+z)/r);
  } else {
    return -log((sqrt(r*r+z*z)-z)/r);
  }
}


void FindEmcINTTtriplet(void) {
  // constants for cuts
  const float dPhi = 0.2;         //dphi from EMC hit to search INTT hit
  const float deta_match = 0.03;  //0.03 deta cut between EMC hit and INTT hit
  const float deta_match2 = 0.01; //tighter deta cut
  const float dphi1_0match = 0.02;   // acceptance range of INTT1phi - INTT0phi
  const float dphi1_0match2 = 0.007; // tight cut (after rotational correction)
  const float Krot = 0.025;          //correction factor for EMdphi vs INTTdphi
  
  vector<int> INTT0m;
  vector<int> INTT1m;
  vector<float> INTT0phim;
  vector<float> INTT1phim;
  vector<float> INTT0detam;
  vector<float> INTT1detam;
  vector<float> INTT0dphim;
  vector<float> INTT1dphim;

  hzvtx->Fill(zvtx);

  vEmcINTT.clear();
  // search for INTT hits that matches with EMC in phi and eta
  for(int iemc=0;iemc<vEMphi.size();iemc++) {
    INTT0m.clear();
    INTT1m.clear();
    INTT0phim.clear();
    INTT1phim.clear();
    INTT0detam.clear();
    INTT1detam.clear();
    INTT0dphim.clear();
    INTT1dphim.clear();

    hEMe->Fill(vEMe.at(iemc));
    float EMCeta = rz2eta(vEMr.at(iemc),vEMz.at(iemc)-zvtx);

    // search for matched INTT1 hits
    int n1=vINTT1phi.size();
    for(int i1=0;i1<n1;i1++){
      float dphi1=vINTT1phi.at(i1)-vEMphi.at(iemc);
      float INTT1eta = rz2eta(vEMr.at(iemc)-vINTT1r.at(i1),vEMz.at(iemc)-vINTT1z.at(i1)); 
      float deta1 = INTT1eta - EMCeta;
      if(abs(dphi1)<dPhi && abs(deta1)<deta_match) {
	INTT1m.push_back(i1);
	INTT1phim.push_back(vINTT1phi.at(i1));
	INTT1detam.push_back(deta1);
	INTT1dphim.push_back(dphi1);
      }
    }

    // search for matched INTT0 hits
    int n0=vINTT0phi.size();
    for(int i0=0;i0<n0;i0++){
      float dphi0=vINTT0phi.at(i0)-vEMphi.at(iemc);
      float INTT0eta = rz2eta(vEMr.at(iemc)-vINTT0r.at(i0),vEMz.at(iemc)-vINTT0z.at(i0));
      float deta0 = INTT0eta - EMCeta;
      if(abs(dphi0)<dPhi && abs(deta0)<deta_match) {
	INTT0m.push_back(i0);
	INTT0phim.push_back(vINTT0phi.at(i0));
	INTT0detam.push_back(deta0);
	INTT0dphim.push_back(dphi0);
      }
    }

    // if there are phi-mathced INTT hit pairs
    if(INTT0m.size()>0 && INTT1m.size()>0) {
      for(int i1=0;i1<INTT1m.size();i1++) {
	for(int i0=0;i0<INTT0m.size();i0++) {
	  float dphi1_0 = INTT1phim.at(i1)-INTT0phim.at(i0);
	  hdphi1_0->Fill(dphi1_0);
	  if(abs(dphi1_0)<0.05) {
	    float dphiEM_1=vEMphi.at(iemc)-INTT1phim.at(i1);
	    hdphiM->Fill(dphiEM_1,dphi1_0);
	    hdphiM2->Fill(dphiEM_1,dphi1_0-Krot*dphiEM_1);

	    if(abs(dphi1_0-0.03*dphiEM_1)<dphi1_0match2) {//matched!
	      hdphi0->Fill(INTT0dphim.at(i0));
	      hdphi1->Fill(INTT1dphim.at(i1));
	      hdeta0->Fill(INTT0detam.at(i0));
	      hdeta1->Fill(INTT1detam.at(i1));
	      hEMC->Fill(vEMz.at(iemc),vEMphi.at(iemc));
    	      hEMeM->Fill(vEMe.at(iemc));
	    }
	    // apply tighter deta cut
	    if(abs(INTT0detam.at(i0))<deta_match2 &&
	       abs(INTT1detam.at(i1))<deta_match2) {
	      hdphiM3->Fill(dphiEM_1,dphi1_0-Krot*dphiEM_1);
	      //apply tighter dphi cut
	      if(abs(dphi1_0-Krot*dphiEM_1)<dphi1_0match2) {
		hdeta1t->Fill(INTT1detam.at(i1));
		hdphi1t->Fill(INTT1dphim.at(i1));
		hEMCt->Fill(vEMz.at(iemc),vEMphi.at(iemc));
		hEMeMt->Fill(vEMe.at(iemc));
		// a triplet is found. Store it.
		EmcINTTtriplet triplet;
		triplet.iEmc = iemc;
		triplet.iINTT0 = INTT0m.at(i0);
		triplet.iINTT1 = INTT1m.at(i1);
		vEmcINTT.push_back(triplet);
		if(imode == 2) {
		  cout <<"one triplet found:"<<iemc<<" "<<INTT0m.at(i0)<< " "<<INTT1m.at(i1)<<endl;
		}
	      }
	    }
	  }
	}//for(i0)
      }//for(i1)
    }//if
  }// iemc
}

void FindEmcINTT_Mvtx(void) {
  // Search for MVTX hits that matches EmcINTTtriplet, and add the
  // matched hits

  const float dphi_MVTX_INTT0 = 0.05;
  
  //  cout << ": # of triplet found:"<<vEmcINTT.size()<<endl;
  for(int i=0;i<vEmcINTT.size();i++) {
    cout << "EMphi = " <<vEMphi.at((vEmcINTT.at(i)).iEmc);
    cout <<" INTT1phi= "<<vINTT1phi.at((vEmcINTT.at(i)).iINTT1);
    cout <<" INTT0phi= "<<vINTT0phi.at((vEmcINTT.at(i)).iINTT0)<<endl;
    float phi0 =vINTT0phi.at((vEmcINTT.at(i)).iINTT0);
    float phi1 =vINTT1phi.at((vEmcINTT.at(i)).iINTT1);
    float rEmc =vEMr.at((vEmcINTT.at(i)).iEmc);
    float zEmc =vEMz.at((vEmcINTT.at(i)).iEmc);
    
    for(int iMvtx=0;iMvtx<vMVTXr.size();iMvtx++) {// vMVTX
      float MVTXphi = vMVTXphi.at(iMvtx);
      if( abs(MVTXphi - phi0) < dphi_MVTX_INTT0) {
	hdphi10_MVTX->Fill(phi1-phi0,phi0-MVTXphi);
      }
    }// for(vMVTX);
  }
}
void INTT_zvtx_Finder(float &zvtxINTT){
  // 2026/05/23
  // 1) form pair of INTT0 hit and INTT1 hit that are close in phi
  // 2) assume zvtx position, and calculate the eta range of the hit strip
  //    of INTT1 and INTT0 pairs
  // 3) calculate the overlap of the eta range of the pairs
  // 4) change zvtx and maximize the sum of the eta overlap of the pairs.
  // 2026/05/24
  // Test results shows that this method doesnt work reasonaly
  // well. But the Zvtx resolution is about 1cm or so.
  // For more precise ztx, need to combine with MVTX.

  const float dphiINTTmax=0.03;
  const float Zmax_A = 12.8;  //Max abs(Z) value of A-type INTT (cm)
  const float Zhalf_A = 0.8;  // half length of A-type strip (cm)
  const float Zhalf_B = 1.0;  // half lenght of B-type strip (cm)


  // INTT hits
  vector<int>   INTT1hit;  //index of INTT1 hit in a_si_x,a_si_y,a_si_z
  vector<float> INTT1phi;  //phi value of INTT1 hit
  vector<int>   INTT0hit;  //index of INTT0 hit in a_si_x,a_si_y,a_si_z
  vector<float> INTT0phi;  //phi value of INTT1 hit

  // pair of (INTT1hit,INTT0hit)
  vector<int>   INTTpair1; //index of the pair in INTT1hit
  vector<int>   INTTpair0; //index of the pair in INTT0hit
  //
  vector<float> pair1eta_max;  //max eta of INTT1hit of the pair
  vector<float> pair1eta_min;  //min eta of INTT1hit of the pair
  vector<float> pair0eta_max;  //max eta of INTT0hit of the pair
  vector<float> pair0eta_min;  //min eta of INTT0hit of the pair

  //form INTT1hit and INTT0hit
  INTT1hit.clear();
  INTT0hit.clear();
  INTT1phi.clear();
  INTT0phi.clear();
  for(int i=0;i<a_layer->size();i++) {
    if(a_layer->at(i)==3 || a_layer->at(i)==4) {//INTT0
      INTT0hit.push_back(i);
      INTT0phi.push_back(atan2(a_si_y->at(i),a_si_x->at(i)));
    }
    if(a_layer->at(i)==5 || a_layer->at(i)==6) {//INTT1
      INTT1hit.push_back(i);
      INTT1phi.push_back(atan2(a_si_y->at(i),a_si_x->at(i)));
    }
  }

  //form pair of INTT1 and INTT0 hit
  INTTpair1.clear();
  INTTpair0.clear();
  for(int i1=0;i1<INTT1hit.size();i1++) {
    for(int i0=0;i0<INTT0hit.size();i0++) {
      if(abs(INTT0phi.at(i0)-INTT1phi.at(i1))<dphiINTTmax) {
	INTTpair1.push_back(i1);
	INTTpair0.push_back(i0);
      }
    }
  }

  // print the formed pairs
  /*
  cout << "INTT pairs:"<<endl;
  for(int ipair=0;ipair<INTTpair0.size();ipair++) {
    cout << ipair <<": ("<<INTTpair1.at(ipair)<<","<<INTTpair0.at(ipair);
    cout <<")";
    cout <<"   ("<<INTT1phi.at(INTTpair1.at(ipair))<<",";
    cout <<"   ("<<INTT0phi.at(INTTpair0.at(ipair))<<")"<< endl;
  }
  */
  
  // slide zvtx from -20 to 20. For a given zvtx, calculate the
  // sum of (eta overlap) of the pairs.
  float max_overlap = 0.;
  float zvtx_max_overlap;
  float zvtx;
  for(zvtx=-20.0;zvtx<20.1;zvtx+=1) {
    float sum_overlap = 0.;
    for(int ipair=0;ipair<INTTpair0.size();ipair++) {
      float z1=a_si_z->at(INTT1hit.at(INTTpair1.at(ipair)));
      float z0=a_si_z->at(INTT0hit.at(INTTpair0.at(ipair)));
      float z1min, z1max;
      float z0min, z0max;
      if(abs(z1)<Zmax_A) {  //A-type INTT sensor
	z1min=z1-Zhalf_A;
	z1max=z1+Zhalf_A;
      } else {              //B-type INTT sensor
	z1min=z1-Zhalf_B;
	z1max=z1+Zhalf_B;
      }
      if(abs(z0)<Zmax_A) {  //A-type INTT sensor
	z0min=z0-Zhalf_A;
	z0max=z0+Zhalf_A;
      } else {              //B-type INTT sensor
	z0min=z1-Zhalf_B;
	z0max=z1+Zhalf_B;
      }
      //convert z to eta
      float x1=a_si_x->at(INTT1hit.at(INTTpair1.at(ipair)));
      float y1=a_si_y->at(INTT1hit.at(INTTpair1.at(ipair)));
      float r1=sqrt(x1*x1+y1*y1);
      float x0=a_si_x->at(INTT0hit.at(INTTpair0.at(ipair)));
      float y0=a_si_y->at(INTT0hit.at(INTTpair0.at(ipair)));
      float r0=sqrt(x0*x0+y0*y0);
      float eta1max=rz2eta(r1,z1max-zvtx);
      float eta1min=rz2eta(r1,z1min-zvtx);
      float eta0max=rz2eta(r0,z0max-zvtx);
      float eta0min=rz2eta(r0,z0min-zvtx);

      float eta_overlap=0;
      if(eta1max > eta0max && eta0max > eta1min) {
	eta_overlap = eta0max-eta1min;
      } else if(eta0max > eta1max && eta1max > eta0min) {
	eta_overlap = eta1max - eta0min;
      }
      //      cout << ipair <<": ("<< eta1max <<","<< eta1min<<")";
      //      cout << " ("<< eta0max <<","<< eta0min<<")";
      //      cout << " overlap = "<< eta_overlap << endl;
      sum_overlap += eta_overlap;
      if(sum_overlap > max_overlap) {
	max_overlap = sum_overlap;
	zvtx_max_overlap = zvtx;
      }
    }//for(ipair)
    //    cout << "zvtx="<<zvtx<<": sum_overlap = "<<sum_overlap;
    //    cout << "  max_overlap="<<max_overlap<<endl;
  }//for(zvtx)
  zvtxINTT = zvtx_max_overlap;
}
