#define __CINT__

#ifdef __linux__
//#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"
#include "../lib/functions.hh"
#include "../lib/INTTEvent.hh"
#include "../lib/EventResidual.hh"

//#define DEBUG

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif
  
vector < EventResidual* > GetTracks( TCanvas* c,
			   vector < pair < TObject*, string > >& obj_saved,
			   TTree* tr,  int chip, string cut_base )
{

  double center = 0.0;
  double correction_value = 0;

  string cut = cut_base + " && " + GetCutForLadders( 3, "chip_id", chip );
  cout << "cut: " << cut << endl;
  
  int num = tr->Draw( "chan_id[2]:chan_id[1]:chan_id[0]", cut.c_str(), "HIST" );
  double *chan0, *chan1, *chan2;
  chan0 = tr->GetV1();
  chan1 = tr->GetV2();
  chan2 = tr->GetV3();

  int num_in_canvas = 50;
  string mg_name = string("Event0-") + to_string( num_in_canvas );
  string mg_title = mg_name + ":Ladder;Channel (ch)";
  TMultiGraph* mg = new TMultiGraph( mg_name.c_str(), mg_title.c_str() );

  auto frame = c->DrawFrame( -0.1, -5, 2.1, 261 );
  frame->SetTitle( mg_title.c_str() );
  vector < TGraph* > graphs;
      
  vector < EventResidual* > ers;
  for( int i=0; i<num; i++ )
    {
      EventResidual* er = new EventResidual( 3, chan0[i], chan1[i], chan2[i] );

      if( debug )
	{
	  er->Print();
	  mg->Add( er->GetGraph() );
	}
      
      ers.push_back( er );
      graphs.push_back( er->GetGraph() );
      

      if( i%num_in_canvas == 0 && i!=0 && debug )
	{
	  mg->Draw( "PMC P" );
	  gPad->Update();
	  for( auto& graph : graphs )
	    {
	      int c1 = graph->GetMarkerColor();
	      auto track = (TF1*)graph->GetListOfFunctions()->FindObject("f");
	      track->SetLineColor(c1);
	    }
	  
	  c->Print( c->GetName() );

	  c->Clear();
	  mg_name = string("Event") + to_string( i+1 ) + "-" + to_string( i + 1 + num_in_canvas );
	  mg_title = mg_name + ":Ladder;Channel (ch)";
	  mg = new TMultiGraph( mg_name.c_str(), mg_title.c_str() );
	  frame = c->DrawFrame( -0.1, -5, 2.1, 261 );
	  frame->SetTitle( mg_title.c_str() );

	}
      
      if( i == 10 && debug )
       	break;
    }

  return ers;
}

//int alignment_fitting( int run_num = 89, string output_dir_arg = "", string cut_arg = "", bool is_MC = false )
int alignment_fitting( int run_num = 89, bool is_MC = false )
{
  //  gStyle->SetOptStat( 1110 );
  gStyle->SetOptFit( true );
  gErrorIgnoreLevel = kError;

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
  string data_suffix = "";
  if( !is_MC )
    data_suffix = ".root";
  else
    data_suffix =  "_MC.root";

  string data_name =  "run" + to_string( run_num ) + data_suffix;
  string data = data_dir + data_name;

  TFile* tf = new TFile( data.c_str(), "READ" );
  TTree* tr = (TTree*)tf->Get( "tr" );
  //  tr->Print();

  string output = output_root_dir + "alignment/fitting/";

  output += Replace( data_name, ".root", ".pdf" );

  cout << " - Input: " << data << endl;
  cout << " - Output: " << output << endl;
  
  TFile* tf_output = new TFile( Replace( output, ".pdf", ".root" ).c_str(), "RECREATE" );
  vector < pair < TObject*, string > > obj_saved;

  TTree* tr_output = new TTree( "tr", "TTree for alignment" );
  double chan_id[3], chan_id_tracking[3], chip_id[3], residual[3], track_slope, track_intercept;
  tr_output->Branch( "chip_id", chip_id, "chip_id[3]/D" );
  tr_output->Branch( "chan_id", chan_id, "chan_id[3]/D" );
  tr_output->Branch( "chan_id_tracking", chan_id_tracking, "chan_id_tracking[3]/D" );
  tr_output->Branch( "residual", residual, "residual[3]/D" );
  tr_output->Branch( "track_slope", &track_slope, "track_slope/D" );
  tr_output->Branch( "track_intercept", &track_intercept, "track_intercept/D" );
  
  TCanvas* c = GetCanvas( output, "square", false );
  c->Print( ((string)c->GetName()+"[").c_str() );

  vector < int > modules;
  modules.push_back( 1 );
  modules.push_back( 6 );
  modules.push_back( 5 );
  const int kModule_num = modules.size();
  
  string cut_base = string("Length$(adc) == ") + to_string( kModule_num ) + " && ";
  cut_base += GetCutForLaddersEachValue( kModule_num, "module", modules );
  cout << cut_base << endl;  
  
  vector < int > hot_chips;
  for( auto& module : modules )
    hot_chips.push_back( GetHotChip( tr, module, cut_base ) );

  string cut_hot_chips = GetCutForLadders( kModule_num, "chip_id", hot_chips[0] );
  cout << " - Cuts to get hot chips: " << cut_hot_chips << endl;

  vector < TGraphErrors* > graphs_residual;
  stringstream output_lines;

  int chip_min = hot_chips[0] - 2 > 0? hot_chips[0] - 2 : 0;
  int chip_max = chip_min + 5 < 14 ? chip_min + 5  : 13;
  for( int j=chip_min; j<chip_max+1; j++ )
    {

      if( debug )
	j = hot_chips[0];
      
      auto ers = GetTracks( c, obj_saved, tr, j, cut_base );
      chip_id[0] = chip_id[1] = chip_id[2] = j;

      for( int i=0; i<kModule_num; i++ )
	{
	  
	  
	  stringstream hist_name;
	  hist_name << "Residual_ladder" << i << "_chip" << to_string(j);
	  TH1D* hist = new TH1D( hist_name.str().c_str(), hist_name.str().c_str(), 512, -256, 256 );

	  for( auto& er : ers )
	    {
	      for( int k = 0; k<3; k++ )
		{
		  chan_id[k] = er->GetChannel(k);
		  chan_id_tracking[k] = er->GetChannelTracking(k);
		  residual[k] = er->GetResidual(k);
		}
	      
	      track_slope = er->GetTrack()->GetParameter(1);
	      track_intercept = er->GetTrack()->GetParameter(0);
	      tr_output->Fill();
	      
	      hist->Fill( er->GetResidual(i) );
	    }
	  
	  hist->Draw();
	  c->Print( c->GetName() );

	  gPad->SetLogy( true );
	  c->Print( c->GetName() );

	  hist->GetXaxis()->SetRangeUser( -50, 50 );
	  c->Print( c->GetName() );
	  
	  gPad->SetLogy( false );
	}


      if( debug )
	break;
    }

  cout << output_lines.str() << endl;

  gPad->SetLogy( false );
  for( auto& g : graphs_residual )
    {
      PushBack( obj_saved, g, {g->GetName()} );
      g->Draw( "AP" );
      TF1* f_flat = new TF1( "flat", "pol0", 0, 14 );
      g->Fit( f_flat );
      c->Print( c->GetName() );
    }
  
  c->Print( ((string)c->GetName()+"]").c_str() );

  obj_saved.push_back( pair < TObject*, string >( tr_output, tr_output->GetName() ) );
  
  for( auto& obj : obj_saved )
    tf_output->WriteTObject( obj.first, obj.second.c_str() );

  tf_output->Close();
  tf->Close();
  return 0;
}
