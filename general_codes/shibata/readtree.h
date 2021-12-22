
// 2021/12/22 Mika Shibata Nara Women's University

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov 27 16:03:31 2018 by ROOT version 5.34/36
// from TTree tree/chip info
// found on file: 
//////////////////////////////////////////////////////////

#ifndef readtree_h
#define readtree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>
#include <TText.h>
#include <TGraphErrors.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class readtree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   //Int_t           adc;
   //Int_t           ampl;
   //Int_t           chip_id;
   //Int_t           fpga_id;
   //Int_t           module;
   //Int_t           chan_id;
   //Int_t           fem_id;
   //Int_t           bco;
   //Int_t           bco_full;
   //Int_t           event;
   //Int_t           camac_adc;
   //Int_t           camac_tdc;
   //Int_t           INTT_event;
   vector<int>    *adc;
   vector<int>    *ampl;
   vector<int>    *chip_id;
   vector<int>    *fpga_id;
   vector<int>    *module;
   vector<int>    *chan_id;
   vector<int>    *fem_id;
   vector<int>    *bco;
   vector<int>    *bco_full;
   vector<int>    *event;
   vector<int>    *camac_adc;
   vector<int>    *camac_tdc;
   bool           *INTT_event;

   // List of branches
   TBranch        *b_adc;   //!
   TBranch        *b_ampl;   //!
   TBranch        *b_chip_id;   //!
   TBranch        *b_fpga_id;   //!
   TBranch        *b_module;   //!
   TBranch        *b_chan_id;   //!
   TBranch        *b_fem_id;   //!
   TBranch        *b_bco;   //!
   TBranch        *b_bco_full;   //!
   TBranch        *b_event;   //!
   TBranch        *b_camac_adc;   //!
   TBranch        *b_camac_tdc;   //!
   TBranch        *b_INTT_event;   //!
   TH2*            m_hcoll_chip[4];
   TH2*            m_hcoll_chan[4];
   TH2*            m_hitmap;
   TH2*            m_hitmap_perLadder[4];
   TH2*            m_hitLaddermap;
   TH2*            m_noise;
   TH1*            m_residual_chip[4];
   TH1*            m_residual_chan[4];
   TF1*            f_chip;
   TF1*            f_chan;
   TH1*            m_nhit;
   TGraphErrors*   g_efficiency_chip;
   TGraphErrors*   g_efficiency_chan;
   TCanvas*        c1;
   TCanvas*        c2;
   TText*          t1 = new TText(0.6, 0.6, "");;
   TText*          t2;
   int             m_NHIT;
   int             m_ent = -1;
   int             m_jentry;
   int             m_prebco, m_prebcofull;
   int             m_Nhitmodule[3] ={0,0,0};
   double          m_module[100];
   double          m_chan[100];
   double          m_chip[100];
   double          m_adc[100];         
   int             m_ef_GOODhit[3] = {0,0,0};
   int             m_ef_Allhit[3] = {0,0,0};
   int             m_LADDERID[3] {-1, -1, -1};

   readtree(TTree *tree=0);
   virtual ~readtree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(Long64_t entry);
   //virtual void     Loop(const int entry=-1);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   
   bool m_NextEventInOneTrigger = false;
   bool m_boolMC = false;
   //bool m_boolMC = true;

   void process_hits(int nhit, int* chipArray, int* chanArray, int* adcArray, int* Nhits_Ret, double* chan_Ret, double* chip_Ret, int* ncls_Ret, int(*adc_Ret)[10],int* COSMIC);
   void initHist();
   void next();
   bool m_NEXTPERBCO = false;
   void next_continue();
};

#endif

#ifdef readtree_cxx
readtree::readtree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   cout<<"Enter the file name."<<endl;
   string STRINGFILENAME;
   cin>>STRINGFILENAME;
   cout<<"DATA or MC?"<<endl;
   cout<<"0. DATA   1.MC"<<endl;
   cin>>m_boolMC;
   cout<<"Enter module_id"<<endl;
   cout<<"Ladder 0 "<<endl;
   cin>>m_LADDERID[0];
   cout<<"Ladder 1 "<<endl;
   cin>>m_LADDERID[1];
   cout<<"Ladder 2 "<<endl;
   cin>>m_LADDERID[2];
   cout<<"Do you want to check hit data per bco?  0. No  1. Yes"<<endl;
   cin>>m_NEXTPERBCO;
   if(m_NEXTPERBCO)cout<<"Please enter  t.next()"<<endl;
   if(!m_NEXTPERBCO)cout<<"Please enter  t.next_continue()"<<endl;
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(STRINGFILENAME.c_str());
      if (!f || !f->IsOpen()) {
         f = new TFile(STRINGFILENAME.c_str());
      }
      f->GetObject("tree_both",tree);
      //f->GetObject("tree",tree);

   }
   Init(tree);
   initHist();
   m_jentry=0;
   m_prebco=-2;
   m_prebcofull=-2;
}

readtree::~readtree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t readtree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t readtree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void readtree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("adc", &adc, &b_adc);
   fChain->SetBranchAddress("ampl", &ampl, &b_ampl);
   fChain->SetBranchAddress("chip_id", &chip_id, &b_chip_id);
   fChain->SetBranchAddress("fpga_id", &fpga_id, &b_fpga_id);
   fChain->SetBranchAddress("module", &module, &b_module);
   fChain->SetBranchAddress("chan_id", &chan_id, &b_chan_id);
   fChain->SetBranchAddress("fem_id", &fem_id, &b_fem_id);
   fChain->SetBranchAddress("bco", &bco, &b_bco);
   fChain->SetBranchAddress("bco_full", &bco_full, &b_bco_full);
   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("camac_adc", &camac_adc, &b_camac_adc);
   fChain->SetBranchAddress("camac_tdc", &camac_tdc, &b_camac_tdc);
   //fChain->SetBranchAddress("INTT_event", &INTT_event, &b_INTT_event);
   Notify();
}

Bool_t readtree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void readtree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t readtree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef readtree_cxx
