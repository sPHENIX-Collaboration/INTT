#include "CalculateRatio.cc"
#include "CalculateRatioWithBgsubtraction.cc"

#include "sPhenixStyle.C"

std::string GetDate();
void PutCaption();
// void PutLegend(TGraphErrors* g_asymmetry_on_l1delay, TGraphErrors* g_points_data_taken);
void PutLegend(TGraphErrors* g_asymmetry_on_l1delay, TGraphErrors* g_points_data_taken, TGraphErrors* g_asymmetry_on_l1delay_OO); /* Run26 O+O */

void DrawSharpnessAsFunctionOfRuns(){

  SetsPhenixStyle();

  // std::string filename = "/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/summary_2025pp_bkgsubtraction.root"; /* if you want it without background subtraction, change the calculation part, too. */
  // std::string filename = "/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/summary_2025pp.root";
  std::string filename = "/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/root/summary_2026OO.root"; /* Run26 O+O */
  TFile* file = TFile::Open(filename.c_str(), "recreate");
  if ( !file || file->IsZombie() ){
    std::cerr << "ERROR: Could not open" << filename << std::endl;
    exit(1);
  }

  std::vector< pair<int, int> > run_L1delay_list_scan6 = {
    //{66522, 90},
    //{66523, 90}, does not exist
    {66525, 90},
    {66526, 90},
    {66527, 108},
    //{66529, 108}, does not exist
    {66531, 109},
    {66532, 109},
    {66534, 110},
    {66535, 110},
    //{66536, 111}, // no data
    {66539, 111},
    {66540, 111},
    {66542, 112},
    {66543, 112},
    {66544, 113},
    {66545, 113},
  
  };

  // std::vector< pair<int, int> > run_L1delay_list_scan6 = {
  //   {79244, 108},
  //   {79252, 114},
  //   {79261, 109},
  //   {79263, 110},
  //   {79264, 106},
  //   {79265, 107},
  //   {79266, 108},

  //   {82391, 108}, /* Run26 O+O */
  // };

  TGraph* g_asymmetry_on_run = new TGraph();
  TGraph* g_ratio_on_run = new TGraph();
  TGraphErrors* g_asymmetry_on_l1delay = new TGraphErrors();
  TGraph* g_ratio_on_l1delay = new TGraph();

  TGraphErrors* g_asymmetry_on_l1delay_OO = new TGraphErrors(); /* Run26 O+O */
  g_asymmetry_on_l1delay_OO->SetPoint(0,108,0.98483333);
  
  TGraphErrors* g_points_data_taken = new TGraphErrors();

  for (int i=0; i<run_L1delay_list_scan6.size(); i++) {
    int run = run_L1delay_list_scan6[i].first;
    int l1_delay = run_L1delay_list_scan6[i].second;
    // std::vector< std::pair<double, double> > asymmetry_and_ratio_with_errors = CalculateRatio(run);
    std::vector< std::pair<double, double> > asymmetry_and_ratio_with_errors = CalculateRatioWithBgsubtraction(run);
    double asymmetry = asymmetry_and_ratio_with_errors[0].first;
    double error_asymmetry = asymmetry_and_ratio_with_errors[0].second;
    double ratio = asymmetry_and_ratio_with_errors[1].first;
    double error_ratio = asymmetry_and_ratio_with_errors[1].second;
    g_asymmetry_on_run->SetPoint(i, run, asymmetry);
    g_ratio_on_run->SetPoint(i, run, ratio);
    g_asymmetry_on_l1delay->SetPoint(i, l1_delay, asymmetry);
    g_asymmetry_on_l1delay->SetPointError(i, 0, error_asymmetry);
    g_ratio_on_l1delay->SetPoint(i, l1_delay, ratio);

    if (l1_delay == 108) {
      g_points_data_taken->SetPoint(i, l1_delay, asymmetry);
      g_points_data_taken->SetPointError(i, 0, error_asymmetry);
    }
  }


  file->cd();
  //g_asymmetry_on_run->Write();
  TCanvas* c1 = new TCanvas("c1", "g_asymmetry_on_run",900, 600);
  //c1->SetGrid();
  //c1->GetFrame()->SetBorderSize(12);
  //c1->SetTopMargin(0.03);
  //c1->SetBottomMargin(0.2);
  //c1->SetRightMargin(0.03);
  //c1->SetLeftMargin(0.15);
  //c1->SetFrameLineWidth(2);
  
  /* Set the title, fonts, fontsizes, etc. */
  g_asymmetry_on_run->SetTitle("; run number; asymmetry");
  g_asymmetry_on_run->SetMaximum(1.0);
  g_asymmetry_on_run->SetMinimum(-1.0);
  g_asymmetry_on_run->GetXaxis()->SetLimits(66522, 66545);

  c1->cd();
  g_asymmetry_on_run->Draw("ALP");
  c1->Write();
  //g_ratio_on_run->Write();

  TCanvas* c2 = new TCanvas("c2", "g_ratio_on_run", 3, 2, 900, 600);
  c2->SetGrid();
  c2->GetFrame()->SetBorderSize(12);


  g_ratio_on_run->SetTitle("; run number; ratio");
  g_ratio_on_run->SetMaximum(15);
  g_ratio_on_run->SetMinimum(-0.1);
  g_ratio_on_run->GetXaxis()->SetLimits(43408, 43440);

  
  c2->cd();
  g_ratio_on_run->Draw("ALP");
  c2->Write();

  TCanvas* c3 = new TCanvas("c3", "g_asymmetry_on_l1delay",1200, 900);
  //c3->SetGrid();
  //c3->GetFrame()->SetBorderSize(12);

  /* A pad for sphenix caption. Top 10% of the canvas*/
  TPad *titlePad = new TPad("titlePad", "Title Pad", 0.0, 0.0, 1.0, 1.0);
  titlePad->SetBottomMargin(0);  
  titlePad->SetTopMargin(0);   
  titlePad->SetLeftMargin(0);  
  titlePad->SetRightMargin(0); 
  titlePad->Draw();



  /* A pad for TGraph. Lower 90% of the canvas*/ 
  TPad *graphPad = new TPad("graphPad", "Graph Pad", 0.0, 0.0, 1.0, 0.835);
  graphPad->SetTopMargin(0.02);  
  //graphPad->SetGrid();
  graphPad->Draw();
  

  titlePad->cd();

  PutCaption();

  graphPad->cd();
  SetsPhenixStyle();
  //gPad->SetTopMargin(2.0);
  g_asymmetry_on_l1delay->SetTitle("; L1 delay [BCO/6]; #it{N}_{L} / (#it{N}_{L}+#it{N}_{R})");
  g_asymmetry_on_l1delay->GetYaxis()->SetTitleOffset(0.95);
  //g_asymmetry_on_l1delay->GetYaxis()->SetTitleSize(0.07);
  g_asymmetry_on_l1delay->SetMaximum(1.0);
  //g_asymmetry_on_l1delay->SetMinimum(-1.0);
  g_asymmetry_on_l1delay->SetMinimum(0.0);
  g_asymmetry_on_l1delay->SetMarkerSize(2);

  g_points_data_taken->SetMarkerSize(2.2);
  g_points_data_taken->SetMarkerColor(kGreen+1);//4);
  g_points_data_taken->SetLineColor(kGreen+1);//4);

  //c3->cd();
  //c3->SetGrid();
  g_asymmetry_on_l1delay->Draw("AP");
  g_points_data_taken->Draw("sameP");

  g_asymmetry_on_l1delay_OO->SetMarkerSize(2.2);
  g_asymmetry_on_l1delay_OO->SetMarkerColor(kRed);//4);
  g_asymmetry_on_l1delay_OO->SetLineColor(kRed);//4);
  g_asymmetry_on_l1delay_OO->Draw("sameP"); /* Run26 O+O */
  // PutCaption();

  // PutLegend(g_asymmetry_on_l1delay, g_points_data_taken);
  PutLegend(g_asymmetry_on_l1delay, g_points_data_taken, g_asymmetry_on_l1delay_OO);

  c3->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/TimingScan/output/image/Imbalance.pdf");
  c3->Write();
  //c3->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/TimingScan/output/image/Imbalance.pdf");

  
  TCanvas* c4 = new TCanvas("c4", "g_ratio_on_l1delay", 3, 2, 900, 600);
  c4->SetGrid();


  g_ratio_on_l1delay->SetTitle("; L1 delay; ratio");
  g_ratio_on_l1delay->SetMaximum(15);
  g_ratio_on_l1delay->SetMinimum(-0.1);
  g_ratio_on_l1delay->GetXaxis()->SetLimits(43408, 43440);

  c4->cd();
  g_ratio_on_l1delay->Draw("AP");
  c4->Write();

  file->Close();

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
  double first_margin = -0.1;//-0.62; // the margin from the top of the canvas to the top line (date). You can modify
  double pos_x = 0.2; //0.55; // you can modify
  double pos_y = 1.0  + first_margin; // - line_height;

  /* Date, you may want to change 0.65 to pos_x */
  tex->DrawLatexNDC( pos_x, pos_y,
             string("#it{#bf{sPHENIX}} Internal        " + ("#it{" + GetDate()) + "}").c_str() );
  // tex->DrawLatexNDC( pos_x, pos_y,
  //           string("#it{#bf{sPHENIX}} Internal        #it{05/16/2025}").c_str() );
             //tex->DrawLatexNDC( 0.65, pos_y,
  //           string("#it{" + GetDate() + "}").c_str() );

  // // sPHENIX Internal or sPHENIX Prelimnary
  // pos_y -= line_height;
  // //tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
  // tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );

  // p+p 200 GeV
  pos_y -= line_height ;// + 0.03;
  // tex->DrawLatexNDC( pos_x, pos_y, ( string("#it{Au+Au} #sqrt{s}=200 GeV")  ).c_str() );

  // pos_y -= line_height;
  // tex->DrawLatexNDC( pos_x, pos_y, "INTT Triggered mode" );
}

/* Run26 O+O */
void PutLegend(TGraphErrors* g_asymmetry_on_l1delay, TGraphErrors* g_points_data_taken, TGraphErrors* g_asymmetry_on_l1delay_OO){
  TLegend* legend = new TLegend(0.20,0.19,0.75,0.355); /* 2025 AuAu*/
  // TLegend* legend = new TLegend(0.25,0.19,0.65,0.305); /* 2025 AuAu*/
  // TLegend* legend = new TLegend(0.55,0.19,0.95,0.305); /* 2025 pp */
  legend->AddEntry(g_asymmetry_on_l1delay, "Scanned point in Run25 Au+Au", "ep");
  // legend->AddEntry(g_points_data_taken, "Current working point" ,"ep");
  legend->AddEntry(g_points_data_taken, "Working point in Run25 Au+Au" ,"ep");
  legend->AddEntry(g_asymmetry_on_l1delay_OO, "Current Setting for O+O" ,"ep");
  gStyle->SetLegendBorderSize(1);
  legend->SetFillStyle(1001); // 1001 は完全な塗りつぶし
  legend->SetFillColor(kWhite); // kWhite は白色背景
  legend->SetBorderSize(1); // 枠線を表示（0で非表示）
  legend->Draw();
}
