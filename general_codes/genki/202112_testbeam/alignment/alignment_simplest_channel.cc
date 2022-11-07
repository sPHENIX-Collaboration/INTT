#define __CINT__

#ifdef __linux__
//#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"
#include "../lib/functions.hh"

bool debug = false;

//vector < double > FitIteration( TCanvas* c,
void ResidualVsChannel( TCanvas* c,
			vector < pair < TObject*, string > >& obj_saved,
			TTree* tr,
			int run,
			int ladder, int module, int chip, string expression, string cut_base )
{

  vector < double > rtn;
  double center = 0.0;
  double correction_value = 0;

  string hist_name = "residual_vs_ch_ladder" + to_string( ladder ) + "_"
    + "chip" + to_string( chip );
  string hist_title = hist_name + ", Run" + to_string( run ) + ";Channel from tracking (ch);Residual (ch);Entries";
  TH2D* hist_residual = new TH2D( hist_name.c_str(), hist_title.c_str(),
				  256, 0, 256,
				  512, -256, 256 );
  
  HistSetting( hist_residual, kBlack );
  
  string this_expression = expression
    + ":chan_id[" + to_string( ladder ) + "] - (" + expression + ")"
    + ">>" + hist_name;
  string cut = cut_base + " && " + GetCutForLadders( 3, "chip_id", chip );
  cout << "Ladder" << ladder << ", cut: " << cut << endl;

  tr->Draw( this_expression.c_str(), cut.c_str(), "HIST" );
  DrawStats( hist_residual, 0.6, 0.6, 0.99, 0.85 );      

  if( hist_residual->GetEntries() == 0 )
    return;
  
  hist_residual->Draw( "colz" );
  hist_residual->GetYaxis()->SetTitleOffset( 1 );
  DrawStats( hist_residual, 0.8, 0.75, 1.0, 0.95 );
  DrawPaletteAxis( hist_residual, 0.905, 0.1, 0.93, 0.725 );
  pair < TObject*, string > obj( hist_residual->Clone(), hist_name );
  obj_saved.push_back( obj );
  c->Print( c->GetName() );

  // selection of the residual range
  cut += " && fabs(" + expression + ")<20"; // selection around the peak, primary cut
  tr->Draw( this_expression.c_str(), cut.c_str(), "goff" );
  if( hist_residual->GetEntries() == 0 )
    return;
  
  double mean_y = hist_residual->GetMean(2);
  double range_y = 3 * hist_residual->GetStdDev(2);
  cut += " && " + to_string( mean_y - range_y ) + "<" + expression
    + " && " + expression + "<" + to_string( mean_y + range_y );
  cout << "cut :" << cut << endl;
  //hist_residual->GetYaxis()->SetRangeUser( mean_y - range_y, mean_y + range_y );
  tr->Draw( this_expression.c_str(), cut.c_str(), "HIST" );
  if( hist_residual->GetEntries() == 0 )
    return;
  

  hist_residual->Draw( "colz" );
  hist_residual->GetYaxis()->SetTitleOffset( 1 );
  DrawStats( hist_residual, 0.8, 0.75, 1.0, 0.95 );
  DrawPaletteAxis( hist_residual, 0.905, 0.1, 0.93, 0.725 );
  c->Print( c->GetName() );

  hist_residual->GetYaxis()->SetRangeUser( mean_y - range_y, mean_y + range_y );
  c->Print( c->GetName() );

  auto prof = hist_residual->ProfileX();
  HistSetting( prof, kBlack );
  prof->Draw( "P same" );
  c->Print( c->GetName() );

  prof->Draw( "P" );
  double mean_x = hist_residual->GetMean(1);
  double range_x = 3 * hist_residual->GetStdDev(1);
  prof->GetXaxis()->SetRangeUser( mean_x - range_x, 
				  mean_x + range_x );
  DrawStats( prof, 0.85, 0.85, 0.98, 0.95 );
  c->Print( c->GetName() );
  
  /*
    TLatex* tex = new TLatex();
    tex->SetTextColor( GetColor(ladder) );
    tex->DrawLatexNDC( 0.12, 0.8, "Alignment" );
    tex->DrawLatexNDC( 0.13, 0.7, "correction:" );
    tex->DrawLatexNDC( 0.15, 0.6, (to_string(correction_value) + "ch").c_str() );
    
    c->Print( c->GetName() );
  */
  
  // double mean = hist_residual->GetMean();
  // double sigma =  hist_residual->GetStdDev();
  // TF1* f = new TF1( "f", "gaus", mean - 3 * sigma, mean + 3 * sigma );
  // f->SetNpx( 1e3 );
  // auto hist_residual_clone = (TH1D*)hist_residual->Clone();
  // hist_residual_clone->Fit( f, "R" );
  
  // hist_residual_clone->Draw( "HIST" );
  // DrawStats( hist_residual_clone, 0.7, 0.7, 0.9, 0.9 );
  // f->Draw( "Lsame" );
  
  pair < TObject*, string > obj_cut( hist_residual->Clone(), hist_name + "_cut" );
  obj_saved.push_back( obj_cut );
  
  pair < TObject*, string > obj_prof( prof->Clone(), hist_name + "_cut_prof" );
  obj_saved.push_back( obj_prof );
  
  // center = f->GetParameter( 1 );
  // correction_value -= center;
  // double range = 20;
  // hist_residual_clone->GetXaxis()->SetRangeUser( center - range * f->GetParameter(2),
  // 						 center + range * f->GetParameter(2) );
  
  // c->Print( c->GetName() );
  
  // c->cd( 4 );
  // DrawStats( (TH1D*)hist_residual_clone->Clone(), 0, 0, 1, 1 );
  
  // c->Print( c->GetName() );
  
  // rtn.push_back( correction_value );
  
      //  return rtn;
}


int alignment_simplest_channel( int run_num = 89, string output_dir_arg = "", string cut_arg = "", bool is_MC = false )
{
  //  gStyle->SetOptStat( 1110 );
  gStyle->SetOptFit( true );

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

  string output = output_root_dir + "alignment/simplest_channel/";

  if( output_dir_arg == "" )
    output_dir_arg = "normal";

  // don't forget "/"
  if( output_dir_arg[ output_dir_arg.size() - 1 ] != '/' )
    output_dir_arg += "/";
      
  output += output_dir_arg + Replace( data_name, ".root", ".pdf" );

  cout << " - Input: " << data << endl;
  cout << " - Output: " << output << endl;
  
  TFile* tf_output = new TFile( Replace( output, ".pdf", ".root" ).c_str(), "RECREATE" );
  vector < pair < TObject*, string > > obj_saved;
  
  TCanvas* c = GetCanvas( output, "square", false );
  c->Print( ((string)c->GetName()+"[").c_str() );

  vector < int > modules;
  modules.push_back( 1 );
  modules.push_back( 6 );
  modules.push_back( 5 );
  const int kModule_num = modules.size();
  
  string cut_base = string("Length$(adc) == ") + to_string( kModule_num ) + " && ";
  cut_base += GetCutForLaddersEachValue( kModule_num, "module", modules );
  if( cut_arg != "" )
    cut_base += " && " + CutArgumentProcess( cut_arg );
  
  cout << cut_base << endl;  
  
  vector < int > hot_chips;
  for( auto& module : modules )
    hot_chips.push_back( GetHotChip( tr, module, cut_base ) );

  string cut_hot_chips = GetCutForLadders( kModule_num, "chip_id", hot_chips[0] );
  cout << " - Cuts to get hot chips: " << cut_hot_chips << endl;

  /////////////////////////////////////////////////////////////////////////////////
  // Residual dists.
  string expression_residual_ladder0 = "";
  string expression_residual_ladder1 = "chan_id[1] - (chan_id[0] + chan_id[2])/2";
  string expression_residual_ladder2 = "chan_id[2] - 2 * chan_id[1] + chan_id[0] ";
  vector < string > expression_residual;
  expression_residual.push_back( expression_residual_ladder0 );
  expression_residual.push_back( expression_residual_ladder1 );
  expression_residual.push_back( expression_residual_ladder2 );

  vector < TGraphErrors* > graphs_residual;
  stringstream output_lines;
  // loop over tested ladders
  for( int i=0; i<kModule_num; i++ )
    {

      string expression = expression_residual[i];
      if( expression == "" )
	continue;

      int chip_min = hot_chips[i] - 2 > 0? hot_chips[i] - 2 : 0;
      int chip_max = chip_min + 4 < 14 ? chip_min + 4  : 13;

      TGraphErrors* g_residual = new TGraphErrors();
      string graph_name = string("graph_alignment_correction_ladder") +  to_string(i);
      string graph_title = string("Alignment correction, ladder") + to_string(i)
				    + ", Run" + to_string( run_num )
				    + ";Chip column;Correction value (ch)";

      g_residual->SetName( graph_name.c_str() );
      g_residual->SetTitle( graph_title.c_str() );
      GraphSetting( g_residual );
      for( int j=chip_min; j<chip_max+1; j++ )
	{

	  int index = obj_saved.end() - obj_saved.begin();

	  //vector < double > correction_value = FitIteration( c, obj_saved, tr, run_num, i, modules[i], j, expression, cut_base );
	  ResidualVsChannel( c, obj_saved, tr, run_num, i, modules[i], j, expression, cut_base );

	  /*
	  output_lines << i << "\t" << j << "\t";
	  for( auto& val : correction_value )
	    output_lines << setprecision(6) << setw(10) << val << "\t";
	  output_lines << "\n";

	  double final_value = correction_value[ correction_value.size()-1 ];
	  auto hist = ((TH1D*)obj_saved[ obj_saved.size()-1 ].first);
	  auto function = (TF1*)hist->GetListOfFunctions()->At(1);

	  if( function != nullptr )
	    final_value += function->GetParameter(1);
	  
	  g_residual->SetPoint( g_residual->GetN(), j, final_value );

	  if( function != nullptr )
	    g_residual->SetPointError( g_residual->GetN()-1, 0, function->GetParameter(2) );
	  */
	  
	  if( debug )
	    break;
	}

      //graphs_residual.push_back( g_residual );      

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
  for( auto& obj : obj_saved )
    tf_output->WriteTObject( obj.first, obj.second.c_str() );

  tf_output->Close();
  tf->Close();
  return 0;
}
