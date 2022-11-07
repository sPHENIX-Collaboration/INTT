#define __CINT__


#ifdef __linux__
//#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"
#endif

vector < TH1D* > GetHists( vector < string >& files, string hist_name )
{
  vector < TH1D* > rtn;
  //  for( auto& file : files )
  for( int i=0; i<files.size(); i++ )
    {
      auto file = files[i];
      TFile* tf = new TFile( file.c_str(), "READ" );
      auto hist = (TH1D*)( tf->Get( hist_name.c_str() )->Clone());
      HistSetting( hist, GetColor(i), 3, 1 );
      rtn.push_back( hist );
      
    }


  return rtn;
}

int channel_dist_comparison_20220309()
{

  string data_dir = "results/ELPH/overview/cluster/normal/";
  vector < string > files;
  files.push_back( data_dir + "run21.root" );
  // files.push_back( data_dir + "run100_MC.root" ); // 2.22
  // files.push_back( data_dir + "run99_MC.root" ); // -3
  files.push_back( data_dir + "run98_MC.root" ); // -2.766, this is the best

  string hist_name = "ch_ladder0_chip11";
  vector < TH1D* > hists = GetHists( files, hist_name );
  
  string output = "results/ELPH/single_use/channel_dist_comparison_20220309.pdf";
  TCanvas* c = GetCanvas( output.c_str(), "ratio", false );
  
  MultiHist* mh = new MultiHist( "mh", "Channel distribution;Channel; Normalized entries (a.u);" );
  // mh->SetTitleSizeY( 0.15 );
  mh->SetTitleOffsetY( 1.3 );
  mh->SetLabelSizeY( 0.03 );
  mh->SetStatsBoxPoint( 0.85, 0.85 );
  mh->SetStatsBoxSize( 0.2, 0.1 );
  
  MultiHist* mh_ratio = new MultiHist( "ratio", "Ratio;Channel; Ratio" );
  mh_ratio->NormalizeHist();
  mh_ratio->SetRatioMode();
  mh_ratio->SetYmin( 0 );
  mh_ratio->SetYmax( 2 );

  mh_ratio->SetTitleSizeX( 0.15 );
  mh_ratio->SetTitleOffsetX( 0.7 );
  mh_ratio->SetLabelSizeX( 0.07 );

  mh_ratio->SetTitleSizeY( 0.15 );
  mh_ratio->SetTitleOffsetY( 0.3 );
  mh_ratio->SetLabelSizeY( 0.07 );

  for( int i=0; i<hists.size(); i++ )
    {

      if( i == 0 )
	{
	  hists[i]->SetName( "Experiment" );
	  hists[i]->SetLineWidth( 1 );
	  hists[i]->SetOption( "HIST" );
	  hists[i]->SetFillColorAlpha( kBlack, 0.1  );
	  mh_ratio->AddBaseHist( hists[i] );
	}
      else
	{
	  hists[i]->SetName( "MC" );
	  hists[i]->SetMarkerStyle( 20 );
	  hists[i]->SetLineWidth( 1 );
	  mh_ratio->Add( hists[i] );
	}


      mh->Add( hists[i] );
    }

  mh->NormalizeHist();

  c->cd(1);
  mh->Draw();
  
  c->cd(2);
  mh_ratio->Draw();
  
  c->Print( c->GetName() );  

  return 0;
}
