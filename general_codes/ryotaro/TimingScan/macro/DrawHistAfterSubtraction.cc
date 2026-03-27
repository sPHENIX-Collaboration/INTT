#include "sPhenixStyle.C"

double GetMeanInOddBins(TH1D* hist, int range[2]);
double GetMeanInEvenBins(TH1D* hist, int range[2]);
int GetPreviousWithoutUnderflow(int bin);
int GetNextWithoutOverflow(int bin);


/* Estimates background levels in plateau region( defined with the range_limit[2]). */
/* Returns <average_in_odd_bins, average_in_even_bins> as an estimation. */
void DrawHistAfterSubtraction(int run) {
  SetsPhenixStyle();

  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/run%d.root", run);
  TFile* file = TFile::Open(filename.c_str(), "update");
  if ( !file || file->IsZombie() ){
    std::cerr << "ERROR: Could not open" << filename << std::endl;
    exit(1);
  }

  file->cd();
  TH1D* h_bco_diff_shifted = (TH1D*)file->Get("h_bco_diff_shifted");

  int range_limit[2] = {10,80}; // range in which the background level will be estimated.

  double average_odd_bins = GetMeanInOddBins(h_bco_diff_shifted, range_limit);
  double average_even_bins = GetMeanInEvenBins(h_bco_diff_shifted, range_limit);

  TCanvas* c1 = new TCanvas("c1", "Title", 900, 600);
  c1->cd();
  c1->SetGrid();
  h_bco_diff_shifted->Draw();

  TF1* f_odd = new TF1("odd", std::to_string(average_odd_bins).c_str(), range_limit[0], range_limit[1]);
  f_odd->Draw("same");

  TF1* f_even = new TF1("even", std::to_string(average_even_bins).c_str(), range_limit[0], range_limit[1]);
  f_even->Draw("same");

  TF1* f_even_dotted = new TF1("even_dotted", std::to_string(average_even_bins).c_str(), 1, 127);
  f_even_dotted->SetLineStyle(3);
  f_even_dotted->Draw("same");

  // std::string fileout = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/TimingScan/output/root/TEST_run%d.root", run);
  // TFile* file_test = TFile::Open(fileout.c_str(), "update");
  //file->cd();
  //c1->Write("c1", TObject::kOverwrite);
  //c1->Print("aa.pdf"); 


  TH1D* h_bco_diff_subtracted = (TH1D*)h_bco_diff_shifted->Clone();
  //TCanvas* c_bco_diff_subtracted = (TCanvas*)h_bco_diff_shifted->Clone();
  h_bco_diff_subtracted->Reset();

  /* do BG subtraction */
  double remaining_count;
  for (int bin=1; bin <= h_bco_diff_shifted->GetNbinsX(); bin++) {
    
    if (bin%2 == 1) {
      remaining_count = h_bco_diff_shifted->GetBinContent(bin) - average_odd_bins;
    } else {
      remaining_count = h_bco_diff_shifted->GetBinContent(bin) - average_even_bins;    
    }
    
    h_bco_diff_subtracted->SetBinContent(bin,remaining_count);
  }

  
  // std::string fileout = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/TimingScan/output/root/TEST_run%d.root", run);
  // TFile* file_test = TFile::Open(fileout.c_str(), "update");
  // if ( !file_test || file_test->IsZombie() ){
  //   std::cerr << "ERROR: Could not open" << fileout << std::endl;
  //   exit(1);
  // }

  //TCanvas* c2 = new TCanvas("c2","", 900, 600); 
  //c2->cd();
  //h_bco_diff_subtracted->SetTitle("; Beam crossing (106 ns / crossing); INTT hit counts per events");
  //h_bco_diff_subtracted->Draw();
  //c2->Write("bco_diff_subtracted", TObject::kOverwrite);
  h_bco_diff_subtracted->Write("bco_diff_subtracted", TObject::kOverwrite);
  
  //file_test->Close();
  
  
  
  file->Close();
  delete file;
  //delete c2;
  delete c1;
  // delete f_even;
  // delete f_odd;
  //delete f_even_dotted;
  /*
  std::pair<double, double> averages = {average_odd_bins, average_even_bins};
  return averages;
  */
}






double GetMeanInOddBins(TH1D* hist, int range[2]){
  std::vector<int> odd_bins;
  for (int i=range[0]; i <= range[1]; i++){
    if ( i%2 == 1 ) {odd_bins.push_back(i);}
  }
  
  std::vector<double> bin_contents;

  for (int i : odd_bins){
    //std::cout << i << std::endl;
    bin_contents.push_back( hist->GetBinContent(i) );
  }
  double sum = std::accumulate(bin_contents.begin(), bin_contents.end(), 0.0);
  double average = sum / bin_contents.size();
  return average;
}

double GetMeanInEvenBins(TH1D* hist, int range[2]){
  std::vector<int> even_bins;
  for (int i=range[0]; i <= range[1]; i++){
    if ( i%2 == 0 ) {even_bins.push_back(i);}
  }
  
  std::vector<double> bin_contents;

  for (int i : even_bins){
    //std::cout << i << std::endl;
    bin_contents.push_back( hist->GetBinContent(i) );
  }
  double sum = std::accumulate(bin_contents.begin(), bin_contents.end(), 0.0);
  double average = sum / bin_contents.size();
  return average;
}

// can be implemented so that it can deal with a different binning, but not now. 
// Now, the implementation assumes the binning of (128,0,128).
int GetPreviousWithoutUnderflow(int bin){
  if (bin == 1) {
    return 128;
  } else {
    return bin-1;
  }
}

// same situation as get_previous_without_underflow(int bin)
int GetNextWithoutOverflow(int bin) {
  if (bin == 128) {
    return 1;
  } else {
    return bin+1;
  }
}