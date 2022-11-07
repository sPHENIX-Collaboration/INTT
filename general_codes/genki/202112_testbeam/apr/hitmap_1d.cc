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

int hitmap_1d( int run = 55, int module = 1 )
{

  //gStyle->SetOptStat( false );
  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( run );

  if( !db->IsBeamRun() )
    return -1;

  string data_dir = "results/ELPH/trim_data/no_clone/";
  string data = data_dir + string("run") + to_string( run ) + "_no_clone.root";;

  //  data = string("data/ELPH/") + db->GetRootFile();
  cout << data << endl;

  TFile* tf = new TFile( data.c_str(), "READ" );
  auto tr = (TTree*)tf->Get( "tree_both" );
  
  string output_name = string("hitmap_run") + to_string(run) + "_1d_module" + to_string(module) + ".pdf";
  string output = string("results/ELPH/apr/") + output_name;

  string cut = "";
  cut = "module==" + to_string( module );
  cut += " && 0<chip_id && chip_id<27";
  cut += " && 0<=chan_id && chan_id<=127";
  cut += " && ampl==0";
  //  cut += " && (chip_id == 8 || chip_id == 9 || chip_id == 21 || chip_id == 22 )";
  //  cut += " && (chip_id == 9 || chip_id == 22 )"; // used for APR
  cut += " && (chip_id == 10 || chip_id == 23 )";
  //  cut += " && (chip_id == 11 || chip_id == 24 )";
  
  string hist_name = "Hitmap";
  string title = string( "run" ) + to_string( run ) + string( ", module " ) + to_string( module ) + ";Silicon Strip Channel;#Hit";
    TH1D* hist = new TH1D( hist_name.c_str(), title.c_str(), 256, 0, 256 );
  hist->SetLineColor( kBlack );
  hist->SetFillColorAlpha( kBlack, 0.2 );
  hist->SetLineWidth( 2 );
  
  string expression = string("int(chip_id / 14) * 255") // it's offsets for y-axis.
    + "+ pow(-1, int(chip_id / 14)) * chan_id"        // Y value increases(decreases) as chan_id decreases(increases) for chips1-13(14-26)
    + ">> " + hist_name;
  
  TCanvas* c = new TCanvas( output.c_str(), "title", 1400, 500 );
  c->SetMargin( 0.15, 0.05, 0.15, 0.05 );
  tr->Draw( expression.c_str(), cut.c_str(), "colz" );
  hist->Draw();

  hist->SetContour( 50 );
  hist->GetXaxis()->SetTitleFont( 22 );
  hist->GetXaxis()->CenterTitle();
  hist->GetXaxis()->SetTitleOffset( 0.7 );
  hist->GetXaxis()->SetTitleSize( 0.1 );
  hist->GetXaxis()->SetLabelFont( 22 );
  //  hist->GetXaxis()->SetLabelOffset( 999 );
  hist->GetXaxis()->SetLabelSize( 0.07 );
  
  hist->GetYaxis()->CenterTitle();
  hist->GetYaxis()->SetTitleFont( 22 );
  hist->GetYaxis()->SetTitleOffset( 0.75 );
  hist->GetYaxis()->SetTitleSize( 0.1 );
  //  hist->GetYaxis()->SetLabelOffset( 0.2 );
  hist->GetYaxis()->SetLabelFont( 22 );
  hist->GetYaxis()->SetLabelSize( 0.07 );
  
  //  gPad->SetLogz( true );
  gPad->SetGrid( true, true );
  gPad->SetTicks();
  //  cout << hist->GetEntries() << endl;
  c->Print( c->GetName() );
  //c->Print( ((string)"documents/APR/2021/accel-tmp/" + output_name).c_str() );
  return 0;
}
