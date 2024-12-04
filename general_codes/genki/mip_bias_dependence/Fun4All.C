#include <InttXYVertexFinder.h>

// Fun4All headers
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>

// Some general header macros
#include <GlobalVariables.C>
#include <G4Setup_sPHENIX.C>
#include <G4_Input.C>

// Trkr headers
#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <G4_ActsGeom.C>

// something else
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

// vertex reconstruction
#include <InttZVertexFinder.h>

// Analysis module
#include <InttClusterAnalyzer.h>

// Loading libraries
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY( libInttClusterAnalyzer.so )

/*!
  @details
  [Runs](https://docs.google.com/spreadsheets/d/175Z06nDFWACKIrvqN_R43ABLtRfE6r23u9CZtVuwDHg/edit?gid=1459536224#gid=1459536224)
  2024/10/16 1:11    54679    beam    On        10min            All Felix    35    114    100    60    intt_triggermode    HV scan HV=50V // raw 0 ongoing
  2024/10/16 1:41    54681    beam    On        10min            All Felix    35    114    100    60    intt_triggermode    HV scan HV=75V // raw 0 ongoing
  2024/10/16 2:05    54685    beam    On        10min            All Felix    35    114    100    60    intt_triggermode    HV scan HV=100V // raw 0 ongoing
  2024/10/16 2:19    54686    beam    On        10min            All Felix    35    114    100    60    intt_triggermode    HV scan HV=50V // raw 100k ongoing
  2024/10/16 2:30    54687    beam    On        10min            All Felix    35    114    100    60    intt_triggermode    HV scan HV=75V // raw 100k ongoiing
  2024/10/16 2:46    54688    beam    On        10min            All Felix    35    114    100    60    intt_triggermode    HV scan HV=100V // 100k? done
*/

int Fun4All(
	    int nEvents = 10,
	    int run = 54688 
	    )
{

  // File path format: /sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-00054688_special.root
  const string kData_dir = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/";
  string data_name = "DST_beam_intt-000" + to_string( run ) + "_no_hot_special.root";
  string data = kData_dir + data_name;
  
  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(0);
  
  // Read DST
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
  in->Verbosity( 1 );
  in->fileopen( data );
  // in->AddListFile(inputfile); // to read a list of files, use it. A path to the text file needs to be given.
  se->registerInputManager( in );

  // Flag Handler is always needed to read flags from input (if used)
  // and update our rc flags with them. At the end it saves all flags
  // again on the DST in the Flags node under the RUN node
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);
  Enable::CDB = true;
  // global tag

  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER", run );

  ///////////////////////////////////////////////////////////////////////////////////
  // Something depends on Acts should be below.... 
  ///////////////////////////////////////////////////////////////////////////////////
  // central tracking
  Enable::MVTX					= true;
  Enable::TPC					= true;
  Enable::MICROMEGAS				= true;
  Enable::INTT					= true;
  Enable::BLACKHOLE				= true;
  G4MAGNET::magfield_rescale = 1.4;

  // Initialize the selected subsystems
  G4Init();
  G4Setup();
  TrackingInit(); // necessary for ActsGeometry

  //////////////////////////
  // 
  //////////////////////////
  string data_vertex_xy = "results/vertex_xy/run_" + to_string( run ) + ".txt";
  ifstream ifs( data_vertex_xy.c_str() );
  double vertex_x, vertex_y;
  ifs >> vertex_x >> vertex_y;
  ifs.close();
  
  InttZVertexFinder* intt_vertex = new InttZVertexFinder( "intt_vertex_finder" );
  intt_vertex->Verbosity( 0 );
  intt_vertex->SetBeamCenter( vertex_x, vertex_y );
  intt_vertex->EnableQA( true );
  intt_vertex->SetOutDirectory( "results/vertex_z" );
  se->registerSubsystem( intt_vertex );
  
  //////////////////////////
  // Your analysis module // 
  //////////////////////////
  InttClusterAnalyzer* ana = new InttClusterAnalyzer( "name" );
  ana->Verbosity( 0 );
  ana->SetRunNum( run );
  //ana->SetRunNum( 3 );
  se->registerSubsystem( ana );
  
  //se->skip(skip);
  se->run(nEvents);
  se->End();
  
  delete se;
  
  gSystem->Exit(0);
  return 0;
}
