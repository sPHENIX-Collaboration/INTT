#include <GlobalVariables.C>

#include <G4Setup_sPHENIX.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>

#include <fun4allraw/SingleInttPoolInput.h>

#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <phool/recoConsts.h>

#include <inttread/InttCombinedRawDataConverterEB.h>
#include <intt/InttCombinedRawDataDecoder.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libinttread.so)
R__LOAD_LIBRARY(libintt.so)
//R__LOAD_LIBRARY(libaaa.so)

// string GetRunType( int run ) // obsolete, Run number in 2023 is not unique!!!

/*!
  @brief A path to the raw data of run from intt_server is returned
  @details Only beam and cosmics runs are expected.
 */
string GetFilePath( int run, string run_type, int intt_server )
{
  assert( 0<=intt_server && intt_server <= 7 );
    
  const string path_base = "/sphenix/lustre01/sphnxpro/commissioning/INTT/";  
  
  stringstream ss_rtn;
  ss_rtn << path_base << run_type << "/"
	 << run_type << "_intt" << intt_server << "-" << setw(8) << setfill( '0' ) << run << "-0000.evt";

  return ss_rtn.str();
}

void Fun4All_Intt_Combiner_2023(int nEvents = 10,
				string run_type = "beam",
				int run = 20869,
				string output_dir = "./output/"
			   )
{

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER", run );

  string debug_prefix = "test_";
  string output_base = "_" + run_type
    + "_intt_" + string( 8 - to_string(run).size(), '0' ) + to_string( run );

  if( nEvents > 0 )
    output_base +=  + "_" + to_string( nEvents ) + "events";
  
  string output_root = output_dir + debug_prefix + "tree" + output_base + ".root";
  string output_dst = output_dir + debug_prefix + "DST" + output_base + ".root";
  ////////////////////////////////////////////////////////////////////////
  
  InttCombinedRawDataConverterEB *inttdecode = new InttCombinedRawDataConverterEB();
  inttdecode->SetOutputFile( output_root );
  se->registerSubsystem( inttdecode );

  //  Fun4AllEvtInputPoolManager *in = new Fun4AllEvtInputPoolManager("Comb");
  Fun4AllStreamingInputManager *in = new Fun4AllStreamingInputManager("Comb");
  //  in->Verbosity(10);

  // loop over all INTT servers
  for( int i=0; i<8; i++ )
    {
      //SingleInttInput *sngl= new SingleInttInput("INTT_" + to_string(i));
      SingleInttPoolInput *sngl= new SingleInttPoolInput("INTT_" + to_string(i) );
      //    sngl->Verbosity(3);
      sngl->AddFile( GetFilePath( run, run_type, i ) );
      
      // //in->registerStreamingInput(sngl, Fun4AllEvtInputPoolManager::INTT);
      in->registerStreamingInput(sngl, Fun4AllStreamingInputManager::INTT);
    }
  se->registerInputManager(in);
  Enable::MVTX					= true;
  // Enable::MVTX_CELL				= Enable::MVTX && true;
  // Enable::MVTX_CLUSTER				= Enable::MVTX_CELL && true;
  // Enable::MVTX_QA				= Enable::MVTX_CLUSTER && Enable::QA && true;

  Enable::INTT					= true;
  //  Enable::INTT_ABSORBER			= true; // enables layerwise support structure readout
  //  Enable::INTT_SUPPORT			= true; // enable global support structure readout
  // Enable::INTT_CELL				= Enable::INTT && true;
  // Enable::INTT_CLUSTER				= Enable::INTT_CELL && true;
  // Enable::INTT_QA				= Enable::INTT_CLUSTER && Enable::QA && true;

  Enable::TPC					= true;
  // Enable::TPC_ABSORBER				= true;
  // Enable::TPC_CELL				= Enable::TPC && true;
  // Enable::TPC_CLUSTER				= Enable::TPC_CELL && true;
  // Enable::TPC_QA				= Enable::TPC_CLUSTER && Enable::QA && true;

  Enable::MICROMEGAS				= true;
  // Enable::MICROMEGAS_CELL			= Enable::MICROMEGAS && true;
  // Enable::MICROMEGAS_CLUSTER			= Enable::MICROMEGAS_CELL && true;
  // Enable::MICROMEGAS_QA				= Enable::MICROMEGAS_CLUSTER && Enable::QA && true;

  // Enable::TRACKING_TRACK			= (Enable::MICROMEGAS_CLUSTER
  // 						   && Enable::TPC_CLUSTER
  // 						   && Enable::INTT_CLUSTER
  // 						   && Enable::MVTX_CLUSTER) && true;

  //////////////////////////////
  // conditions DB flags      //
  //////////////////////////////
  Enable::CDB					= true;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);

  // aaa* aaa_aaa = new aaa( "aaa" );
  // se->registerSubsystem( aaa_aaa );
  InttCombinedRawDataDecoder* intt_decoder = new InttCombinedRawDataDecoder();
  intt_decoder->Verbosity(0);
  se->registerSubsystem(intt_decoder);

  
  
  // Initialize the selected subsystems
  G4Init();
  G4Setup();
  
  TrackingInit();
  //Intt_Cells();
  Intt_Clustering();

  Fun4AllOutputManager *out = new Fun4AllDstOutputManager("DST", output_dst );
  se->registerOutputManager(out);

  se->run(nEvents);

  se->End();
  delete se;
  gSystem->Exit(0);
}
