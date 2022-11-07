#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif


TH1D* GetHist( string data )
{
  TFile* tf = new TFile( data.c_str(), "READ" );
  auto hist = (TH1D*)( tf->Get( "hist" ) ->Clone() );
  hist->Sumw2();
  return hist;
}

TGaxis* GetAxis( string data, int color )
{
  TFile* tf = new TFile( data.c_str(), "READ" );
  auto axis = (TGaxis*)( tf->Get( "time_axis" ) ->Clone() );
  axis->SetLineColor( color );
  axis->SetTextColor( color );
  axis->SetLabelColor( color );
  axis->SetLabelOffset( -0.01 );
  axis->CenterTitle();
  return axis;
}

/*!
  comparison of BCO phase measurements in 2021/Jan-Mar and 2021/Dec (ELPH)
 */
int bco_phase_comparison_20220120()
{
  string data_ELPH = "results/ELPH/bco/run/all.root";
  string data_ELPH_axis_data = "results/ELPH/bco/bco_phase.root";
  string data_cosmic = "results/ELPH/bco/all_no_last_2.root";
  string data_cosmic_before_ELPH = "results/ELPH/bco/run/run1-4.root";

  // run1, 2, 3, 4, 

  
  auto hist_ELPH = GetHist( data_ELPH );
  hist_ELPH->SetName( "ELPH" );
  hist_ELPH->Scale( 1.0 / hist_ELPH->Integral() );
  HistSetting( hist_ELPH );
  hist_ELPH->SetFillColorAlpha( kBlack, 0.2 );
  hist_ELPH->SetMarkerStyle( 1 );
  
  auto hist_cosmic = GetHist( data_cosmic );
  hist_cosmic->SetName( "Cosmic_202102" );
  hist_cosmic->Scale( 1.0 / hist_cosmic->Integral() );
  HistSetting( hist_cosmic, kRed );
  hist_cosmic->SetLineColorAlpha( kRed, 0.5 );
  hist_cosmic->SetMarkerStyle( 1 );

  auto hist_cosmic_before_ELPH = GetHist( data_cosmic_before_ELPH );
  hist_cosmic_before_ELPH->SetName( "Cosmic_Before_ELPH" );
  hist_cosmic_before_ELPH->Scale( 1.0 / hist_cosmic_before_ELPH->Integral() );
  HistSetting( hist_cosmic_before_ELPH, kAzure, 3 );
  hist_cosmic_before_ELPH->SetFillColor( kAzure );
  hist_cosmic_before_ELPH->SetFillStyle( 3004 );
  hist_cosmic_before_ELPH->SetLineColorAlpha( kAzure, 1.0 );
  hist_cosmic_before_ELPH->SetMarkerStyle( 1 );

  int rebin_factor = 4;
  hist_cosmic_before_ELPH->Rebin( rebin_factor );
  hist_cosmic_before_ELPH->Scale( 1.0 / rebin_factor );
  

  string output = "results/ELPH/single_use/bco_phase_comparison_20220120.pdf";
  TCanvas* c = GetCanvas( output, "3:2", false );
  MultiHist* mh = new MultiHist( "BCO phase measurements", "BCO phase measurements;TDC ch;Normalized entries" );
  mh->SetXmin( 400 );
  mh->SetXmax( 1500 );
  mh->SetYmin( -0.002 );
  mh->SetStatsBoxSize( 0.2, 0.15 );
  mh->SetStatsBoxPoint( 1.0, 1.0 );
  mh->Add( hist_ELPH );
  mh->Add( hist_cosmic );
  mh->Add( hist_cosmic_before_ELPH );

  mh->Draw( "HISTE" );

  auto axis_ELPH = GetAxis( data_ELPH_axis_data, kBlack );
  axis_ELPH->SetY1( 0.008 );
  axis_ELPH->SetY2( 0.008 );
  axis_ELPH->Draw( "same" );

  auto axis_cosmic = GetAxis( data_cosmic, kRed );
  axis_cosmic->SetY1( -0.001 );
  axis_cosmic->SetY2( -0.001 );
  axis_cosmic->Draw( "same" );

  // auto axis_cosmic_before_ELPH = GetAxis( data_cosmic_before_ELPH, kAzure + 1 );
  // axis_cosmic_before_ELPH->SetY1( -0.003 );
  // axis_cosmic_before_ELPH->SetY2( -0.003 );
  // axis_cosmic_before_ELPH->Draw( "same" );

  c->Print( c->GetName() );
  return 0;
}
