#include "InttBCOFinder.h"

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <iostream>
#include <vector>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"

InttBCOFinder::InttBCOFinder(const std::string &name, const std::string &filename, const std::string &filename2, int nevents)
    : SubsysReco(name), inFile_(nullptr), outFile_(nullptr), tree_(nullptr), ievent_(0)
{
  fname_ = filename;
  outfname_ = filename2;
  nevents_ = nevents;
}

// Destructor
InttBCOFinder::~InttBCOFinder()
{
}
/**
 * Initialize the module and prepare looping over events
 */
int InttBCOFinder::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttBCOFinder" << std::endl;
  }
  outFile_ = new TFile(outfname_.c_str(), "RECREATE");
  for (int j = 0; j < 8; j++)
  {
    h2_bco_ladder_[j] = new TH2D(Form("h2_bco_felix_%d", j), Form("h2_bco_felix_%d", j), 14, 0, 14, 128, 0, 128);
    h2_bco_ladder_cut_[j] = new TH2D(Form("h2_bco_felix_cut%d", j), Form("h2_bco_felix_cut%d", j), 14, 0, 14, 128, 0, 128);
  }

  return 0;
}

int InttBCOFinder::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttBCOFinder" << std::endl;
  }

  if (!topNode)
  {
    std::cout << "InttBCOFinder::InitRun(PHCompositeNode* topNode)" << std::endl;
    std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

    return -1;
  }
  inFile_ = TFile::Open(fname_.c_str());
  if (inFile_ == nullptr)
  {
    std::cout << "could not find input file" << std::endl;
  }
  tree_ = (TTree *)inFile_->Get("intt_tree");
  if (tree_ == nullptr)
  {
    std::cout << "could not find input file" << std::endl;
  }

  tree_->SetBranchAddress("bco", &bco_);
  tree_->SetBranchAddress("adc", &adc_);
  tree_->SetBranchAddress("chn", &chn_);
  tree_->SetBranchAddress("chp", &chp_);
  tree_->SetBranchAddress("fee", &fee_);
  tree_->SetBranchAddress("pid", &pid_);
  tree_->SetBranchAddress("gtm", &gtm_);

  tree_->SetBranchStatus("*", 0);
  tree_->SetBranchStatus("adc", 1);
  tree_->SetBranchStatus("fee", 1);
  tree_->SetBranchStatus("pid", 1);
  tree_->SetBranchStatus("bco", 1);
  tree_->SetBranchStatus("gtm", 1);

  std::cout << "Total events : " << tree_->GetEntries() << std::endl;
  std::cout << "nevents : " << nevents_ << std::endl;
  if (nevents_ > tree_->GetEntries())
  {
    std::cout << "nevents is higher than total events." << std::endl;
    std::cout << "nevents is changed to total events." << std::endl;
    nevents_ = tree_->GetEntries();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttBCOFinder::process_event(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in AnaTutorial" << std::endl;
  }

  if (ievent_ >= tree_->GetEntries() )
  {
    std::cout << "all events are already processed." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  //  nevents_ = 1000;
  if (ievent_ >= nevents_ )
  {
    std::cout << "Last event is processed." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if (ievent_ % 100 == 0 )
  {
    std::cout << "ievent/nevents : [" << ievent_ << "/" << nevents_ << "]" << std::endl;
  }
  tree_->GetEntry(ievent_++);
  std::cout<<bco_->size()<<std::endl;
  for (size_t k = 0; k < bco_->size(); k++)
  {
    //int in_chip = (*chp_)[k]; unused
    //int in_chan = (*chn_)[k]; unused
    int in_module = (*fee_)[k];
    int in_bco = (*bco_)[k];
    int in_adc = (*adc_)[k];
    int in_pid = (*pid_)[k];
    int felixnumber = in_pid - 3001;
    Long64_t lo_bco_full = gtm_;
    int bco_diff = (lo_bco_full & 0x7F) - in_bco;
    if (in_adc == 0 && IsADCcutON_)
    {
      std::cout << "HELLO" << std::endl;
      continue;
    }
    if (bco_diff < 0)
      h2_bco_ladder_[felixnumber]->Fill(in_module, bco_diff + 128);
    else
      h2_bco_ladder_[felixnumber]->Fill(in_module, bco_diff);
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttBCOFinder::End(PHCompositeNode * /*topNode*/)
{
  FindBCOPeak();
//  if (Verbosity() > 1)
  {
    std::cout << "Processing InttBCOFinder done" << std::endl;
  }
  if (outFile_ != nullptr)
  {
      outFile_->Write();
  }
  outFile_->Close();
  return 0;
}

void InttBCOFinder::FindBCOPeak()
{
  for (int felixnumber = 0; felixnumber < 8; felixnumber++)
  {
    int numXBins = h2_bco_ladder_[felixnumber]->GetXaxis()->GetNbins();
    int numYBins = h2_bco_ladder_[felixnumber]->GetYaxis()->GetNbins();

    // Find the maximum bin half ladder by half ladder//
    for (int binX = 1; binX <= numXBins; binX++)
    {
      double maxXValue = -1.0;
      int maxYBin = -1;
      for (int binY = 1; binY <= numYBins; binY++)
      {
        double binContent = h2_bco_ladder_[felixnumber]->GetBinContent(binX, binY);
        if (binContent > maxXValue)
        {
          maxXValue = binContent;
          maxYBin = binY;
        }
      }
      h2_bco_ladder_cut_[felixnumber]->SetBinContent(binX, maxYBin, maxXValue); // Fill the peak position in the 2D histogram( it will be used for BCO cut)
      // Check the closest bin content (peak-1, peak+1)
      int BinY1 = maxYBin - 1;
      if (maxYBin == 1)
        BinY1 = 128;
      int BinY2 = maxYBin + 1;
      if (maxYBin == 128)
        BinY2 = 1;
      double ClosestBinContent1 = h2_bco_ladder_[felixnumber]->GetBinContent(binX, BinY1);
      double ClosestBinContent2 = h2_bco_ladder_[felixnumber]->GetBinContent(binX, BinY2);
      h2_bco_ladder_cut_[felixnumber]->SetBinContent(binX, BinY1, ClosestBinContent1);
      h2_bco_ladder_cut_[felixnumber]->SetBinContent(binX, BinY2, ClosestBinContent2);
    }
    h2_bco_ladder_[felixnumber]->SetTitle(Form("felix %d evt %d", felixnumber, ievent_));
    h2_bco_ladder_[felixnumber]->SetXTitle("module");
    h2_bco_ladder_[felixnumber]->SetYTitle("BCO_Full - BCO");
    h2_bco_ladder_cut_[felixnumber]->SetTitle(Form("felix %d evt %d cut", felixnumber, ievent_));
    h2_bco_ladder_cut_[felixnumber]->SetXTitle("module");
    h2_bco_ladder_cut_[felixnumber]->SetYTitle("BCO_Full - BCO");
  }
}
