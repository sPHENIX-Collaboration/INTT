#define __CINT__

#ifdef __linux__
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../lib/INTTEvent.hh"


TTree* GetTree( string data_path )
{
  TFile* tf = new TFile(  data_path.c_str(), "READ" );
  if( tf->IsOpen() == false )
    {
      cerr << tf->GetName()  << " is not found" << endl;
      return nullptr;
    }

  return (TTree*)tf->Get( "tr" )->Clone();
}

vector < TH1D* > GetHists( TTree* tr_geo, TTree* tr_fit, string name, string title,
			   int bin, double xmin, double xmax,
			   string expression, string cut )
{

  ////////////////////////////////////
  string name_geo = name + "_geo";
  string title_geo = name_geo + ";" + title;
  TH1D* hist_geo = new TH1D( name_geo.c_str(), title_geo.c_str(), bin, xmin, xmax );
  HistSetting( hist_geo, kBlack, 3, 1 );

  string expression_geo = expression + ">>" + name_geo;
  tr_geo->Draw( expression_geo.c_str(), cut.c_str(), "goff" );
  hist_geo->Scale( 1.0 / hist_geo->Integral() );

  cout << expression_geo << "\t" << cut << "\t" << tr_geo->GetEntries() << "\t" << hist_geo->GetEntries() << endl;
  
  ////////////////////////////////////
  string name_fit = name + "_fit";
  string title_fit = name_fit + ";" + title;
  TH1D* hist_fit = new TH1D( name_fit.c_str(), title_fit.c_str(), bin, xmin, xmax );
  HistSetting( hist_fit, kRed, 3, 1 );

  string expression_fit = expression + ">>" + name_fit;
  tr_fit->Draw( expression_fit.c_str(), cut.c_str(), "goff" );
  hist_fit->Scale( 1.0 / hist_fit->Integral() );
  cout << expression_fit << "\t" << cut << "\t" << tr_fit->GetEntries() << "\t" << hist_fit->GetEntries() << endl;
  
  vector < TH1D* > hists;
  hists.push_back( hist_geo );
  hists.push_back( hist_fit );

  return hists;
}


void DrawEachLadder( TCanvas* c, TTree* tr_geo, TTree* tr_fit,
		     string name, string title,
		     int bin, double xmin, double xmax,
		     string expression, string cut_arg )
{


  for( int i=0; i<3; i++ )
    {
      int module = INTTEvent::Ladder2Module( i );
      
      string cut_module = string( "focusing_module==" ) + to_string(module);

      string cut = cut_module;
      if( cut_arg != "" )
	cut += " && " + cut_arg;
            
      string name_ladder = name + "_ladder" + to_string(i);
      string title_ladder = name_ladder;
      string title_axis = title;
      string title_whole = title_ladder + ";" + title_axis;
      MultiHist* mh = new MultiHist( "name", title_whole.c_str() );
      mh->SetStatsFormat( 111111 );
      mh->SetStatsBoxSize( 0.2, 0.2 );
      mh->SetYmax( 1 );
      auto hists = GetHists( tr_geo, tr_fit, name_ladder, title_axis,
			     bin, xmin, xmax,
			     expression, 
			     cut );

      for( auto& hist : hists )
	mh->Add( hist );

      gPad->SetLogy( true );
      mh->Draw( "HISTE" );
      c->Print( c->GetName() );
    }

}

int geometrical_fitting_comparison_20220328( int run = 89, bool is_MC = false )
{
  string data_root_dir = "results/ELPH/efficiency/";

  string data_name = string("run") + to_string( run );
  if( is_MC )
    data_name += "_MC";

  data_name += ".root";

  string data_geo = data_root_dir + "simplest/" + data_name;
  auto tr_geo = GetTree( data_geo );

  string data_fit = data_root_dir + "fitting/" + data_name;
  auto tr_fit = GetTree( data_fit );

  string output = "results/ELPH/single_use/geometrical_fitting_comparison_20220328.pdf";
  TCanvas* c = GetCanvas( output.c_str(), "square", false );
  c->Print( ((string)c->GetName()+"[").c_str() );

  
  string cut_chip = "focusing_chip==10";

  string cut = cut_chip;
  DrawEachLadder( c, tr_geo, tr_fit, "TrackSlope", "Slope (ch/ladder distance);Normalized entires",
		  200, -100, 100,
		  "track_slope",
		  cut );

  // ladder0: mean -2.4, std. dev. 0.75
  // ladder1: mean -2.5, std. dev. 0.87
  // ladder2: mean -2.0, std. dev. 0.88
  cut = cut_chip + " && "
    + "("
    +   "( module==1 && -4.65 < track_slope && track_slope < -0.15 ) || "
    +   "( module==6 && -5.11 < track_slope && track_slope <  0.11 ) || "
    +   "( module==5 && -4.64 < track_slope && track_slope <  0.64 )"
    + ")";
  string name = "Residual_slope_around_mean";
  DrawEachLadder( c, tr_geo, tr_fit, name, "Residual (ch);Normalized entires",
		  512, -256, 256,
		  "diff_chan",
		  cut );

  
  for( int i=0; i<10; i++ )
    {
      double accepted_slope = pow( 2, i );
      cut = cut_chip + " && fabs(track_slope)<" + to_string( accepted_slope ); 
      cout << cut << endl;

      string name = "Residual_slope_less_" + to_string( int(accepted_slope) );
      DrawEachLadder( c, tr_geo, tr_fit, name, "Residual (ch);Normalized entires",
		      512, -256, 256,
		      "diff_chan",
		      cut );
    }
    
  //cut = cut_chip + " && fabs(track_slope)<3 && chan_id[1]>10 && chan_id[2]>10";

  
  c->Print( ((string)c->GetName()+"]").c_str() );

  cout << "Inputs: " << data_geo << " & " << data_fit << endl;
  cout << "Output : " << output << endl;
  
  return 0;
}
