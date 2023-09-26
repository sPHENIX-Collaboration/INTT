//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jul 27 02:04:56 2023 by ROOT version 6.26/06
// from TTree ntp_clus/Cluster Ntuple
// found on file: ../AnaTutorial.root
//////////////////////////////////////////////////////////

#ifndef inttcls_h
#define inttcls_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class inttcls {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Float_t         nclus;
   Float_t         nclus2;
   Float_t         bco_full;
   Float_t         evt;
   Float_t         size;
   Float_t         adc;
   Float_t         x;
   Float_t         y;
   Float_t         z;
   Float_t         lay;
   Float_t         lad;
   Float_t         sen;

   // List of branches
   TBranch        *b_nclus;   //!
   TBranch        *b_nclus2;   //!
   TBranch        *b_bco_full;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_size;   //!
   TBranch        *b_adc;   //!
   TBranch        *b_x;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!
   TBranch        *b_lay;   //!
   TBranch        *b_lad;   //!
   TBranch        *b_sen;   //!

   inttcls(TTree *tree=0);
   virtual ~inttcls();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef inttcls_cxx
inttcls::inttcls(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../AnaTutorial.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("../AnaTutorial.root");
      }
      f->GetObject("ntp_clus",tree);

   }
   Init(tree);
}

inttcls::~inttcls()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t inttcls::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t inttcls::LoadTree(Long64_t entry)
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

void inttcls::Init(TTree *tree)
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

   fChain->SetBranchAddress("nclus", &nclus, &b_nclus);
   fChain->SetBranchAddress("nclus2", &nclus2, &b_nclus2);
   fChain->SetBranchAddress("bco_full", &bco_full, &b_bco_full);
   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("size", &size, &b_size);
   fChain->SetBranchAddress("adc", &adc, &b_adc);
   fChain->SetBranchAddress("x", &x, &b_x);
   fChain->SetBranchAddress("y", &y, &b_y);
   fChain->SetBranchAddress("z", &z, &b_z);
   fChain->SetBranchAddress("lay", &lay, &b_lay);
   fChain->SetBranchAddress("lad", &lad, &b_lad);
   fChain->SetBranchAddress("sen", &sen, &b_sen);
   Notify();
}

Bool_t inttcls::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void inttcls::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t inttcls::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef inttcls_cxx
