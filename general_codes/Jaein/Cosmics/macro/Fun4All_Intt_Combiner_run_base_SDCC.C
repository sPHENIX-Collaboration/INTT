#include <fun4all/Fun4AllServer.h>

#include <fun4all/Fun4AllInputManager.h>

#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <fun4allraw/SingleInttPoolInput.h>
#include <fun4allraw/Fun4AllStreamingInputManager.h>

#include <fun4allraw/InputManagerType.h>

#include <phool/recoConsts.h>

#include <intt/InttCombinedRawDataDecoder.h>
#include <Trkr_Clustering.C>

#include "constant_values.hh"
#include "functions.hh"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libintt.so)

SingleInttPoolInput *sngl[9]{};

void ShowHelp()
{
  cout << "\n"
       << "+----------------------------------------------------------------------------------------------------+"
       << "| This is a Fun4All macro to produce a DST files using only INTT data for commissioning.             |\n"
       << "| It finds all root files using given run number.                                                    |\n"
       << "| Warning: Data year (" << kYear << ") is hard-corded in constant_values.hh. Change it by hand.      |\n"
       << "|                                                                                                    |\n\n"
       << "| void Fun4All_Intt_Combiner_run_base( int run_num = 26975, int nEvents = 10, bool is_debug = true ) |\n"    
       << "+----------------------------------------------------------------------------------------------------+\n"
       << endl;
}

//void Fun4All_Intt_Combiner_run_base(int nEvents = 10, int run_oum = 26975 )
void Fun4All_Intt_Combiner_run_base( int run_num = 38554, int nEvents = 10000, bool use_cdb = false, bool is_debug = true )
{
  if( is_debug == true )
    ShowHelp();
  
  ////////////////////////////////////////////////////////////////////////
  // flags
  ////////////////////////////////////////////////////////////////////////
  bool is_trkr_hit_on = false;
  bool is_trkr_cluster_on = false;
  bool is_raw_hit_on = false;

  if( is_debug == false )
    is_trkr_hit_on = is_trkr_cluster_on = is_raw_hit_on = true;
  
  is_trkr_hit_on = is_trkr_cluster_on = is_raw_hit_on = true;
  is_raw_hit_on = false;
  
  ////////////////////////////////////////////////////////////////////////
  // Config for input/output files
  ////////////////////////////////////////////////////////////////////////
  string run_type = GetRunType( run_num );
  
  // output_base format: {run_tpye}_intt_{run number}
  // For example, cosmics_intt_01234567
  string output_base = "_" + run_type
    + "_intt_" + string( 8 - to_string(run_num).size(), '0' ) + to_string( run_num );
  
  if( nEvents > 0 )
    output_base +=  + "_" + to_string( nEvents ) + "events";

  string debug_prefix = is_debug ? "test_" : "" ;  
  string output_dst = (is_debug ? "./" : kIntt_dst_dir )
    + debug_prefix + "DST" + output_base;

  if( use_cdb == true )
    output_dst += "_no_hot";
  
  output_dst += ".root";
  
  ////////////////////////////////////////////////////////////////////////
  // Fun4All main part
  ////////////////////////////////////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  
  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag); 
  rc->set_IntFlag("RUNNUMBER", run_num );
  
  Fun4AllStreamingInputManager *in = new Fun4AllStreamingInputManager("Comb");
  // if( is_debug )
  //   in->Verbosity(10);
  
  for( int i=0; i<8; i++ )
  {
    SingleInttPoolInput *sngl = new SingleInttPoolInput( "INTT_" + to_string(i) );
    
    // if( is_debug )
    //   sngl->Verbosity(3);

      cout << " - " << GetFilePath( run_num, run_type, i  ) << endl;
      sngl->AddFile( GetFilePath( run_num, run_type, i ) );

    // int nBcoVal = is_trkr_hit_on ? 0 : 2;
    // sngl->SetNegativeBco(nBcoVal);
    // sngl->SetBcoRange(2);
    in->registerStreamingInput(sngl, InputManagerType::INTT);
  }
  se->registerInputManager(in);

  if( is_trkr_hit_on )
  {
    InttCombinedRawDataDecoder *myDecoder = new InttCombinedRawDataDecoder("myUnpacker");
    myDecoder->runInttStandalone(true);
    myDecoder->writeInttEventHeader(true);

    if( use_cdb == true )
      {
	string cdbttree_name = kIntt_cdb_dir + "cdb_" + to_string(run_num) + ".root";
	myDecoder->LoadHotChannelMapLocal(cdbttree_name);

	//	string cdbttree_name_bco = cdb_output_path + "cdb_bco_" + to_string(run_num) + ".root";
	string cdbttree_name_bco = kIntt_cdb_dir + "cdb_bco_" + to_string(run_num) + ".root";
	myDecoder->SetCalibBCO(cdbttree_name_bco, InttCombinedRawDataDecoder::FILE);

      }
    
    se->registerSubsystem(myDecoder);
  }

  if( is_trkr_cluster_on )
  {
    Intt_Clustering(); //Be careful!!! INTT z-clustering may be off which is not what you want!
  }

  Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out", output_dst );
  se->registerOutputManager(out);

  se->run( nEvents );

  se->End();
  delete se;

  cout << "Output file: " << output_dst << endl;
  gSystem->Exit(0);
}
