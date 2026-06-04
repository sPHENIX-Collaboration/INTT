#include "Utility/sPhenixStyle.C"
#include "CalculateRatioWithBgsubtraction.cc"
#include "CalculateRatio.cc"

std::string GetDate();
void PutExplanation(double fraction, int run);

void PrintPdf(int run){
  SetsPhenixStyle();
  
  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/run%d.root", run); // output from Fun4All module, but at the same time, input for this macro
  TFile* file = TFile::Open(filename.c_str(), "update");
  if ( !file || file->IsZombie() ){
    std::cerr << "ERROR: Could not open" << filename << std::endl;
    exit(1);
  }

  file->cd();
  TH1D* h_bco_diff_shifted = (TH1D*)file->Get("h_bco_diff_shifted"); //if your root file has a non-TH1D object with this name, it will cause empty canvas. Problem. We should check the type. 


  TCanvas* c1 = new TCanvas("c1", "", 900,600);
  c1->cd();
  h_bco_diff_shifted->SetTitle("; Crossing relative to GTM [BCO];  INTT hit counts per event");
  h_bco_diff_shifted->SetMaximum(5000);
  h_bco_diff_shifted->SetMinimum(0.00001);
  gPad->SetLogy();
  h_bco_diff_shifted->Draw("HIST");
  // std::vector< std::pair<double, double> > frac_and_ratio = CalculateRatioWithBgsubtraction(run); /* Returns ( {asymmetry, error},  {ratio, error} ) for given run. */
  std::vector< std::pair<double, double> > frac_and_ratio = CalculateRatio(run); /* Returns ( {asymmetry, error},  {ratio, error} ) for given run. */
  double fraction = frac_and_ratio[0].first;
  PutExplanation(fraction, run);


  std::string output_canvas = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/image/run%d.pdf" , run);
  c1->Print(output_canvas.c_str());
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
}

void PutExplanation(double fraction, int run){
  TLatex* tex = new TLatex();
  double line_height = 0.05; // no need to change
  double first_margin = -0.12; // the margin from the top of the canvas to the top line (date). You can modify
  double pos_x = 0.50; // you can modify
  double pos_y = 1.0  + first_margin; // - line_height;

  /* Date, you may want to change 0.65 to pos_x */
  tex->DrawLatexNDC( pos_x, pos_y,
             string("#it{" + GetDate() + "} " + "#it{#bf{sPHENIX}} Internal").c_str() );

  // p+p 200 GeV
  pos_y -= line_height  + 0.03;
  //tex->DrawLatexNDC( pos_x, pos_y, "Run-25 #it{Au+Au} 200 GeV" );
  tex->DrawLatexNDC( pos_x, pos_y, Form("Run %d", run) );


  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, (std::string("#it{N}_{L} / (#it{N}_{L} + #it{N}_{R}) = ") + std::to_string(fraction)).c_str() );

}
