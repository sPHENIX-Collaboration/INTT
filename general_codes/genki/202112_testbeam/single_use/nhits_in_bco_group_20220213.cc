#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"
#endif

#include "../Database.hh"

void FillHist( int run_num, TH2D* hist2d )
{
  string data = string("results/ELPH/trim_data/bco_grouping/run") + to_string( run_num ) + ".root";
  TFile* tf = new TFile( data.c_str(), "READ" );
  TTree* tr = (TTree*)tf->Get( "tr" );

  string hist_name = string("hist1d") + to_string( run_num );
  TH1D* hist = new TH1D( hist_name.c_str(), "title", 100, 0, 100 );
  string expression = string("Length$(adc)>>") + hist_name;
  tr->Draw( expression.c_str(), "", "goff" );
  
  double factor = 1.0 / GetMaxBinContent( hist );
  hist->Scale( factor );
  
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    {
      hist2d->SetBinContent( run_num - 21, i, hist->GetBinContent(i) );
    }
  
  tf->Close();
}

int nhits_in_bco_group_20220213()
{

  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  string output = "results/ELPH/single_use/nhits_in_bco_group_20220213.pdf";
  TCanvas* c = GetCanvas( output, "square", false );
  c->Print( ((string)c->GetName() + "[" ).c_str() );
  TH2D* hist_trend = new TH2D( "hist", "#hit in a BCO group;run;#hit",
			       89-21, 21, 89,
			       100, 0, 100 );

  bool debug = false;
  for( int i=21; i<90; i++ )
    {
      cout << flush << "\r" << i << "  ";

      if( debug == true && i < 50 )
	continue;
      
      db->SetRun( i );
      
      if( db->IsBeamRun() == false )
	continue;

      FillHist( i, hist_trend );
    }

  hist_trend->Draw( "colz" );
  DrawStats( hist_trend, 0.8, 0.8, 1.0, 1.0 );
  DrawPaletteAxis( hist_trend, 0.91, 0.1, 0.93, 0.75, 0.05 );
  c->Print( c->GetName() );

  hist_trend->SetMarkerStyle( 21 );
  hist_trend->SetMarkerColor( kRed - 4 );
  hist_trend->Draw( "proj P same" );
  hist_trend->GetYaxis()->SetRangeUser( 0, 20 );
  //  DrawStats( hist_trend, 0.8, 0.8, 1.0, 1.0 );
  //DrawPaletteAxis( hist_trend, 0.9, 0.1, 0.93, 0.75, 0.05 );
  c->Print( c->GetName() );

  c->Print( ((string)c->GetName() + "]" ).c_str() );
  return 0;
}
