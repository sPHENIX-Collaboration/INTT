#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"

int num_in_trigger_event()
{
  string data_dir =  "results/ELPH/trim_data/no_clone/";

  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( 89 );
  string data_original = "data/ELPH/" + db->GetRootFile();
  string data_no_clone = data_dir + "1run89_no_clone.root";
  string data_clone = data_dir + "1run89_clone.root";

  TFile* tf_original = new TFile( data_original.c_str(), "READ" );
  TTree* tr_original = (TTree*)tf_original->Get( "tree_both" );

  TFile* tf_no_clone = new TFile( data_no_clone.c_str(), "READ" );
  TTree* tr_no_clone = (TTree*)tf_no_clone->Get( "tree_both" );
  
  TFile* tf_clone = new TFile( data_clone.c_str(), "READ" );
  TTree* tr_clone = (TTree*)tf_clone->Get( "tree_both" );

  string output_dir = "results/ELPH/clone_study/";
  string output = output_dir + "num_in_trigger_event.pdf";
  TCanvas* c = GetCanvas( output, "square", false );

  double xmin = 0, xmax = 500;
  int bin = (xmax - xmin) / 10;
  TH1D* hist_original = new TH1D( "original", "INTT hit num (no clone hits); #hit;Entries", bin, xmin, xmax );
  HistSetting( hist_original );  
  tr_original->Draw( "Length$(adc)>>original", "", "goff" );

  TH1D* hist_no_clone = new TH1D( "no_clone", "INTT hit num (no clone hits); #hit;Entries", bin, xmin, xmax );
  HistSetting( hist_no_clone, kRed );  
  tr_no_clone->Draw( "Length$(adc)>>no_clone", "", "goff" );

  TH1D* hist_clone = new TH1D( "clone", "INTT hit num (no clone hits); #hit;Entries", bin, xmin, xmax );
  HistSetting( hist_clone, kAzure + 1 );  
  tr_clone->Draw( "Length$(adc)>>clone", "", "goff" );

  MultiHist* mh = new MultiHist( "#INTT hit in trigger events", "#INTT hit in trigger events;#INTT hit;Entries" );
  mh->Add( hist_original );
  mh->Add( hist_no_clone );
  mh->Add( hist_clone );
  mh->Draw( "HIST" );
  
  c->Print( c->GetName() );
  return 0;
}
