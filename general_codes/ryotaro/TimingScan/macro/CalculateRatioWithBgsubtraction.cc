#include "MyUtility/GetBinWithoutOverUnderFlow.cc"

// std::pair<double, double> GetMeanStdInOddBins(TH1D* hist, int range[2]);
// std::pair<double, double> GetMeanStdInEvenBins(TH1D* hist, int range[2]);
double GetMeanInOddBins(TH1D* hist, int range[2]);
double GetStdInOddBins(TH1D* hist, int range[2]);
double GetMeanInEvenBins(TH1D* hist, int range[2]);
double GetStdInEvenBins(TH1D* hist, int range[2]);

// int GetPreviousWithoutUnderflow(int bin);
// int GetNextWithoutOverflow(int bin);
std::vector <std::pair<double, double> > EstimateBackgroundLevels(int run);






/* Returns ( {asymmetry, error},  {ratio, error} ) for a given run. */
/* This code is assuming that h_bco_diff has an binning of (128,0,128). If not, cannot deal with overflow/underflow properly. */
/* Being used in DrawSharpnessAsFunctionOfRuns() */
std::vector< std::pair<double, double> > CalculateRatioWithBgsubtraction(int run){
  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/run%d.root", run);
  TFile* file = TFile::Open(filename.c_str(), "read");
  if ( !file || file->IsZombie() ){
    std::cerr << "ERROR: Could not open" << filename << std::endl;
    exit(1);
  }

  file->cd();
  TH1D* h_bco_diff_shifted = (TH1D*)file->Get("h_bco_diff_shifted");
  if (!h_bco_diff_shifted) {
    std::cerr << "For run: " << run << ", Cannot get the histgram" << std::endl;
    exit(1);
  }

  std::vector< std::pair<double, double> > backgrounds_witherrors  = EstimateBackgroundLevels(run); /* backgrounds_witherrors = { {average_odd, std_odd}, {average_even, std_even} } */
  double bg_estimation_odd_bins = backgrounds_witherrors[0].first;  /* using average as an esitimation */
  double error_odd_bin = backgrounds_witherrors[0].second;     /* using standard deviatoin as error */
  
  double bg_estimation_even_bins = backgrounds_witherrors[1].first;  /* using average as an esitimation */
  double error_even_bin = backgrounds_witherrors[0].second;    /* using standard deviatoin as error */


  int peak_bin = h_bco_diff_shifted->GetMaximumBin();
  double peak_height_raw = h_bco_diff_shifted->GetBinContent(peak_bin);
  double error_raw = h_bco_diff_shifted->GetBinError(peak_bin); /* sqrt(peak_height_raw) is not correct because we did scaling per e_events */
  double peak_height_subtracted;
  double error_subtracted;
  if (peak_bin%2 == 1) {
    peak_height_subtracted = peak_height_raw - bg_estimation_odd_bins;
    error_subtracted = sqrt( error_raw*error_raw + error_odd_bin*error_odd_bin);
  } else {
    peak_height_subtracted = peak_height_raw - bg_estimation_even_bins;
    error_subtracted = sqrt( error_raw*error_raw + error_even_bin*error_even_bin);
  }

  int previous_bin = GetPreviousWithoutUnderflow(peak_bin); 
  double previous_bin_height_raw = h_bco_diff_shifted->GetBinContent(previous_bin);
  double error_previous_raw = h_bco_diff_shifted->GetBinError(previous_bin); /* sqrt(previous_bin_height_raw); is not correct because we did scaling per e_events */
  double previous_bin_height_subtracted;
  double error_previous_subtracted;
  if (previous_bin%2 == 1){
    previous_bin_height_subtracted = previous_bin_height_raw - bg_estimation_odd_bins;
    error_previous_subtracted = sqrt( error_previous_raw*error_previous_raw + error_odd_bin*error_odd_bin);
  } else {
    previous_bin_height_subtracted = previous_bin_height_raw - bg_estimation_even_bins;
    error_previous_subtracted = sqrt( error_previous_raw*error_previous_raw + error_even_bin*error_even_bin);
  }

  int next_bin = GetNextWithoutOverflow(peak_bin);
  double next_bin_height_raw = h_bco_diff_shifted->GetBinContent(next_bin);
  double error_next_raw = h_bco_diff_shifted->GetBinError(next_bin); /* sqrt(next_bin_height_raw); is not correct because we did scaling per e_events */
  double next_bin_height_subtracted;
  double error_next_subtracted;
  if (next_bin%2 == 1){
    next_bin_height_subtracted = next_bin_height_raw - bg_estimation_odd_bins;
    error_next_subtracted = sqrt(error_next_raw*error_next_raw + error_odd_bin*error_odd_bin);
  } else {
    next_bin_height_subtracted = next_bin_height_raw - bg_estimation_even_bins;
    error_next_subtracted = sqrt(error_next_raw*error_next_raw + error_even_bin*error_even_bin);
  }

  double the_other_bin_height;
  double error_other;
  double asymmetry;
  double error_asymmetry;
  double ratio; /* not needed for now */
  double error_ratio=0; /* not needed for now */ 
  if ( previous_bin_height_subtracted > next_bin_height_subtracted ) {
    the_other_bin_height = previous_bin_height_subtracted;
    error_other = error_previous_subtracted;
    //asymmetry = (the_other_bin_height - peak_height_subtracted) / (the_other_bin_height + peak_height_subtracted ); // (before - peak) / (before + peak)
    asymmetry = ( the_other_bin_height ) / (the_other_bin_height + peak_height_subtracted ); /* changed the definition 20250512. This corresponds to always choosing the left bin */
    //asymmetry = ( peak_height_subtracted ) / (the_other_bin_height + peak_height_subtracted );
    error_asymmetry = 2*sqrt( TMath::Power(peak_height_subtracted,2) * TMath::Power(error_other,2) + TMath::Power(the_other_bin_height,2) * TMath::Power(error_subtracted,2)) / TMath::Power( (the_other_bin_height + peak_height_subtracted) ,2);
    ratio = peak_height_subtracted / the_other_bin_height;
  } else {
    the_other_bin_height = next_bin_height_subtracted;
    error_other = error_next_subtracted;
    asymmetry = ( peak_height_subtracted ) / (peak_height_subtracted + the_other_bin_height);  // (peak - next) / (peak + next)
    error_asymmetry = 2*sqrt( TMath::Power(peak_height_subtracted,2) * TMath::Power(error_other,2) + TMath::Power(the_other_bin_height,2) * TMath::Power(error_subtracted,2)) / TMath::Power( (the_other_bin_height + peak_height_subtracted) ,2); // same as above 
    ratio = the_other_bin_height / peak_height_subtracted;
  }


  // std::cout << "binning: " << h_bco_diff_shifted->GetNbinsX() << std::endl;
  // std::cout << "peak_bin: " << peak_bin << std::endl;
  // std::cout << "next_bin: " << next_bin << std::endl;
  // std::cout << previous_bin_height << " " << peak_height << " " << next_bin_height << std::endl;
  // std::cout << "asymmetry: " << (double)asymmetry << std::endl;
  // std::cout << "ratio: " << (double)ratio << std::endl;
   

  file->Close();
  delete file;

  return { {abs(asymmetry), error_asymmetry}, {ratio, error_ratio} };
}

/* Estimates background levels in plateau region( defined with the range_limit[2]). */
/* Returns estimation with errors. */
/* estimations[0] = {average_odd_bins, standard_deviation_odd_bins} */
/* estimations[1] = {average_even_bins, standard_deviation_even_bins} */
std::vector< std::pair<double,double> > EstimateBackgroundLevels(int run) {

  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/run%d.root", run);
  TFile* file = TFile::Open(filename.c_str(), "update");
  if ( !file || file->IsZombie() ){
    std::cerr << "ERROR: Could not open" << filename << std::endl;
    exit(1);
  }

  file->cd();
  TH1D* h_bco_diff_shifted = (TH1D*)file->Get("h_bco_diff_shifted");

  int peak_bin = h_bco_diff_shifted->GetMaximumBin();
  int range_limit[2] = {10,80}; // range in which the background level will be estimated.

  double average_odd_bins = GetMeanInOddBins(h_bco_diff_shifted, range_limit);
  double standard_deviation_odd_bins = GetStdInOddBins(h_bco_diff_shifted, range_limit);

  double average_even_bins = GetMeanInEvenBins(h_bco_diff_shifted, range_limit);
  double standard_deviation_even_bins = GetStdInOddBins(h_bco_diff_shifted, range_limit);

  // TCanvas* c1 = new TCanvas("name", "Title", 900, 600);
  // c1->cd();
  // h_bco_diff_shifted->Draw();

  // TF1* f_odd = new TF1("odd", std::to_string(average_odd_bins).c_str(), range_limit[0], range_limit[1]);
  // f_odd->Draw("same");

  // TF1* f_even = new TF1("even", std::to_string(average_even_bins).c_str(), range_limit[0], range_limit[1]);
  // f_even->Draw("same");

  // std::string fileout = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/TimingScan/output/root/TEST_run%d.root", run);
  // TFile* file_test = TFile::Open(fileout.c_str(), "recreate");
  // c1->Write();

  file->Close();

  std::vector< std::pair<double, double> > estimations;
  estimations.push_back( {average_odd_bins, standard_deviation_odd_bins} );
  estimations.push_back( {average_even_bins, standard_deviation_even_bins} );
  
  return estimations;
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

double GetStdInOddBins(TH1D* hist, int range[2]){

  std::vector<int> odd_bins;  /*same as GetMeanInOddBins()*/
  for (int i=range[0]; i <= range[1]; i++){
    if ( i%2 == 1 ) {odd_bins.push_back(i);}
  }
  
  std::vector<double> bin_contents; /*same as GetMeanInOddBins()*/
  for (int i : odd_bins){
    //std::cout << i << std::endl;
    bin_contents.push_back( hist->GetBinContent(i) );
  }

  double sum = std::accumulate(bin_contents.begin(), bin_contents.end(), 0.0);
  double average = sum / bin_contents.size();

  double quad_sum = 0;
  for (double content : bin_contents){
    quad_sum += content*content;
  }
  double standard_deviation = quad_sum / ( bin_contents.size()) - average*average;

  return standard_deviation;

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


double GetStdInEvenBins(TH1D* hist, int range[2]){
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

  double quad_sum = 0;
  for (double content : bin_contents){
    quad_sum += content*content;
  }
  double standard_deviation = quad_sum / ( bin_contents.size()) - average*average;

  return standard_deviation;
}

// // can be implemented so that it can deal with a different binning, but not now. 
// // Now, the implementation assumes the binning of (128,0,128).
// int GetPreviousWithoutUnderflow(int bin){
//   if (bin == 1) {
//     return 128;
//   } else {
//     return bin-1;
//   }
// }

// // same situation as get_previous_without_underflow(int bin)
// int GetNextWithoutOverflow(int bin) {
//   if (bin == 128) {
//     return 1;
//   } else {
//     return bin+1;
//   }
// }
