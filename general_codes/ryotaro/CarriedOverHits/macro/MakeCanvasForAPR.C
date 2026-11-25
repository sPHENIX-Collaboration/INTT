#include "utilities/sPhenixStyle.C"
#include "MyUtilities/my.cc"


int FindEdgeBin(TH1D* hist, int peak_bin);
TH1D* ShiftHistoWithoutDupulication(TH1D* hist, int offset);
int AddOffsetWithoutOverflow(int this_bin, int offset);
int GetPreviousWithoutUnderflow(int bin);
void DrawBoxFromValue( TH2D* h2D_NInttHitInner_NInttHitOuter, double x_val_min, double x_val_max, double y_val_min, double y_val_max, int color );

void MakeCanvasForAPR(int run){

  SetsPhenixStyle();

  // std::string counts = "100k";
  int n_collision = GetNCollisionFromRunNum(run);
  int open_time = GetOpenTimeFromRunNum(run);

  // std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/%s/10M_%d_%s.root", run, tag.c_str(), run, tag.c_str());
  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/completed/4646files_Criteria2_woTTree_0hitHandled_halfladderCutted.root");
  TFile* file = TFile::Open(filename.c_str());
  if ( !file || file->IsZombie() ){
    std::cout << "Error: cannot open file " << filename << std::endl;
    exit(1);
  }
  
  TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_all_event = (TH2D*)file->Get("h2D_EventBcoSpacing_NHitInPreviousEvent_all_event_");
  TH2D* h2D_EventBcoSpacing_CorrectedMultiplicity_all_event = (TH2D*)file->Get("h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_");
  

  TCanvas* c1 = new TCanvas("c1", "Title", 900, 600);
  c1->cd();
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->SetTitle(Form("h2D_EventBcoSpacing_NHitInPreviousEvent_all_event (run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->GetXaxis()->SetTitle("Event interval to the next event [Beam Collision Clock 9.4MHz]");
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->GetXaxis()->SetNdivisions(407);
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->GetYaxis()->SetTitle("Number of INTT hits");
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->GetYaxis()->SetNdivisions(206);
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->Draw();
  c1->SetLogz();
  PutCaption_BeforeCorrection(run, -0.1, 0.2, 0.94); /* PutCaption(int run, double first_margin = -0.1, double pos_x = 0.2, double pos_y = 0.9) */
  // c1->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/ForAPR/h2D_EventBcoSpacing_NHitInPreviousEvent_all_event_%d.pdf", run, run));

  TCanvas* c2 = new TCanvas("c2", "Title", 900, 600);
  c2->cd();
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event->SetTitle(Form("h2D_EventBcoSpacing_CorrectedMultiplicity_all_event (run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event->GetXaxis()->SetTitle("Event interval to the next event [Beam Collision Clock 9.4MHz]");
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event->GetXaxis()->SetNdivisions(407);
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event->GetYaxis()->SetRange(-1,25000);
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event->GetYaxis()->SetTitle("Number of INTT hits");
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event->GetYaxis()->SetNdivisions(206);
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event->Draw();
  c2->SetLogz();
  PutCaption_AfterCorrection(run, -0.1, 0.2, 0.94); /* PutCaption(int run, double first_margin = -0.1, double pos_x = 0.2, double pos_y = 0.9) */
  // c2->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/ForAPR/h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_%d.pdf", run, run) );



  TCanvas* c3 = new TCanvas("c3", "Title", 600,700);
  // TCanvas* c3 = new TCanvas("c3", "Title", 900, 600);
  c3->cd();

  TPad *pad_top = new TPad("pad_top", "Top Pad", 0, 0.5, 1, 1);
  // pad_top->SetBottomMargin(0.001); 
  pad_top->SetBottomMargin(0.03); 
  pad_top->SetTopMargin(0.08);
  pad_top->Draw();
  pad_top->cd();

  gPad->SetLogz();
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->GetXaxis()->SetLabelSize(0);
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->GetXaxis()->SetTitleSize(0);
  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event->Draw();
  PutCaption_BeforeCorrection(run, -0.1, 0.2, 0.9); /* PutCaption(int run, double first_margin = -0.1, double pos_x = 0.2, double pos_y = 0.9) */


  c3->cd();
  TPad *pad_bottom = new TPad("pad_bottom", "Bottom Pad", 0, 0, 1, 0.5);
  // pad_bottom->SetTopMargin(0.0001);  
  pad_bottom->SetTopMargin(0.001); 
  // pad_bottom->SetBottomMargin(0.12);
  pad_bottom->Draw();
  pad_bottom->cd();

  gPad->SetLogz();
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event->Draw();
  PutCaption_AfterCorrection(run, -0.1, 0.2, 0.99); /* PutCaption(int run, double first_margin = -0.1, double pos_x = 0.2, double pos_y = 0.9) */

  c3->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/ForAPR/Combined_h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_%d.pdf", run, run) );



  file->Close();
  delete file;
  delete c1;
  delete c2;
}

int GetNCollisionFromRunNum(int run){
  /* {run_number, {n_collision, open_time}} */
  std::vector< std::pair<int, std::pair<int, int>> > run_ncoll_open = {
    
    {66522, {100,60}}, /* physics run */
    {66682, {100,60}}, /* physics run */
    {68002, {100,60}}, /* physics run */
    {71345, {100,60}}, /* physics run */
    {71346, {126,60}}, /* physics run */
    {71347, {126,127}}, /* physics run */

    {75519, {126,127}}, /* physics run */
    
    {67542, {100,127}},
    {67544, {100,110}},
    {67545, {100,90}},
    //{67546, {100,80}},
    {67547, {100,60}},
    {67548, {100,40}},
    {67549, {100,25}},
    {67550, {2,127}},
    {67552, {2,110}},
    {67553, {2,90}},
    {67554, {2,80}},
    {67555, {2,60}},
    //{67556, {2,40}},
    {67557, {2,25}},
    {67558, {50,127}},
    {67559, {50,110}},
    {67560, {50,90}},
    {67563, {50,60}},
    {67564, {50,40}},
    {67565, {50,25}},
    {67566, {50,80}},

    {68007, {100,60}},
    {68008, {127,127}},
    {68009, {3,127}},
    {68010, {3,60}},
    {68012, {100,60}},
    {68013, {127,127}},
    {68014, {3,127}},
    {68015, {3,60}}

  };

  int n_collision;
  for (auto pair : run_ncoll_open){
    if (pair.first == run){
      n_collision = pair.second.first;
    }
  }

  return n_collision;

}

int GetOpenTimeFromRunNum(int run){
  std::vector< std::pair<int, std::pair<int, int>> > run_ncoll_open = {

    {66522, {100,60}}, /* physics run */
    {66682, {100,60}}, /* physics run */
    {68002, {100,60}}, /* physics run */
    {71345, {100,60}}, /* physics run */
    {71346, {126,60}}, /* physics run */
    {71347, {126,127}}, /* physics run */

    {75519, {126,127}}, /* physics run */

    {67542, {100,127}},
    {67544, {100,110}},
    {67545, {100,90}},
    //{67546, {100,80}},
    {67547, {100,60}},
    {67548, {100,40}},
    {67549, {100,25}},
    {67550, {2,127}},
    {67552, {2,110}},
    {67553, {2,90}},
    {67554, {2,80}},
    {67555, {2,60}},
    //{67556, {2,40}},
    {67557, {2,25}},
    {67558, {50,127}},
    {67559, {50,110}},
    {67560, {50,90}},
    {67563, {50,60}},
    {67564, {50,40}},
    {67565, {50,25}},
    {67566, {50,80}},

    {68007, {100,60}},
    {68008, {127,127}},
    {68009, {3,127}},
    {68010, {3,60}},
    {68012, {100,60}},
    {68013, {127,127}},
    {68014, {3,127}},
    {68015, {3,60}}
  };

  int open_time;
  for (auto pair : run_ncoll_open){
    if (pair.first == run){
      open_time = pair.second.second;
    }
  }

  return open_time;
}

int FindEdgeBin(TH1D* hist, int peak_bin){
  double threshold = 0.0000075;
  //double threshold = 0.0075;
  double height_ratio = 1;
  int this_bin = peak_bin;
  
  for (int i=0; i < hist->GetNbinsX(); i--){
    this_bin = GetPreviousWithoutUnderflow(this_bin); // This is basically identical to (this_bin - 1), except in case of reaching the underflow bin. 
    height_ratio = hist->GetBinContent(this_bin) / hist->GetBinContent(peak_bin);
    if (height_ratio < threshold) {
      return this_bin;
    }
  }

  std::cerr << "No edge found!" << std::endl;
  exit(1);

}

TH1D* ShiftHistoWithoutDupulication(TH1D* hist, int offset){

  TH1D* hist_shifted = (TH1D*)hist->Clone("h_bco_diff_shifted");
  
  int bin_num_before;
  for (int i=0; i < hist->GetNbinsX(); i++) {
    bin_num_before = AddOffsetWithoutOverflow(1+i, offset);
    hist_shifted->SetBinContent(1+i, hist->GetBinContent(bin_num_before));
    hist_shifted->SetBinError(1+i, hist->GetBinError(bin_num_before));
  }
  
  return hist_shifted;
}

int GetPreviousWithoutUnderflow(int bin){
 if (bin == 1) {
   return 128;
 } else {
   return bin-1;
 }
}

int AddOffsetWithoutOverflow(int this_bin, int offset) {
  if ( this_bin + offset == 128){
    return 128;
  } else {
    return (this_bin + offset)%128;
  }
}


void DrawBoxFromValue( TH2D* h2D_NInttHitInner_NInttHitOuter, double x_val_min, double x_val_max, double y_val_min, double y_val_max, int color ){
  int xbin_min = h2D_NInttHitInner_NInttHitOuter->GetXaxis()->FindBin(x_val_min);
  int xbin_max = h2D_NInttHitInner_NInttHitOuter->GetXaxis()->FindBin(x_val_max);
  int ybin_min = h2D_NInttHitInner_NInttHitOuter->GetYaxis()->FindBin(y_val_min);
  int ybin_max = h2D_NInttHitInner_NInttHitOuter->GetYaxis()->FindBin(y_val_max);

  double x1 = h2D_NInttHitInner_NInttHitOuter->GetXaxis()->GetBinLowEdge(xbin_min);
  double x2 = h2D_NInttHitInner_NInttHitOuter->GetXaxis()->GetBinUpEdge(xbin_max);
  double y1 = h2D_NInttHitInner_NInttHitOuter->GetYaxis()->GetBinLowEdge(ybin_min);
  double y2 = h2D_NInttHitInner_NInttHitOuter->GetYaxis()->GetBinUpEdge(ybin_max);

  TBox* box = new TBox(x1, y1, x2, y2);
  box->SetLineColor(color);
  box->SetLineWidth(2);
  box->SetFillStyle(0);
  box->Draw("same");
}

