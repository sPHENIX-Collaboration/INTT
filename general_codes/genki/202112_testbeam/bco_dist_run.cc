#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "Database.hh"

int bco_dist_run( int run = 21 )
{
  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  cout << "Run: " << run << endl;
  db->SetRun( run );

  string data_dir = "data/ELPH/";
  string data_path = data_dir + db->GetRootFile();
  cout << "Input: " << data_path << endl;

  TFile* tf = new TFile( data_path.c_str(), "READ" );
  TTree* tr = (TTree*)tf->Get( "tree_camac" );

  string output = "results/ELPH/bco/run/";
  output += "run" + to_string( run ) + ".root";
  cout << "Output: " << output << endl;
  
  TFile* tf_output = new TFile( output.c_str(), "RECREATE" );
  int xmin = 400;
  int xmax = 1500;
  int bin = xmax - xmin;
  TH1D* hist = new TH1D( "hist", "title", bin, xmin, xmax );
  tr->Draw( "camac_tdc[3]>>hist", "", "" );

  tf_output->WriteTObject( hist, hist->GetName() );
  tf_output->Close();
  tf->Close();
  
  return 0;
}
