#define __CINT__

#ifdef __linux__
//#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh" // for ?
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh" // for ?

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

//#define DEBUG

#include "../Database.hh"
#include "../lib/functions.hh"
#include "../lib/NoisyChannelList.hh"
#include "../lib/INTTEvent.hh"
#include "../lib/Results.hh"

int global_counter = 0;

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif
  
TH1D* GetHist( TTree* tr, string name_arg, string expression_arg, string cut, int bin, double xmin, double xmax )
{
  string hist_name = name_arg;
  if( name_arg == "" )
    hist_name = string("hist") + to_string( ++global_counter );
  TH1D* hist = new TH1D( hist_name.c_str(), hist_name.c_str(), bin, xmin, xmax );
  HistSetting( hist, kBlack, 2, 1 );

  
  string expression = expression_arg + ">>" + hist_name;
  tr->Draw( expression.c_str(), cut.c_str(), "goff" );

  if( false )
    {
      cout << expression << endl;
      cout << hist_name << endl;
      cout << hist->GetEntries() << endl;
      for( int i=0; i<hist->GetNbinsX(); i++ )
	cout << i << " " << hist->GetBinContent(i) << endl;
    }
  
  return (TH1D*)hist->Clone();
}

void CalcEfficiency0D( TCanvas* c, TTree* tr, int ladder_num, string cut_arg, vector < pair < TObject*, string> >& obj_saved )
{

  int module_num = INTTEvent::Ladder2Module( ladder_num );
  
  string cut_module = string("focusing_module == ") + to_string( module_num );
  int hot_chip = GetHotChip( tr, INTTEvent::Ladder2Module(ladder_num) );

  vector < Results* > eff_results;
		    
  // loop over hot chip-2 - hot chip+2
  int first_chip = hot_chip-2 > 0 ? hot_chip-2 : 1;
  int last_chip = hot_chip+2+1 < 15 ? hot_chip+2+1 : 14;

  for( int i=first_chip; i<last_chip; i++ )
    {
      
      string cut_chip = string( "focusing_chip==" ) + to_string( i );
      string expression = "diff_chan";

      double xmin = -256;
      double xmax = 256;
      int bin = xmax - xmin;

      string cut = cut_module + " && " + cut_chip;
      if( cut_arg != "" )
	cut += " && " + cut_arg;
      
      //TH1D* hist_numerator = GetHist( tr, "hist_numerator", expression, cut, bin, xmin, xmax );
      int numerator_num = tr->GetEntries( cut.c_str() );
        
      string cut_deno = cut;
      if( cut_deno != "" )
	cut_deno += " && ";
      cut_deno += "chip_id[0]>-1";

      string hist_name = string("ladder") + to_string(ladder_num)
	+ "_chip" + to_string( i );
      TH1D* hist_denominator = GetHist( tr, hist_name, expression, cut_deno, bin, xmin, xmax );
      gPad->SetLogy( true );
      hist_denominator->Draw( "HISTE" );
      DrawStats( hist_denominator, 0.6, 0.6, 0.9, 0.9 );
      c->Print( c->GetName() );

      //      string formula = "[p0]*exp(-0.5*((x-[p1])/[p2])*((x-[p1])/[p2]))+[p3]*exp(-0.5*((x-[p4])/[p5])*((x-[p4])/[p5]))";
      string formula = "[p0]*exp(-0.5*((x-[p1])/[p2])*((x-[p1])/[p2]))+[p3]*exp(-0.5*((x-[p1])/[p4])*((x-[p1])/[p4]))";
      TF1* fit = new TF1( "f", formula.c_str(), xmin, xmax );
      fit->SetNpx( 1e4 );
      // 1st fitting -- only primary Gaussian ////////////////////////
      fit->SetParLimits(1, hist_denominator->GetMean() - 20, hist_denominator->GetMean() + 20 );
      fit->SetParLimits(2, 0, 10 );
      fit->SetParLimits(5, 0, 1e2 );
      
      fit->SetParameter(0, GetMaxBinContent( hist_denominator ) );
      fit->SetParameter(1, hist_denominator->GetMean() );
      fit->SetParameter(2, 10 );
      fit->SetParameter(3, 1e3 );
      //      fit->SetParameter(4, hist_denominator->GetMean() );
      //fit->SetParameter(5, 10 );
      fit->SetParameter(4, 10 );

      fit->FixParameter( 3, 0 );
      fit->FixParameter( 4, 0 );
      
      hist_denominator->Fit( fit );
      c->Print( c->GetName() );

      for( int i=0; i<6; i++ ) // for the primary gaussian
	{
	  if( i<3 )
	    fit->FixParameter( i, fit->GetParameter( i ) );
	  else
	    fit->ReleaseParameter( i );
	}

      // 2nd fitting -- fit with the 2nd Gaussian, fix primary Gaussian, ////////////////////////
      fit->SetParLimits( 3, 0, 1e5 );
      fit->SetParameter( 3, fit->GetParameter(0) / 100 );
      //fit->SetParLimits( 4, fit->GetParameter(1) - 10, fit->GetParameter(1) + 10 );
      //fit->SetParLimits( 5, 0, 1e2 );
      fit->SetParLimits( 4, 1, 50 );
      hist_denominator->Fit( fit );

      c->Print( c->GetName() );

      
      // 3rd fitting -- use both Gaussians ////////////////////////
      for( int i=0; i<6; i++ ) // for the primary gaussian
	fit->ReleaseParameter( i );

      fit->SetParLimits( 0, 0, fit->GetParameter(0) * 1.5 );
      fit->SetParLimits( 1, fit->GetParameter(1) - 3 , fit->GetParameter(1) + 3 );
      fit->SetParLimits( 2, 0, fit->GetParameter(2) * 1.5 );
      fit->SetParLimits( 3, 0, fit->GetParameter(3) * 1.5 );
      //fit->SetParLimits( 4, fit->GetParameter(4) - 3 , fit->GetParameter(4) + 3 );
      //fit->SetParLimits( 5, 0, fit->GetParameter(5) * 1.5 );
      fit->SetParLimits( 4, 0, fit->GetParameter(4) * 1.5 );
      hist_denominator->Fit( fit );

      hist_denominator->GetXaxis()->SetRangeUser( fit->GetParameter(1) - 15 * fit->GetParameter(2),
						  fit->GetParameter(1) + 15 * fit->GetParameter(2) );
      fit->Draw( "sameL" );
      DrawStats( hist_denominator, 0.6, 0.6, 0.9, 0.9 );
      c->Print( c->GetName() );

      //      obj_saved.push_back( pair < TObject*, string >(hist_denomoninator, hist_denominator->GetName() );
      
      double height = fit->GetParameter(0);
      double mean = fit->GetParameter(1);
      double sigma = fit->GetParameter(2);

      TLine* mean_line = new TLine();
      mean_line->SetLineColor( kRed );
      mean_line->SetLineStyle( 7 );
      mean_line->DrawLine( mean, 0, mean, height * 2 );
            
      // detection efficiency for whole region
      int denominator_num_whole = hist_denominator->GetEntries();

      cout << "Eff(ladder\t" << ladder_num << "\t"
	   << "chip=\t" << i << "\t"
	   << "+/-\t" << "-" << "\t * sigma"
	   << ")=\t"
	   << denominator_num_whole << "\t/\t" << numerator_num << "\t=\t"
	   << 1.0 * denominator_num_whole / numerator_num
	   << endl;
      
      Results* results_whole = new Results( ladder_num, i, -1, numerator_num, denominator_num_whole );
      eff_results.push_back( results_whole );
      
      // add graph at first      
      for( int j=1; j<4; j++ )
	{
	  
	  string cut_region = cut_deno + " && "
	    + to_string( mean - sigma * j ) + "<diff_chan"
	    + " && " + "diff_chan<" + to_string( mean + sigma * j );
	  int denominator_num_region = tr->GetEntries( cut_region.c_str() );
	  
	  double efficiency = 1.0 * denominator_num_region / numerator_num;
	  cout << "Eff(ladder\t" << ladder_num << "\t"
	       << "chip=\t" << i << "\t"
	       << "+/-\t" << i << "\t * sigma"
	       << ")=\t"
	       << denominator_num_region << "\t/\t" << numerator_num << "\t=\t"
	       << efficiency
	       << endl;
	  
	  TBox* box = new TBox();
	  box->SetFillColorAlpha( kBlue, 0.2 );	  
	  box->DrawBox( mean - sigma * j , 0, mean + sigma * j, height * 2 );
	  
	  Results* this_results = new Results( ladder_num, i, j, numerator_num, denominator_num_region );
	  eff_results.push_back( this_results );

	  TLatex* tex = new TLatex();
	  tex->SetTextSize( 0.02 );
	  stringstream comment;
	  comment << "* " << setprecision(3) << mean - sigma * j << "ch - "
		  << setprecision(3) << mean + sigma * j << "ch"
		  << "(" << setprecision(3) << mean << " #pm " << j << " #sigma ): ";
	  tex->DrawLatexNDC( 0.12, 0.9 - 0.075 * j, comment.str().c_str() );

	  comment.str( "" );
	  comment << denominator_num_region << " / " << numerator_num << " ="
		  << setprecision(3) << this_results->GetEfficiency()
		  << "{}^{+" << this_results->GetUpperError() << "}"
		  << "_{-" << this_results->GetLowerError() << "}";
	  tex->DrawLatexNDC( 0.16, 0.9 - 0.075 * j - 0.025, comment.str().c_str() );
	    
	}
      
      c->Print( c->GetName() );

      hist_denominator->GetXaxis()->SetRangeUser( -255, 255 );
      c->Print( c->GetName() );

    }

  vector < TGraphAsymmErrors* > graphs;
  int graph_num = 5 * 4; // chip * type of sigma window

  TLegend* leg = new TLegend( 0.4, 0.2, 0.85, 0.4 );
  // loop over sigma window ( 0 is for no window)
  for( int j=0; j<4; j++ )
    {

      TGraphAsymmErrors* graph = new TGraphAsymmErrors();
      GraphSetting( graph, GetColor(j), 2, 20 + j );
      graph->SetMarkerSize( 2 );
      string graph_name = string("Ladder") + to_string( ladder_num );
	//+ "_ChipCol" + to_string(i)
      if( j != 0 )
	graph_name += + "_" + to_string(j) + "#sigma";
      else
	graph_name += + "_no_sigma_window";

      graph->SetName( graph_name.c_str() );
      graph->SetTitle( ( graph_name + ";Chip Column;Efficiency (%)" ).c_str() );


      for( int i=first_chip; i<last_chip; i++ )
	{
	  
	  for( auto& result : eff_results )
	    {
	      if( result->chip_ != i )
		continue;

	      if( j==0 && result->sigma_ != -1 )
		continue;		  

	      if( result->sigma_ != j && j != 0)
		continue;
	      
	      double dx = 0.05 * j;
	      graph->SetPoint( graph->GetN(), i + dx, result->GetEfficiency() );
	      graph->SetPointError( graph->GetN()-1, 0, 0, result->GetLowerError(), result->GetUpperError() );
	      
	    }
	  graphs.push_back( graph );
	}
      leg->AddEntry( graph, graph->GetName(), "lp" );
    }

  gPad->SetLogy( false );
  auto frame = c->DrawFrame( first_chip - 0.5, 0, last_chip - 0.5, 1.01 );
  string frame_name = string("Ladder") + to_string( ladder_num );
  frame->SetName( frame_name.c_str() );

  string frame_title = frame_name + ";Chip column;Efficiency";
  frame->SetTitle( frame_title.c_str() );
  
  for( auto& graph : graphs )
    graph->Draw( "Psame" );

  leg->Draw();
  c->Print( c->GetName() );

  frame->GetYaxis()->SetRangeUser( 0.9, 1.05 );
  leg->SetX1NDC( 0.5 );
  leg->SetX2NDC( 0.9 );
  leg->SetY1NDC( 0.7 );
  leg->SetY2NDC( 0.9 );
  leg->Draw();
  c->Print( c->GetName() );
  
  cout << "=-----------------------" << endl;
  for( auto& it : eff_results )
    if( it->sigma_ == -1 || it->sigma_ == 3 )
      it->Print();
  
  
}

int ana_simplest( int run_num = 89, string output_dir_arg = "",
		  string cut_arg = "", bool is_MC = false )
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
  
  string output_root_dir = "results/ELPH/efficiency/simplest/";
  string data = output_root_dir + "run" + to_string( run_num );

  if( !is_MC )
    data += + ".root";
  else
    data += + "_MC.root";

  TFile* tf = new TFile( data.c_str(), "READ" );
  TTree* tr = (TTree*)tf->Get( "tr" );
  //  tr->Print();

  string output = output_root_dir + "/ana/";

  if( output_dir_arg == "" )
    output_dir_arg = "normal";

  if( !is_MC )
    output += output_dir_arg + "/run" + to_string( run_num ) + ".pdf";
  else
    output += output_dir_arg + "/run" + to_string( run_num ) + "_MC.pdf";

  TFile* tf_output = new TFile( Replace( output, ".pdf", ".root" ).c_str(), "RECREATE" );
  vector < pair < TObject*, string > > obj_saved;
  
  TCanvas* c = GetCanvas( output, "square", false );
  c->Print( ((string)c->GetName()+"[").c_str() );

  //////////////////////////////////////////////////////////
  // Integration over everything
  int num_whole = tr->GetEntries( cut_arg.c_str() );
  string cut_deno_whole = (cut_arg=="") ? "chan_id[0]>-1" : cut_arg + " && " + "chan_id[0]>-1";
  int deno_whole = tr->GetEntries( cut_deno_whole.c_str() );

  cout << " - Detection efficiency() = "
       << setw(8) << deno_whole << " / " 
       << setw(8) << num_whole << " = "
       << setw(6) << setprecision(4) << 100.0 * deno_whole / num_whole << "%"
       << endl;
				   
  //////////////////////////////////////////////////////////
  // efficiency for each module
  
  for( int i=0; i<3; i++ )
    {

      CalcEfficiency0D( c, tr, i, cut_arg, obj_saved );

      if( debug )
	break;
    }
			     
  c->Print( ((string)c->GetName()+"]").c_str() );
  
  for( auto& it : obj_saved )
    tf_output->WriteTObject( it.first, it.second.c_str() );
  
  tf_output->Close();
  tf->Close();

  cerr << "Output: " << output << endl;
  return 0;
}
