#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cdbobjects/CDBTTree.h>

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"

R__LOAD_LIBRARY(libcdbobjects.so)

using namespace std;
//////////////
// Funtions //
//////////////
double SingleGaussianFit(TH1D *hist, double &mean1, double &sigma1); 
//Function to do fitting return value : value for HotChannel cut

void GetFirstFilledBinCenter(TH1D *hist, double &a, double &b);      
//Function to find bin center not used for now.                                                                    

/////////////////////
//Global parameters//
/////////////////////
int chip = 26;
int mod = 14;
double sig_cut = 3.0;
bool Writecsv = false;
//chip : # of chips on each half ladder(defalt : 26)
//mod : # of moudles for each felix server(default : 14)
//sig_cut : sigma cut for hot/cold channel determination default : 4.0
//Writecsv : flag to write csv file (default : false )

/////////////
//File Path//
/////////////
std::string map_input_path =  "./";
std::string root_output_path = "./";
std::string cdb_output_path = "./";
//std::string csv_output_path = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/cosmic/HitMap/GaussianClassifier/macro/";
//map_input_path : location of the HitMap file 
//root_output_path : output file path
//cdb_output_path : cdb output file path 
//csv_output_path : csv output file path (used for Grafana online monitoring)


struct Half_Chip {
  int felix_id_;
  int module_id_;
  int chip_id_;
};
////////////////////////////
//List of Half entry chips//
////////////////////////////
std::vector<Half_Chip> half_chips = // Chip number 0~25
    {
        // Felix 0
        {0, 7, 14},
        // Felix 2
        {2, 9, 15},
        // Felix 3
        {3, 13, 20},
        {3, 13, 22},
        {3, 13, 24},
        // Felix 7
        {7, 0, 0},
        {7, 0, 1},
        {7, 0, 2},
        {7, 0, 3},
        {7, 0, 4},
        {7, 0, 13},
        {7, 0, 14},
        {7, 0, 15},
        {7, 0, 16},
        {7, 0, 17},
        {7, 0, 6},
        {7, 0, 8},
        {7, 0, 10},
        {7, 0, 12},
        {7, 0, 19},
        {7, 0, 21},
        {7, 0, 23},
        {7, 0, 25},
        {7, 1, 0},
        {7, 1, 1},
};

void InttChannelClassifier_cosmics(int runnumber = 38554) //runnumber
{
  gStyle->SetOptFit();
  ////////////////////////////////////////
  // Load HitMap                        //
  ////////////////////////////////////////
  int in_sig = 10*sig_cut;
  std::string rootfilename = map_input_path + "hitmap_run" + to_string(runnumber) + ".root";
  std::string cdbttree_name = cdb_output_path + "cdb_"+to_string(runnumber)+".root";
  TFile *file = nullptr;
  file = TFile::Open(rootfilename.c_str(), "READ");
  
  
  ///////////////////////////////////////////////////
  // Create Histogram for check MPV hit rate       //
  ///////////////////////////////////////////////////

  TH1D* h1_hist_MPV = new TH1D("hist_MPV","hist_MPV",200, 1, 201);

  ///////////////////////////////////////////////////
  // Create Histogram for Hot and Dead channel map //
  ///////////////////////////////////////////////////
  
  TH2D *h2_AllMap[8][14]; // Original HitMap
  TH2D *h2_ColdMap[8][14];// 2D histogram for coldmap
  TH2D *h2_HalfMap[8][14];// 2D histogram for halfmap (half entry chips)
  TH2D *h2_DeadMap[8][14];// 2D histogram for deadmap
  TH2D *h2_HotMap[8][14]; // 2D histogram for hotmap
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 14; j++)
    {
      h2_AllMap[i][j] = (TH2D *)file->Get(Form("HitMap_%d_%d", i, j));
      h2_ColdMap[i][j] = new TH2D(Form("ColdMap_%d_%d", i, j), Form("ColdMap_%d_%d", i, j), 128, 0, 128, 27, 0, 27);
      h2_HalfMap[i][j] = new TH2D(Form("HalfMap_%d_%d", i, j), Form("HalfMap_%d_%d", i, j), 128, 0, 128, 27, 0, 27);
      h2_HotMap[i][j] = new TH2D(Form("HotMap_%d_%d", i, j), Form("HotMap_%d_%d", i, j), 128, 0, 128, 27, 0, 27);
      h2_DeadMap[i][j] = new TH2D(Form("DeadMap_%d_%d", i, j), Form("DeadMap_%d_%d", i, j), 128, 0, 128, 27, 0, 27);
    }
  }
  //////////////////////////////////////////
  // Define condition for the hot channel //
  //////////////////////////////////////////
  double HotChannelCut_A_Fit[8][14] = {0.};
  double HotChannelCut_B_Fit[8][14] = {0.};
  double ColdChannelCut_A_Fit[8][14] = {0.};
  double ColdChannelCut_B_Fit[8][14] = {0.};
  double par_meanA[8][14] = {0.};  // Array to save the mean & sigma value, [0][module] = mean, [1][module] = sigma
  double par_sigmaA[8][14] = {0.}; // Array to save the mean & sigma value, [0][module] = mean, [1][module] = sigma
  double par_meanB[8][14] = {0.};  // Array to save the mean & sigma value, [0][module] = mean, [1][module] = sigma
  double par_sigmaB[8][14] = {0.}; // Array to save the mean & sigma value, [0][module] = mean, [1][module] = sigma
  /////////////////////////////////////////////////
  // Create TFile and TTree to save information  //
  // TTree is mainly used to convert to CDBTTree //
  /////////////////////////////////////////////////
  // std::string csvfilename = csv_output_path + "NumOfHot.csv";
  // std::ofstream csvFile(csvfilename, std::ios::app);
  // int NumOfHot = 0;
  // if (!csvFile.is_open())
  // {
  //   std::cout<<csvfilename<<std::endl;
  //   std::cerr << "Unable to open the file." << std::endl;
  //   return;
  // }
  std::string outputfile = root_output_path + "InttHotDeadMap_"+to_string(runnumber)+"_"+to_string(in_sig)+".root";
  //TFile *sfile = new TFile(Form("./rootfile/InttHotDeadMap_%d_%d.root", runnumber, in_sig), "RECREATE");
  TFile *sfile = new TFile(outputfile.c_str(), "RECREATE");
  TTree *st = new TTree("tree", "tree");
  double ch_entry, mean_gaus, sigma_gaus = 0.;
  int felix_, module_, chip_id_, chan_, type_, flag_;
  st->Branch("felix", &felix_);
  st->Branch("module", &module_);
  st->Branch("chip_id", &chip_id_);
  st->Branch("chan", &chan_);
  st->Branch("flag", &flag_);
  st->Branch("ch_entry", &ch_entry);
  st->Branch("type", &type_);
  st->Branch("mean", &mean_gaus);
  st->Branch("sigma", &sigma_gaus);

  ///////////////////////////////////
  // Fill the Dead & Cold & HotMap //
  ///////////////////////////////////
  TCanvas *canA[8];
  TCanvas *canB[8];
  for (int i = 0; i < 8; i++)
  {
    canA[i] = new TCanvas(Form("TypeA_Felix_%d", i), Form("TypeA_Felix_%d", i), 1200, 1200);
    canB[i] = new TCanvas(Form("TypeB_Felix_%d", i), Form("TypeB_Felix_%d", i), 1200, 1200);
    canA[i]->Divide(7, 2);
    canB[i]->Divide(7, 2);
  }
  TH1D *h1_hist_fit_A[8][14];
  TH1D *h1_hist_fit_B[8][14];
  double mean_first = 0;
  for (int felix = 0; felix < 8; felix++)
  {
    for (int i = 0; i < 14; i++)
    {
      h1_hist_fit_A[felix][i] = new TH1D(Form("h1_hist_fit_A%d_%d", felix, i), Form("h1_hist_fit_A%d_%d", felix, i), 200, 1, 201);
      h1_hist_fit_B[felix][i] = new TH1D(Form("h1_hist_fit_B%d_%d", felix, i), Form("h1_hist_fit_B%d_%d", felix, i), 200, 1, 201);
      for (int j = 0; j < 26; j++)
      {
        for (int chan = 0; chan < 128; chan++)
        {
          if (j < 5 || (j > 12 && j < 18)) // Condition for type B
          {
            h1_hist_fit_B[felix][i]->Fill(h2_AllMap[felix][i]->GetBinContent(chan + 1, j + 1));
            h1_hist_MPV->Fill(h2_AllMap[felix][i]->GetBinContent(chan + 1, j + 1));
          }
          else
          {
            h1_hist_fit_A[felix][i]->Fill(h2_AllMap[felix][i]->GetBinContent(chan + 1, j + 1));
            h1_hist_MPV->Fill(h2_AllMap[felix][i]->GetBinContent(chan + 1, j + 1));
          }
        }
      }
      double mean, sigma;
      canA[felix]->cd(i + 1);
      std::cout << "CHENCK !! Felix : " << felix << " module : " << i << std::endl;
    }
  }

  sfile->cd();
  h1_hist_MPV->Write();
  int hotchannelcut = h1_hist_MPV->GetXaxis()->GetBinCenter(h1_hist_MPV->GetMaximumBin()) * 4;
  std::cout << "hot channel cut : " << hotchannelcut << std::endl;
  CDBTTree *cdbttree = new CDBTTree(cdbttree_name);
  int size = 0;
  TDirectory *dir[8];
  for (int felix = 0; felix < 8; felix++)
  {
    dir[felix] = sfile->mkdir(Form("Felix_%d", felix));
    dir[felix]->cd();
    for (int i = 0; i < 14; i++)
    {
    //  cout << "HELLO moudle : " << i << " Type A " << HotChannelCut_A_Fit[felix][i] << endl;
    //  cout << "HELLO moudle : " << i << " Type B " << HotChannelCut_B_Fit[felix][i] << endl;
      for (int j = 0; j < 26; j++)
      {
    //    cout << "Felix : " << felix << " moudle : " << i << " Type A and chip : " << j << "  " << HotChannelCut_A_Fit[felix][i] << endl;
        for (int chan = 0; chan < 128; chan++)
        {
          // double entry = h1_chip[i][j]->GetBinContent(chan + 1);
          double entry = h2_AllMap[felix][i]->GetBinContent(chan + 1, j + 1);
          felix_ = felix;
          ch_entry = entry;
          module_ = i;
          chip_id_ = j;
          chan_ = chan;
          flag_ = 0;

          if (ch_entry > hotchannelcut)
          {
            flag_ = 8;
            // std::cout<<felix_<<std::endl;
            // std::cout<<module_<<std::endl;
            // std::cout<<chip_id_<<std::endl;
            // std::cout<<chan_<<std::endl;
            cdbttree->SetIntValue(size, "felix_server", felix);
            cdbttree->SetIntValue(size, "felix_channel", module_);
            cdbttree->SetIntValue(size, "chip", chip_id_);
            cdbttree->SetIntValue(size, "channel", chan_);
            cdbttree->SetIntValue(size, "flag", flag_);
            ++size;
            h2_HotMap[felix][i]->SetBinContent(chan+1,j+1,ch_entry);
          }
          // Masking all north side
            /*
          else if (felix_ > 3) 
           {
             flag_ = 8;
             cdbttree->SetIntValue(size, "felix_server", felix);
             cdbttree->SetIntValue(size, "felix_channel", module_);
             cdbttree->SetIntValue(size, "chip", chip_id_);
             cdbttree->SetIntValue(size, "channel", chan_);
             cdbttree->SetIntValue(size, "flag", flag_);
             ++size;
           h2_HotMap[felix][i]->SetBinContent(chan+1,j+1,ch_entry);
           }
          */
          // For debugging purpose
          // else if (h2_AllMap[felix][i]->GetEntries() > 1000)
          // {
          //   flag_ = 8;
          //   // std::cout<<felix_<<std::endl;
          //   // std::cout<<module_<<std::endl;
          //   // std::cout<<chip_id_<<std::endl;
          //   // std::cout<<chan_<<std::endl;
          //   cdbttree->SetIntValue(size, "felix_server", felix);
          //   cdbttree->SetIntValue(size, "felix_channel", module_);
          //   cdbttree->SetIntValue(size, "chip", chip_id_);
          //   cdbttree->SetIntValue(size, "channel", chan_);
          //   cdbttree->SetIntValue(size, "flag", flag_);
          //   ++size;
          //   h2_HotMap[felix][i]->SetBinContent(chan + 1, j + 1, ch_entry);
          // }
          // else if ( (felix==1 && module_ == 10) || (felix==1 && module_<3) )
          // {
          //   flag_ = 8;
          //   cdbttree->SetIntValue(size, "felix_server", felix);
          //   cdbttree->SetIntValue(size, "felix_channel", module_);
          //   cdbttree->SetIntValue(size, "chip", chip_id_);
          //   cdbttree->SetIntValue(size, "channel", chan_);
          //   cdbttree->SetIntValue(size, "flag", flag_);
          //   ++size;
          //   h2_HotMap[felix][i]->SetBinContent(chan + 1, j + 1, ch_entry);
          // }

          st->Fill();
        }
      }
   
      h2_AllMap[felix][i]->Write();
      h2_DeadMap[felix][i]->Write();
      h2_ColdMap[felix][i]->Write();
      h2_HalfMap[felix][i]->Write();
      h2_HotMap[felix][i]->Write();
      h1_hist_fit_A[felix][i]->Write();
      h1_hist_fit_B[felix][i]->Write();
    }
    sfile->cd();
    canA[felix]->Write();
    canB[felix]->Write();
  }
  cdbttree->SetSingleIntValue("size", size);
  cdbttree->Commit();
  cdbttree->CommitSingle();
  cdbttree->WriteCDBTTree();
  st->Write();

  // Add content to the end of the file
  //if(Writecsv) csvFile << runnumber << "," << NumOfHot << "\n";

  // Close the file
  //csvFile.close();

  sfile->Close();
  file->Close();
}

