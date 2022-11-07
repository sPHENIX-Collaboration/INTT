#define __CINT__

#ifdef __linux__
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "Database.hh"

const int kFirst_run = 22;
const string kData_dir = "data/ELPH/";

TH2D* MakeIntoOne( vector < TH2D* >& hists )
{
  Long64_t bin_num = 0;
  for( auto& hist : hists )
    {
      bin_num += hist->GetNbinsX() + 1;
    }

  string hist_name = string(hists[0]->GetName()).substr( 6, string(hists[0]->GetName()).size() );
  TH2D* hist = new TH2D( hist_name.c_str(), hist_name.c_str(),
			 bin_num, 0, bin_num,
			 hists[0]->GetYaxis()->GetNbins(), hists[0]->GetYaxis()->GetXmin(), hists[0]->GetYaxis()->GetXmax()
			 );

  int current_bin = 0;
  for( auto& hist_run : hists )
    {

      for( int i=0; i<hist_run->GetNbinsX()+1; i++ )
	{
	  current_bin++;

	  for( int j=0; j<hist_run->GetNbinsY()+1; j++ )
	    {
	  
	      hist->SetBinContent( current_bin, j, hist_run->GetBinContent( i, j ) );
	      hist->SetBinError( current_bin, j, hist_run->GetBinError( i, j ) );
	    }

	}
    }
  
  return hist;
}

void MentionRuns( TH2D* hist_merged, vector < TH2D* >& hists )
{
  double pos_left = 0;
  double pos_right = 0;

  auto hist_last = hists[ hists.size()-1 ];
  double xmin = hist_merged->GetXaxis()->GetBinLowEdge(1);
  double xmax = hist_merged-> GetXaxis()->GetBinUpEdge( hist_merged->GetNbinsX() );

  double ymin = hist_merged->GetYaxis()->GetXmin();
  double ymax = hist_merged->GetYaxis()->GetXmax();
  double pos_y = ymax;
  
  double value_min = stoi( string(hists[0]->GetName()).substr( 3, 2 ) );
  double value_max = stoi( string(hist_last->GetName()).substr( 3, 2 ) ) + 1; // +1 needed since it's right edge
  
  TGaxis* axis_run = new TGaxis( xmin, pos_y, xmax, pos_y, value_min, value_max, 10 + 10 * 100, "-" );
  axis_run->SetTitle( "Run" );
  axis_run->SetLineWidth( 2 );
  axis_run->SetLineColor( kRed );
  axis_run->SetLabelColor( axis_run->GetLineColor() );
  axis_run->SetTitleColor( axis_run->GetLineColor() );
  axis_run->SetLabelOffset( 0.02 );
  
  axis_run->Draw( "same" );
}

TH2D* GetTrendOfRun( Run* run_info, string parameter, int param_bin, double param_min, double param_max, string cut_base )
{
  string root_file = kData_dir + Replace( run_info->data_, ".dat", ".root" );
  auto tf = new TFile( root_file.c_str(), "READ" );

  int mode = 0; // 0: tree, 1: tree_camac, 2: tree_both
  if( parameter.find( "camac" ) != string::npos )
    mode = 1;
  else if( parameter.find( "$" ) != string::npos )
    mode = 2;

  TTree* tr;
  if( mode == 1 )
    tr = (TTree*)tf->Get( "tree_camac" );
  else if( mode == 2 )
    tr = (TTree*)tf->Get( "tree_both" );
  else
    tr = (TTree*)tf->Get( "tree" );   // mode == 0

  auto entries = tr->GetEntries( cut_base.c_str() );
  double xmin = 0;
  double xmax = entries;
  int binx = 10;
  binx = 5;
  /*
  if( is_camac )
    binx = xmax / 100;
  else
    binx = xmax / 1e3;

  if( binx < 2 )
    binx = xmax / 10;
  */
  
  string parameter_name = Replace( Replace( parameter, "[", ""), "]", "" );
  string hist_name = string( "run" ) + to_string( run_info->run_num_ ) + "_" + parameter_name;
  TH2D* hist = new TH2D( hist_name.c_str(), hist_name.c_str(),
			 binx, xmin, xmax,
			 param_bin, param_min, param_max );
  string expression = parameter + ":Entry$>>" + hist_name;
  tr->Draw( expression.c_str(), cut_base.c_str(), "goff" );

  cout << " Run" << run_info->run_num_ << " "
       << root_file << "\t"
       << expression << ":"
       << "( " << binx << ", " << xmin << ", " << xmax
       << ", " << param_bin << ", " << param_min << ", " << param_max << ")\t"
    //<< cut_base
       << endl;

  hist->Sumw2();

  double max_bin_content = 1.0;
  for( int i=0; i<hist->GetNbinsX()+1; i++ )
    {
      for( int j=0; j<hist->GetNbinsY()+1; j++ )
	{
	  if( max_bin_content < hist->GetBinContent( i, j ) )
	    max_bin_content = hist->GetBinContent( i, j );
	}
    }
  //  hist->Scale( 1.0 / hist->Integral() );
  hist->Scale( 1.0 / max_bin_content );
  return (TH2D*)hist->Clone();
}

TH2D* DrawTrend( string parameter, int param_bin, double param_min, double param_max, string cut_base, int module )
{
  string db_file = "documents/2021_test_beam/run_list - Setup.tsv";
  Database* db = new Database( db_file );

  bool is_camac = false;
  string cut_module = "";
  if( parameter.find( "camac" ) != string::npos )
    is_camac = true;      
  else
    cut_module = string("module==") + to_string( module );

  const int kLast_run = db->GetNumberOfRuns();
  vector < TH2D* > hists;
  for( int i=kFirst_run; i<kLast_run+1; i++ )
    {
      // if( i<50 )
      // 	continue;
      
      db->SetRun( i );
      
      bool is_skipped = false;
      if( db->IsBeamRun() == false )
	is_skipped = true;
      
      if( parameter.find( "camac" ) != string::npos )
	if( db->GetDAQMode() != "CAMAC" )
	  is_skipped = true;
      
      if( is_skipped )
	{
	  auto hist_dummy = (TH2D*)hists[0]->Clone();
	  hist_dummy->Reset();
	  hists.push_back( hist_dummy );
	  continue;
	}
      
      //db->GetRun()->Print();
      string cut = cut_base;
      if( cut_module != "" )
	{
	  if( cut == "" )
	    cut = cut_module;
	  else
	    cut = string("(") + cut + ") && (" + cut_module + ")";
	}
	
      //cout << cut << endl;
      auto hist_run = GetTrendOfRun( db->GetRun(), parameter, param_bin, param_min, param_max, cut );
      hists.push_back( hist_run );
      
      // if( i > 30 )
      //  	break;
      
    }

  TH2D* hist = MakeIntoOne( hists );
  string hist_title = parameter;
  if( is_camac == false )
    hist_title += ", module" + to_string( module );
  
  hist_title += string(";") + to_string(hists[0]->GetNbinsX()) + " bins/run"; // x axis
  hist_title += string(";") + parameter;
  hist_title += string(";") + "nomalized entry";
  
  hist->SetTitle( hist_title.c_str() );

  hist->GetYaxis()->SetLabelSize( 0.05 );
  hist->GetYaxis()->SetTitleSize( 0.05 );
  hist->GetYaxis()->SetTitleOffset( 0.4 );
  hist->GetYaxis()->CenterTitle();
  hist->Draw( "colz" );

  MentionRuns( hist, hists );
  auto hist_profile = hist->ProfileX();
  hist_profile->SetMarkerStyle( 6 );
  hist_profile->SetMarkerColor( kBlack );
  hist_profile->SetLineColor( kBlack );  
  hist_profile->Draw( "sameP" );

  return hist;
}

int trend_over_runs( string parameter = "adc", int param_bin = 8, double param_min = 0.0, double param_max = 8.0, string cut_arg = "")
{
  string cut_base = "ampl==0 && 0<chan_id && chan_id < 27 && 0 <= chip_id && chip_id <= 128";

  bool is_camac = false;
  if( parameter.find( "camac" ) != string::npos )
    is_camac = true;

  // if camac parameter is given, remove the quality cut for INTT data
  if( is_camac )
    cut_base = "";
  
  if( cut_arg != "" )
    {
      if( cut_base != "" )
	cut_base += " && (" + cut_arg + ")";
      else
	cut_base = cut_arg;
    }
  
  gStyle->SetGridColor( kGray );
  gStyle->SetOptStat( false );

  string output = "results/ELPH/trend_over_runs/"
    + Replace( Replace( parameter, "[", ""), "]", "" ) + ".pdf";
  output = Replace( Replace(output, "$", ""), "%", "_mod_" );

  string output_root = Replace( output, ".pdf", ".root" );
  TFile* tf = new TFile( output_root.c_str(), "RECREATE" );
  
  TCanvas* c = new TCanvas( output.c_str(), "title", 1600, 400 );
  c->SetMargin( 0.11, 0.09, 0.08, 0.12 );
  PadSetting();
  gStyle->SetPalette( kBird );

  c->Print( ((string)c->GetName() + "[").c_str() );

  for( auto& module : {1, 6, 5} )
    {
      cout << cut_base << endl;
      auto hist2d = DrawTrend( parameter, param_bin, param_min, param_max, cut_base, module );
      c->Print( c->GetName() );
      tf->WriteTObject( hist2d, hist2d->GetName() );

      //break;
      if( is_camac )
	break;
    }

  tf->WriteTObject( c, "canvas" );
  c->Print( ((string)c->GetName() + "]").c_str() );

  tf->Close();
  
  return 0;
}
