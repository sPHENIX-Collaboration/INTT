#include "Fun4All_CosmicAnalysis.hh"

//const string &inputFile = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/cosmic/DST_creation/new_DST_cosmics_intt_00035692.root", 
//const string &inputFile = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/cosmic/NEW_DST_creation/test_clustering_DST_cosmics_intt_00038554_10000events.root", 
//const string &inputFile = "test_clustering_DST_cosmics_intt_00038554_10000events.root", 
int Fun4All_CosmicAnalysis(
			   int run_num, 
			   int nEvents = 10000, //5,
			   const int skip = 0
			   )
{
  
  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(0);
  
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

  string run_num_str = GetRunNum8digits( run_num ); // string( 8 - to_string(run_num).size(), '0' ) + to_string( run_num );

  // DSTs are stored in "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/{year}"
  // A DST to be fed to this macro is supposed to
  // - have Trkr_Cluster
  // - have no hot channel
  // File name format: DST_cosmics_intt_{8-digit run number}_no_hot_clusterinzed.root
  // for exmaple: DST_cosmics_intt_00038540_no_hot_clusterinzed.root
  string inputFile = kIntt_dst_dir
    + "DST_cosmics_intt_" + run_num_str
    + "_no_hot_clusterized.root";

  cout << inputFile << endl;
  
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

  /////////////////////
  //const string &outputFile = "cluster_result_00038554_4point_only.root",
  //  const string kIntt_qa_cosmics_dir = kIntt_qa_dir + "cosmics/";
  // string outputFile = kIntt_qa_cosmics_dir + "root/"
  //   + "cosmics_intt_" + run_num_str + ".root";

  string output_name = "cosmics_intt_" + run_num_str + ".root";
  AnalysisInttCosmicCommissioning* intt_cosmic
    = new AnalysisInttCosmicCommissioning("AnalysisInttCosmicCommissioning", output_name );
  
  //  intt_cosmic->SetData( inputFile );
  intt_cosmic->SetOutputPath( kIntt_qa_cosmics_dir );
  se->registerSubsystem( intt_cosmic );
  
  //  QaInttCosmicCommissioning* qicc = new QaInttCosmicCommissioning();
  //  qicc->SetData( inputFile );
  //  se->registerSubsystem( qicc );

  se->skip(skip);
  se->run(nEvents);
  se->End();

  cout << "  Input:  " << inputFile << endl;
  cout << "  Output: " << intt_cosmic->GetOutputRoot() << endl;
  cout << "  Output PDF: " << intt_cosmic->GetOutputPdf() << endl;
  delete se;

  gSystem->Exit(0);
  return 0;
}
