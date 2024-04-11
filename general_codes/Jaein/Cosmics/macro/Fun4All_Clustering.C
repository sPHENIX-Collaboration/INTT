#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <GlobalVariables.C>

#include <G4Setup_sPHENIX.C>
#include <G4_Input.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
// #include <Trkr_LaserClustering.C>
// #include <Trkr_Reco.C>
// #include <Trkr_Eval.C>
// #include <Trkr_QA.C>

// #include <Trkr_Diagnostics.C>
// #include <G4_User.C>
// #include <QA.C>

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

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <intt/InttClusterizer.h>
#include <InttEventInfov1.h>
#include <InttEventInfo.h>

#include <trackingdiagnostics/TrackResiduals.h>
#include <trackingdiagnostics/TrkrNtuplizer.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtrack_io.so)
R__LOAD_LIBRARY(libTrackingDiagnostics.so)

int Fun4All_Clustering(
		     int run_num = 38554,
		     int nEvents = 10000, //5,
         const int skip = 0
		     )
{
  std::string inputFile = "test_DST_cosmics_intt_000"+std::to_string(run_num)+"_"+std::to_string(nEvents)+"events.root";
  std::string outputFile = "test_clustering_DST_cosmics_intt_000"+std::to_string(run_num)+"_"+std::to_string(nEvents)+"events.root";
  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(0);
  
  recoConsts *rc = recoConsts::instance();
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

  Enable::CDB					= true;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);


  //////////////////////////////
  Enable::MVTX					= true;
  Enable::INTT					= true;
  Enable::TPC					= true;
  Enable::MICROMEGAS				= true;
  Enable::BLACKHOLE				= true;

  /////////////////
  // Magnet Settings
  /////////////////
  G4MAGNET::magfield_rescale = 0.0;  // for zero field

  // Initialize the selected subsystems
  ///////////////////////
  // GEANT4 Detector description
  ///////////////////////
  G4Init();
  G4Setup();

  //  if ((Enable::MBD && Enable::MBDRECO) || Enable::MBDFAKE) Mbd_Reco();
  //if (Enable::INTT_CELL) Intt_Cells();

  //////////////////////////////////
  // CEMC towering and clustering //
  //////////////////////////////////
  // if (Enable::CEMC_TOWER) CEMC_Towers();
  // if (Enable::CEMC_CLUSTER) CEMC_Clusters();

  /////////////////
  // SVTX tracking
  //////////////--

  TrackingInit(); // necessary for ActsGeometry

  // if(Enable::TRACKING_TRACK)
  //   {
  //     TrackingInit();
  //   }
  Intt_Clustering();
  // InttCombinedRawDataDecoder *inttdecode = new InttCombinedRawDataDecoder();
  // inttdecode->runInttStandalone(true);
  // inttdecode->writeInttEventHeader(true);
  //  inttdecode->LoadHotChannelMapLocal("/sphenix/tg/tg01/commissioning/INTT/work/jaein/cosmic/CDB/InttHotDeadMap_cdb-00035692_30.root");
  // se->registerSubsystem(inttdecode);
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
    std::cout<<"HELLO 7"<<std::endl;
    */

  // TString ntupoutfile =  "cosmic_35692_ntup.root";
  // std::string ntupstring(ntupoutfile.Data());

  // auto ntp = new TrkrNtuplizer("TrkrNtuplizer", ntupstring);
  //  ntp->do_cluster_eval(true);
  //  se->registerSubsystem(ntp);

  
  std::string o_file = outputFile;
  Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", o_file.c_str());
	se->registerOutputManager(out);
  /////////////////////

 // se->skip(skip);
  se->run(nEvents);
  se->End();
  delete se;

  gSystem->Exit(0);
  return 0;
}
