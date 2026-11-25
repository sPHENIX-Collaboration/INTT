#include "utilities/sPhenixStyle.C"

int GetNCollisionFromRunNum(int run);
int GetOpenTimeFromRunNum(int run);
int FindEdgeBin(TH1D* hist, int peak_bin);
TH1D* ShiftHistoWithoutDupulication(TH1D* hist, int offset);
int AddOffsetWithoutOverflow(int this_bin, int offset);
int GetPreviousWithoutUnderflow(int bin);
void DrawBoxFromValue( TH2D* h2D_NInttHitInner_NInttHitOuter, double x_val_min, double x_val_max, double y_val_min, double y_val_max, int color );

void PrintPdfs(int run){

  SetsPhenixStyle();

  // std::string counts = "100k";
  int n_collision = GetNCollisionFromRunNum(run);
  int open_time = GetOpenTimeFromRunNum(run);

  // std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/completed/1M_%d.root", run, run);
  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/completed/30files_Criteria2_woTTree_0hitHandled.root", run);
  TFile* file = TFile::Open(filename.c_str());
  if ( !file || file->IsZombie() ){
    std::cout << "Error: cannot open file " << filename << std::endl;
    exit(1);
  }
  

  TH2D* h2D_NInttHitInner_NInttHitOuter = (TH2D*)file->Get("h2D_NInttHitInner_NInttHitOuter");
  TH2D* h2D_NInttHitInner_NInttHitOuter_corrected = (TH2D*)file->Get("h2D_NInttHitInner_NInttHitOuter_corrected");
  TH2D* h2D_EventBcoSpacing_NCohInNextEvent_ = (TH2D*)file->Get("h2D_EventBcoSpacing_NCohInNextEvent");
  TH2D* h2D_NCarriedInner_NCarriedOuter = (TH2D*)file->Get("h2D_NCarriedInner_NCarriedOuter");
  TH2D* h2D_Halfladder_NCOH = (TH2D*)file->Get("h2D_Halfladder_NCOH");
  TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent = (TH2D*)file->Get("h2D_EventBcoSpacing_NHitInPreviousEvent");
  TH2D* h2D_EventBcoSpacing_NCohInNextEvent = (TH2D*)file->Get("h2D_EventBcoSpacing_NCohInNextEvent"); 
  TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder = (TH2D*)file->Get("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder");
  TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_bychip = (TH2D*)file->Get("h2D_EventBcoSpacing_NHitInPreviousEvent_bychip");
  TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner = (TH2D*)file->Get("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner"); 
  TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer = (TH2D*)file->Get("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer"); 

  TH2D* h2D_MisasignedBcoDiff_NHitInPreviousEvent = (TH2D*)file->Get("h2D_MisasignedBcoDiff_NHitInPreviousEvent");
  TH2D* h2D_MisasignedBcoDiff_NCohInNextEvent = (TH2D*)file->Get("h2D_MisasignedBcoDiff_NCohInNextEvent");

  TH2D* h2D_NInttHitInner_NInttHitOuter_narrow_spacing = (TH2D*)file->Get("h2D_NInttHitInner_NInttHitOuter_narrow_spacing");
  
  TCanvas* c1 = new TCanvas("c1", "Title", 900, 600);
  c1->cd();
  h2D_NInttHitInner_NInttHitOuter->SetTitle(Form("h2D_NInttHitInner_NInttHitOuter(run %d, n_collision %d, open time %d); # of trigger timing hits (INTT inner); # of trigger timing hits (INTT outer)", run, n_collision, open_time));
  h2D_NInttHitInner_NInttHitOuter->Draw();
  c1->SetLogz();
  c1->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_NInttHitInner_NInttHitOuter.png", run) );
  TF1 *f = new TF1("f","x",0,10000);
  f->Draw("same");
  c1->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_NInttHitInner_NInttHitOuter_with_line.png", run) );
  h2D_NInttHitInner_NInttHitOuter->GetXaxis()->SetRangeUser(0,1500);
  h2D_NInttHitInner_NInttHitOuter->GetYaxis()->SetRangeUser(0,1500);
  h2D_NInttHitInner_NInttHitOuter->Draw();
  c1->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_NInttHitInner_NInttHitOuter_zoomed.png", run) );

  TCanvas* c2 = new TCanvas("c2", "Title", 900, 600);
  c2->cd();
  h2D_NInttHitInner_NInttHitOuter_corrected->SetTitle(Form("h2D_NInttHitInner_NInttHitOuter_corrected(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_NInttHitInner_NInttHitOuter_corrected->Draw();
  c2->SetLogz();
  c2->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_NInttHitInner_NInttHitOuter_corrected.png", run) );
  h2D_NInttHitInner_NInttHitOuter_corrected->GetXaxis()->SetRangeUser(0,1500);
  h2D_NInttHitInner_NInttHitOuter_corrected->GetYaxis()->SetRangeUser(0,1500);
  h2D_NInttHitInner_NInttHitOuter_corrected->Draw();
  c2->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_NInttHitInner_NInttHitOuter_corrected_zoomed.png", run) );

  TCanvas* c3 = new TCanvas("c3", "Title", 900, 600);
  c3->cd();
  h2D_EventBcoSpacing_NCohInNextEvent_->SetTitle(Form("h2D_EventBcoSpacing_NCohInNextEvent_(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_NCohInNextEvent_->Draw();
  c3->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_EventBcoSpacing_NCarriedHits.png", run) );
  
  TCanvas* c4 = new TCanvas("c4", "Title", 900, 600);
  c4->cd();
  h2D_NCarriedInner_NCarriedOuter->SetTitle(Form("h2D_NCarriedInner_NCarriedOuter(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_NCarriedInner_NCarriedOuter->Draw();
  c4->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_NCarriedInner_NCarriedOuter.png", run) );
  h2D_NCarriedInner_NCarriedOuter->GetXaxis()->SetRangeUser(0,1000);
  h2D_NCarriedInner_NCarriedOuter->GetYaxis()->SetRangeUser(0,1000);
  h2D_NCarriedInner_NCarriedOuter->Draw();
  c4->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_NCarriedInner_NCarriedOuter_zoomed.png", run) );

  TCanvas* c5 = new TCanvas("c5", "Title", 900, 600);
  c5->cd();
  h2D_Halfladder_NCOH->SetTitle(Form("h2D_Halfladder_NCOH(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_Halfladder_NCOH->Draw();
  c5->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_Halfladder_NCOH.png", run) );

  TCanvas* c6 = new TCanvas("c6", "Title", 900, 600);
  c6->cd();
  h2D_EventBcoSpacing_NHitInPreviousEvent->SetTitle(Form("h2D_EventBcoSpacing_NHitInPreviousEvent(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_NHitInPreviousEvent->Draw();
  c6->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_EventBcoSpacing_NHitInPreviousEvent.png", run) );

  TCanvas* c7 = new TCanvas("c7", "Title", 900, 600);
  c7->cd();
  h2D_EventBcoSpacing_NCohInNextEvent->SetTitle(Form("h2D_EventBcoSpacing_NCohInNextEvent(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_NCohInNextEvent->Draw();
  c7->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_EventBcoSpacing_NCohInNextEvent.png", run) );
 
  TCanvas* c8 = new TCanvas("c8", "Title", 900, 600);
  c8->cd();
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder->SetTitle(Form("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder->Draw();
  c8->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder.png", run) );
   
  TCanvas* c9 = new TCanvas("c9", "Title", 900, 600);
  c9->cd();
  h2D_EventBcoSpacing_NHitInPreviousEvent_bychip->SetTitle(Form("h2D_EventBcoSpacing_NHitInPreviousEvent_bychip(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_NHitInPreviousEvent_bychip->Draw();
  c9->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_EventBcoSpacing_NHitInPreviousEvent_bychip.png", run) );
   
  TCanvas* c10 = new TCanvas("c10", "Title", 900, 600);
  c10->cd();
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner->SetTitle(Form("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner->Draw();
  c10->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner.png", run) );
  
  TCanvas* c11 = new TCanvas("c11", "Title", 900, 600);
  c11->cd();
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer->SetTitle(Form("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer->Draw();
  c11->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer.png", run) );
    
  TCanvas* c12 = new TCanvas("c12", "Title", 900, 600);
  c12->cd();
  h2D_MisasignedBcoDiff_NHitInPreviousEvent->SetTitle(Form("h2D_MisasignedBcoDiff_NHitInPreviousEvent(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_MisasignedBcoDiff_NHitInPreviousEvent->Draw();
  c12->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_MisasignedBcoDiff_NHitInPreviousEvent.png", run) );
    
  TCanvas* c13 = new TCanvas("c13", "Title", 900, 600);
  c13->cd();
  h2D_MisasignedBcoDiff_NCohInNextEvent->SetTitle(Form("h2D_MisasignedBcoDiff_NCohInNextEvent(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_MisasignedBcoDiff_NCohInNextEvent->Draw();
  c13->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_MisasignedBcoDiff_NCohInNextEvent.png", run) );
  
  TCanvas* c14 = new TCanvas("c14", "Title", 900, 600);
  c14->cd();
  h2D_NInttHitInner_NInttHitOuter_narrow_spacing->SetTitle(Form("h2D_NInttHitInner_NInttHitOuter_narrow_spacing(run %d, n_collision %d, open time %d)", run, n_collision, open_time));
  h2D_NInttHitInner_NInttHitOuter_narrow_spacing->Draw();
  c14->Print( Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/h2D_NInttHitInner_NInttHitOuter_narrow_spacing.png", run) );
   
   
 

  file->Close();
  delete file;
  delete c1;
  delete c2;
  delete c3;
  delete c4;
  delete c5;
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
    {68015, {3,60}},

    {82400, {126,60}},

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
    {68015, {3,60}},

    {82400, {126,60}},
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
