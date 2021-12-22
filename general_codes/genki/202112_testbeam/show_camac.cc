#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "DrawPlots.hh"
#include "Database.hh"

int show_camac( string cut_arg = "" )
{

  gStyle->SetGridColor( kGray );
  gStyle->SetOptStat( false );

  Database* db = new Database();
  string dir = "data/ELPH/";
  db->SetRun( data );

  for( int i=21 ; i<db->GetLastRunNum(); i++ )
    {
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

    }
  c->Print( ((string)c->GetName() + "]").c_str() );

  return 0;
}
