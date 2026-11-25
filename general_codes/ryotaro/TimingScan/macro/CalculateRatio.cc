#include "MyUtility/GetBinWithoutOverUnderFlow.cc"

// int get_previous_without_underflow(int bin);
// int get_next_without_overflow(int bin);


// Returns (asymmetry, ratio) for given run.
// This code is assuming that h_bco_diff has an binning of (128,0,128). If not, cannot deal with overflow/underflow properly.
std::vector< std::pair<double, double> > CalculateRatio(int run){
  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/run%d.root", run);
  TFile* file = TFile::Open(filename.c_str(), "read");
  if ( !file || file->IsZombie() ){
    std::cerr << "ERROR: Could not open" << filename << std::endl;
    exit(1);
  }

  file->cd();
  TH1D* h_bco_diff_shifted = (TH1D*)file->Get("h_bco_diff_shifted");
  if (!h_bco_diff_shifted) {
    std::cerr << "Cannot get the histgram" << std::endl;
    exit(1);
  }
  
  int peak_bin = h_bco_diff_shifted->GetMaximumBin();
  double peak_height = h_bco_diff_shifted->GetBinContent(peak_bin);
  double error = h_bco_diff_shifted->GetBinError(peak_bin);

  int previous_bin = GetPreviousWithoutUnderflow(peak_bin); 
  int next_bin = GetNextWithoutOverflow(peak_bin);
  double previous_bin_height = h_bco_diff_shifted->GetBinContent( previous_bin );
  double error_previous = h_bco_diff_shifted->GetBinError(previous_bin);
  double next_bin_height = h_bco_diff_shifted->GetBinContent(next_bin); 
  double error_next= h_bco_diff_shifted->GetBinError(next_bin);
  
  double the_other_bin_height;
  double error_other;
  double asymmetry;
  double error_asymmetry;
  double ratio; /* not needed for now */
  double error_ratio=0; /* not needed for now */
  if ( previous_bin_height > next_bin_height ) {
    the_other_bin_height = previous_bin_height;
    error_other = error_previous;
    //asymmetry = (the_other_bin_height - peak_height) / (the_other_bin_height + peak_height ); // (before - peak) / (before + peak)
    asymmetry = ( the_other_bin_height ) / (the_other_bin_height + peak_height );
    error_asymmetry = sqrt( TMath::Power(peak_height,2) * TMath::Power(error_other,2) + TMath::Power(the_other_bin_height,2) * TMath::Power(error,2)) / TMath::Power( (the_other_bin_height + peak_height) ,2);
    cout << asymmetry << endl;
    ratio = peak_height / the_other_bin_height;
  } else {
    the_other_bin_height = next_bin_height;
    error_other = error_next;
    //asymmetry = ( peak_height - the_other_bin_height) / (peak_height + the_other_bin_height);  // (peak - next) / (peak + next)
    asymmetry = ( peak_height ) / (peak_height + the_other_bin_height);
    error_asymmetry = sqrt( TMath::Power(peak_height,2) * TMath::Power(error_other,2) + TMath::Power(the_other_bin_height,2) * TMath::Power(error,2)) / TMath::Power( (the_other_bin_height + peak_height) ,2); // same as above
    ratio = the_other_bin_height / peak_height;
  }
    /*
    std::cout << "binning: " << h_bco_diff->GetNbinsX() << std::endl;
    std::cout << "peak_bin: " << peak_bin << std::endl;
    std::cout << "next_bin: " << next_bin << std::endl;
    std::cout << previous_bin_height << " " << peak_height << " " << next_bin_height << std::endl;
    std::cout << "asymmetry: " << (double)asymmetry << std::endl;
    std::cout << "ratio: " << (double)ratio << std::endl;
    */
 
  file->Close();
  delete file;

  //return { asymmetry, ratio}; 
  return { {abs(asymmetry), error_asymmetry}, {ratio, error_ratio} };
}

// can be implemented so that it can deal with a different binning, but not now. 
// Now, the implementation assumes the binning of (128,0,128).
// int get_previous_without_underflow(int bin){
//   if (bin == 1) {
//     return 128;
//   } else {
//     return bin-1;
//   }
// }

// // same situation as get_previous_without_underflow(int bin)
// int get_next_without_overflow(int bin) {
//   if (bin == 128) {
//     return 1;
//   } else {
//     return bin+1;
//   }
// }
