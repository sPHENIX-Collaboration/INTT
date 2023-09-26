#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"

int chip = 26;
int mod = 14;
double ratio_cut = 0.05; // ratio cut
using namespace std;
bool save_root = false;
std::string root_input_path = "/sphenix/tg/tg01/commissioning/INTT/root_files/";
std::string root_output_path = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/HotChannelFinder/rootfile/";
std::string txt_output_path = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/HotChannelFinder/hotchannelmap/";

// If you just would like to find the mask channels for specific Felix Channel, change FC = {Felix Channel} part.
// Default is FC = -1, default setting finds the mask channels for all Felix Channels.
void HotChannel(int runnumber = -1, int inttserver = 1, int FC = -1, std::string runtype = "cosmics")
{
  bool bFC = false;
  if (FC != -1)
    bFC = true;
  cout << "inttserver(daq) : " << inttserver << endl;
  std::string filename = runtype + "_intt" + to_string(inttserver) + "-000" + to_string(runnumber) + "-0000.root";
  std::string rootfilename1 = root_input_path + "cosmics_intt" + to_string(inttserver) + "-000" + to_string(runnumber) + "-0000.root";
  std::string rootfilename2 = root_input_path + "beam_intt" + to_string(inttserver) + "-000" + to_string(runnumber) + "-0000.root";
  std::string rootfilename3 = root_input_path + "intt_intt" + to_string(inttserver) + "-000" + to_string(runnumber) + "-0000.root";
  //  std::string rootfilename = root_input_path + runtype +"_intt" + to_string(inttserver) + "-000" + to_string(runnumber) + "-0000.root";
  TFile *file = nullptr;
  file = TFile::Open(rootfilename1.c_str(), "READ");
  if (!file)
  {
    file = TFile::Open(rootfilename2.c_str(), "READ");
  }
  if (!file)
    file = TFile::Open(rootfilename3.c_str(), "READ");

  std::size_t pos;
  pos = filename.find(".");
  std::string outfile = filename.substr(0, pos);
  outfile += "_out.root";
  outfile = root_output_path + outfile;
  cout << outfile << endl;
  std::string outfiletxtname = txt_output_path + "intt" + to_string(inttserver) + "-000" + to_string(runnumber) + "-0000_hot_list.txt";
  std::ofstream outfiletxt(outfiletxtname);
  cout << outfiletxtname << endl;
  TTree *tree = (TTree *)file->Get("tree");
  int module, chan_id, chip_id;
  tree->SetBranchAddress("module", &module);
  tree->SetBranchAddress("chan_id", &chan_id);
  tree->SetBranchAddress("chip_id", &chip_id);
  tree->SetBranchStatus("*", 0);
  tree->SetBranchStatus("module", 1);
  tree->SetBranchStatus("chip_id", 1);
  tree->SetBranchStatus("chan_id", 1);
  TH1D *h_chip[mod][chip + 1];
  for (int i = 0; i < 14; i++)
  {
    for (int j = 0; j < 27; j++)
    {
      h_chip[i][j] = new TH1D(Form("module_%d_chip_%d", i, j), Form("module_%d_chip_%d", i, j), 128, 0, 128);
    }
  }
  double TotalNumber = tree->GetEntries();
  Long64_t increment = TotalNumber / 10;
  for (Long64_t n = 0; n < TotalNumber; n++)
  {
    if (n % increment == 0)
    {
      int progress = (n * 100) / TotalNumber;
      std::cout << "Progress: " << progress << "%" << std::endl;
    }
    tree->GetEntry(n);
    h_chip[module][chip_id]->Fill(chan_id);
  }

  double NumberOfChannels = 0;
  for (int i = 0; i < 14; i++)
  {
    for (int j = 0; j < 27; j++)
    {
      for (int chan = 0; chan < 128; chan++)
      {
        if (h_chip[i][j]->GetBinContent(chan + 1) != 0)
          NumberOfChannels++;
      }
    }
  }

  std::cout << "# of Total Hits  " << TotalNumber << std::endl;
  std::cout << " # of chans " << NumberOfChannels << std::endl;
  double MeanOfTotalHits = TotalNumber / NumberOfChannels;
  std::cout << "mean " << MeanOfTotalHits << std::endl;
  outfiletxt << "mean " << MeanOfTotalHits << "# of Total Hits  " << TotalNumber << " # of chans " << NumberOfChannels << endl;
  TFile *ofile = TFile::Open(outfile.c_str(), "RECREATE");
  TTree *st = new TTree("tree", "tree");
  Long64_t ch_entry = 0;
  int module_, chip_id_, server_;
  st->Branch("ch_entry", &ch_entry);
  st->Branch("module", &module_);
  st->Branch("chip_id", &chip_id_);
  ofile->cd();

  for (int i = 0; i < 14; i++)
  {
    if (bFC && i != FC)
      continue;
    for (int j = 0; j < 27; j++)
    {
      double tothit = h_chip[i][j]->GetEntries();
      h_chip[i][j]->Write();
      if (tothit == 0)
        continue;
      for (int chan = 0; chan < 128; chan++)
      {
        module_ = i;
        chip_id_ = j;
        ch_entry = h_chip[i][j]->GetBinContent(chan + 1);
        if (ch_entry != 0)
          st->Fill();
        double ratio = (double)ch_entry / tothit; // Not used for now
        {
          int FelixCh_ID = i; // 0~13
          int chip_ID = j;    // 1 ~ 26
          int chan_ID = chan; // 0~127
          if (ch_entry > MeanOfTotalHits * 3.0)
            outfiletxt << 3.0 << " " << FelixCh_ID << " " << chip_ID << " " << chan_ID << " "
                       << " " << ch_entry << endl;
          else if (ch_entry > MeanOfTotalHits * 2.5)
          {
            outfiletxt << 2.5 << " " << FelixCh_ID << " " << chip_ID << " " << chan_ID << " "
                       << " " << ch_entry << endl;
          }
          else if (ch_entry > MeanOfTotalHits * 2.0)
          {
            outfiletxt << 2.0 << " " << FelixCh_ID << " " << chip_ID << " " << chan_ID << " "
                       << " " << ch_entry << endl;
          }
          else if (ch_entry > MeanOfTotalHits * 1.5)
          {
            outfiletxt << 1.5 << " " << FelixCh_ID << " " << chip_ID << " " << chan_ID << " "
                       << " " << ch_entry << endl;
		  }
		  else if (ch_entry > MeanOfTotalHits * 1.4)
		  {
			outfiletxt << 1.3 << " " << FelixCh_ID << " " << chip_ID << " " << chan_ID << " "
			  << " " << ch_entry << endl;
		  }
		  else if (ch_entry > MeanOfTotalHits * 1.3)
		  {
			outfiletxt << 1.3 << " " << FelixCh_ID << " " << chip_ID << " " << chan_ID << " "
			  << " " << ch_entry << endl;
		  }
		}
	  }
    }
  }
  st->Write();
  ofile->Close();
}
