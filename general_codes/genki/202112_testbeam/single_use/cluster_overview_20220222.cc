#define __CINT__

#ifdef __linux__
//#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh" // for ?
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh" // for ?

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"
#include "../lib/functions.hh"

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


int GetHotChip( TTree* tr, int module, string additional_cut )
{
  string cut = string( "module==" ) + to_string( module );
  if( additional_cut != "" )
    cut += "&& " + additional_cut;

  auto hist = DrawHist1D( tr, "", 13, 1, 14, "chip_id", cut );

  vector < int > contents_vec;
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    {
      contents_vec.push_back( hist->GetBinContent(i) );
    }
  
  int index = max_element( contents_vec.begin(), contents_vec.end() ) - contents_vec.begin() + 1;

  return index;
}

int cluster_overview_20220222( int run_num = 89 )
{
  //  gStyle->SetOptStat( 1110 );
  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( run_num );
  const string kOriginal_data_dir = "data/ELPH/";

  if( db->IsBeamRun() == false && run_num <= 89 )
    return 0;

  string output_root_dir = "results/ELPH/";
  string data_dir = output_root_dir + "trim_data/clustered/";
  string data = data_dir + "run" + to_string( run_num ) + ".root";

  TFile* tf = new TFile( data.c_str(), "READ" );
  TTree* tr = (TTree*)tf->Get( "tr" );
  //  tr->Print();

  string output = output_root_dir + "single_use/" + "cluster_overview/run" + to_string( run_num ) + ".pdf";
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
  string cut_noise = "!(module==5 && chip_id==6 && chan_id==136)"; // run 89

  if( (36 <= run_num &&  run_num <= 39) || run_num == 390 )
    {
      cut_noise += "&& !( module == 5 && (chip_id==2 || chip_id==4 || chip_id==6) && chan_id==0)"; // it works although chan_id is a double variable somethow
      cut_noise += "&& !( module == 1 && chip_id==2 && chan_id==0)"; // it works although chan_id is a double variable somethow
    }
  else if( run_num == 25 )
    {
      cut_noise += " && !(module==6 && chip_id==1 && chan_id>253)";
    }

  
  cut_base += "&&" + cut_noise;

  string run = "Run" + to_string( run_num );
  ////////////////////////////////////////////////////////////////////////
  // number of hits in a cluster
  string title = run + ", #hits in a cluster;#hits in a cluster;Entries";
  auto hist_nhit = DrawHist1D( tr, title, 50, 0, 50, "nhit_in_cluster", cut_base );
  hist_nhit->SetName( "all" );
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
      
      for( int j=i+1; j<module_num.size(); j++ )	
	{
	  string expression_y = string("nhit_in_cluster[") + to_string(j) + "]";
	  string expression = expression_y + ":" + expression_x;

	  string this_title = run + string(";ladder ") + to_string( i )
	    + string(";ladder ") + to_string( j )
	    + ";Entries";

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
	  DrawPaletteAxis( hist_nhit_nhit, 0.91, 0.1, 0.93, 0.9, 0.04 );
	  gPad->SetLogz( true );
	  //hist_nhit_nhit->Draw( "colz" );
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
      
      string this_title = run + ", " + "Ladder " + to_string( i ) + title_chip_ch;
      string this_cut = cut_base + "&&" + string( "module==" ) + to_string( module_num[i] );

      c->cd( i + 1 );
      auto hist_chip_ch = DrawHist2D( tr, this_title,
				      13, 1, 14, 256, 0, 256,
				      expression, this_cut );

      hist_chip_ch->Draw( "colz" );
      hist_chip_ch->SetLineColor( kBlack );
      DrawStats( hist_chip_ch, 0.905, 0.8, 0.99, 0.99 );
      DrawPaletteAxis( hist_chip_ch, 0.91, 0.1, 0.93, 0.77, 0.04 );
      gPad->SetLogz( false );
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

      string expression = "chan_id";
      
      string this_title = run + ", " + "Ladder " + to_string( i ) + title_ch;


      MultiHist* mh_ch = new MultiHist( "ch", this_title );
      //mh_ch->SetYmax( 1e3 );      
      c->cd( i + 1 );
      PadSetting();
      
      int central_chip = GetHotChip( tr, module_num[i], cut_base );
      cout << "ladder" << i << ", " << central_chip << endl;
      int draw_range = 2;
      for( int j = central_chip-draw_range; j < central_chip + draw_range + 1 ; j++ )
	{
	  string this_cut = cut_base + "&&" + string( "module==" ) + to_string( module_num[i] )
	    + " && chip_id==" + to_string( j );
	  
	  auto hist_ch = DrawHist1D( tr, this_title,
				     256, 0, 256,
				     expression, this_cut );
	  hist_ch->SetName( ((string)"L" + to_string(i) + ",chip" + to_string(j)).c_str() );
	  HistSetting( hist_ch, GetColor( j - central_chip + draw_range ) );
	  hist_ch->SetFillColorAlpha( GetColor( j - central_chip + draw_range ), 0.05  );

	  //j==central_chip ? hist_ch->Draw( "HIST" ) : hist_ch->Draw( "HIST SAMES" );
	  //	  hist_ch->SetLineColor( GetColor( j - central_chip - 1 ) );
	  mh_ch->Add( hist_ch );
	  //DrawStats( hist_ch, 0.905, 0.8, 0.99, 0.99 );

	}
      mh_ch->Draw( "HIST", 0.8, 0.3, 1.0, 1.0);
    }

  c->Print( c->GetName() );
  
  ////////////////////////////////////////////////////////////////////////
  // DAC
  /*
  title = "DAC ch distribution;DAC ch;Entries";
  auto hist_dac = DrawHist1D( tr, title, 40, 0, 1200, "dac", cut_base );

  gPad->SetLogy( true );
  hist_dac->Draw( "HIST" );
  c->Print( c->GetName() );
  */
  
  c->Print( ((string)c->GetName()+"]").c_str() );
  tf->Close();
  return 0;
}
