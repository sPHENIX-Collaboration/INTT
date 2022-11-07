#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"

TTree* GetTree( string data )
{
  TFile* tf = new TFile( data.c_str(), "READ" );
  if( tf == nullptr )
    return nullptr;
  
  return (TTree*)( tf->Get( "tree_both" )->Clone() );
}

int global_tag = 0;

MultiHist* DrawAll( TCanvas* c, TTree* tr_original, TTree* tr_no_clone, TTree* tr_clone, string name,  int bin, double xmin, double xmax, string expression, string cut, int normalize_flag )
{
  c->cd(1);
  
  if( normalize_flag == 2 )
    {
      DrawAll( c, tr_original, tr_no_clone, tr_clone, name, bin, xmin, xmax, expression, cut, 0 );
      DrawAll( c, tr_original, tr_no_clone, tr_clone, name, bin, xmin, xmax, expression, cut, 1 );
      return nullptr;
    }
  else if( normalize_flag == 3 )
    {
      auto mh_results
	= DrawAll( new TCanvas( "temp.pdf", "title", 100, 100 ),
		   tr_original, tr_no_clone, tr_clone, name, bin, xmin, xmax, expression, cut, 0 );
      
      c->cd(1);
      mh_results->Draw( "HISTE" );
      
      auto hists = mh_results->GetHists();
      TH1D* hist_original_copy = (TH1D*)hists[0]->Clone();
      TH1D* hist_no_clone_copy = (TH1D*)hists[1]->Clone();
      TH1D* hist_clone_copy = (TH1D*)hists[2]->Clone();
      hist_original_copy->Scale( 1.0 / hist_original_copy->Integral() );
      hist_no_clone_copy->Scale( 1.0 / hist_no_clone_copy->Integral() );
      hist_clone_copy->Scale( 1.0 / hist_clone_copy->Integral() );
      
      //      MultiHist* mh_ratio = new MultiHist( "ratio", "ratio", hists[1], hists[0] );
      MultiHist* mh_ratio = new MultiHist( "ratio", "ratio", hist_clone_copy, hist_no_clone_copy );
      mh_ratio->SetYmin( 0 );
      c->cd(2);
      mh_ratio->Draw( "PE" );
      c->Print( c->GetName() );
      return nullptr;
    }
  

  string expression_for_name = Replace4Cut( expression );
  string hist_name_original = "hist_original_" + expression_for_name + to_string( ++global_tag );
  TH1D* hist_original = new TH1D( hist_name_original.c_str(), hist_name_original.c_str(), bin, xmin, xmax );
  HistSetting( hist_original, kBlack, 3 );
  hist_original->SetMarkerStyle( 1 );
  tr_original->Draw( (expression+">>"+hist_name_original).c_str(), cut.c_str(), "goff" );
  
  string hist_name_no_clone = "hist_no_clone_" + expression_for_name + to_string( ++global_tag );
  TH1D* hist_no_clone = new TH1D( hist_name_no_clone.c_str(), hist_name_no_clone.c_str(), bin, xmin, xmax );
  HistSetting( hist_no_clone, kRed, 1 );
  hist_no_clone->SetMarkerStyle( 1 );
  tr_no_clone->Draw( (expression+">>"+hist_name_no_clone).c_str(), cut.c_str(), "goff" );
  
  string hist_name_clone = "hist_clone_" + expression_for_name + to_string( ++global_tag );
  TH1D* hist_clone = new TH1D( hist_name_clone.c_str(), hist_name_clone.c_str(), bin, xmin, xmax );
  HistSetting( hist_clone, kBlue, 1 );
  hist_clone->SetMarkerStyle( 1 );
  hist_clone->SetOption( "P" );
  tr_clone->Draw( (expression+">>"+hist_name_clone).c_str(), cut.c_str(), "goff" );

  MultiHist* mh = new MultiHist( name, name );
  mh->Add( hist_original );
  mh->Add( hist_no_clone );
  mh->Add( hist_clone );

  if( normalize_flag == 1 )
    mh->NormalizeHist();
  
  mh->Draw( "HISTE" );

  c->Print( c->GetName() );
  return mh;
}

int bco_original_clone_comparison_20220121( int run = 89 )
{
  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( run );
  
  if( db->IsBeamRun() == false )
    return 0;

  string data_original = "data/ELPH/" + db->GetRootFile();
  
  string data_dir = "results/ELPH/trim_data/no_clone/";
  string data_no_clone = data_dir + "run" + to_string(run) + "_no_clone.root";
  string data_clone = data_dir + "run" + to_string(run) + "_clone.root";

  auto tr_original = GetTree( data_original );
  auto tr_no_clone = GetTree( data_no_clone );
  auto tr_clone = GetTree( data_clone );

  if( tr_no_clone == nullptr || tr_clone == nullptr )
    return 0;
  
  string output = string("results/ELPH/single_use/bco_original_clone_comparison_20220121_run") + to_string(run) + ".pdf";
  TCanvas* c = GetCanvas( output, "double", false );
  c->Print( ((string)c->GetName()+"[").c_str() );

  string cut_base = "ampl==0";
  DrawAll( c, tr_original, tr_no_clone, tr_clone, "FPGA_ID", 5, 0, 6, "fpga_id", cut_base, 2 );
  DrawAll( c, tr_original, tr_no_clone, tr_clone, "FEM_ID", 5, 0, 6, "fem_id", cut_base, 2 );  
  DrawAll( c, tr_original, tr_no_clone, tr_clone, "Module", 6, 1, 7, "module", cut_base, 2 );  
  DrawAll( c, tr_original, tr_no_clone, tr_clone, "Chip_id", 26, 1, 27, "chip_id", cut_base, 2 );  
  DrawAll( c, tr_original, tr_no_clone, tr_clone, "Chan_id", 128, 0, 127, "chan_id", cut_base, 2 );    

  c->Print( ((string)c->GetName()+"]").c_str() );


  c = GetCanvas( Replace( output, ".pdf", "_details.pdf"), "ratio", false );
  c->SetWindowSize( 2000, 1450 );
  c->Print( ((string)c->GetName()+"[").c_str() );

  bool debug = false;
  
  for( auto module : {1, 6, 5} )
    {
      for( int chip=1; chip<27; chip++ )
	{
	  if( debug )
	    chip = 10;
	  
	  string name = string("Channel dist (")
	    + "run" + to_string(run) + ", "
	    + "module " + to_string(module) + ","
	    + "chip" + to_string(chip) + ")";

	  string cut = cut_base + " && module == " + to_string(module);

	  if( false ) // draw chip_id
	    {
	      cut +=  " && chip_id == " + to_string(chip);
	      cout << cut << endl;
	      
	      string expression = "chan_id";
	      DrawAll( c, tr_original, tr_no_clone, tr_clone, name, 128, 0, 127, expression, cut, 3 );
	    }
	  else // draw 2 chips in one
	    {
	      cut += "&& (chip_id == " + to_string(chip) + "|| chip_id == " + to_string( chip + 13 ) + ")";
	      cout << cut << endl;
	      
	      string expression = string("int(chip_id / 14) * 255") // it's offsets for y-axis.
		// Y value increases(decreases) as chan_id decreases(increases) for chips1-13(14-26)
		+ "+ pow(-1, int(chip_id / 14)) * chan_id";
	      
	      DrawAll( c, tr_original, tr_no_clone, tr_clone, name, 256, 0, 256, expression, cut, 3 );

	      if( chip == 13 )
		break;
	    }

	  if( debug )
	    break;
	  
	}

      if( debug )
	    break;
	  
    }
  
  c->Print( ((string)c->GetName()+"]").c_str() );  
  
  return 0;
}
