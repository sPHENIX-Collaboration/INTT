#include "InttCosmicsBCOFinder.h"

#include <cdbobjects/CDBTTree.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>

#include <iostream>
#include <vector>
#include <string>

R__LOAD_LIBRARY(libintt.so)
R__LOAD_LIBRARY(libcdbobjects.so)

InttCosmicsBCOFinder::InttCosmicsBCOFinder(const std::string &name, const std::string &filename, const std::string &filename2, int nevents)
    : SubsysReco(name), outFile_(nullptr), tree_(nullptr), ievent_(0)
{
  outfname_ = filename;
  cdbname_ = filename2;
  nevents_ = nevents;
  for (int felix_server = 0; felix_server < 8; felix_server++)
  {
    for (int felix_channel = 0; felix_channel < 14; felix_channel++)
    {
      for (int chip = 0; chip < 26; chip++)
      {
        for (int chan = 0; chan < 128; chan++)
        {
          m_hot[felix_server][felix_channel][chip][chan] = 0;
        }
      }
    }
  }
}

// Destructor
InttCosmicsBCOFinder::~InttCosmicsBCOFinder()
{
}
/**
 * Initialize the module and prepare looping over events
 */
int InttCosmicsBCOFinder::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttCosmicsBCOFinder" << std::endl;
  }
  for (int j = 0; j < 8; j++)
  {
    h2_bco_ladder_[j] = new TH2D(Form("h2_bco_felix_%d", j), Form("h2_bco_felix_%d", j), 14, 0, 14, 128, 0, 128);
    h2_bco_ladder_cut_[j] = new TH2D(Form("h2_bco_felix_cut%d", j), Form("h2_bco_felix_cut%d", j), 14, 0, 14, 128, 0, 128);
  }
  return 0;
}

int InttCosmicsBCOFinder::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttCosmicsBCOFinder" << std::endl;
  }

  if (!topNode)
  {
    std::cout << "InttCosmicsBCOFinder::InitRun(PHCompositeNode* topNode)" << std::endl;
    std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

    return 1;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttCosmicsBCOFinder::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "InttCosmicsBCOFinder::Beginning process_event in InttCosmicsBCOFinder.cc" << std::endl;
  }

  if (ievent_ >= nevents_)
  {
    if (Verbosity() > 5)
    {
      std::cout << "InttCosmicsBCOFinder::Last event is processed." << std::endl;
    }
    return Fun4AllReturnCodes::EVENT_OK;
    // return Fun4AllReturnCodes::ABORTRUN;
  }
  if (ievent_ % 100 == 0)
  {
    std::cout << "ievent/nevents : [" << ievent_ << "/" << nevents_ << "]" << std::endl;
  }
  InttRawHitContainer *inttcont = findNode::getClass<InttRawHitContainer>(topNode, m_InttRawNodeName);
  if (!inttcont)
  {
    std::cout << PHWHERE << std::endl;
    std::cout << "InttCosmicsBCOFinder::process_event(PHCompositeNode* topNode)" << std::endl;
    std::cout << "Could not get \"" << m_InttRawNodeName << "\" from Node Tree" << std::endl;
    std::cout << "Exiting" << std::endl;

    gSystem->Exit(1);
    exit(1);
  }
  for (unsigned int i = 0; i < inttcont->get_nhits(); i++)
  {
    InttRawHit *intthit = inttcont->get_hit(i);
    // if (!intthit)
    //   continue;
    uint64_t bco_full = intthit->get_bco();
    if (i == 0)
      std::cout << bco_full << std::endl;
    int bco = intthit->get_FPHX_BCO();
    int felixnumber = intthit->get_packetid() - 3001;
    int felixchannel = intthit->get_fee();
    int chip = (intthit->get_chip_id() + 25) % 26;
    int chan = intthit->get_channel_id();

    if (m_hot[felixnumber][felixchannel][chip][chan]==1)
    {
      // std::cout<<"hotchan removed : "<<raw.felix_server<<" "<<raw.felix_channel<<" "<<raw.chip<<" "<<raw.channel<<std::endl;
      continue;
    }

    // std::cout << bco << " " << felixnumber << std::endl;
    int bco_diff = (bco_full & 0x7FU) - bco;

    if (bco_diff < 0)
      h2_bco_ladder_[felixnumber]->Fill(felixchannel, bco_diff + 128);
    else
      h2_bco_ladder_[felixnumber]->Fill(felixchannel, bco_diff);
  }
  ievent_++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttCosmicsBCOFinder::End(PHCompositeNode * /*topNode*/)
{
  FindBCOPeak();
  if (WriteCDBTTree_)
  {
    cdbttree_ = new CDBTTree(cdbname_);
    int size = 0;
    if (Verbosity() > 1)
    {
      std::cout << "Creating CDBTTree..." << std::endl;
    }
    for (int felix_server = 0; felix_server < 8; felix_server++)
    {
      for (int felix_channel = 0; felix_channel < 14; felix_channel++)
      {
        for (int bco_diff = 0; bco_diff < 128; bco_diff++)
        {
          if (h2_bco_ladder_cut_[felix_server]->GetBinContent(felix_channel + 1, bco_diff + 1) > 0)
          {
            cdbttree_->SetIntValue(size, "felix_server", felix_server);
            cdbttree_->SetIntValue(size, "felix_channel", felix_channel);
            cdbttree_->SetIntValue(size, "bco_diff", bco_diff);
            size++;
          }
        }
      }
    }

    cdbttree_->SetSingleIntValue("size", size);
    cdbttree_->Commit();
    cdbttree_->CommitSingle();
    cdbttree_->WriteCDBTTree();
  }

  delete cdbttree_;

  if (Verbosity() > 1)
  {
    std::cout << "Processing InttCosmicsBCOFinder done" << std::endl;
  }
  if (WriteQAFile_)
  {
    outFile_ = new TFile(outfname_.c_str(), "RECREATE");
    if (outFile_ != nullptr)
    {
      outFile_->cd();
      for (int j = 0; j < 8; j++)
      {
        h2_bco_ladder_[j]->Write();
        h2_bco_ladder_cut_[j]->Write();
      }
      outFile_->Write();
    }
    outFile_->Close();
  }
  return 0;
}

void InttCosmicsBCOFinder::FindBCOPeak()
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
int InttCosmicsBCOFinder::LoadHotChannelMapLocal(std::string const & filename)
{
  if (filename.empty())
  {
    std::cout << "int InttCosmicsBCOFinder::LoadHotChannelMapLocal(std::string const& filename)" << std::endl;
    std::cout << "\tArgument 'filename' is empty string" << std::endl;
    return 1;
  }

  if (!std::filesystem::exists(filename))
  {
    std::cout << "int InttCosmicsBCOFinder::LoadHotChannelMapLocal(std::string const& filename)" << std::endl;
    std::cout << "\tFile '" << filename << "' does not exist" << std::endl;
    return 1;
  }

  CDBTTree cdbttree(filename);
  // need to checkt for error exception
  cdbttree.LoadCalibrations();

  uint64_t N = cdbttree.GetSingleIntValue("size");
  for (uint64_t n = 0; n < N; ++n)
  {
    int felix_server = cdbttree.GetIntValue(n, "felix_server");
    int felix_channel = cdbttree.GetIntValue(n, "felix_channel");
    int chip = cdbttree.GetIntValue(n, "chip");
    int channel = cdbttree.GetIntValue(n, "channel");
    m_hot[felix_server][felix_channel][chip][channel] = 1;
  }

  // if(Verbosity() < 1)
  // {
  //    continue;
  // }
  // std::cout << "Masking channel:\n" << std::endl;
  // std::cout << "\t" << cdbttree.GetIntValue(n, "felix_server")
  //           << "\t" << cdbttree.GetIntValue(n, "felix_channel")
  //           << "\t" << cdbttree.GetIntValue(n, "chip")
  //           << "\t" << cdbttree.GetIntValue(n, "channel") << std::endl;
  return 0;
}
