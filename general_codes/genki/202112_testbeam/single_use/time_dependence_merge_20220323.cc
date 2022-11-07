#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

int time_dependence_merge_20220323()
{

  gStyle->SetEndErrorSize(0) ;
  //const string kOutput_root_dir = "results/ELPH/efficiency/simplest/";
  const string kOutput_root_dir = "results/ELPH/single_use/time_dependence_20220323/";

  string output = kOutput_root_dir + "merged_plot.pdf";
  TCanvas* c = GetCanvas( output, "triple", false );
  c->Print( ((string)c->GetName()+"[").c_str() );
  
  TGraphErrors* g = new TGraphErrors();
  GraphSetting( g );
  g->SetMarkerStyle(1);
  for( int i=21; i<90; i++ )
    {
      string data = kOutput_root_dir + "run" + to_string( i ) + ".root";
      TFile* tf = new TFile( data.c_str(), "READ" );

      if( tf == nullptr || !tf->IsOpen())
	continue;
      
      auto profile = (TProfile*)tf->Get( "profile" );

      int num =  profile->GetNbinsX();

      for( int j=1; j<num+1; j++ )
	{
	  double x = i + 1.0 * j / (num+1);
	  double y = profile->GetBinContent( j );
	  g->SetPoint( g->GetN(), x, y );

	  double dy = profile->GetBinError( j );
	  g->SetPointError( g->GetN()-1, 0, dy );
	}
    }
  //  string output = kOutput_root_dir + "/single_use/time_dependence_202203023/" + "run" + to_string( run_num ) + ".pdf";

  for( int i=21; i<90; i+=10 )
    {

      auto frame = c->DrawFrame( i, 0, i+11, 256 );
      frame->SetTitle( "Run vs Channel;Run;Channel" );
      frame->GetXaxis()->SetLabelSize( 0.07 );
      frame->GetYaxis()->SetLabelSize( 0.07 );
      g->Draw( "PE" );

      c->Print( c->GetName() );
    }

  c->Print( ((string)c->GetName()+"]").c_str() );
  return 0;
}
