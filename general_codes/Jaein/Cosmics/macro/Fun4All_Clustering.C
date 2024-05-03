#include "Fun4All_Clustering.hh"

int Fun4All_Clustering(
		     int run_num = 38554,
		     int nEvents = 10000, //5,
		     const int skip = 0
		     )
{
  string run_num_str = string( 8 - to_string(run_num).size(), '0' ) + to_string( run_num );

  std::string inputFile = kIntt_dst_dir
    + "DST_cosmics_intt_"
    + run_num_str;

  if( nEvents != 0 )
    inputFile += "_" + std::to_string(nEvents) + "events";

  inputFile += "_no_hot.root";
  
  std::string outputFile = kIntt_dst_dir
    + "DST_cosmics_intt_"
    + run_num_str ;
  if( nEvents != 0 )
    outputFile += "_" + std::to_string(nEvents) + "events";

  outputFile += "_no_hot_clusterized.root";
  
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

  //if (Enable::INTT_CELL) Intt_Cells();

  /////////////////
  // SVTX tracking
  //////////////--
  TrackingInit(); // necessary for ActsGeometry

  // if(Enable::TRACKING_TRACK)
  //   {
  //     TrackingInit();
  //   }
  Intt_Clustering();
  
  std::string o_file = outputFile;
  Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", o_file.c_str());
  se->registerOutputManager(out);
	/////////////////////

 // se->skip(skip);
  se->run(nEvents);
  se->End();
  delete se;

  cout << "  Input:  " << inputFile << endl;
  cout << "  Output: " << outputFile << endl;

  gSystem->Exit(0);
  return 0;
}
