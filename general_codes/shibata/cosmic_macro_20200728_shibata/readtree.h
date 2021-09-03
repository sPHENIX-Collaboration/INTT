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
   TFile*          m_froot;
   TH2*            m_hitmap;
   TH1F*           singlemV ;
   TH1F*           hnhits   ;
   TH2F*           nhitsnChip   ;
   TH1F*           nclsnChip;
   TH1F*           SingleAdc;
   TH1F*           DAdc     ;
   TH1F*           TAdc     ;
   TH1F*					 m_noiseAdc;
   TH1F*           originalenergyloss             ;
   TH1F*           Wadc             ;
   TH2F*           NHITChip         ;
   TH2F*           chan_1hit        ;
   TH2F*           chan_1hit2       ;
 //  TH1F*           chan_1hit2        ;
   TH2F*           adc_0hit         ;
   TH2F*           multiclschip     ;
   TH1F*           NHITChipmV       ;
   TH1F*           SumDoublehitAdc2 ;
   TH1F*           SinglehitAdc2    ;
   TH1F*           SumTriplehitAdc2 ;
   TH2F*           Adc_2hit         ;
   TH2F*           chan_2hit        ;
   TH2F*           sa_correlation   ;
   TH1F*           bco_interval			;
   TH1F*           bcofull_interval	;
   TH1F*           adc_bcofull			;
   TH2F*           ELOSSDIFF 				;
   TH1F*           ELOSSDIFFSA 			;
   TH2F*           ELOSSTRAN 				;
   TH1F*           ELOSSTRANSA 			;
   TH2F*           CHANDIFF 				;
   TH2F*           CHANDIFFS 				;
   TH2F*           CHIPDIFF 				;
   TH2F*           ADCDIFF					;
   TH1F*           CHANDIFFSA1 			;
   TH1F*           CHANDIFFSA2 			;
   TH1F*           CHANDIFFSA3 			;
   TH1F*           CHANDIFFSA4 			;
   TH1F*           CHANDIFFSAh 			;
   TH1F*           CHIPDIFFSAh 			;
   TH2F*           ELOSSDIFF1 			;
   TH2F*           ELOSSDIFF2 			;
   TH2F*           ELOSSDIFF3 			;
   TH2F*           ELOSSDIFF4 			;
   TH1F*           ELOSSCLS1_8_1    ;                  
   TH1F*           ELOSSCLS1_6_1    ;
   TH1F*           ELOSSCLS2_8_1    ;
   TH1F*           ELOSSCLS2_6_1    ;
   TH1F*           ELOSSCLS1_8_2    ;
   TH1F*           ELOSSCLS1_6_2    ;
   TH1F*           ELOSSCLS2_8_2    ;
   TH1F*           ELOSSCLS2_6_2    ;
   TH1F*           ELOSSCLS1_8_3    ;
   TH1F*           ELOSSCLS1_6_3    ;
   TH1F*           ELOSSCLS2_8_3    ;
   TH1F*           ELOSSCLS2_6_3    ;
   TH1F*           ELOSSCLS1_8_4    ;
   TH1F*           ELOSSCLS1_6_4    ;
   TH1F*           ELOSSCLS2_8_4    ;
   TH1F*           ELOSSCLS2_6_4    ;
   TH2F*           ELOSSTRAN1 			;
   TH2F*           ELOSSTRAN2 			;
   TH2F*           ELOSSTRAN3				;
   TH2F*           ELOSSTRAN4				;
   TH2F*           ELOSSTRANS				;
	 TH1F*					 ELOSSCLS1_1		 	;  
	 TH1F*					 ELOSSCLS2_1		  ;
	 TH1F*					 ELOSSCLS1_2		 	; 
	 TH1F*					 ELOSSCLS2_2		 	; 
	 TH1F*					 ELOSSCLS1_3		 	; 
	 TH1F*					 ELOSSCLS2_3		 	; 
	 TH1F*					 ELOSSCLS1_4		 	; 
	 TH1F*					 ELOSSCLS2_4		 	; 
	 TH1F*					 ELOSSCLS1_S		 	; 
	 TH1F*					 ELOSSCLS2_S		 	; 
	 TH1F*					 ELOSSCLS1__S		 	; 
	 TH1F*					 ELOSSCLS2__S		 	; 
	 TH1F*					 NCLS6					 	; 
	 TH1F*					 NCLS8					 	; 
	 TH1F*					 NSTRIP6				 	; 
	 TH1F*					 NSTRIP8				 	; 
	 TH1F*					 RECELOSS1			 	; 
	 TH1F*					 RECELOSS2			 	; 
	 TH1F*					 RECELOSS3			 	; 
	 TH1F*					 ReNhit					 	; 
   int             m_jentry;
   int             m_prebco, m_prebcofull, m_prenhit;
   readtree(TTree *tree=0, const char* outfile="histo.root");
   virtual ~readtree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(const int entry=-1);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   void initHist();
   void init_histo();
   void process_hits(int nhit, int* chipArray, int* chanArray, int* adcArray, int* eventArray, int* bcoArray, int* bco_fullArray, double* summV_Ret, int* Nhits_Ret, int* chan_Ret, int* chip_Ret, int ncls_Ret, int(*adc_Ret)[10]);
   void end();
};
#endif
#ifdef readtree_cxx
readtree::readtree(TTree *tree, const char *outfile) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/Users/sphenix/Documents/INTT_testbench/data/fphx_raw_20200727-1917_0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/Users/sphenix/Documents/INTT_testbench/data/fphx_raw_20200727-1917_0.root");
      }
      f->GetObject("tree",tree);
   }
   Init(tree);
   m_froot = TFile::Open(outfile, "recreate");
   //initHist();
   init_histo();
   m_jentry=0;
   m_prebco=-1;
   m_prebcofull=-1;
   m_prenhit=-1;
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
