#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"
#endif


#include "../Database.hh"
#include "../lib/functions.hh"
//#include "INTThit.hh"
#include "../lib/INTTEvent.hh"

int time_dependence_20220323( int run_num = 89, bool is_MC = false  )
{
  
  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( run_num );
  const string kOriginal_data_dir = "data/ELPH/";
  
  if( is_MC == false )
    {
      
      if( db->IsBeamRun() == false )
	return 0;
      
    }
  else // for MC data
    {
      
      // use Takashi's config in any case for the moment
      db->SetRun( 89 );
      db->SetRun( run_num );
      
    }

  
  
  //const string kOutput_root_dir = "results/ELPH/efficiency/simplest/";
  const string kOutput_root_dir = "results/ELPH/";
  string data = kOutput_root_dir + "trim_data/clustered/" + "run" + to_string( run_num ) + ".root";
  if( is_MC )
    data = data.substr( 0, data.size() - 5 ) + "_MC.root";

  cout << "Input: " << data << endl;
  TFile* tf = new TFile( data.c_str(), "READ" );
  auto tr = (TTree*)tf->Get( "tr" );

  string output = kOutput_root_dir + "/single_use/time_dependence_20220323/" + "run" + to_string( run_num ) + ".pdf";
  string output_root = Replace( output, ".pdf", ".root" );
  TFile* tf_output = new TFile( output_root.c_str(), "RECREATE" );
  TCanvas* c = GetCanvas( output, "double", false );
  c->Print( ((string)c->GetName()+"[").c_str() );

  int num = tr->Draw( "trigger_id:Entry$", "", "goff" );
  int trigger_id_max = tr->GetV1()[ num - 1];
  //TH2D* hist = new TH2D( "hist", "Channel distribution;Trigger ID;Channel;Entries", 50, 0, 5e3, 256, 0, 256 );
  TH2D* hist = new TH2D( "hist", "Channel distribution;Trigger ID;Channel;Entries", 50, 0, trigger_id_max * 1.1, 256, 0, 256 );

  int hot_chip = GetHotChip( tr, 1 );
  string cut = string("module[0]==1 && chip_id[0]==") + to_string( hot_chip );
  tr->Draw( "chan_id[0]:trigger_id>>hist", cut.c_str(), "goff" );
  hist->Draw( "colz" );
  DrawStats( hist, 0.8, 0.7, 0.9, 0.9 );

  auto profile = hist->ProfileX();
  profile->SetLineWidth( 3 );
  profile->SetLineColor( kRed );
  profile->Draw( "Psame" );
  //  tr->Draw( "chan_id[0]:trigger_id", "module[0]==1 && chip_id[0]==10", "prof same" );
  c->Print( c->GetName() );

  hist->GetYaxis()->SetRangeUser( hist->GetMean(2) - 50 ,hist->GetMean(2)+50 );
  c->Print( c->GetName() );

  c->Print( ((string)c->GetName()+"]").c_str() );

  tf_output->WriteTObject( hist, "hist2D" );
  tf_output->WriteTObject( profile, "profile" );
  tf_output->Close();
  tf->Close();
  return 0;
}
