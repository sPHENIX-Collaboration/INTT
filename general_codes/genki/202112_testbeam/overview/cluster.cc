#define __CINT__

#ifdef __linux__
//#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh" // for ?
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh" // for ?

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"
#include "../lib/functions.hh"
#include "../lib/NoisyChannelList.hh"

int global_counter = 0;

TH2D* DrawHist2D( TTree* tr, string title,
		  int xbin, double xmin, double xmax,
		  int ybin, double ymin, double ymax,
		  string expression_arg, string cut )
{
  string hist_name = string("hist") + to_string( global_counter );
  TH2D* hist = new TH2D( hist_name.c_str(), title.c_str(), xbin, xmin, xmax, ybin, ymin, ymax );
  //HistSetting( hist, kBlack, 3, 1 );
  //hist->SetFillColorAlpha( kBlack, 0.1 );
  
  string expression = expression_arg + ">>" + hist_name;
  tr->Draw( expression.c_str(), cut.c_str(), "colz" );
  DrawStats( hist, 0.9, 0.9, 0.99, 0.99 );
  
  global_counter++;
  return (TH2D*)hist->Clone();
}

TH1D* DrawHist1D( TTree* tr, string title, int bin, double xmin, double xmax, string expression_arg, string cut )
{
  string hist_name = string("hist") + to_string( global_counter );
  TH1D* hist = new TH1D( hist_name.c_str(), title.c_str(), bin, xmin, xmax );
  HistSetting( hist, kBlack, 3, 1 );
  hist->SetFillColorAlpha( kBlack, 0.1 );
  
  string expression = expression_arg + ">>" + hist_name;
  tr->Draw( expression.c_str(), cut.c_str(), "HIST" );
  DrawStats( hist, 0.7, 0.7, 0.89, 0.89 );
  
  global_counter++;
  return (TH1D*)hist->Clone();
}


int cluster( int run_num = 89, string output_dir_arg = "", string cut_arg = "", bool is_noise_cut = false, bool is_MC = false )
{
  //  gStyle->SetOptStat( 1110 );

  if( !is_MC )
    {
      Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
      db->SetRun( run_num );
      const string kOriginal_data_dir = "data/ELPH/";
      
      if( db->IsBeamRun() == false && run_num <= 89 )
	return 0;
    }
  
  string output_root_dir = "results/ELPH/";
  string data_dir = output_root_dir + "trim_data/clustered/";
  string data = data_dir + "run" + to_string( run_num );

  if( !is_MC )
    data += + ".root";
  else
    data += + "_MC.root";

  TFile* tf = new TFile( data.c_str(), "READ" );
  TTree* tr = (TTree*)tf->Get( "tr" );
  //  tr->Print();

  string output = output_root_dir + "overview/";

  if( output_dir_arg == "" )
    output_dir_arg = "normal";

  if( !is_MC )
    output += "cluster/" + output_dir_arg + "/run" + to_string( run_num ) + ".pdf";
  else
    output += "cluster/" + output_dir_arg + "/run" + to_string( run_num ) + "_MC.pdf";

  TFile* tf_output = new TFile( Replace( output, ".pdf", ".root" ).c_str(), "RECREATE" );
  vector < pair < TObject*, string > > obj_saved;
  
  TCanvas* c = GetCanvas( output, "square", false );
  c->Print( ((string)c->GetName()+"[").c_str() );

  gPad->SetLogy( true );
  vector < int > module_num;
  module_num.push_back( 1 );
  module_num.push_back( 6 );
  module_num.push_back( 5 );

  ////////////////////////////////////////////////////////////////////////
  // common settings
  string cut_base = "(module==1 || module==6 || module==5)";
  if( cut_arg != "" )
    cut_base += " && " + cut_arg;

  string cut_noise = "";
  if( is_noise_cut && !is_MC )
    {
      NoisyChannelList* ncl = new NoisyChannelList( "documents/2021_test_beam/run_list - Noisy_ch_list.tsv" );
      cut_noise = ncl->GetCutWithChipColumn( run_num );
      if( cut_noise != "" )
	cout << "Noise cut(s): " << cut_noise << endl;
    }
  
  if( cut_noise != "" )
    cut_base += "&&" + cut_noise;

  string run = "Run" + to_string( run_num );
  ////////////////////////////////////////////////////////////////////////
  // number of hits in a cluster
  string title = run + ", #hits in a cluster;#hits in a cluster;Entries";
  auto hist_nhit = DrawHist1D( tr, title, 50, 0, 50, "nhit_in_cluster", cut_base );
  hist_nhit->SetName( "all" );
  PushBack( obj_saved, hist_nhit, {"nhit"} );
  c->Print( c->GetName() );

  MultiHist* mh_nhit = new MultiHist( "#hits in a cluster", title );
  mh_nhit->SetStatsFormat( 1111 );
  mh_nhit->Add( hist_nhit );
  
  for( int i=0; i<module_num.size(); i++ )
    {
      string cut_module = cut_base + "&&" + string("module==") + to_string( module_num[i] );
      auto hist_nhit_ladder = DrawHist1D( tr, "#hits in a cluster;#hits in a cluster;Entries", 50, 0, 50, "nhit_in_cluster", cut_module );

      hist_nhit_ladder->SetName( (string("ladder")+to_string(i)).c_str() );
      HistSetting( hist_nhit_ladder, GetColor( i + 1 ), 3, 1 );
      hist_nhit_ladder->SetFillColorAlpha( hist_nhit_ladder->GetLineColor(), 0.1 );

      mh_nhit->Add( hist_nhit_ladder );
      PushBack( obj_saved, hist_nhit_ladder, {"nhit", hist_nhit_ladder->GetName()} );
      
    }

  mh_nhit->NormalizeHist();
  mh_nhit->Draw( "HISTE", 0.6, 0.4, 0.89, 0.89 );
  c->Print( c->GetName() );

  // for comparison to the previous results
  gPad->SetLogy( false );
  mh_nhit->SetXmax( 6 );
  mh_nhit->SetYmin( 0 );
  mh_nhit->SetYmax( 1 );
  mh_nhit->Draw( "HISTE", 0.6, 0.4, 0.89, 0.89 ); 
  c->Print( c->GetName() );

  ////////////////////////////////////////////////////////////////////////
  // nhit_in_cluster vs nhit_in_cluster
  title = run + ", #hits; #hits;#hits;";

  string cut_cleanest = cut_base + "&& Length$(adc)==3 && module[0]==1 && module[1]==6 && module[2]==5";
  for( int i=0; i<module_num.size(); i++ )
    {

      string expression_x = string("nhit_in_cluster[") + to_string(i) + "]";
      string ladder_x = string( "ladder" ) + to_string(i);
      
      for( int j=i+1; j<module_num.size(); j++ )	
	{
	  string expression_y = string("nhit_in_cluster[") + to_string(j) + "]";
	  string expression = expression_y + ":" + expression_x;
	  string ladder_y = string( "ladder" ) + to_string(j);

	  string this_title = run + ";" + ladder_x + ";" + ladder_y + ";Entries";

	  int central_chip = GetHotChip( tr, module_num[i], cut_cleanest );
	  string this_cut = cut_cleanest
	    + "&& " + "chip_id[" + to_string(i) + "]==" + to_string( central_chip )
	    + "&& " + "chip_id[" + to_string(j) + "]=="+ to_string( central_chip );

	  cout << expression << "\t" << this_cut << endl;

	  double max_hits = 15;
	  auto hist_nhit_nhit = DrawHist2D( tr, this_title, max_hits, 0, max_hits, max_hits, 0, max_hits,
					    expression, this_cut );

	  hist_nhit_nhit->Draw( "colz" );
	  hist_nhit_nhit->SetLineColor( kBlack );
	  DrawStats( hist_nhit_nhit, 0.7, 0.7, 0.9, 0.9 );
	  if( hist_nhit_nhit->GetEntries() != 0 )
	    DrawPaletteAxis( hist_nhit_nhit, 0.91, 0.1, 0.93, 0.9, 0.04 );
	  gPad->SetLogz( true );
	  //hist_nhit_nhit->Draw( "colz" );
	  PushBack( obj_saved, hist_nhit_nhit, {"nhit_nhit", ladder_x, ladder_y} );
	  c->Print( c->GetName() );
  
	}
    }

  
  ////////////////////////////////////////////////////////////////////////
  // chip_id vs chan_id
  string title_chip_ch = ", chip vs channel; Chip column;Channel;Entries";

  c->Clear();
  c->Divide( 1, 3 );
  
  for( int i=0; i<module_num.size(); i++ )
    {

      string expression = "chan_id:chip_id";
      string ladder = "ladder" + to_string( i );
      
      string this_title = run + ", " + ladder + title_chip_ch;
      string this_cut = cut_base + "&&" + string( "module==" ) + to_string( module_num[i] );

      c->cd( i + 1 );
      auto hist_chip_ch = DrawHist2D( tr, this_title,
				      13, 1, 14, 256, 0, 256,
				      expression, this_cut );

      hist_chip_ch->Draw( "colz" );
      hist_chip_ch->SetLineColor( kBlack );
      DrawStats( hist_chip_ch, 0.905, 0.8, 0.99, 0.99 );

      if( hist_chip_ch->GetEntries() != 0 )
	DrawPaletteAxis( hist_chip_ch, 0.91, 0.1, 0.93, 0.77, 0.04 );
      
      gPad->SetLogz( false );
      PushBack( obj_saved, hist_chip_ch, {"chip_ch", ladder} );
      //hist_chip_ch->Draw( "colz" );
  
    }

  c->Print( c->GetName() );
  
  ////////////////////////////////////////////////////////////////////////
  // chan_id
  string title_ch = ", channel distribution; Channel;Entries";

  c->Clear();
  c->Divide( 1, 3 );
  
  for( int i=0; i<module_num.size(); i++ )
    {

      string ladder = "ladder" + to_string(i);
      string expression = "chan_id";
      
      string this_title = run + ", " + ladder + title_ch;


      MultiHist* mh_ch = new MultiHist( "ch", this_title );
      //mh_ch->SetYmax( 1e3 );      
      c->cd( i + 1 );
      PadSetting();
      
      int central_chip = GetHotChip( tr, module_num[i], cut_base );
      cout << "ladder" << i << ", " << central_chip << endl;
      int draw_range = 2;
      for( int j = central_chip-draw_range; j < central_chip + draw_range + 1 ; j++ )
	{
	  string chip = "chip" + to_string(j);
	  
	  string this_cut = cut_base + "&&" + string( "module==" ) + to_string( module_num[i] )
	    + " && chip_id==" + to_string( j );
	  
	  auto hist_ch = DrawHist1D( tr, this_title,
				     256, 0, 256,
				     expression, this_cut );

	  hist_ch->SetName( ( ladder + "," + chip ).c_str() );
	  
	  HistSetting( hist_ch, GetColor( j - central_chip + draw_range ) );
	  hist_ch->SetFillColorAlpha( GetColor( j - central_chip + draw_range ), 0.05  );

	  //j==central_chip ? hist_ch->Draw( "HIST" ) : hist_ch->Draw( "HIST SAMES" );
	  //	  hist_ch->SetLineColor( GetColor( j - central_chip - 1 ) );
	  mh_ch->Add( hist_ch );
	  //DrawStats( hist_ch, 0.905, 0.8, 0.99, 0.99 );
	  PushBack( obj_saved, hist_ch, {"ch", ladder, chip} );

	}
      mh_ch->Draw( "HIST", 0.8, 0.3, 1.0, 1.0);
    }

  c->Print( c->GetName() );
  
  ////////////////////////////////////////////////////////////////////////
  // DAC
  c->Clear();
  title = "DAC ch distribution;DAC ch;Entries";
  auto hist_dac = DrawHist1D( tr, title, 40, 0, 1200, "dac", cut_base );
  hist_dac->SetName( "all" );
  PushBack( obj_saved, hist_dac, { "dac" } );
  
  gPad->SetLogy( true );
  hist_dac->Draw( "HIST" );
  gPad->SetLogy( false );
  c->Print( c->GetName() );

  gPad->SetLogy( true );
  c->Print( c->GetName() );

  MultiHist* mh_dac = new MultiHist( "DAC ch distribution", title );
  mh_dac->SetStatsFormat( 1111 );
  mh_dac->Add( hist_dac );

  for( int i=0; i<module_num.size(); i++ )
    {
      string ladder = "ladder" + to_string( i );
      string cut_module = cut_base + "&&" + string("module==") + to_string( module_num[i] );
      auto hist_dac_ladder = DrawHist1D( tr, "DAC distribution;DAC ch;Entries",
					 30, 0, 900, "dac", cut_module );

      hist_dac_ladder->SetName( ladder.c_str() );
      HistSetting( hist_dac_ladder, GetColor( i + 1 ), 3, 1 );
      hist_dac_ladder->SetFillColorAlpha( hist_dac_ladder->GetLineColor(), 0.1 );

      mh_dac->Add( hist_dac_ladder );
      PushBack( obj_saved, hist_dac_ladder, {"dac", hist_dac_ladder->GetName()} );
      
    }

  mh_dac->Draw( "HISTE", 0.6, 0.4, 0.89, 0.89 );
  c->Print( c->GetName() );

  mh_dac->NormalizeHist();
  mh_dac->Draw( "HISTE", 0.6, 0.4, 0.89, 0.89 );
  gPad->SetLogy( false );
  c->Print( c->GetName() );




  
  c->Print( ((string)c->GetName()+"]").c_str() );

  for( auto& it : obj_saved )
    tf_output->WriteTObject( it.first, it.second.c_str() );
  
  tf_output->Close();
  tf->Close();
  
  return 0;
}
