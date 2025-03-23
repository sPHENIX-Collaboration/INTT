#include <TH1.h>
#include <TGraphAsymmErrors.h>
#include <TBox.h>
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TProfile.h>
#include <iostream>
#include <TStyle.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TGraphErrors.h>

using namespace std;

// how many seperate area by multiplicity
const int nq = 20;
// number of histgrams
const int N = 21;

// RP
TH1F *intt_psi[N];
TH1F *intt_psi_rec[N];
TH1F *intt_psi_flat[N];
TH1F *intt_psi_south[N];
TH1F *intt_psi_rec_south[N];
TH1F *intt_psi_flat_south[N];
TH1F *intt_psi_north[N];
TH1F *intt_psi_rec_north[N];
TH1F *intt_psi_flat_north[N];
TH1F *mbd_psi[N];
TH1F *mbd_psi_rec[N];
TH1F *mbd_psi_flat[N];
TH1F *mbd_psi_south[N];
TH1F *mbd_psi_rec_south[N];
TH1F *mbd_psi_flat_south[N];
TH1F *mbd_psi_north[N];
TH1F *mbd_psi_rec_north[N];
TH1F *mbd_psi_flat_north[N];
TH1F *epd_psi_south[N];
TH1F *epd_psi_flat_south[N];
TH1F *epd_psi_north[N];
TH1F *epd_psi_flat_north[N];
// number of cluster
TH1F *intt_nclus[N];
TH1F *intt_nclus_south[N];
TH1F *intt_nclus_north[N];
// charge
TH1F *mbd_charge[N];
TH1F *mbd_charge_south[N];
TH1F *mbd_charge_north[N];
TH1F *epd_charge[N];
TH1F *epd_charge_south[N];
TH1F *epd_charge_north[N];
// tprofile to calcurate resolution
TProfile *intt_epds;
TProfile *intts_epds;
TProfile *inttn_epds;
TProfile *intt_epdn;
TProfile *intts_epdn;
TProfile *inttn_epdn;
TProfile *epds_epdn;
TProfile *intt_mbds;
TProfile *intts_mbds;
TProfile *inttn_mbds;
TProfile *intt_mbdn;
TProfile *intts_mbdn;
TProfile *inttn_mbdn;
TProfile *mbds_mbdn;
TProfile *intts_inttn;
// observed v2
TProfile *v2_intt_mbd;
TProfile *v2_intt_mbds;
TProfile *v2_intt_mbdn;
TProfile *v2_intts_mbd;
TProfile *v2_intts_mbds;
TProfile *v2_intts_mbdn;
TProfile *v2_inttn_mbd;
TProfile *v2_inttn_mbds;
TProfile *v2_inttn_mbdn;
// resolution
TGraphErrors *reso_3sub_intt_epd;
TGraphErrors *reso_3sub_intts_epd;
TGraphErrors *reso_3sub_inttn_epd;
TGraphErrors *reso_3sub_intt_cross_epd;
TGraphErrors *reso_3sub_intt_mbd;
TGraphErrors *reso_3sub_intts_mbd;
TGraphErrors *reso_3sub_inttn_mbd;
TGraphErrors *reso_3sub_intt_cross_mbd;
TGraphErrors *reso_2sub_mbd;
TGraphErrors *reso_2sub_epd;
TGraphErrors *reso_2sub_intt;
// corrected v2
TGraphErrors *v2_cor_intt_mbd;
TGraphErrors *v2_cor_intt_mbds;
TGraphErrors *v2_cor_intt_mbdn;
TGraphErrors *v2_cor_intts_mbd;
TGraphErrors *v2_cor_intts_mbd_cross;
TGraphErrors *v2_cor_intts_mbds;
TGraphErrors *v2_cor_intts_mbds_cross;
TGraphErrors *v2_cor_intts_mbdn;
TGraphErrors *v2_cor_intts_mbdn_cross;
TGraphErrors *v2_cor_inttn_mbd;
TGraphErrors *v2_cor_inttn_mbd_cross;
TGraphErrors *v2_cor_inttn_mbds;
TGraphErrors *v2_cor_inttn_mbds_cross;
TGraphErrors *v2_cor_inttn_mbdn;
TGraphErrors *v2_cor_inttn_mbdn_cross;
// corrected v2 (plot in master thesis)
TGraphErrors *v2_cor_diff_intt_mbds;
TGraphErrors *v2_cor_diff_intt_mbdn;
// eta vs v2 (for compare with phobos result)
TGraphErrors *v2_eta;
// phobos result
TGraphAsymmErrors *phobosresult;
TBox *phoboserror;

// std::vector<int> *LiveTrigger_Vec = nullptr;
float EPD_south_charge_sum, EPD_north_charge_sum, EPD_charge_sum;
// std::vector<float> *EPDPhi_south = nullptr, *EPDPhi_north = nullptr;
float EPD_Psi_south, EPD_Psi_north, EPD_Psi_flat_south, EPD_Psi_flat_north;
// float MBD_z_vtx;
float MBD_south_charge_sum, MBD_north_charge_sum, MBD_charge_sum;
std::vector<float> *MBD_phi = nullptr;
std::vector<float> *MBD_pmt_q = nullptr;
// int NClus;
// std::vector<float> *ClusX = nullptr, *ClusY = nullptr, *ClusZ = nullptr;
// std::vector<unsigned int> *ClusAdc = nullptr;
// double Intt_Qx, Intt_Qy;
double Intt_Psi, Adcfiltered_NClus;
// double Intt_Qx_south, Intt_Qy_south;
double Adcfiltered_NClus_south;
double Intt_Psi_south;
// double Intt_Qx_north, Intt_Qy_north;
double Adcfiltered_NClus_north;
double Intt_Psi_north;
// double Mbd_Qx, Mbd_Qy, Mbd_Psi;
// double Mbd_Qx_south, Mbd_Qy_south, Mbd_Psi_south;
// double Mbd_Qx_north, Mbd_Qy_north, Mbd_Psi_north;
// double Qx_rec, Qy_rec;
double Mbd_Psi, Mbd_Psi_south, Mbd_Psi_north;
double psi_rec, psi_rec_south, psi_rec_north;
double Mbd_psi_rec, Mbd_psi_rec_south, Mbd_psi_rec_north;
double psi_flat, psi_flat_south, psi_flat_north;
double Mbd_psi_flat, Mbd_psi_flat_south, Mbd_psi_flat_north;

void setBranchAdresses(TTree *tree);
void newHist();
void newProf_reso(vector<double> binEdges, TArrayD bins);
void newProf_v2(vector<double> binEdges, TArrayD bins);
void newgraph_reso();
void newgraph_v2_cor();
void newgraph_v2_diff();
void fillHist(int m);
void fillProf_reso();
void fillProf_v2();
void setHistLine(int m);
void cal_3sub_reso(TProfile *a_bs, TProfile *a_bn, TProfile *bs_bn, TGraphErrors *reso, vector<double> binEdges);
void cal_2sub_reso(TProfile *bs_bn, TGraphErrors *reso, vector<double> binEdges);
void drawHists(TCanvas *c1, TH1F *hist1[], TH1F *hist2[], TH1F *hist3[], const std::string &pdfname);
void drawHists(TCanvas *c1, TH1F *hist1[], TH1F *hist2[], const std::string &pdfname);
void phobos_result();
void phobos_systematic_error();
double diff_double(double a, double b);
bool IsValidPsi(double a, double b, double c);
double fix_phi_psi(double a);
void fill_3Prof_v2(int j, double psi, TProfile *prof_, TProfile *prof_s, TProfile *prof_n);
void correct_v2(TGraphErrors *v2_cor_graph, TProfile *v2, TGraphErrors *reso_prof);
void v2_graph_diff(TGraphErrors *v2_base, TGraphErrors *v2_err, TGraphErrors *v2_diff);
void SaveToFile();
void draw_phobosresult();
void set_point_eta_v2(TFile *ifile, TGraphErrors *reso_prof, vector<double> binEdges, TArrayD bins,TGraphErrors*eta_vs_v2);

void draw_results()
{
  TFile *file = TFile::Open("epinfo_inttcombined54280_flattened.root", "READ");
  if (!file || file->IsZombie())
  {
    std::cerr << "Error opening file!" << std::endl;
    return;
  }

  gStyle->SetMarkerColor(kRed);
  gStyle->SetMarkerSize(0.5);
  gStyle->SetMarkerStyle(20);
  gStyle->SetHistLineWidth(2);

  vector<double> binEdges;

  for (int i = 0; i < nq; i++)
  {
    TTree *itree = (TTree *)file->Get(Form("EventTree_%d", i));

    itree->SetBranchStatus("*", 0);
    itree->SetBranchStatus("Adcfiltered_NClus", 1);

    binEdges.push_back(itree->GetMinimum("Adcfiltered_NClus"));

    if (i == 19)
      binEdges.push_back(itree->GetMaximum("Adcfiltered_NClus"));

    itree->SetBranchStatus("*", 1);
    delete itree;
  }

  TArrayD bins(binEdges.size(), binEdges.data());

  TTree *tree;
  newHist();
  newProf_reso(binEdges, bins);
  newProf_v2(binEdges, bins);
  newgraph_reso();
  newgraph_v2_cor();
  newgraph_v2_diff();

  tree = (TTree *)file->Get("EventTree");
  if (!tree)
  {
    std::cerr << "Error getting tree!" << std::endl;
    return;
  }
  setBranchAdresses(tree);
  Long64_t nEntries = tree->GetEntries();
  for (Long64_t i = 0; i < nEntries; i++)
  {
    tree->GetEntry(i);
    fillHist(0);
    setHistLine(0);
  }
  tree->SetBranchStatus("*", 1);

  for (int k = 0; k < nq; k++)
  {
    delete tree;
    tree = (TTree *)file->Get(Form("EventTree_%d", k));
    if (!tree)
    {
      std::cerr << "Error getting tree!" << std::endl;
      return;
    }
    setBranchAdresses(tree);
    setHistLine(k + 1);
    nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++)
    {
      tree->GetEntry(i);
      fillHist(k + 1);
      fillProf_reso();
      fillProf_v2();
    }
    tree->SetBranchStatus("*", 1);
  }

  cal_3sub_reso(intt_epds, intt_epdn, epds_epdn, reso_3sub_intt_epd, binEdges);
  cal_3sub_reso(intts_epds, intts_epdn, epds_epdn, reso_3sub_intts_epd, binEdges);
  cal_3sub_reso(inttn_epds, inttn_epdn, epds_epdn, reso_3sub_inttn_epd, binEdges);
  cal_3sub_reso(intt_mbds, intt_mbdn, mbds_mbdn, reso_3sub_intt_mbd, binEdges);
  cal_3sub_reso(intts_mbds, intts_mbdn, mbds_mbdn, reso_3sub_intts_mbd, binEdges);
  cal_3sub_reso(inttn_mbds, inttn_mbdn, mbds_mbdn, reso_3sub_inttn_mbd, binEdges);
  cal_3sub_reso(inttn_epds, intts_epdn, epds_epdn, reso_3sub_intt_cross_epd, binEdges);
  cal_3sub_reso(inttn_mbds, intts_mbdn, mbds_mbdn, reso_3sub_intt_cross_mbd, binEdges);

  cal_2sub_reso(mbds_mbdn, reso_2sub_mbd, binEdges);
  cal_2sub_reso(epds_epdn, reso_2sub_epd, binEdges);
  cal_2sub_reso(intts_inttn, reso_2sub_intt, binEdges);

  correct_v2(v2_cor_intt_mbd, v2_intt_mbd, reso_3sub_intt_epd);
  correct_v2(v2_cor_intt_mbds, v2_intt_mbds, reso_3sub_intt_epd);
  correct_v2(v2_cor_intt_mbdn, v2_intt_mbdn, reso_3sub_intt_epd);

  correct_v2(v2_cor_intts_mbd, v2_intts_mbd, reso_3sub_intts_epd);
  correct_v2(v2_cor_intts_mbd_cross, v2_intts_mbd, reso_3sub_intt_cross_epd);
  correct_v2(v2_cor_intts_mbds, v2_intts_mbds, reso_3sub_intts_epd);
  correct_v2(v2_cor_intts_mbds_cross, v2_intts_mbds, reso_3sub_intt_cross_epd);
  correct_v2(v2_cor_intts_mbdn, v2_intts_mbdn, reso_3sub_intts_epd);
  correct_v2(v2_cor_intts_mbdn_cross, v2_intts_mbdn, reso_3sub_intt_cross_epd);

  correct_v2(v2_cor_inttn_mbd, v2_inttn_mbd, reso_3sub_inttn_epd);
  correct_v2(v2_cor_inttn_mbd_cross, v2_inttn_mbd, reso_3sub_intt_cross_epd);
  correct_v2(v2_cor_inttn_mbds, v2_inttn_mbds, reso_3sub_inttn_epd);
  correct_v2(v2_cor_inttn_mbds_cross, v2_inttn_mbds, reso_3sub_intt_cross_epd);
  correct_v2(v2_cor_inttn_mbdn, v2_inttn_mbdn, reso_3sub_inttn_epd);
  correct_v2(v2_cor_inttn_mbdn_cross, v2_inttn_mbdn, reso_3sub_intt_cross_epd);

  v2_graph_diff(v2_cor_inttn_mbds_cross, v2_cor_intts_mbds_cross, v2_cor_diff_intt_mbds);
  v2_graph_diff(v2_cor_intts_mbdn_cross, v2_cor_inttn_mbdn_cross, v2_cor_diff_intt_mbdn);

  string pdfname = "All_hist.pdf";
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 1000);
  c1->Print((pdfname + "[").c_str());
  // print all event  sepd, mbd and intt RP
  c1->Divide(3, 4);
  c1->cd(1);
  intt_psi[0]->Draw();
  intt_psi_rec[0]->Draw("same");
  intt_psi_flat[0]->Draw("same");
  c1->cd(2);
  intt_psi_south[0]->Draw();
  intt_psi_rec_south[0]->Draw("same");
  intt_psi_flat_south[0]->Draw("same");
  c1->cd(3);
  intt_psi_north[0]->Draw();
  intt_psi_rec_north[0]->Draw("same");
  intt_psi_flat_north[0]->Draw("same");
  c1->cd(4);
  mbd_psi[0]->Draw();
  mbd_psi_rec[0]->Draw("same");
  mbd_psi_flat[0]->Draw("same");
  c1->cd(5);
  mbd_psi_south[0]->Draw();
  mbd_psi_rec_south[0]->Draw("same");
  mbd_psi_flat_south[0]->Draw("same");
  c1->cd(6);
  mbd_psi_north[0]->Draw();
  mbd_psi_rec_north[0]->Draw("same");
  mbd_psi_flat_north[0]->Draw("same");
  c1->cd(7);
  epd_psi_south[0]->Draw();
  epd_psi_flat_south[0]->Draw("same");
  c1->cd(8);
  epd_psi_north[0]->Draw();
  epd_psi_flat_north[0]->Draw("same");
  c1->cd(10);
  intt_nclus[0]->Draw();
  intt_nclus_south[0]->Draw("same");
  intt_nclus_north[0]->Draw("same");
  c1->cd(11);
  mbd_charge[0]->Draw();
  mbd_charge_south[0]->Draw("same");
  mbd_charge_north[0]->Draw("same");
  c1->cd(12);
  epd_charge[0]->Draw();
  epd_charge_south[0]->Draw("same");
  epd_charge_north[0]->Draw("same");
  c1->Print(pdfname.c_str());
  c1->Clear();

  // print 3sub resolution all combination
  c1->Divide(2, 4);
  c1->cd(1);
  reso_3sub_intt_epd->Draw("AP");
  c1->cd(2);
  reso_3sub_intts_epd->Draw("AP");
  c1->cd(3);
  reso_3sub_inttn_epd->Draw("AP");
  c1->cd(4);
  reso_3sub_intt_cross_epd->Draw("AP");
  c1->cd(5);
  reso_3sub_intt_mbd->Draw("AP");
  c1->cd(6);
  reso_3sub_intts_mbd->Draw("AP");
  c1->cd(7);
  reso_3sub_inttn_mbd->Draw("AP");
  c1->cd(8);
  reso_3sub_intt_cross_mbd->GetYaxis()->SetRangeUser(-2, 2);
  reso_3sub_intt_cross_mbd->Draw("AP");
  c1->Print(pdfname.c_str());
  c1->Clear();

  // print observed v2 all combination
  c1->Divide(3, 3);
  c1->cd(1);
  v2_intt_mbd->Draw();
  c1->cd(2);
  v2_intt_mbds->Draw();
  c1->cd(3);
  v2_intt_mbdn->Draw();
  c1->cd(4);
  v2_intts_mbd->Draw();
  c1->cd(5);
  v2_intts_mbds->Draw();
  c1->cd(6);
  v2_intts_mbdn->Draw();
  c1->cd(7);
  v2_inttn_mbd->Draw();
  c1->cd(8);
  v2_inttn_mbds->Draw();
  c1->cd(9);
  v2_inttn_mbdn->Draw();
  c1->Print(pdfname.c_str());
  c1->Clear();

  // print corrected v2
  c1->Divide(3, 5);
  c1->cd(1);
  v2_cor_intt_mbd->Draw("AP");
  c1->cd(2);
  v2_cor_intt_mbds->Draw("AP");
  c1->cd(3);
  v2_cor_intt_mbdn->Draw("AP");
  c1->cd(4);
  v2_cor_intts_mbd->Draw("AP");
  c1->cd(5);
  v2_cor_intts_mbd_cross->Draw("AP");
  c1->cd(6);
  v2_cor_intts_mbds->Draw("AP");
  c1->cd(7);
  v2_cor_intts_mbds_cross->Draw("AP");
  c1->cd(8);
  v2_cor_intts_mbdn->Draw("AP");
  c1->cd(9);
  v2_cor_intts_mbdn_cross->Draw("AP");
  c1->cd(10);
  v2_cor_inttn_mbd->Draw("AP");
  c1->cd(11);
  v2_cor_inttn_mbd_cross->Draw("AP");
  c1->cd(12);
  v2_cor_inttn_mbds->Draw("AP");
  c1->cd(13);
  v2_cor_inttn_mbds_cross->Draw("AP");
  c1->cd(14);
  v2_cor_inttn_mbdn->Draw("AP");
  c1->cd(15);
  v2_cor_inttn_mbdn_cross->Draw("AP");
  c1->Print(pdfname.c_str());
  c1->Clear();

  v2_eta = new TGraphErrors();
  v2_eta->SetTitle("eta vs v2");
  draw_phobosresult();
  set_point_eta_v2(file, reso_3sub_intt_cross_epd, binEdges,bins,v2_eta);
  v2_eta->SetMarkerStyle(20);
  v2_eta->SetMarkerColor(kRed);
  v2_eta->SetMarkerSize(2);
  v2_eta->SetLineWidth(2);

  v2_eta->Draw("P");
  c1->Print(pdfname.c_str());
  c1->Clear();

  drawHists(c1, intt_psi, intt_psi_rec, intt_psi_flat, pdfname);
  drawHists(c1, intt_psi_south, intt_psi_rec_south, intt_psi_flat_south, pdfname);
  drawHists(c1, intt_psi_north, intt_psi_rec_north, intt_psi_flat_north, pdfname);
  drawHists(c1, mbd_psi, mbd_psi_rec, mbd_psi_flat, pdfname);
  drawHists(c1, mbd_psi_south, mbd_psi_rec_south, mbd_psi_flat_south, pdfname);
  drawHists(c1, mbd_psi_north, mbd_psi_rec_north, mbd_psi_flat_north, pdfname);
  drawHists(c1, epd_psi_south, epd_psi_flat_south, pdfname);
  drawHists(c1, epd_psi_north, epd_psi_flat_north, pdfname);

  drawHists(c1, intt_nclus, intt_nclus_south, intt_nclus_north, pdfname);
  drawHists(c1, mbd_charge, mbd_charge_south, mbd_charge_north, pdfname);
  drawHists(c1, epd_charge, epd_charge_south, epd_charge_north, pdfname);

  c1->Print((pdfname + "]").c_str());

  SaveToFile();
}

void setBranchAdresses(TTree *tree)
{
  tree->SetBranchStatus("*", 0);

  // SetBranchStatus と SetBranchAddress のペアを定義
  // tree->SetBranchStatus("LiveTrigger_Vec", 1);
  // tree->SetBranchAddress("LiveTrigger_Vec", &LiveTrigger_Vec);

  tree->SetBranchStatus("EPD_south_charge_sum", 1);
  tree->SetBranchAddress("EPD_south_charge_sum", &EPD_south_charge_sum);

  tree->SetBranchStatus("EPD_north_charge_sum", 1);
  tree->SetBranchAddress("EPD_north_charge_sum", &EPD_north_charge_sum);

  tree->SetBranchStatus("EPD_charge_sum", 1);
  tree->SetBranchAddress("EPD_charge_sum", &EPD_charge_sum);

  // tree->SetBranchStatus("EPDPhi_south", 1);
  // tree->SetBranchAddress("EPDPhi_south", &EPDPhi_south);

  // tree->SetBranchStatus("EPDPhi_north", 1);
  // tree->SetBranchAddress("EPDPhi_north", &EPDPhi_north);

  tree->SetBranchStatus("EPD_Psi_south", 1);
  tree->SetBranchAddress("EPD_Psi_south", &EPD_Psi_south);

  tree->SetBranchStatus("EPD_Psi_north", 1);
  tree->SetBranchAddress("EPD_Psi_north", &EPD_Psi_north);

  tree->SetBranchStatus("EPD_Psi_flat_south", 1);
  tree->SetBranchAddress("EPD_Psi_flat_south", &EPD_Psi_flat_south);

  tree->SetBranchStatus("EPD_Psi_flat_north", 1);
  tree->SetBranchAddress("EPD_Psi_flat_north", &EPD_Psi_flat_north);

  // tree->SetBranchStatus("MBD_z_vtx", 1);
  // tree->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);

  tree->SetBranchStatus("MBD_south_charge_sum", 1);
  tree->SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);

  tree->SetBranchStatus("MBD_north_charge_sum", 1);
  tree->SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);

  tree->SetBranchStatus("MBD_charge_sum", 1);
  tree->SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);

  tree->SetBranchStatus("MBD_phi", 1);
  tree->SetBranchAddress("MBD_phi", &MBD_phi);

  tree->SetBranchStatus("MBD_pmt_q", 1);
  tree->SetBranchAddress("MBD_pmt_q", &MBD_pmt_q);

  // tree->SetBranchStatus("NClus", 1);
  // tree->SetBranchAddress("NClus", &NClus);

  // tree->SetBranchStatus("ClusX", 1);
  // tree->SetBranchAddress("ClusX", &ClusX);

  // tree->SetBranchStatus("ClusY", 1);
  // tree->SetBranchAddress("ClusY", &ClusY);

  // tree->SetBranchStatus("ClusZ", 1);
  // tree->SetBranchAddress("ClusZ", &ClusZ);

  // tree->SetBranchStatus("ClusAdc", 1);
  // tree->SetBranchAddress("ClusAdc", &ClusAdc);

  // tree->SetBranchStatus("Intt_Qx", 1);
  // tree->SetBranchAddress("Intt_Qx", &Intt_Qx);

  // tree->SetBranchStatus("Intt_Qy", 1);
  // tree->SetBranchAddress("Intt_Qy", &Intt_Qy);

  tree->SetBranchStatus("Intt_Psi", 1);
  tree->SetBranchAddress("Intt_Psi", &Intt_Psi);

  tree->SetBranchStatus("Adcfiltered_NClus", 1);
  tree->SetBranchAddress("Adcfiltered_NClus", &Adcfiltered_NClus);

  // tree->SetBranchStatus("Intt_Qx_south", 1);
  // tree->SetBranchAddress("Intt_Qx_south", &Intt_Qx_south);

  // tree->SetBranchStatus("Intt_Qy_south", 1);
  // tree->SetBranchAddress("Intt_Qy_south", &Intt_Qy_south);

  tree->SetBranchStatus("Intt_Psi_south", 1);
  tree->SetBranchAddress("Intt_Psi_south", &Intt_Psi_south);

  tree->SetBranchStatus("Adcfiltered_NClus_south", 1);
  tree->SetBranchAddress("Adcfiltered_NClus_south", &Adcfiltered_NClus_south);

  // tree->SetBranchStatus("Intt_Qx_north", 1);
  // tree->SetBranchAddress("Intt_Qx_north", &Intt_Qx_north);

  // tree->SetBranchStatus("Intt_Qy_north", 1);
  // tree->SetBranchAddress("Intt_Qy_north", &Intt_Qy_north);

  tree->SetBranchStatus("Intt_Psi_north", 1);
  tree->SetBranchAddress("Intt_Psi_north", &Intt_Psi_north);

  tree->SetBranchStatus("Adcfiltered_NClus_north", 1);
  tree->SetBranchAddress("Adcfiltered_NClus_north", &Adcfiltered_NClus_north);

  // tree->SetBranchStatus("Mbd_Qx", 1);
  // tree->SetBranchAddress("Mbd_Qx", &Mbd_Qx);

  // tree->SetBranchStatus("Mbd_Qy", 1);
  // tree->SetBranchAddress("Mbd_Qy", &Mbd_Qy);

  tree->SetBranchStatus("Mbd_Psi", 1);
  tree->SetBranchAddress("Mbd_Psi", &Mbd_Psi);

  // tree->SetBranchStatus("Mbd_Qx_south", 1);
  // tree->SetBranchAddress("Mbd_Qx_south", &Mbd_Qx_south);

  // tree->SetBranchStatus("Mbd_Qy_south", 1);
  // tree->SetBranchAddress("Mbd_Qy_south", &Mbd_Qy_south);

  tree->SetBranchStatus("Mbd_Psi_south", 1);
  tree->SetBranchAddress("Mbd_Psi_south", &Mbd_Psi_south);

  // tree->SetBranchStatus("Mbd_Qx_north", 1);
  // tree->SetBranchAddress("Mbd_Qx_north", &Mbd_Qx_north);

  // tree->SetBranchStatus("Mbd_Qy_north", 1);
  // tree->SetBranchAddress("Mbd_Qy_north", &Mbd_Qy_north);

  tree->SetBranchStatus("Mbd_Psi_north", 1);
  tree->SetBranchAddress("Mbd_Psi_north", &Mbd_Psi_north);

  tree->SetBranchStatus("psi_rec", 1);
  tree->SetBranchAddress("psi_rec", &psi_rec);

  // tree->SetBranchStatus("Qx_rec", 1);
  // tree->SetBranchAddress("Qx_rec", &Qx_rec);

  // tree->SetBranchStatus("Qy_rec", 1);
  // tree->SetBranchAddress("Qy_rec", &Qy_rec);

  // tree->SetBranchStatus("Qx_rec_south", 1);
  // tree->SetBranchAddress("Qx_rec_south", &Qx_rec_south);

  // tree->SetBranchStatus("Qy_rec_south", 1);
  // tree->SetBranchAddress("Qy_rec_south", &Qy_rec_south);

  tree->SetBranchStatus("psi_rec_south", 1);
  tree->SetBranchAddress("psi_rec_south", &psi_rec_south);

  // tree->SetBranchStatus("Qx_rec_north", 1);
  // tree->SetBranchAddress("Qx_rec_north", &Qx_rec_north);

  // tree->SetBranchStatus("Qy_rec_north", 1);
  // tree->SetBranchAddress("Qy_rec_north", &Qy_rec_north);

  tree->SetBranchStatus("psi_rec_north", 1);
  tree->SetBranchAddress("psi_rec_north", &psi_rec_north);

  // tree->SetBranchStatus("Mbd_Qx_rec", 1);
  // tree->SetBranchAddress("Mbd_Qx_rec", &Mbd_Qx_rec);

  // tree->SetBranchStatus("Mbd_Qy_rec", 1);
  // tree->SetBranchAddress("Mbd_Qy_rec", &Mbd_Qy_rec);

  tree->SetBranchStatus("Mbd_psi_rec", 1);
  tree->SetBranchAddress("Mbd_psi_rec", &Mbd_psi_rec);

  // tree->SetBranchStatus("Mbd_Qx_rec_south", 1);
  // tree->SetBranchAddress("Mbd_Qx_rec_south", &Mbd_Qx_rec_south);

  // tree->SetBranchStatus("Mbd_Qy_rec_south", 1);
  // tree->SetBranchAddress("Mbd_Qy_rec_south", &Mbd_Qy_rec_south);

  tree->SetBranchStatus("Mbd_psi_rec_south", 1);
  tree->SetBranchAddress("Mbd_psi_rec_south", &Mbd_psi_rec_south);

  // tree->SetBranchStatus("Mbd_Qx_rec_north", 1);
  // tree->SetBranchAddress("Mbd_Qx_rec_north", &Mbd_Qx_rec_north);

  // tree->SetBranchStatus("Mbd_Qy_rec_north", 1);
  // tree->SetBranchAddress("Mbd_Qy_rec_north", &Mbd_Qy_rec_north);

  tree->SetBranchStatus("Mbd_psi_rec_north", 1);
  tree->SetBranchAddress("Mbd_psi_rec_north", &Mbd_psi_rec_north);

  tree->SetBranchStatus("psi_flat", 1);
  tree->SetBranchAddress("psi_flat", &psi_flat);

  tree->SetBranchStatus("psi_flat_south", 1);
  tree->SetBranchAddress("psi_flat_south", &psi_flat_south);

  tree->SetBranchStatus("psi_flat_north", 1);
  tree->SetBranchAddress("psi_flat_north", &psi_flat_north);

  tree->SetBranchStatus("Mbd_psi_flat", 1);
  tree->SetBranchAddress("Mbd_psi_flat", &Mbd_psi_flat);

  tree->SetBranchStatus("Mbd_psi_flat_south", 1);
  tree->SetBranchAddress("Mbd_psi_flat_south", &Mbd_psi_flat_south);

  tree->SetBranchStatus("Mbd_psi_flat_north", 1);
  tree->SetBranchAddress("Mbd_psi_flat_north", &Mbd_psi_flat_north);
}
void newHist()
{
  int start = 100;
  int step = 5;

  for (int i = 0; i < N; i++)
  {
    TString range;
    TString intt = "intt";
    TString mbd = "mbd";
    TString epd = "epd";
    TString _rec = "_rec";
    TString _flat = "_flat";
    TString _south = "_south";
    TString _north = "_north";
    TString nclus = "nclus";
    TString charge = "charge";

    if (i == 0)
    {
      range = "All";
    }
    else
    {
      int lower = start - (i)*step;
      int upper = start - (i - 1) * step;
      range = Form("%d~%d", upper, lower);
    }

    TString name;
    name = intt + range;
    intt_psi[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + _rec + range;
    intt_psi_rec[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + _flat + range;
    intt_psi_flat[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + _south + range;
    intt_psi_south[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + _rec + _south + range;
    intt_psi_rec_south[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + _flat + _south + range;
    intt_psi_flat_south[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + _north + range;
    intt_psi_north[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + _rec + _north + range;
    intt_psi_rec_north[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + _flat + _north + range;
    intt_psi_flat_north[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + range;
    mbd_psi[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + _rec + range;
    mbd_psi_rec[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + _flat + range;
    mbd_psi_flat[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + _south + range;
    mbd_psi_south[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + _rec + _south + range;
    mbd_psi_rec_south[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + _flat + _south + range;
    mbd_psi_flat_south[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + _north + range;
    mbd_psi_north[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + _rec + _north + range;
    mbd_psi_rec_north[i] = new TH1F(name, "", 100, -2, 2);
    name = mbd + _flat + _north + range;
    mbd_psi_flat_north[i] = new TH1F(name, "", 100, -2, 2);
    name = epd + _south + range;
    epd_psi_south[i] = new TH1F(name, "", 100, -2, 2);
    name = epd + _flat + _south + range;
    epd_psi_flat_south[i] = new TH1F(name, "", 100, -2, 2);
    name = epd + _north + range;
    epd_psi_north[i] = new TH1F(name, "", 100, -2, 2);
    name = epd + _flat + _north + range;
    epd_psi_flat_north[i] = new TH1F(name, "", 100, -2, 2);
    name = intt + nclus + range;
    intt_nclus[i] = new TH1F(name, "", 100, 0, 8000);
    name = intt + nclus + _south + range;
    intt_nclus_south[i] = new TH1F(name, "", 100, 0, 8000);
    name = intt + nclus + _north + range;
    intt_nclus_north[i] = new TH1F(name, "", 100, 0, 8000);
    name = mbd + charge + range;
    mbd_charge[i] = new TH1F(name, "", 100, 0, 6000);
    name = mbd + charge + _south + range;
    mbd_charge_south[i] = new TH1F(name, "", 100, 0, 6000);
    name = mbd + charge + _north + range;
    mbd_charge_north[i] = new TH1F(name, "", 100, 0, 6000);
    name = epd + charge + range;
    epd_charge[i] = new TH1F(name, "", 300, 0, 30000);
    name = epd + charge + _south + range;
    epd_charge_south[i] = new TH1F(name, "", 300, 0, 30000);
    name = epd + charge + _north + range;
    epd_charge_north[i] = new TH1F(name, "", 300, 0, 30000);
  }
}
void newProf_reso(vector<double> binEdges, TArrayD bins)
{
  intt_epds = new TProfile("intt_epds", "", binEdges.size() - 1, bins.GetArray());
  intts_epds = new TProfile("intts_epds", "", binEdges.size() - 1, bins.GetArray());
  inttn_epds = new TProfile("inttn_epds", "", binEdges.size() - 1, bins.GetArray());
  intt_epdn = new TProfile("intt_epdn", "", binEdges.size() - 1, bins.GetArray());
  intts_epdn = new TProfile("intts_epdn", "", binEdges.size() - 1, bins.GetArray());
  inttn_epdn = new TProfile("inttn_epdn", "", binEdges.size() - 1, bins.GetArray());
  epds_epdn = new TProfile("epds_epdn", "", binEdges.size() - 1, bins.GetArray());
  intt_mbds = new TProfile("intt_mbds", "", binEdges.size() - 1, bins.GetArray());
  intts_mbds = new TProfile("intts_mbds", "", binEdges.size() - 1, bins.GetArray());
  inttn_mbds = new TProfile("inttn_mbds", "", binEdges.size() - 1, bins.GetArray());
  intt_mbdn = new TProfile("intt_mbdn", "", binEdges.size() - 1, bins.GetArray());
  intts_mbdn = new TProfile("intts_mbdn", "", binEdges.size() - 1, bins.GetArray());
  inttn_mbdn = new TProfile("inttn_mbdn", "", binEdges.size() - 1, bins.GetArray());
  mbds_mbdn = new TProfile("mbds_mbdn", "", binEdges.size() - 1, bins.GetArray());
  intts_inttn = new TProfile("intts_inttn", "", binEdges.size() - 1, bins.GetArray());
}
void newProf_v2(vector<double> binEdges, TArrayD bins)
{
  v2_intt_mbd = new TProfile("v2_intt_mbd", "", binEdges.size() - 1, bins.GetArray());
  v2_intt_mbds = new TProfile("v2_intt_mbds", "", binEdges.size() - 1, bins.GetArray());
  v2_intt_mbdn = new TProfile("v2_intt_mbdn", "", binEdges.size() - 1, bins.GetArray());
  v2_intts_mbd = new TProfile("v2_intts_mbd", "", binEdges.size() - 1, bins.GetArray());
  v2_intts_mbds = new TProfile("v2_intts_mbds", "", binEdges.size() - 1, bins.GetArray());
  v2_intts_mbdn = new TProfile("v2_intts_mbdn", "", binEdges.size() - 1, bins.GetArray());
  v2_inttn_mbd = new TProfile("v2_inttn_mbd", "", binEdges.size() - 1, bins.GetArray());
  v2_inttn_mbds = new TProfile("v2_inttn_mbds", "", binEdges.size() - 1, bins.GetArray());
  v2_inttn_mbdn = new TProfile("v2_inttn_mbdn", "", binEdges.size() - 1, bins.GetArray());
}
void newgraph_reso()
{
  reso_3sub_intt_epd = new TGraphErrors();
  reso_3sub_intt_epd->SetTitle("3sub Reso INTT sEPD");
  reso_3sub_intts_epd = new TGraphErrors();
  reso_3sub_intts_epd->SetTitle("3sub Reso INTT south sEPD");
  reso_3sub_inttn_epd = new TGraphErrors();
  reso_3sub_inttn_epd->SetTitle("3sub Reso INTT north sEPD");
  reso_3sub_intt_cross_epd = new TGraphErrors();
  reso_3sub_intt_cross_epd->SetTitle("3sub Reso INTT half sEPD");
  reso_3sub_intt_mbd = new TGraphErrors();
  reso_3sub_intt_mbd->SetTitle("3sub Reso INTT MBD");
  reso_3sub_intts_mbd = new TGraphErrors();
  reso_3sub_intts_mbd->SetTitle("3sub Reso INTT south MBD");
  reso_3sub_inttn_mbd = new TGraphErrors();
  reso_3sub_inttn_mbd->SetTitle("3sub Reso INTT north MBD");
  reso_3sub_intt_cross_mbd = new TGraphErrors();
  reso_3sub_intt_cross_mbd->SetTitle("3sub Reso INTT half MBD");
  reso_2sub_mbd = new TGraphErrors();
  reso_2sub_mbd->SetTitle("2sub Reso MBD");
  reso_2sub_epd = new TGraphErrors();
  reso_2sub_mbd->SetTitle("2sub Reso sEPD");
  reso_2sub_intt = new TGraphErrors();
  reso_2sub_mbd->SetTitle("2sub Reso INTT");
}
void newgraph_v2_cor()
{
  v2_cor_intt_mbd = new TGraphErrors();
  v2_cor_intt_mbd->SetTitle("v2 corrected INTT MBD");
  v2_cor_intt_mbds = new TGraphErrors();
  v2_cor_intt_mbds->SetTitle("v2 corrected INTT MBDS");
  v2_cor_intt_mbdn = new TGraphErrors();
  v2_cor_intt_mbdn->SetTitle("v2 corrected INTT MBDN");
  v2_cor_intts_mbd = new TGraphErrors();
  v2_cor_intts_mbd->SetTitle("v2 corrected INTTS MBD");
  v2_cor_intts_mbd_cross = new TGraphErrors();
  v2_cor_intts_mbd_cross->SetTitle("v2 corrected cross 3sub INTTS MBD");
  v2_cor_intts_mbds = new TGraphErrors();
  v2_cor_intts_mbds->SetTitle("v2 corrected INTTS MBDS");
  v2_cor_intts_mbds_cross = new TGraphErrors();
  v2_cor_intts_mbds_cross->SetTitle("v2 corrected cross 3sub INTTS MBDS ");
  v2_cor_intts_mbdn = new TGraphErrors();
  v2_cor_intts_mbdn->SetTitle("v2 corrected INTTS MBDN");
  v2_cor_intts_mbdn_cross = new TGraphErrors();
  v2_cor_intts_mbdn_cross->SetTitle("v2 corrected cross 3sub INTTS MBDN");
  v2_cor_inttn_mbd = new TGraphErrors();
  v2_cor_inttn_mbd->SetTitle("v2 corrected INTTN MBD");
  v2_cor_inttn_mbd_cross = new TGraphErrors();
  v2_cor_inttn_mbd_cross->SetTitle("v2 corrected cross 3sub INTTN MBD");
  v2_cor_inttn_mbds = new TGraphErrors();
  v2_cor_inttn_mbds->SetTitle("v2 corrected INTTN MBDS");
  v2_cor_inttn_mbds_cross = new TGraphErrors();
  v2_cor_inttn_mbds_cross->SetTitle("v2 corrected cross 3sub INTTN MBDS");
  v2_cor_inttn_mbdn = new TGraphErrors();
  v2_cor_inttn_mbdn->SetTitle("v2 corrected INTTN MBDN");
  v2_cor_inttn_mbdn_cross = new TGraphErrors();
  v2_cor_inttn_mbdn_cross->SetTitle("v2 corrected cross 3sub INTTN MBDN");
}
void newgraph_v2_diff()
{
  v2_cor_diff_intt_mbds = new TGraphErrors();
  v2_cor_diff_intt_mbds->SetTitle("v2 corrected diff INTT MBDS");
  v2_cor_diff_intt_mbdn = new TGraphErrors();
  v2_cor_diff_intt_mbdn->SetTitle("v2 corrected diff INTT MBDN");
}
void fillHist(int m)
{
  intt_psi[m]->Fill(Intt_Psi);
  intt_psi_rec[m]->Fill(psi_rec);
  intt_psi_flat[m]->Fill(psi_flat);
  intt_psi_south[m]->Fill(Intt_Psi_south);
  intt_psi_rec_south[m]->Fill(psi_rec_south);
  intt_psi_flat_south[m]->Fill(psi_flat_south);
  intt_psi_north[m]->Fill(Intt_Psi_north);
  intt_psi_rec_north[m]->Fill(psi_rec_north);
  intt_psi_flat_north[m]->Fill(psi_flat_north);
  mbd_psi[m]->Fill(Mbd_Psi);
  mbd_psi_rec[m]->Fill(Mbd_psi_rec);
  mbd_psi_flat[m]->Fill(Mbd_psi_flat);
  mbd_psi_south[m]->Fill(Mbd_Psi_south);
  mbd_psi_rec_south[m]->Fill(Mbd_psi_rec_south);
  mbd_psi_flat_south[m]->Fill(Mbd_psi_flat_south);
  mbd_psi_north[m]->Fill(Mbd_Psi_north);
  mbd_psi_rec_north[m]->Fill(Mbd_psi_rec_north);
  mbd_psi_flat_north[m]->Fill(Mbd_psi_flat_north);
  epd_psi_south[m]->Fill(EPD_Psi_south);
  epd_psi_flat_south[m]->Fill(EPD_Psi_flat_south);
  epd_psi_north[m]->Fill(EPD_Psi_north);
  epd_psi_flat_north[m]->Fill(EPD_Psi_flat_north);
  intt_nclus[m]->Fill(Adcfiltered_NClus);
  intt_nclus_south[m]->Fill(Adcfiltered_NClus_south);
  intt_nclus_north[m]->Fill(Adcfiltered_NClus_north);
  mbd_charge[m]->Fill(MBD_charge_sum);
  mbd_charge_south[m]->Fill(MBD_south_charge_sum);
  mbd_charge_north[m]->Fill(MBD_north_charge_sum);
  epd_charge[m]->Fill(EPD_charge_sum);
  epd_charge_south[m]->Fill(EPD_south_charge_sum);
  epd_charge_north[m]->Fill(EPD_north_charge_sum);
}
void fillProf_reso()
{
  if (!isnan((Adcfiltered_NClus)) && IsValidPsi(psi_flat, psi_flat_south, psi_flat_north) && !isnan(EPD_Psi_flat_south) && !isnan(EPD_Psi_flat_north))
  {
    intt_epds->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat, EPD_Psi_flat_south)));
    intts_epds->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_south, EPD_Psi_flat_south)));
    inttn_epds->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_north, EPD_Psi_flat_south)));
    intt_epdn->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat, EPD_Psi_flat_north)));
    intts_epdn->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_south, EPD_Psi_flat_north)));
    inttn_epdn->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_north, EPD_Psi_flat_north)));
    epds_epdn->Fill(Adcfiltered_NClus, cos(diff_double(EPD_Psi_flat_south, EPD_Psi_flat_north)));
  }
  if (!isnan((Adcfiltered_NClus)) && IsValidPsi(psi_flat, psi_flat_south, psi_flat_north) && IsValidPsi(Mbd_psi_flat, Mbd_psi_flat_south, Mbd_psi_flat_north))
  {
    intt_mbds->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat, Mbd_psi_flat_south)));
    intts_mbds->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_south, Mbd_psi_flat_south)));
    inttn_mbds->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_north, Mbd_psi_flat_south)));
    intt_mbdn->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat, Mbd_psi_flat_north)));
    intts_mbdn->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_south, Mbd_psi_flat_north)));
    inttn_mbdn->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_north, Mbd_psi_flat_north)));
    mbds_mbdn->Fill(Adcfiltered_NClus, cos(diff_double(Mbd_psi_flat_south, Mbd_psi_flat_north)));
  }
  if (!isnan((Adcfiltered_NClus)) && !isnan(psi_flat_south) && !isnan(psi_flat_north))
    intts_inttn->Fill(Adcfiltered_NClus, cos(diff_double(psi_flat_south, psi_flat_north)));
}
void fillProf_v2()
{
  for (int j = 0; j < MBD_phi->size(); j++)
  {
    fill_3Prof_v2(j, psi_flat, v2_intt_mbd, v2_intt_mbds, v2_intt_mbdn);
    fill_3Prof_v2(j, psi_flat_south, v2_intts_mbd, v2_intts_mbds, v2_intts_mbdn);
    fill_3Prof_v2(j, psi_flat_north, v2_inttn_mbd, v2_inttn_mbds, v2_inttn_mbdn);
  }
}
void setHistLine(int m)
{
  // gStyle->SetLineWidth(2);
  // gStyle->SetHistLineWidth(3);
  intt_psi[m]->SetLineColor(kBlack);
  intt_psi_rec[m]->SetLineColor(kRed);
  intt_psi_flat[m]->SetLineColor(kBlue);
  intt_psi_south[m]->SetLineColor(kBlack);
  intt_psi_rec_south[m]->SetLineColor(kRed);
  intt_psi_flat_south[m]->SetLineColor(kBlue);
  intt_psi_north[m]->SetLineColor(kBlack);
  intt_psi_rec_north[m]->SetLineColor(kRed);
  intt_psi_flat_north[m]->SetLineColor(kBlue);
  mbd_psi[m]->SetLineColor(kBlack);
  mbd_psi_rec[m]->SetLineColor(kRed);
  mbd_psi_flat[m]->SetLineColor(kBlue);
  mbd_psi_south[m]->SetLineColor(kBlack);
  mbd_psi_rec_south[m]->SetLineColor(kRed);
  mbd_psi_flat_south[m]->SetLineColor(kBlue);
  mbd_psi_north[m]->SetLineColor(kBlack);
  mbd_psi_rec_north[m]->SetLineColor(kRed);
  mbd_psi_flat_north[m]->SetLineColor(kBlue);
  epd_psi_south[m]->SetLineColor(kBlack);
  epd_psi_flat_south[m]->SetLineColor(kBlue);
  epd_psi_north[m]->SetLineColor(kBlack);
  epd_psi_flat_north[m]->SetLineColor(kBlue);
  intt_nclus[m]->SetLineColor(kBlack);
  intt_nclus_south[m]->SetLineColor(kRed);
  intt_nclus_north[m]->SetLineColor(kBlue);
  mbd_charge[m]->SetLineColor(kBlack);
  mbd_charge_south[m]->SetLineColor(kRed);
  mbd_charge_north[m]->SetLineColor(kBlue);
  epd_charge[m]->SetLineColor(kBlack);
  epd_charge_south[m]->SetLineColor(kRed);
  epd_charge_north[m]->SetLineColor(kBlue);
}
void cal_3sub_reso(TProfile *a_bs, TProfile *a_bn, TProfile *bs_bn, TGraphErrors *reso, vector<double> binEdges)
{
  Int_t nbins = binEdges.size() - 1;
  for (int i = 1; i <= nbins; ++i)
  {
    // x 座標: bin の中心
    double x = 0.5 * (binEdges[i - 1] + binEdges[i]); // bin の左端と右端の平均
    // double x = a_bs->GetMean(1);

    // y 座標: sqrt(2 * mean)
    double mean1 = a_bs->GetBinContent(i);  // bin の mean 値
    double mean2 = a_bn->GetBinContent(i);  // bin の mean 値
    double mean3 = bs_bn->GetBinContent(i); // bin の mean 値
    double transformed = std::sqrt((mean1 * mean2) / mean3);
    // cout << "reso =" << transformed << endl;

    double error1 = a_bs->GetBinError(i);  // bin の mean 値
    double error2 = a_bn->GetBinError(i);  // bin の mean 値
    double error3 = bs_bn->GetBinError(i); // bin の mean 値

    double yerr = transformed * sqrt(pow(error1 / mean1, 2.0) + pow(error2 / mean2, 2.0) + pow(error3 / mean3, 2.0));

    /*
    if (transformed > 1.0)
    {
      transformed = 1.0;
    }
    */
    if (!isnan(transformed))
    {
      // グラフにポイントを追加
      reso->SetPoint(i, x, transformed);
      reso->SetPointError(i, 0, yerr);
    }
    else
    {
      reso->SetPoint(i, x, -1);
      reso->SetPointError(i, 0, 0);
    }
    // cout << "reso =" << transformed << endl;
    // cout << "error =" << yerr << endl;
  }
}
void cal_2sub_reso(TProfile *bs_bn, TGraphErrors *reso, vector<double> binEdges)
{
  Int_t nbins = binEdges.size() - 1;
  for (int i = 1; i <= nbins; ++i)
  {
    // x 座標: bin の中心
    double x = 0.5 * (binEdges[i - 1] + binEdges[i]); // bin の左端と右端の平均
    double mean = bs_bn->GetBinContent(i);
    double yerr = bs_bn->GetBinError(i);
    double transformed = std::sqrt(2 * mean);

    if (!isnan(transformed))
    {
      // グラフにポイントを追加
      reso->SetPoint(i, x, transformed);
      reso->SetPointError(i, 0, yerr);
    }
    else
    {
      reso->SetPoint(i, x, -1);
      reso->SetPointError(i, 0, 0);
    }
  }
}
void drawHists(TCanvas *c1, TH1F *hist1[], TH1F *hist2[], TH1F *hist3[], const std::string &pdfname)
{
  c1->Clear();
  c1->Divide(4, 5);
  for (int i = 1; i < N; i++)
  {
    c1->cd(i);
    hist1[i]->Draw();
    hist2[i]->Draw("same");
    hist3[i]->Draw("same");
  }
  c1->Print(pdfname.c_str());
}
void drawHists(TCanvas *c1, TH1F *hist1[], TH1F *hist2[], const std::string &pdfname)
{
  c1->Clear();
  c1->Divide(4, 5);
  for (int i = 1; i < N; i++)
  {
    c1->cd(i);
    hist1[i]->Draw();
    hist2[i]->Draw("same");
  }
  c1->Print(pdfname.c_str());
}
void phobos_result()
{
  const int n = 14; // データポイントの数

  // 元の座標 (X, Y)
  double x[n] = {-4.864864864864865, -4.009009009009009, -3.048048048048048, -1.7567567567567561,
                 -1.2462462462462467, -0.7807807807807805, -0.3003003003003002, 0.3153153153153152,
                 0.7657657657657664, 1.2612612612612617, 1.741741741741742, 3.6486486486486482,
                 4.24924924924925, 5.030030030030031};

  double y[n] = {0.01629778672032193, 0.02314285714285714, 0.030748490945674046, 0.03639839034205232,
                 0.03922334004024145, 0.04574245472837023, 0.0463943661971831, 0.04432997987927566,
                 0.04454728370221328, 0.04063581488933602, 0.03607243460764588, 0.02651106639839034,
                 0.019122736418511067, 0.024772635814889338};

  // エラー最大 (Y)
  double y_max[n] = {0.020426559356136825, 0.025967806841046277, 0.032704225352112676, 0.039983903420523145,
                     0.04139637826961771, 0.04856740442655936, 0.049436619718309864, 0.04726358148893361,
                     0.04748088531187123, 0.04324346076458753, 0.038571428571428576, 0.02890140845070423,
                     0.021730382293762576, 0.0345513078470825};

  // エラー最小 (Y)
  double y_min[n] = {0.012820925553319915, 0.0210784708249497, 0.029661971830985918, 0.03335613682092556,
                     0.03759356136820926, 0.04324346076458753, 0.04367806841046278, 0.04226559356136821,
                     0.04248289738430584, 0.0387887323943662, 0.034442655935613686, 0.024772635814889338,
                     0.017492957746478868, 0.01575452716297787};

  // 非対称エラーバーの計算
  double ey_low[n], ey_high[n];
  for (int i = 0; i < n; ++i)
  {
    ey_low[i] = y[i] - y_min[i];  // 下側エラー
    ey_high[i] = y_max[i] - y[i]; // 上側エラー
  }

  // Xエラーが必要なら以下を設定（ここではエラーなし）
  double ex_low[n] = {0};  // X方向の下側エラー（0ならエラーなし）
  double ex_high[n] = {0}; // X方向の上側エラー（0ならエラーなし）

  // TGraphAsymmErrorsの作成
  phobosresult = new TGraphAsymmErrors(n, x, y, ex_low, ex_high, ey_low, ey_high);
  phobosresult->SetName("phobosresult");
  gROOT->Add(phobosresult);
  gDirectory->Append(phobosresult);
}
void phobos_systematic_error()
{
  const int n = 14; // データポイントの数

  // ボックスの左下の座標 (X_min, Y_min)
  double x_min[n] = {-5.065389339228691, -4.189732252517809, -3.208286252354048, -1.970850732825678,
                     -1.4127568983869647, -0.9286170474084985, -0.47644313436502017, 0.12547285679194342,
                     0.5777122738065996, 1.0586096782117416, 1.5997379841152863, 3.4794071890608382,
                     4.0645214116105794, 4.858757062146893};

  double y_min[n] = {0.014239130434782608, 0.020326086956521737, 0.0275, 0.034239130434782605,
                     0.03641304347826087, 0.043152173913043476, 0.043804347826086956, 0.04141304347826087,
                     0.042282608695652174, 0.03826086956521739, 0.034130434782608694, 0.022282608695652177,
                     0.014130434782608697, 0.0};

  // ボックスの右上の座標 (X_max, Y_max)
  double x_max[n] = {-4.641840661590109, -3.7959551297797427, -2.8293785310734467, -1.5470400393023818,
                     -1.049078850405305, -0.5502006059117335, -0.08289527552607812, 0.4892164087447801,
                     0.986260542045363, 1.4672889543928598, 1.9787112093670682, 3.8895603045934664,
                     4.459379349873087, 5.290919511995414};

  double y_max[n] = {0.019891304347826086, 0.02717391304347826, 0.035, 0.04076086956521739,
                     0.043369565217391305, 0.04902173913043478, 0.049891304347826085, 0.048586956521739125,
                     0.048152173913043474, 0.04456521739130434, 0.041847826086956516, 0.033478260869565214,
                     0.02456521739130435, 0.034239130434782605};

  for (int i = 0; i < n; ++i)
  {
    phoboserror = new TBox(x_min[i], y_min[i], x_max[i], y_max[i]);
    phoboserror->SetFillColorAlpha(kGray, 0.2);
    // box->SetLineColor(kRed);
    phoboserror->Draw("same");
  }
}
double diff_double(double a, double b)
{
  return 2.0 * (a - b);
}
bool IsValidPsi(double a, double b, double c)
{
  return !std::isnan(a) && !std::isnan(b) && !std::isnan(c);
}
double fix_phi_psi(double a)
{
  while (a > M_PI_2)
    a -= M_PI;
  while (a < -M_PI_2)
    a += M_PI;

  return a;
}
void fill_3Prof_v2(int j, double psi, TProfile *prof_, TProfile *prof_s, TProfile *prof_n)
{
  if (!isnan(psi))
  {
    double phi = (*MBD_phi)[j];
    double v2_raw;
    double phi_psi;

    phi_psi = phi - psi;

    fix_phi_psi(phi_psi);

    v2_raw = cos(2 * phi_psi);
    float pmt_q = (*MBD_pmt_q)[j];
    if (!isnan(v2_raw) && !isnan(Adcfiltered_NClus) && !isnan(pmt_q))
    {
      prof_->Fill(Adcfiltered_NClus, v2_raw, pmt_q);

      if (j < 64)
      {
        prof_s->Fill(Adcfiltered_NClus, v2_raw, pmt_q);
      }
      else if (j >= 64)
      {
        prof_n->Fill(Adcfiltered_NClus, v2_raw, pmt_q);
      }
    }
  }
}
void correct_v2(TGraphErrors *v2_cor_graph, TProfile *v2, TGraphErrors *reso_prof)
{
  // cout<<v2->GetName()<<endl;
  double x, reso, reso_err, v2_raw, v2_raw_err, v2_cor, v2_cor_err;
  for (int i = 0; i < nq; i++)
  {
    reso_prof->GetPoint(i, x, reso);
    reso_err = reso_prof->GetErrorY(i);

    if (reso > 1)
    {
      reso = 1.0;
    }
    else if (reso <= 0)
    {
      continue;
    }

    v2_raw = v2->GetBinContent(i);
    v2_cor = v2_raw / reso;
    v2_raw_err = v2->GetBinError(i);
    v2_cor_err = abs(v2_cor * sqrt(pow(reso_err / reso, 2.0) + pow(v2_raw_err / v2_raw, 2.0)));

    // cout<<"x = "<<x<<" bincenter = "<<v2->GetBinCenter(i)<<endl;

    // cout<<"reso = "<<reso<<"  v2 raw = "<<v2_raw<<"  v2 raw err = "<<v2_raw_err<<"  v2 cor = "<<v2_cor<<"  v2 cor err = "<<v2_cor_err<<endl;

    if (!isnan(v2_cor) && !isnan(v2_cor_err))
    {
      // cout<<"reso = "<<reso<<"  v2 raw = "<<v2_raw<<"  v2 raw err = "<<v2_raw_err<<"  v2 cor = "<<v2_cor<<"  v2 cor err = "<<v2_cor_err<<endl;
      v2_cor_graph->SetPoint(v2_cor_graph->GetN(), x, v2_cor);
      v2_cor_graph->SetPointError(v2_cor_graph->GetN() - 1, 0, v2_cor_err);
    }
  }
}
void v2_graph_diff(TGraphErrors *v2_base, TGraphErrors *v2_err, TGraphErrors *v2_diff)
{
  for (int i = 0; i < v2_base->GetN(); i++)
  {
    double v2_diff_x, v2_diff_y, v2_err_y, v2_diff_err;
    v2_base->GetPoint(i, v2_diff_x, v2_diff_y);
    v2_err_y = v2_err->GetY()[i];
    v2_diff_err = abs(v2_diff_y - v2_err_y);
    // cout<<"v2_base (x,y) = ("<<v2_diff_x<<" , "<<v2_diff_y<<")"<<endl;
    // cout<<"v2_err y = "<<v2_err_y<<endl;

    if (!isnan(v2_diff_x) && !isnan(v2_diff_err))
    {
      v2_diff->SetPoint(v2_diff->GetN(), v2_diff_x, v2_diff_y);
      v2_diff->SetPointError(v2_diff->GetN() - 1, 0, v2_diff_err);
    }
  }
}
void SaveToFile()
{
  TFile *file = new TFile("output.root", "RECREATE");

  // 保存する関数
  auto saveHistograms = [&](TH1F *histArray[N])
  {
    for (int i = 0; i < N; i++)
    {
      if (histArray[i])
        histArray[i]->Write();
    }
  };

  auto saveProfile = [&](TProfile *prof)
  {
    if (prof)
      prof->Write();
  };

  auto saveGraphs = [&](TGraphErrors *graph, const char *name)
  {
    if (graph)
      graph->Write(name);
  };

  auto saveAsymmGraphs = [&](TGraphAsymmErrors *graph, const char *name)
  {
    if (graph)
      graph->Write(name);
  };

  // TH1F配列を保存
  saveHistograms(intt_psi);
  saveHistograms(intt_psi_rec);
  saveHistograms(intt_psi_flat);
  saveHistograms(intt_psi_south);
  saveHistograms(intt_psi_rec_south);
  saveHistograms(intt_psi_flat_south);
  saveHistograms(intt_psi_north);
  saveHistograms(intt_psi_rec_north);
  saveHistograms(intt_psi_flat_north);
  saveHistograms(mbd_psi);
  saveHistograms(mbd_psi_rec);
  saveHistograms(mbd_psi_flat);
  saveHistograms(mbd_psi_south);
  saveHistograms(mbd_psi_rec_south);
  saveHistograms(mbd_psi_flat_south);
  saveHistograms(mbd_psi_north);
  saveHistograms(mbd_psi_rec_north);
  saveHistograms(mbd_psi_flat_north);
  saveHistograms(epd_psi_south);
  saveHistograms(epd_psi_flat_south);
  saveHistograms(epd_psi_north);
  saveHistograms(epd_psi_flat_north);
  saveHistograms(intt_nclus);
  saveHistograms(intt_nclus_south);
  saveHistograms(intt_nclus_north);
  saveHistograms(mbd_charge);
  saveHistograms(mbd_charge_south);
  saveHistograms(mbd_charge_north);
  saveHistograms(epd_charge);
  saveHistograms(epd_charge_south);
  saveHistograms(epd_charge_north);

  // TProfileを保存
  saveProfile(intt_epds);
  saveProfile(intts_epds);
  saveProfile(inttn_epds);
  saveProfile(intt_epdn);
  saveProfile(intts_epdn);
  saveProfile(inttn_epdn);
  saveProfile(epds_epdn);
  saveProfile(intt_mbds);
  saveProfile(intts_mbds);
  saveProfile(inttn_mbds);
  saveProfile(intt_mbdn);
  saveProfile(intts_mbdn);
  saveProfile(inttn_mbdn);
  saveProfile(mbds_mbdn);
  saveProfile(intts_inttn);

  saveProfile(v2_intt_mbd);
  saveProfile(v2_intt_mbds);
  saveProfile(v2_intt_mbdn);
  saveProfile(v2_intts_mbd);
  saveProfile(v2_intts_mbds);
  saveProfile(v2_intts_mbdn);
  saveProfile(v2_inttn_mbd);
  saveProfile(v2_inttn_mbds);
  saveProfile(v2_inttn_mbdn);

  // TGraphErrorsを保存
  saveGraphs(reso_3sub_intt_epd, "reso_3sub_intt_epd");
  saveGraphs(reso_3sub_intts_epd, "reso_3sub_intts_epd");
  saveGraphs(reso_3sub_inttn_epd, "reso_3sub_inttn_epd");
  saveGraphs(reso_3sub_intt_cross_epd, "reso_3sub_intt_cross_epd");
  saveGraphs(reso_3sub_intt_mbd, "reso_3sub_intt_mbd");
  saveGraphs(reso_3sub_intts_mbd, "reso_3sub_intts_mbd");
  saveGraphs(reso_3sub_inttn_mbd, "reso_3sub_inttn_mbd");
  saveGraphs(reso_3sub_intt_cross_mbd, "reso_3sub_intt_cross_mbd");

  saveGraphs(reso_2sub_mbd, "reso_2sub_mbd");
  saveGraphs(reso_2sub_epd, "reso_2sub_epd");
  saveGraphs(reso_2sub_intt, "reso_2sub_intt");

  saveGraphs(v2_cor_intt_mbd, "v2_cor_intt_mbd");
  saveGraphs(v2_cor_intt_mbds, "v2_cor_intt_mbds");
  saveGraphs(v2_cor_intt_mbdn, "v2_cor_intt_mbdn");
  saveGraphs(v2_cor_intts_mbd, "v2_cor_intts_mbd");
  saveGraphs(v2_cor_intts_mbd_cross, "v2_cor_intts_mbd_cross");
  saveGraphs(v2_cor_intts_mbds, "v2_cor_intts_mbds");
  saveGraphs(v2_cor_intts_mbds_cross, "v2_cor_intts_mbds_cross");
  saveGraphs(v2_cor_intts_mbdn, "v2_cor_intts_mbdn");
  saveGraphs(v2_cor_intts_mbdn_cross, "v2_cor_intts_mbdn_cross");
  saveGraphs(v2_cor_inttn_mbd, "v2_cor_inttn_mbd");
  saveGraphs(v2_cor_inttn_mbd_cross, "v2_cor_inttn_mbd_cross");
  saveGraphs(v2_cor_inttn_mbds, "v2_cor_inttn_mbds");
  saveGraphs(v2_cor_inttn_mbds_cross, "v2_cor_inttn_mbds_cross");
  saveGraphs(v2_cor_inttn_mbdn, "v2_cor_inttn_mbdn");
  saveGraphs(v2_cor_inttn_mbdn_cross, "v2_cor_inttn_mbdn_cross");
  saveGraphs(v2_cor_diff_intt_mbds, "v2_cor_diff_intt_mbds");
  saveGraphs(v2_cor_diff_intt_mbdn, "v2_cor_diff_intt_mbdn");

  saveGraphs(v2_eta, "eta_vs_v2");
  // TGraphAsymmErrorsを保存
  saveAsymmGraphs(phobosresult, "phobosresult");

  file->Write();
  file->Close();
}

void draw_phobosresult()
{
  if (!phobosresult || phobosresult->GetN() == 0)
  {
    std::cout << "Creating a new TGraph..." << std::endl;
    delete phobosresult; // 既存のオブジェクトを削除
    phobos_result();
  }

  phobosresult->GetXaxis()->SetLimits(-6, 6);
  phobosresult->GetYaxis()->SetRangeUser(0, 0.05);

  phobosresult->SetMarkerStyle(26);
  phobosresult->SetMarkerSize(2);

  phobosresult->Draw("AP");
  phobos_systematic_error();
}

void set_point_eta_v2(TFile *ifile, TGraphErrors *reso_prof, vector<double> binEdges, TArrayD bins,TGraphErrors*eta_vs_v2)
{
  TProfile *intts_mbdn_ = new TProfile("", "", binEdges.size() - 1, bins.GetArray());
  TProfile *intts_mbds_ = new TProfile("", "", binEdges.size() - 1, bins.GetArray());
  TProfile *inttn_mbdn_ = new TProfile("", "", binEdges.size() - 1, bins.GetArray());
  TProfile *inttn_mbds_ = new TProfile("", "", binEdges.size() - 1, bins.GetArray());

  double x, reso;
  for (int i = 0; i < nq; i++)
  {
    TTree *itree = (TTree *)ifile->Get(Form("EventTree_%d", i));
    setBranchAdresses(itree);
    Long64_t nEntries = itree->GetEntries();
    reso_prof->GetPoint(i+1, x, reso);
    if (reso > 1)
    {
      reso = 1.0;
    }
    else if (reso <= 0)
    {
      continue;
    }

    cout<<"reso "<<i+1 <<" = "<<reso<<endl;

    for (Long64_t k = 0; k < nEntries; k++)
    {
      itree->GetEntry(k);
      if (Adcfiltered_NClus != 0 && !isnan(psi_flat))
      {
        for (int j = 0; j < MBD_phi->size(); j++)
        {
          double phi = (*MBD_phi)[j];
          double v2_raw_south, v2_raw_north;
          double psi_phi_south, psi_phi_north;

          // psi_phi=atan(tan(phi-psi_flat));
          psi_phi_south = phi - psi_flat_south;
          psi_phi_north = phi - psi_flat_north;

          fix_phi_psi(psi_phi_south);
          fix_phi_psi(psi_phi_north);

          v2_raw_south = cos(2 * psi_phi_south);
          v2_raw_north = cos(2 * psi_phi_north);

          double v2_reco_south = v2_raw_south / reso;
          double v2_reco_north = v2_raw_north / reso;
          float pmt_q = (*MBD_pmt_q)[j];
          if (reso <= 1.0 && !isnan(reso))
          {
            if (!isnan(v2_reco_south) && !isnan(Adcfiltered_NClus) && !isnan(pmt_q))
            {
              if (j < 64)
                intts_mbds_->Fill(Adcfiltered_NClus, v2_reco_south, pmt_q);
              if (j >= 64)
                intts_mbdn_->Fill(Adcfiltered_NClus, v2_reco_south, pmt_q);
            }

            if (!isnan(v2_reco_north) && !isnan(Adcfiltered_NClus) && !isnan(pmt_q))
            {
              if (j < 64)
                inttn_mbds_->Fill(Adcfiltered_NClus, v2_reco_north, pmt_q);
              if (j >= 64)
                inttn_mbdn_->Fill(Adcfiltered_NClus, v2_reco_north, pmt_q);
            }
          }
        }
      }
    }
  }


  cout<<"v2 intts mbds = "<<intts_mbds_->GetMean(2)<<"  v2 inttn mbds = "<<inttn_mbds_->GetMean(2)<<endl;
  cout<<"v2 intts mbdn = "<<intts_mbdn_->GetMean(2)<<"  v2 inttn mbdn = "<<inttn_mbdn_->GetMean(2)<<endl;

  eta_vs_v2->SetPoint(eta_vs_v2->GetN(),-4.06,inttn_mbds_->GetMean(2));
  eta_vs_v2->SetPointError(eta_vs_v2->GetN()-1,0,abs(inttn_mbds_->GetMean(2)-intts_mbds_->GetMean(2)));
  eta_vs_v2->SetPoint(eta_vs_v2->GetN(),4.06,intts_mbdn_->GetMean(2));
  eta_vs_v2->SetPointError(eta_vs_v2->GetN()-1,0,abs(inttn_mbdn_->GetMean(2)-intts_mbdn_->GetMean(2)));
}
