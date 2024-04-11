// Fun4All headers
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

// Some general headers
#include <GlobalVariables.C>

#include <G4Setup_sPHENIX.C>
#include <G4_Input.C>

// Trkr headers
#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>

#include <TrkrHitSetContainerv1.h>
#include <TrkrHitSetContainer.h>

#include <TrkrHitSet.h>
#include <TrkrCluster.h>

#include <TrkrClusterContainerv4.h>
#include <TrkrClusterContainer.h>

#include <TrkrClusterHitAssocv3.h>
#include <TrkrClusterHitAssoc.h>

#include <TrkrClusterCrossingAssocv1.h>
#include <TrkrClusterCrossingAssoc.h>

#include <TrkrHitSetv1.h>

#include <TrkrHit.h>
#include <TrkrHitv2.h>

// something else
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <intt/InttClusterizer.h>
#include <InttEventInfov1.h>
#include <InttEventInfo.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtrack_io.so)

#include <AnalysisInttCosmicCommissioning.h>
R__LOAD_LIBRARY( libAnalysisInttCosmicCommissioning.so )

//#include <QaInttCosmicCommissioning.h>
//R__LOAD_LIBRARY( libQaInttCosmicCommissioning.so )

int Fun4All_CosmicAnalysis(
		     int nEvents = 10000, //5,
		     //const string &inputFile = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/cosmic/DST_creation/new_DST_cosmics_intt_00035692.root", 
		     const string &inputFile = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/cosmic/NEW_DST_creation/test_clustering_DST_cosmics_intt_00038554_10000events.root", 
		     const string &outputFile = "cluster_result_00038554_4point_only.root",
		     const int skip = 0
		     )
{

  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(0);
  
  //Opt to print all random seed used for debugging reproducibility. Comment out to reduce stdout prints.
  //PHRandomSeed::Verbosity(1);

  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();
  // By default every random number generator uses
  // PHRandomSeed() which reads /dev/urandom to get its seed
  // if the RANDOMSEED flag is set its value is taken as seed
  // You can either set this to a random value using PHRandomSeed()
  // which will make all seeds identical (not sure what the point of
  // this would be:
  //  rc->set_IntFlag("RANDOMSEED",PHRandomSeed());
  // or set it to a fixed value so you can debug your code
  //  rc->set_IntFlag("RANDOMSEED", 12345);

  INPUTREADHITS::filename[0] = inputFile;
  // This is needed to read a DST file(s).
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
  in->fileopen( inputFile );
  // in->AddListFile(inputfile); // to read a list of files, use it. A path to the text file needs to be given.
  se->registerInputManager( in );
  se->Print("NODETREE"); // useless

  //  InputInit();

  // Flag Handler is always needed to read flags from input (if used)
  // and update our rc flags with them. At the end it saves all flags
  // again on the DST in the Flags node under the RUN node
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  // QaInttCosmicCommissioning* qicc = new QaInttCosmicCommissioning();
  // qicc->SetData( inputFile );
  // se->registerSubsystem( qicc );

  // se->skip(skip);
  // se->run(nEvents);
  // se->End();
  // return 0;

  ///////////////////////////////////////////////////////////////////////////////////
  // Something depends on Acts should be below.... 
  ///////////////////////////////////////////////////////////////////////////////////  
  Enable::CDB					= true;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);

  //////////////////////////////
   // central tracking
  Enable::MVTX					= true;
  Enable::TPC					= true;
  Enable::MICROMEGAS				= true;

  Enable::INTT					= true;
  //  Enable::INTT_ABSORBER			= true; // enables layerwise support structure readout
  //  Enable::INTT_SUPPORT			= true; // enable global support structure readout
//  Enable::INTT_CELL				= Enable::INTT && true;
  Enable::INTT_CLUSTER				= Enable::INTT_CELL && true;
  Enable::INTT_QA				= Enable::INTT_CLUSTER && Enable::QA && true;


  Enable::TRACKING_TRACK			= (Enable::MICROMEGAS_CLUSTER
						   && Enable::TPC_CLUSTER
						   && Enable::INTT_CLUSTER
						   && Enable::MVTX_CLUSTER) && false;

  Enable::BLACKHOLE				= true;
  G4MAGNET::magfield_rescale = 0.0;  // for zero field

  // Initialize the selected subsystems
  G4Init();

  // GEANT4 Detector description
  if (!Input::READHITS)
    {
      G4Setup();
    }

  // Detector Division //
  if (Enable::INTT_CELL) Intt_Cells();

  // SVTX tracking
  TrackingInit(); // necessary for ActsGeometry
  if (Enable::INTT_CLUSTER) Intt_Clustering();

  /*
  InttClusterizer* inttclusterizer = new InttClusterizer("InttClusterizer", G4MVTX::n_maps_layer, G4MVTX::n_maps_layer + G4INTT::n_intt_layer - 1);
  //  inttclusterizer->Verbosity(verbosity);
  // no Z clustering for Intt type 1 layers (we DO want Z clustering for type 0 layers)
  // turning off phi clustering for type 0 layers is not necessary, there is only one strip
  // per sensor in phi
  for (int i = G4MVTX::n_maps_layer; i < G4MVTX::n_maps_layer + G4INTT::n_intt_layer; i++)
  {
    if (G4INTT::laddertype[i - G4MVTX::n_maps_layer] == PHG4InttDefs::SEGMENTATION_PHI)
    {
      inttclusterizer->set_z_clustering(i, false);
    }
  }
  se->registerSubsystem(inttclusterizer);
  */

  /////////////////////

  AnalysisInttCosmicCommissioning* intt_cosmic = new AnalysisInttCosmicCommissioning("AnalysisInttCosmicCommissioning",outputFile);

  intt_cosmic->SetData( inputFile );
  se->registerSubsystem( intt_cosmic );

//  QaInttCosmicCommissioning* qicc = new QaInttCosmicCommissioning();
//  qicc->SetData( inputFile );
//  se->registerSubsystem( qicc );

  se->skip(skip);
  se->run(nEvents);
  se->End();

  delete se;

  gSystem->Exit(0);
  return 0;
}
