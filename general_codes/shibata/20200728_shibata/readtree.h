//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov 27 16:03:31 2018 by ROOT version 5.34/36
// from TTree tree/chip info
// found on file: fphx_raw_20191029-0947_0.root
//////////////////////////////////////////////////////////

#ifndef readtree_h
#define readtree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>
#include <TH1.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class readtree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           adc;
   Int_t           ampl;
   Int_t           chip_id;
   Int_t           fpga_id;
   Int_t           module;
   Int_t           chan_id;
   Int_t           fem_id;
   Int_t           bco;
   Int_t           bco_full;
   Int_t           event;

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

   TH2*            m_hitmap;

   int             m_jentry;
   int             m_prebco, m_prebcofull;

   readtree(TTree *tree=0);
   virtual ~readtree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(const int entry=-1);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   void initHist();
   void next();
};

#endif

#ifdef readtree_cxx
readtree::readtree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("fphx_raw_20191129-1039_0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("fphx_raw_20191129-1039_0.root");
      }
      f->GetObject("tree",tree);

   }
   Init(tree);

   initHist();

   m_jentry=0;
   m_prebco=-1;
   m_prebcofull=-1;
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
