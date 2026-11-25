#include "Utility/sPhenixStyle.C"
#include "CalculateRatioWithBgsubtraction.cc"
#include "CalculateRatio.cc"

int FindEdgeBin(TH1D* hist, int peak_bin);
TH1D* ShiftHistoWithoutDupulication(TH1D* hist, int offset, TFile* file);
int AddOffsetWithoutOverflow(int this_bin, int offset);
void PutCaption();
std::string GetDate();
void PutExplanation(double fraction);




void ShiftPlot(int run){

  SetsPhenixStyle();
  
  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/run%d.root", run); // output from Fun4All module, but at the same time, input for this macro
  TFile* file = TFile::Open(filename.c_str(), "update");
  if ( !file || file->IsZombie() ){
    std::cerr << "ERROR: Could not open" << filename << std::endl;
    exit(1);
  }

  file->cd();
  TH1D* h_bco_diff = (TH1D*)file->Get("h_bco_diff"); //if your root file has a non-TH1D object with this name, it will cause empty canvas. Problem. We should check the type. 

  int peak_bin = h_bco_diff->GetMaximumBin();
  int offset = FindEdgeBin(h_bco_diff, peak_bin); // We will shift the histogram by this offset, which is the bin number of the falling edge.

  TH1D* h_bco_diff_shifted = ShiftHistoWithoutDupulication(h_bco_diff, offset, file);
  
  
  // TCanvas* c1 = new TCanvas("c1", "", 900,600);
  // c1->cd();
  // //h_bco_diff_shifted->SetTitle("; Beam crossing (106 ns / crossing);  INTT hit counts per event");
  // h_bco_diff_shifted->SetTitle("; Crossing relative to GTM [BCO];  INTT hit counts per event");
  // h_bco_diff_shifted->SetMaximum(100);
  // h_bco_diff_shifted->SetMinimum(0.01);
  // gPad->SetLogy();
  // h_bco_diff_shifted->Draw("HIST");
  // //PutCaption();
  // std::vector< std::pair<double, double> > frac_and_ratio = CalculateRatioWithBgsubtraction(run); /* Returns ( {asymmetry, error},  {ratio, error} ) for given run. */
  // //std::vector< std::pair<double, double> > frac_and_ratio = CalculateRatio(run); /* Returns ( {asymmetry, error},  {ratio, error} ) for given run. */
  // double fraction = frac_and_ratio[0].first;
  // PutExplanation(fraction);

  // std::string output_canvas = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/image/run%d.pdf" , run);
  // c1->Print(output_canvas.c_str());

  file->cd();
  h_bco_diff_shifted->Write("h_bco_diff_shifted", TObject::kOverwrite);
  //h_bco_diff_shifted->Write();
  //c1->Write("c_bco_diff_shifted_sStyle", TObject::kOverwrite);
  

  file->Close();
  delete file;
}




int FindEdgeBin(TH1D* hist, int peak_bin){
  double threshold = 0.0000075;
  // double threshold = 0.00007; /* for run 79252 */
  // double threshold = 0.007; /* for run 79266 */
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

TH1D* ShiftHistoWithoutDupulication(TH1D* hist, int offset, TFile* file){

  file->cd();
  TH1D* hist_shifted = NULL;
  hist_shifted = (TH1D*)file->Get("h_bco_diff_shifted");

  if (hist_shifted == NULL){
    // if it does not exists in the file, create a histogram with same parameters as the hist.
    hist_shifted = (TH1D*)hist->Clone("h_bco_diff_shifted");
    hist_shifted->Reset();
  } else {
    // if it already exists in the file, just reset it.
    hist_shifted->Reset();
  }
  
  int bin_num_before;
  for (int i=0; i < hist->GetNbinsX(); i++) {
    bin_num_before = AddOffsetWithoutOverflow(1+i, offset);
    hist_shifted->SetBinContent(1+i, hist->GetBinContent(bin_num_before));
    hist_shifted->SetBinError(1+i, hist->GetBinError(bin_num_before));
  }
  
  return hist_shifted;
};

//int GetPreviousWithoutUnderflow(int bin){
//  if (bin == 1) {
//    return 128;
//  } else {
//    return bin-1;
//  }
//}

int AddOffsetWithoutOverflow(int this_bin, int offset) {
  if ( this_bin + offset == 128){
    return 128;
  } else {
    return (this_bin + offset)%128;
  }
}

std::string GetDate(){
  //  return "5/13/2025"; // use this once the dete is determined
  TDatime dt;
  int year    = dt.GetYear();
  int month    = dt.GetMonth();
  int day    = dt.GetDay();

  // format: mm/dd/yyyy
  std::stringstream ss;
  ss << month << "/" << day << "/" << year;

  return ss.str();
};

void PutCaption(){
/*from Genki*/
  TLatex* tex = new TLatex();
  double line_height = 0.05; // no need to change
  double first_margin = -0.12; // the margin from the top of the canvas to the top line (date). You can modify
  double pos_x = 0.60; // you can modify
  double pos_y = 1.0  + first_margin; // - line_height;

  /* Date, you may want to change 0.65 to pos_x */
  tex->DrawLatexNDC( 0.65, pos_y,
             string("#it{" + GetDate() + "}").c_str() );

  // sPHENIX Internal or sPHENIX Prelimnary
  pos_y -= line_height;
  //tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
  tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );

  // p+p 200 GeV
  pos_y -= line_height  + 0.03;
  tex->DrawLatexNDC( pos_x, pos_y, ("Run-24 #it{p+p} 200 GeV") );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "INTT Triggered mode" );
}

// void PutExplanation(double fraction){
//   TLatex* tex = new TLatex();
//   double line_height = 0.05; // no need to change
//   double first_margin = -0.12; // the margin from the top of the canvas to the top line (date). You can modify
//   double pos_x = 0.50; // you can modify
//   double pos_y = 1.0  + first_margin; // - line_height;

//   /* Date, you may want to change 0.65 to pos_x */
//   tex->DrawLatexNDC( pos_x, pos_y,
//              string("#it{" + GetDate() + "} " + "#it{#bf{sPHENIX}} Internal").c_str() );

//   // p+p 200 GeV
//   pos_y -= line_height  + 0.03;
//   tex->DrawLatexNDC( pos_x, pos_y, ("Run-25 #it{Au+Au} 200 GeV") );

//   pos_y -= line_height;
//   tex->DrawLatexNDC( pos_x, pos_y, (std::string("#it{N}_{L} / (#it{N}_{L} + #it{N}_{R}) = ") + std::to_string(fraction)).c_str() );

// }
