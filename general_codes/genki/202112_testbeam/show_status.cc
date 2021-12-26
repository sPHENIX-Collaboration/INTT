#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "DrawPlots.hh"
#include "Database.hh"

pair < int, int > GetRange( string target )
{
  pair < int, int > rtn(0,100);
  if( target.find( "Length" ) != string::npos )
    rtn = pair< int, int >(0, 500);
  else if( target == "adc" )
    rtn = pair< int, int >(0, 8);
  else if( target.find( "chan_id" ) != string::npos )
    rtn = pair< int, int >(0, 128);
  else if( target.find( "chip_id" ) != string::npos )
    rtn = pair< int, int >(1, 27);
  else if( target.find( "ampl" ) != string::npos )
    rtn = pair< int, int >(0, 100);
  else if( target.find( "module" ) != string::npos )
    rtn = pair< int, int >(-1, 10);
  else if( target.find( "fem_id" ) != string::npos )
    rtn = pair< int, int >(-1, 10);
  else if( target.find( "fpga_id" ) != string::npos )
    rtn = pair< int, int >(-1, 10);
  else if( target.find( "bco_full" ) != string::npos )
    rtn = pair< int, int >(0, pow(2,16));
  else if( target.find( "bco" ) != string::npos )
    rtn = pair< int, int >(0, 129);
  else if( target.find( "camac_adc" ) != string::npos )
    rtn = pair< int, int >(0, 500);
  else if( target.find( "camac_tdc[1]" ) != string::npos )
    rtn = pair< int, int >(200, 300);
  else if( target.find( "camac_tdc[2]" ) != string::npos )
    rtn = pair< int, int >(370, 420);
  else if( target.find( "camac_tdc[3]" ) != string::npos )
    rtn = pair< int, int >(450, 1200);
  else if( target.find( "camac_tdc[4]" ) != string::npos )
    rtn = pair< int, int >(250, 350);
  else
    rtn = pair< int, int >(0, 10);
      
  return rtn;
}

void DrawParameter( TTree* tr, string target, int module, string cut_base, int bin, double xmin, double xmax )
{
  /*
  auto hists = GetHists( target, module, bin, xmin, xmax );

  stringstream expression, cut;
  expression << target << ">>" << hist->GetName();
  cut << "module==" << module;
  tr->Draw( expression.str().c_str(), cut.str().c_str(), "goff" );

  hist->Draw();
  //  hist->GetXaxis()->SetLabelSize( 0.07 );
  
  DrawStats( hist, 0.9, 0.8, 1.0, 0.9 );
*/
}

void DrawStatus( TCanvas* c, TTree* tr, int module, int module_num, int total_module_num, string cut_arg )
{

  stringstream cut_base;
  cut_base << "module==" << module;
  cut_base << " && 0 < chip_id && chip_id < 27";
  cut_base << " && -1 < chan_id && chan_id < 129";
  cut_base << " && ampl == 0 ";

  if( cut_arg != "" )
    cut_base << " && " << cut_arg;
  

  // adc
  TVirtualPad* pad_adc = c->cd( 1 );
  pad_adc->Divide(13, 2, 0, 0 );
  auto hists_adc = GetAdcChps( tr, cut_base.str() );

  vector < double > adc_max;
  for( auto& hist : hists_adc )
    adc_max.push_back( GetMaxBinContent( hist ) );

  for( int i=0; i<hists_adc.size(); i++ )
    {
      auto pad = pad_adc->cd( i+1 );
      gPad->SetGrid( true, true );
      string title = hists_adc[i]->GetTitle();
      title = (string)"Chip" + title.substr( title.find( "==" ) + 2 );
      hists_adc[i]->SetTitle( title.c_str() );
      hists_adc[i]->Draw();
      hists_adc[i]->GetYaxis()->SetRangeUser( 0, *max_element( adc_max.begin(), adc_max.end() ) );
      DrawTitle( gPad, 0.15 );
    }

  // CH
  TVirtualPad* pad_ch = c->cd( 2 );
  pad_ch->Divide(13, 2, 0, 0 );
  auto hists_ch = GetChChps( tr, cut_base.str(), module );

  vector < double > ch_max;
  for( auto& hist : hists_ch )
    ch_max.push_back( GetMaxBinContent( hist ) );

  for( int i=0; i<hists_ch.size(); i++ )
    {
      pad_ch->cd( i+1 );
      gPad->SetGrid( true, true );
      
      string title = hists_ch[i]->GetTitle();
      title = (string)"Chip" + title.substr( title.find( "==" ) + 2 );
      hists_ch[i]->SetTitle( title.c_str() );
	
      if( i > 12 )
	hists_ch[i]->Draw( "hbar" );
      else
	hists_ch[i]->Draw( "hbar" );	  

      hists_ch[i]->GetYaxis()->SetRangeUser( 0, *max_element( ch_max.begin(), ch_max.end() ) );
      hists_ch[i]->GetXaxis()->SetLabelSize( 0.05 );
      hists_ch[i]->GetYaxis()->SetLabelSize( 0.06 );
      DrawTitle( gPad, 0.15 );
    }
  
  
  // hit map
  c->cd( 3 );
  gPad->SetGrid( true, true );
  auto hitmap = GetHitmap( tr, cut_base.str() );
  hitmap->Draw( "colz" );
  //  DrawStats( hitmap, 0.0, 0.0, 0.08, 0.3 );

  
}

void DrawVariables( TCanvas* c, TTree* tr, string cut_arg )
{
  c->Clear();
  gStyle->SetOptStat( 11111 );
  c->Divide( 2, 1 );
  c->cd(1);
  auto branch_list = tr->GetListOfBranches();

  vector < string > branch_names;
  for( int i=0; i<branch_list->GetEntries(); i++ )
    {
      auto branch = (TBranch*)branch_list->At(i);
      string branch_name = branch->GetName();
      //      cout << branch_name << endl;

      if( branch_name == "event" )
	continue;

      if( branch_name == "INTT_event" )
	continue;
      
      if( branch_name.find( "camac" ) == string::npos )
	branch_names.push_back( branch_name );
    }

  branch_names.push_back( "camac_adc[1]" );
  branch_names.push_back( "camac_adc[2]" );
  branch_names.push_back( "camac_tdc[1]" );
  branch_names.push_back( "camac_tdc[2]" );
  branch_names.push_back( "camac_tdc[3]" );
  branch_names.push_back( "camac_tdc[4]" );
  //branch_names.push_back( "camac_tdc[5]" );
  branch_names.push_back( "Length$(adc)" );
  
  for( int i=0; i<branch_names.size(); i++ )
    { 
      stringstream expression;
      expression << branch_names[i];
      c->cd(1);
      gPad->SetGrid(true, true );
      
      string hist_name = "hist_" + Replace( branch_names[i], "$", "" );
      hist_name = Replace( Replace( hist_name, "(", "_" ) , ")", "" );

      auto range = GetRange( branch_names[i] );
      double val_min = range.first;
      double val_max = range.second;
      int val_bin  = val_max - val_min;

      if( val_bin > 5e3 )
	val_bin = 200;

	      
      TH1D* hist = new TH1D( hist_name.c_str(), hist_name.c_str(),
			     val_bin, val_min, val_max );
      // double GetMaxVal( TTree* tr, string target, string cut );
      expression << ">>" << hist->GetName();
      
      auto entry_bin = tr->Draw( expression.str().c_str(), cut_arg.c_str(), "" );
      //      int entry_max = entry_max;
      auto entry_max = tr->GetEntries();
      DrawStats( hist, 0.9, 0.9, 1.0, 1.0 );
      
      //cout << expression.str() << endl;
      if( entry_bin > 700 )
	entry_bin = 50;
      // if( biny > 1000 )
      // 	biny /=
      
      c->cd(2);
      gPad->SetGrid(true, true );

      expression.str( "" );
      string hist_name2d = "hist2d_" + Replace( branch_names[i], "$", "" );
      hist_name2d = Replace( Replace( hist_name2d, "(", "_" ) , ")", "" );
      TH2D* hist2d = new TH2D( hist_name2d.c_str(), hist_name2d.c_str(), entry_bin, 0, entry_max,  val_bin, val_min, val_max );
      cout << entry_bin << " " << 0 << " " << entry_max << "\t" << val_bin << " " << val_min << " " << val_max << endl;

      expression << branch_names[i] << ":Entry$>>" << hist_name2d;
      cout << expression.str() << endl;
      tr->Draw( expression.str().c_str(), cut_arg.c_str(), "colz" );
      DrawStats( hist2d, 0.9, 0.9, 1.0, 1.0 );

      c->Print( c->GetName() );
      
    }

}

int show_status( string data = "data/ELPH/NWU_fphx_raw_20211128-0947_0.root", string cut_arg = "" )
{

  string mask_cut = string("!(module==6 && chip_id==14 && chan_id==0)")
    + " && !(module==5 && chip_id==19 && chan_id==119)"
    + " && !(module==5 && chip_id==4 && chan_id==0)";

  if( cut_arg == "" )
    cut_arg = mask_cut;
  else
    cut_arg = cut_arg + " && " + mask_cut;


  gStyle->SetGridColor( kGray );
  gStyle->SetOptStat( false );

  Database* db = new Database();
  db->SetRun( data );
  auto module_num = db->GetModules();

  string data_name = GetBaseName( data );
  cout << "Data: " << data << endl;

  TFile* tf = new TFile( data.c_str(), "READ" );
  if( tf == nullptr )
    {
      cerr << data << " is not found" << endl;
      return -1;
    }

  TTree* tr = (TTree*)tf->Get( "tree_both" );
    
  string output = "results/ELPH/" + data_name + ".pdf";
  cout << "Output: " << output << endl;

  TCanvas* c = new TCanvas( output.c_str(), "title", 1600, 800 );
  //  c->Divide( module_num.size(), 4 );
  c->Print( ((string)c->GetName() + "[").c_str() );

  for( int i=0; i<module_num.size(); i++ )
    {
      c->Clear();
      c->Divide( 1, 3 );
      DrawStatus( c, tr, module_num[i], i, module_num.size(), cut_arg );
      c->Print( c->GetName() );

    }

  DrawVariables( c, tr, cut_arg );
  
  c->Print( ((string)c->GetName() + "]").c_str() );

  return 0;
}
