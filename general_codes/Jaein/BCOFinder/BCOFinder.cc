#include "InttEvent.h"
#include "InttEvent.cc"
#include "TH1D.h"
#include "TH2D.h"
#include <cdbobjects/CDBTTree.h>

R__LOAD_LIBRARY(libcdbobjects.so)

void bcofull_bco_lad_all(int rnumber, int fnumber);
void bcofull_bco_merged(int rnumber);


TH2D *h2_bco_ladder[8]; //histogram for BCO alignment check half ladder by half ladder
TH2D *h2_bco_ladder_cut[8]; // histogram after BCO cut 
//////////////////////////////////////////////////////////////////////////////
// Hits belongs to [peak+1,peak-1] (3BCO region) will survive after BCO cut //
// to ensure that we don't miss the hit due to sperated to 2BCK             //
//////////////////////////////////////////////////////////////////////////////


int n_event = 10000;
bool ADC0cut = false;
//Flag for ADC0 cut

std::string outputFilePath = "/sphenix/tg/tg01/commissioning/INTT/QA/bco_bcofull_difference/rootfile/2023/";
std::string cdboutputFilePath = "/sphenix/tg/tg01/commissioning/INTT/QA/bco_bcofull_difference/cdb/2023/";
//std::string outputFilePath = "./";
// Used for merged files
std::string merged_inputFile;
//std::string merged_inputFilePath = "/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00020867-0000_event_base_ana.root";
std::string merged_inputFilePath = "/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/";
// Used for unmerged files intt0 ~ intt7
std::string inputFilePath = "/sphenix/tg/tg01/commissioning/INTT/data/root_files/2023/";

void BCOFinder(int i = 20867, bool isMerged = true, bool ADC0cut_ = false)
{
  ADC0cut = ADC0cut_;
  gROOT->SetBatch(kTRUE);
  std::string outputfilename = outputFilePath + "ladder_" + to_string(i) + ".root";
  merged_inputFile = merged_inputFilePath+"beam_inttall-000"+to_string(i)+"-0000_event_base_ana.root";
  TFile *sf = new TFile(outputfilename.c_str(), "RECREATE");
  for (int j = 0; j < 8; j++)
  {
    h2_bco_ladder[j] = new TH2D(Form("h2_bco_felix_%d", j), Form("h2_bco_felix_%d", j), 14, 0, 14, 128, 0, 128);
    h2_bco_ladder_cut[j] = new TH2D(Form("h2_bco_felix_cut%d", j), Form("h2_bco_felix_cut%d", j), 14, 0, 14, 128, 0, 128);
  }
  TH2D* hist = new TH2D("hist","test hist",100,0,1,100,0,1);
  if (!isMerged)
  {
    for (int j = 0; j < 8; j++)
    {
      bcofull_bco_lad_all(i, j);
      sf->cd();
      h2_bco_ladder[j]->Write();
      h2_bco_ladder_cut[j]->Write();
    }
  }
  else
  {
    bcofull_bco_merged(i);
    sf->cd();
    for (int j = 0; j < 8; j++)
    {
      h2_bco_ladder[j]->Write();
      h2_bco_ladder_cut[j]->Write();
    }
  }

///////////////////////////////////////////////////////////
//              Creating CDBTTree                        //
///////////////////////////////////////////////////////////
  std::string cdboutputfilename = cdboutputFilePath + "cdb_bco_" + to_string(i) + ".root";
  CDBTTree* cdbttree = new CDBTTree(cdboutputfilename); 
  int size=0;
  for(int felix_server=0;felix_server<8;felix_server++)
  {
    for(int felix_channel=0;felix_channel<14;felix_channel++)
    {
      for(int bco_diff = 0;bco_diff<128;bco_diff++)
      {
        if(h2_bco_ladder_cut[felix_server]->GetBinContent(felix_channel+1,bco_diff+1)>0)
        {
          cdbttree->SetIntValue(size,"felix_server",felix_server); 
          cdbttree->SetIntValue(size,"felix_channel",felix_channel); 
          cdbttree->SetIntValue(size,"bco_diff",bco_diff);
          size++; 
        }
      }
    }
  }
  cdbttree->SetSingleIntValue("size",size);
  cdbttree->Commit();
  cdbttree->CommitSingle();
  cdbttree->WriteCDBTTree();
  delete cdbttree;
  sf->Close();
}
void bcofull_bco_merged(int runnumber)
{
  TFile *f;
  f = TFile::Open(merged_inputFile.c_str()); 
  if (!f)
  {
    std::cout<<"Canot find ROOT file!  " <<std::endl;
    std::cout<<merged_inputFile<<std::endl;
    return;
  }
  TTree *tree = (TTree *)f->Get("tree");
  cout << "check" << endl;
  TH2D *h1 = new TH2D("h1", "h1", 14, 0, 14, 128, 0, 128);
  TH2D *h2 = new TH2D("h2", "h2", 14, 0, 14, 128, 0, 128);
  // CHANGE
  InttEvent *evt = new InttEvent();
  tree->SetBranchAddress("event", &evt);
  Long64_t totalevt = tree->GetEntries();
  Long64_t increment = tree->GetEntries() / 10;
  int loop = n_event;
  if (n_event == -1)
    loop = totalevt;
  //Making the BCO distribution as a function of half ladder. 2D histogram(ladder, BCOFULL-BCO)
  for (int j = 0; j < loop; j++)
  {
    tree->GetEvent(j);
    for (int k = 0; k < evt->getNHits(); k++)
    {
      auto hit = evt->getHit(k);
      int in_chip = hit->chip_id;
      int in_chan = hit->chan_id;
      int in_module = hit->module;
      int in_bco = hit->bco;
      int in_adc = hit->adc;
      int in_pid = hit->pid;
      int felixnumber = in_pid-3001;
      Long64_t lo_bco_full = hit->bco_full;
      int bco_diff = (lo_bco_full & 0x7F) - in_bco;
      if(in_adc==0 && ADC0cut) continue;
      if (bco_diff < 0)
        h2_bco_ladder[felixnumber]->Fill(in_module, bco_diff + 128);
      else
        h2_bco_ladder[felixnumber]->Fill(in_module, bco_diff);
    }
  
  }
  for(int felixnumber = 0;felixnumber<8;felixnumber++)
  {
    int numXBins = h2_bco_ladder[felixnumber]->GetXaxis()->GetNbins();
    int numYBins = h2_bco_ladder[felixnumber]->GetYaxis()->GetNbins();

    // Find the maximum bin half ladder by half ladder//
    for (int binX = 1; binX <= numXBins; binX++)
    {
      double maxXValue = -1.0;
      int maxYBin = -1;
      for (int binY = 1; binY <= numYBins; binY++)
      {
        double binContent = h2_bco_ladder[felixnumber]->GetBinContent(binX, binY);
        if (binContent > maxXValue)
        {
          maxXValue = binContent;
          maxYBin = binY;
        }
      }
      h2_bco_ladder_cut[felixnumber]->SetBinContent(binX, maxYBin, maxXValue); // Fill the peak position in the 2D histogram( it will be used for BCO cut)
      // Check the closest bin content (peak-1, peak+1)
      int BinY1 = maxYBin - 1;
      if (maxYBin == 1)
        BinY1 = 128;
      int BinY2 = maxYBin + 1;
      if (maxYBin == 128)
        BinY2 = 1;
      double ClosestBinContent1 = h2_bco_ladder[felixnumber]->GetBinContent(binX, BinY1);
      double ClosestBinContent2 = h2_bco_ladder[felixnumber]->GetBinContent(binX, BinY2);
      h2_bco_ladder_cut[felixnumber]->SetBinContent(binX, BinY1, ClosestBinContent1);
      h2_bco_ladder_cut[felixnumber]->SetBinContent(binX, BinY2, ClosestBinContent2);
    }
    h2_bco_ladder[felixnumber]->SetTitle(Form("run %d felix %d evt %d", runnumber, felixnumber, n_event));
    h2_bco_ladder[felixnumber]->SetXTitle("module");
    h2_bco_ladder[felixnumber]->SetYTitle("BCO_Full - BCO");
    h2_bco_ladder_cut[felixnumber]->SetTitle(Form("run %d ", runnumber));
    h2_bco_ladder_cut[felixnumber]->SetXTitle("module");
    h2_bco_ladder_cut[felixnumber]->SetYTitle("BCO_Full - BCO");

    cout << "felix=" << felixnumber << "  run=" << runnumber << endl;
  }
  return;
}

void bcofull_bco_lad_all(int rnumber, int fnumber)
{
  const int felixnumber = fnumber;
  const int runnumber = rnumber;
  TFile *f;
  std::string inputfile_beam = inputFilePath + "beam_intt"+to_string(fnumber)+"-000"+to_string(rnumber)+"-0000_event_base.root";
  std::string inputfile_cosmics = inputFilePath + "cosmics_intt"+to_string(fnumber)+"-000"+to_string(rnumber)+"-0000_event_base.root";
  f = TFile::Open(inputfile_beam.c_str());
  if (!f)
  {
    f = TFile::Open(inputfile_cosmics.c_str());
  }
  if (!f)
  {
    std::cout<<"ROOT file are not found! Skip this Felix server! " <<fnumber<<" Run number : "<<rnumber<<std::endl;
    return;
  }
  TTree *tree = (TTree *)f->Get("tree");
  cout << "check" << endl;
  TH2D *h1 = new TH2D("h1", "h1", 14, 0, 14, 128, 0, 128);
  TH2D *h2 = new TH2D("h2", "h2", 14, 0, 14, 128, 0, 128);
  // CHANGE
  InttEvent *evt = new InttEvent();
  tree->SetBranchAddress("event", &evt);
  Long64_t totalevt = tree->GetEntries();
  Long64_t increment = tree->GetEntries() / 10;
  int loop = n_event;
  if (n_event == -1)
    loop = totalevt;

  //Making the BCO distribution as a function of half ladder. 2D histogram(ladder, BCOFULL-BCO)
  for (int j = 0; j < loop; j++)
  {
    tree->GetEvent(j);
    for (int k = 0; k < evt->getNHits(); k++)
    {
      auto hit = evt->getHit(k);
      int in_chip = hit->chip_id;
      int in_chan = hit->chan_id;
      int in_module = hit->module;
      int in_bco = hit->bco;
      int in_adc = hit->adc;
      Long64_t lo_bco_full = hit->bco_full;
      int bco_diff = (lo_bco_full & 0x7F) - in_bco;
      if(in_adc==0 && ADC0cut) continue;
      if (bco_diff < 0)
        h2_bco_ladder[felixnumber]->Fill(in_module, bco_diff + 128);
      else
        h2_bco_ladder[felixnumber]->Fill(in_module, bco_diff);
    }
  
  }
  int numXBins = h2_bco_ladder[felixnumber]->GetXaxis()->GetNbins();
  int numYBins = h2_bco_ladder[felixnumber]->GetYaxis()->GetNbins();

//Find the maximum bin half ladder by half ladder//
  for (int binX = 1; binX <= numXBins; binX++) {
    double maxXValue = -1.0;  
    int maxYBin=-1;
    for (int binY = 1; binY <= numYBins; binY++) {
      double binContent = h2_bco_ladder[felixnumber]->GetBinContent(binX, binY);
      if (binContent > maxXValue) {
        maxXValue = binContent;
        maxYBin = binY;
        }
      }
      h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,maxYBin,maxXValue); // Fill the peak position in the 2D histogram( it will be used for BCO cut)
  ////////////////////////////////////////////////
      
      //Test for BCO region//
      
     // h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,maxYBin+1,h2_bco_ladder[felixnumber]->GetBinContent(binX,maxYBin+1));
     // h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,maxYBin+2,h2_bco_ladder[felixnumber]->GetBinContent(binX,maxYBin+2));
     // h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,maxYBin+3,h2_bco_ladder[felixnumber]->GetBinContent(binX,maxYBin+3));
     // h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,maxYBin+4,h2_bco_ladder[felixnumber]->GetBinContent(binX,maxYBin+4));
      
      ///////////////////////
      
      // Check the closest bin content (peak-1, peak+1)
      // If (# of hit of bin content) > (# of hit of BCO peak) * 0.3 (30%) I'll peak up this one. 
      int BinY1 = maxYBin-1; if(maxYBin==1) BinY1 = 128;
      int BinY2 = maxYBin+1; if(maxYBin==128) BinY2 = 1;
      double ClosestBinContent1 = h2_bco_ladder[felixnumber]->GetBinContent(binX,BinY1);
      double ClosestBinContent2 = h2_bco_ladder[felixnumber]->GetBinContent(binX,BinY2);
  //   if(ClosestBinContent1 > maxXValue*0.3) h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,BinY1,ClosestBinContent1);
  //  if(ClosestBinContent2 > maxXValue*0.3) h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,BinY2,ClosestBinContent2);
    h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,BinY1,ClosestBinContent1);
    h2_bco_ladder_cut[felixnumber]->SetBinContent(binX,BinY2,ClosestBinContent2);
    }
  h2_bco_ladder[felixnumber]->SetTitle(Form("run %d felix %d evt %d", rnumber, felixnumber,n_event ));
  h2_bco_ladder[felixnumber]->SetXTitle("module");
  h2_bco_ladder[felixnumber]->SetYTitle("BCO_Full - BCO");
  h2_bco_ladder_cut[felixnumber]->SetTitle(Form("run %d ", rnumber));
  h2_bco_ladder_cut[felixnumber]->SetXTitle("module");
  h2_bco_ladder_cut[felixnumber]->SetYTitle("BCO_Full - BCO");

  cout << "felix=" << felixnumber << "  run=" << runnumber << endl;

  return;
}
