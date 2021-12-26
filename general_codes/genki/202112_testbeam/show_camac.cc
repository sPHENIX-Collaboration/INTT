#define __CINT__

#ifdef __linux__
//#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "DrawPlots.hh"
#include "Database.hh"

const int kFirst_run = 21;
const int kAdc_num = 5;
const int kTdc_num = 6;
const int kXmax = 2048;
const int kXmin = 0;
const int kBin_num = (kXmax - kXmin) / 10;

void FillHists( string data, int run_num, vector < TH2D* >& hists_adc, vector < TH2D* >& hists_tdc, string cut_arg = "" )
{

      
  TFile* tf = new TFile( data.c_str(), "READ" );
  if( tf == nullptr )
    {
      cerr << data << " is not found" << endl;
      return;
    }
  
  TTree* tr = (TTree*)tf->Get( "tree_camac" );
  //cout << tr->GetEntries() << endl;
  
  for( int i=0; i<kAdc_num || i<kTdc_num; i++ )
    {
      for( auto type : { "ADC", "TDC" } )
	{

	  string name = string("hist") + type + to_string(i);
	  string expression = string("camac_") + ( type=="ADC" ? "adc" : "tdc" ) + "[" + to_string( i ) + "]" + ">>" + name;
	  
	  TH1D* hist = new TH1D( name.c_str(), name.c_str(), kBin_num, kXmin, kXmax );
	  tr->Draw( expression.c_str(), cut_arg.c_str(), "goff" );

	  //double* values = tr->GetV1();
	  for( int j=0; j<hist->GetNbinsX()+1; j++ )
	    {

	      
	      if( type == "ADC" && i<kAdc_num )
		hists_adc[i]->SetBinContent( run_num - kFirst_run, i, hist->GetBinContent(j) );
	      else if( type == "TDC" && i<kTdc_num )
		hists_tdc[i]->SetBinContent( run_num - kFirst_run, i, hist->GetBinContent(j) );
	      
	      if( hist->GetBinContent(j) != 0 )
		{
		  cout << run_num << " " << type << " ch" << i << " " << j << " "
		       << hist->GetBinContent(j) << "\t";
		  
		  if( type == "ADC" && i<kAdc_num )
		    cout << hists_adc[i]->GetBinContent( run_num - kFirst_run, i );
		  else
		    cout << hists_tdc[i]->GetBinContent( run_num - kFirst_run, i );

		  cout << endl;
		}
	      

	    }
	  
	}
      
    }
  
}
      
int show_camac( string cut_arg = "" )
{

  gStyle->SetGridColor( kGray );
  gStyle->SetOptStat( false );

  string db_file = "documents/2021_test_beam/run_list - Setup.tsv";
  Database* db = new Database( db_file );
  //  db->Print();
  string data_dir = "data/ELPH/";
  
  vector < TH2D* > hists_adc, hists_tdc; // run vs adc[i], run vs tdc[i]

  for( int i=0; i<kAdc_num || i<kTdc_num; i++ )
    {
      for( auto type : { "ADC", "TDC" } )
	{
	  string name = type + to_string( i );
	  TH2D* hist = new TH2D( name.c_str(), name.c_str(),
				 db->GetNumberOfRuns() - kFirst_run, kFirst_run, db->GetNumberOfRuns()+1,
				 kBin_num, kXmin, kXmax );
	  if( type == "ADC" && i<kAdc_num )
	    hists_adc.push_back( hist );
	  else if( type == "TDC" && i<kTdc_num )
	    hists_tdc.push_back( hist );
	  
	}
    }

  
  for( int i=kFirst_run ; i<db->GetNumberOfRuns()+1; i++ )
    {
      db->SetRun( i );
      //auto run = db->GetRun();

      string data = data_dir + db->GetRootFile();

      if( db->GetSource() != "beam" )
	continue;

      if( db->GetDAQMode() != "CAMAC" )
	continue;
      
      if( db->GetRunNum() == 88 )
	continue;
      
      cout << db->GetRunNum() << " " << data << endl;

      FillHists( data, db->GetRunNum(), hists_adc, hists_tdc, cut_arg );

      if( i == 40 )
	break;
    }

  
  string output = string("results/ELPH/") + "show_camac.pdf";
  cout << "Output: " << output << endl;
  
  TCanvas* c = new TCanvas( output.c_str(), "title", 1600, 800 );
  c->Divide( 2, kTdc_num );
  c->Print( ((string)c->GetName() + "[").c_str() );
  
  for( int i=0; i<kAdc_num || i<kTdc_num; i++ )
    {
      c->cd( 2 * i + 1); // ADC
      if( i<kAdc_num )
	hists_adc[i]->Draw( "colz" );
      
      c->cd( 2 * i + 2 ); // ADC
      if( i<kTdc_num )
	hists_tdc[i]->Draw( "colz" );
      
    }
  
  /*
    for( auto& hist : hists_adc )
    {
    for( int i=0; i<hist->GetNbinsX(); i++ )
    {
    for( int j=0; j<hist->GetNbinsY(); j++ )
    {
    if( hist->GetBinContent(i, j) != 0 )
    cout << hist->GetName() << " " << i << "-" << j << "\t" << hist->GetBinContent(i, j) << endl;
    }
    
    }
    }
  */
  
  c->Print( c->GetName() );
  c->Print( ((string)c->GetName() + "]").c_str() );
  
  return 0;
}
