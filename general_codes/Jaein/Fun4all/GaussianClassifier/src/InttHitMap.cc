#include "InttHitMap.h"

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2.h"

#include "InttEvent.h"

// Constructor
InttHitMap::InttHitMap(const std::string &name, const std::string &filename, const std::string &filename2,int nevents)
    : SubsysReco(name), inFile_(nullptr), outFile_(nullptr), tree_(nullptr), inttEvt_(nullptr),ievent_(0)
{
  fname_ = filename;
  outfname_ = filename2;
  nevents_ = nevents;
}

// Destructor
InttHitMap::~InttHitMap()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttHitMap::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttHitMap" << std::endl;
  }
  outFile_ = new TFile(outfname_.c_str(), "RECREATE");
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 14; j++)
    {
      h2_AllMap_[i][j] = new TH2D(Form("HitMap_%d_%d", i, j), Form("Normalized_HitMap_%d_%d", i, j), 128, 0, 128, 27, 0, 27);
    }
  }
  
  return 0;
}

int InttHitMap::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttHitMap" << std::endl;
  }

  if (!topNode)
  {
    std::cout << "InttHitMap::InitRun(PHCompositeNode* topNode)" << std::endl;
    std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

    return -1;
  }
  inFile_ = TFile::Open(fname_.c_str());
  if (inFile_ == nullptr)
  {
    std::cout << "could not find input file" << std::endl;
  }
  tree_ = (TTree *)inFile_->Get("tree");
  if (tree_ == nullptr)
  {
    std::cout << "could not find input file" << std::endl;
  }
  inttEvt_ = nullptr;
  tree_->SetBranchAddress("event", &inttEvt_);
  ievent_ = 0;
  
  std::cout<<"Total events : "<<tree_->GetEntries()<<std::endl;
  std::cout<<"nevents : "<<nevents_<<std::endl;
  if(nevents_>tree_->GetEntries())
  {
    std::cout<<"nevents is higher than total events."<<std::endl;
    std::cout<<"nevents is changed to total events."<<std::endl;
    nevents_ = tree_->GetEntries();
  }
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitMap::process_event(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in AnaTutorial" << std::endl;
  }
  
  
  if (ievent_ >= tree_->GetEntries())
  {
    std::cout << "all events are already processed." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
//  nevents_ = 1000;
  if (ievent_ >= nevents_)
  {
    std::cout << "Last event is processed." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (ievent_ % 100 == 0)
  {
    std::cout << "ievent/nevents : [" << ievent_ << "/"<<nevents_<<"]"<<std::endl;
  }
  tree_->GetEntry(ievent_++);
  memset(IsCloneHit_, false, sizeof(IsCloneHit_));
  for (int n = 0; n < inttEvt_->getNHits(); ++n)
  {
    InttHit *hit = inttEvt_->getHit(n);
    int in_felix = hit->pid - 3001;
    int in_chip = hit->chip_id;
    int in_chan = hit->chan_id;
    int in_module = hit->module;
    // int in_adc = hit->adc; //not used
    int in_barrel = hit->barrel;
    int in_bco = hit->bco;
    Long64_t lo_bco_full = hit->bco_full;
    double norm_factor = 0.; // Normalization factor

    if (isBCOcutON_ && (inBCOFile_!=nullptr))
    {
      if(!isBCOPeak(in_felix,in_module,in_bco,lo_bco_full))
      {
        continue;
      }
    }

    // logic for Clone hit remover
    if (IsCloneHit_[in_felix][in_module][in_chip][in_chan])
      continue;
    else
    {
      IsCloneHit_[in_felix][in_module][in_chip][in_chan] = true;
    }
    if (in_chip < 6 || (in_chip > 13 && in_chip < 19)) // Condition for Type B
    {

      norm_factor = (nevents_);
      if (in_barrel == 1) // Inner barrel(= 0) : 7.188[cm], Outer barrel(= 1) : 9.680[cm]
      {
        norm_factor = norm_factor / (9.680 / 7.1888);
      }
      h2_AllMap_[in_felix][in_module]->Fill(in_chan, in_chip, 1. / norm_factor);
    }
    else
    {
      norm_factor = (nevents_) / 1.25; // Normailzed by chip type
      if (in_barrel == 1)              // Inner barrel(= 0) : 7.188[cm], Outer barrel(= 1) : 9.680[cm]
      {
        norm_factor = norm_factor / (9.680 / 7.1888);
      }
      h2_AllMap_[in_felix][in_module]->Fill(in_chan, in_chip, 1. / (norm_factor));
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitMap::End(PHCompositeNode * /*topNode*/)
{

  if (Verbosity() > 1)
  {
    std::cout << "Processing InttHitMap done" << std::endl;
  }
  if (outFile_ != nullptr)
  
  outFile_->Write();
  outFile_->Close();
  return 0;
}

int InttHitMap::SetBCOfile(const char* bcofile)
{
  if(!isBCOcutON_)
  {
    std::cout<<"BCO cut option is OFF. isBCOcutON_ == false)"<<std::endl;
    return 0;
  }
  bcofname_ = std::string(bcofile);
  inBCOFile_ = TFile::Open(bcofname_.c_str());
  if(inBCOFile_ == nullptr)
  {
    std::cout<<"BCO file is not sucessfully loaded."<<std::endl;
    return -1;
  }
 
  for(int i=0;i<8;i++)
  {
    h2_bco_cut_[i] = (TH2D*)inBCOFile_->Get(Form("h2_bco_felix_cut%d",i));
  }
  return 0;
}

bool InttHitMap::isBCOPeak(int felix,int ladder, int bco, Long64_t bcofull)
{
  int bco_diff = (bcofull & 0x7F) - bco;
  if (bco_diff < 0) 
  {
    bco_diff = bco_diff + 128;
  }
  //Main part for BCO cut.
  //Load the BCO histogram and apply the BCO cut 
  //Default : Hits belongs to [peak-1,peak+1] (3BCO) region will survice after BCO cut
  //To change the BCO region, h2_bco_cut_[felix] needs to be modified
  if(h2_bco_cut_[felix]->GetBinContent(ladder+1,bco_diff+1)!=0) 
  {
    return true; 
  }
  return false;
}