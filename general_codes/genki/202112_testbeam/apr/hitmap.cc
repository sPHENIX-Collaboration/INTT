#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"
//#include "../../../repo/INTT/general_codes/functions/DrawHitMap.c"
#include "../../../functions/DrawHitMap.c"

int GetMaxBinContent( TH2D* hist )
{
  vector < int > values;
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    for( int j=1; j<hist->GetNbinsY()+1; j++ )
      values.push_back( hist->GetBinContent(i, j ) );
    
  return *max_element( values.begin(), values.end() );
}

int hitmap( int run = 89 )
{

  gStyle->SetOptStat( false );
  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( run );

  //  string data_dir = "data/ELPH/";
  string data_dir = "results/ELPH/trim_data/no_clone/";
  string data = data_dir + string("run") + to_string( run ) + "_no_clone.root";;
  cout << data << endl;

  TFile* tf = new TFile( data.c_str(), "READ" );
  auto tr = (TTree*)tf->Get( "tree_both" );

  string output_name = string("hitmap_run") + to_string(run) + ".pdf";
  string output = string("results/ELPH/apr/hitmap_run") + output_name;

  string cut = "";
  cut = "module==1";
  cut += " && 0<chip_id && chip_id<27";
  cut += " && 0<=chan_id && chan_id<=127";
  cut += " && ampl==0";
  
  //  TH2D* hist = DrawHitMap( tr, cut, "goff", 257 );
  string hist_name = "Hitmap";
  TH2D* hist = new TH2D( hist_name.c_str(), ";;Channel;#Hits", 13, 0, 13, 257, 0, 257 );
  string expression = string("int(chip_id / 14) * 256") // it's offsets for y-axis.
    + "+ pow(-1, int(chip_id / 14)) * chan_id"        // Y value increases(decreases) as chan_id decreases(increases) for chips1-13(14-26)
    + ": -chip_id"                                   // Basically, x value increases as chip_id decreases (chip13 at x=0, chip12 at x=-1, ...)
    + "+ (1 + int(chip_id / 14)) * 13"               // In addition to above, offset is needed. 
    + ">> " + hist_name;
  
  TCanvas* c = new TCanvas( output.c_str(), "title", 1200, 200 );
  tr->Draw( expression.c_str(), cut.c_str(), "colz" );
  hist->Draw( "colz" );

  hist->SetContour( 50 );
  hist->GetXaxis()->SetTitleFont( 22 );
  hist->GetXaxis()->CenterTitle();
  hist->GetXaxis()->SetTitleOffset( 0.22 );
  hist->GetXaxis()->SetTitleSize( 0.13 );
  hist->GetXaxis()->SetLabelFont( 22 );
  hist->GetXaxis()->SetLabelOffset( 999 );
  hist->GetXaxis()->SetLabelSize( 0.0 );
  
  hist->GetYaxis()->CenterTitle();
  hist->GetYaxis()->SetTitleFont( 22 );
  hist->GetYaxis()->SetTitleOffset( 0.3 );
  hist->GetYaxis()->SetTitleSize( 0.15 );
  //  hist->GetYaxis()->SetLabelOffset( 0.2 );
  hist->GetYaxis()->SetLabelFont( 22 );
  hist->GetYaxis()->SetLabelSize( 0.15 );
  
  //  hist->GetZaxis()->SetRangeUser( 1, GetMaxBinContent(hist) );
  hist->GetZaxis()->SetRangeUser( 0, GetMaxBinContent(hist) );
  hist->GetZaxis()->CenterTitle();
  hist->GetZaxis()->SetTitleFont( 22 );
  hist->GetZaxis()->SetTitleOffset( 0.17 );
  hist->GetZaxis()->SetTitleSize( 0.15 );
  hist->GetZaxis()->SetLabelFont( 22 );
  hist->GetZaxis()->SetLabelOffset( 0.01 );
  hist->GetZaxis()->SetLabelSize( 0.08 );

  // draw chip_id on the hist
  for (int i = 0; i < 26; i++) {
    TLatex* tex = new TLatex();
    //double x = i < 13 ? 12.25 - i : 12.25 - i + 13;
    //double y = i < 13 ? 64 : 192;
    
    //double x = i < 13 ? 0.25 + i : 0.25 + i - 13;
    double x = i<13 ? 13.0 - i - 3.0/4 : 13.0 - (i-13) - 3.0/4 ;
    //    double y = i < 13 ? 16 : 256 - 16;
    double y = i < 13 ? 64 : 192 ;
    tex->SetTextSize( 0.07 );
    tex->SetTextColor( kWhite );
    tex->DrawLatex(x, y, Form("Chip%d", i + 1));
  }

  TLine* chip_boundary = new TLine();
  chip_boundary->SetLineStyle( 7 );
  chip_boundary->SetLineColor( kWhite );
  chip_boundary->DrawLine( 0, 128, 13, 128 );
  for( int i=0; i<12; i++ )
    chip_boundary->DrawLine( i+1, 0, i+1, 256 );
  
  //  gPad->SetLogz( true );
  cout << hist->GetEntries() << endl;
  c->Print( c->GetName() );
  c->Print( ((string)"documents/APR/2021/accel-tmp/" + output_name).c_str() );
  return 0;
}
