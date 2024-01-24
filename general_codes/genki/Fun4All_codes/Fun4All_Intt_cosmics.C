#include "Fun4All_Intt_cosmics.hh"

string GetRunNum( int run, int digits = 8 )
{
  stringstream run_string; // 00026123
  run_string << setfill( '0' ) << setw( digits ) << run ;
  return run_string.str();
}

int Fun4All_Intt_cosmics(
			 int nEvents = 1, //5,
			 //const string &inputFile = "https://www.phenix.bnl.gov/WWW/publish/phnxbld/sPHENIX/files/sPHENIX_G4Hits_sHijing_9-11fm_00000_00010.root",
			 int run = 25926, // 26960,
			 //			 const string &inputFile = "/sphenix/tg/tg01/commissioning/INTT/merged_root_files/cosmics_inttall-00026960-0000_event_base_ana.root",
			 //			 const string &outputFile = "INTT_cosmics.root",
			 //		    const string &embed_input_file = "https://www.phenix.bnl.gov/WWW/publish/phnxbld/sPHENIX/files/sPHENIX_G4Hits_sHijing_9-11fm_00000_00010.root",
			 bool use_mask = false,
			 const int skip = 0
			 //			 const string &outdir = "."
			 )
{
  //  gSystem->ListLibraryies();
  const string kInput_dir = "/sphenix/tg/tg01/commissioning/INTT/merged_root_files/";
  string input_name = "cosmics_inttall-" + GetRunNum( run ) + "-0000_event_base_ana.root";
  const string inputFile = kInput_dir + input_name;

  string outputFile = string("./results/") + "run" + GetRunNum( run );
  if( use_mask )
    outputFile += "_no_noise.root";
  else
    outputFile += ".root";

  cout << "Input:" << inputFile << endl;
  cout << "Output: " << outputFile << endl;
  if( nEvents < 0 )
    {
      TFile* tf = new TFile( inputFile.c_str(), "READ" );
      nEvents = ((TTree*)tf->Get( "tree" ))->GetEntries();
      cout << nEvents << endl;
      tf->Close();
      delete tf;
      //      return 0;
    }

  if( use_mask )
    {
      cerr << "Currently, removing noisy channels is not good idea somehow..." << endl;
      return 0;
    }

  Fun4AllServer *se = Fun4AllServer::instance();
  //  se->Print("NODETREE"); // useless
  se->Verbosity(0);

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

  //===============
  // Input options
  //===============
  // verbosity setting (applies to all input managers)
  Input::VERBOSITY = 0;
  // First enable the input generators
  // Either:
  // read previously generated g4-hits files, in this case it opens a DST and skips
  // the simulations step completely. The G4Setup macro is only loaded to get information
  // about the number of layers used for the cell reco code
  //Input::READHITS = true;
  INPUTREADHITS::filename[0] = inputFile;
  
  // if you use a filelist
  // INPUTREADHITS::listfile[0] = inputFile;
  // Or:
  // Use particle generator
  // And
  // Further choose to embed newly simulated events to a previous simulation. Not compatible with `readhits = true`
  // In case embedding into a production output, please double check your G4Setup_sPHENIX.C and G4_*.C consistent with those in the production macro folder
  // E.g. /sphenix/sim//sim01/production/2016-07-21/single_particle/spacal2d/
  //  Input::EMBED = true;
  //  INPUTEMBED::filename[0] = embed_input_file;
  // if you use a filelist
  //INPUTEMBED::listfile[0] = embed_input_file;

  //////////////////-
  // Initialize the selected Input/Event generation
  //////////////////-
  // This creates the input generator(s)
  InputInit();

  //////////////--
  // Set generator specific options
  //////////////--
  // can only be set after InputInit() is called

  // register all input generators with Fun4All
  InputRegister();

  if (! Input::READHITS)
    {
      rc->set_IntFlag("RUNNUMBER",1);

      SyncReco *sync = new SyncReco();
      se->registerSubsystem(sync);

      HeadReco *head = new HeadReco();
      se->registerSubsystem(head);
    }
  // Flag Handler is always needed to read flags from input (if used)
  // and update our rc flags with them. At the end it saves all flags
  // again on the DST in the Flags node under the RUN node
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);
  
  // set up production relatedstuff
  //   Enable::PRODUCTION = true;

  //======================
  // Write the DST
  //======================

  //Enable::DSTOUT = true;
  Enable::DSTOUT_COMPRESS = false;
  DstOut::OutputDir = "./results/";
  DstOut::OutputFile = outputFile;

  //Option to convert DST to human command readable TTree for quick poke around the outputs
  //  Enable::DSTREADER = true;

  // turn the display on (default off)
  //Enable::DISPLAY = true;

  //======================
  // What to run
  //======================

  // Global options (enabled for all enables subsystems - if implemented)
  //  Enable::ABSORBER				= true;
  //  Enable::OVERLAPCHECK			= true;
  //  Enable::VERBOSITY				= 1;

  // Enable::PIPE					= false; // true;
  // Enable::PIPE_ABSORBER				= false; // true;

  Enable::MVTX					= true;
  Enable::TPC                                   = true;
  Enable::MICROMEGAS				= true;
  
  Enable::INTT					= true;
  //  Enable::INTT_ABSORBER			= true; // enables layerwise support structure readout
  //  Enable::INTT_SUPPORT			= true; // enable global support structure readout
  Enable::INTT_CELL				= Enable::INTT && true;
  Enable::INTT_CLUSTER				= Enable::INTT_CELL && true;
  Enable::INTT_QA				= Enable::INTT_CLUSTER && Enable::QA && true;

  // Enable::MAGNET				= true;
  // Enable::MAGNET_ABSORBER			= true;

  //  Enable::BEAMLINE				= true;
  //  Enable::BEAMLINE_ABSORBER			= true;  // makes the beam line magnets sensitive volumes
  //  Enable::BEAMLINE_BLACKHOLE		= true; // turns the beamline magnets into black holes

  // new settings using Enable namespace in GlobalVariables.C
  Enable::BLACKHOLE				= true;
  //Enable::BLACKHOLE_SAVEHITS			= false; // turn off saving of bh hits
  //Enable::BLACKHOLE_FORWARD_SAVEHITS		= false; // disable forward/backward hits
  //BlackHoleGeometry::visible			= true;

  // run user provided code (from local G4_User.C)
  //Enable::USER				= true;

  /////////////////////////
  // conditions DB flags //
  /////////////////////////
  Enable::CDB					= true;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  
  /////////////////
  // World Settings
  /////////////////
  //  G4WORLD::PhysicsList			= "FTFP_BERT"; //FTFP_BERT_HP best for calo
  //  G4WORLD::WorldMaterial			= "G4_AIR"; // set to G4_GALACTIC for material scans

  /////////////////
  // Magnet Settings
  /////////////////
  //  G4MAGNET::magfield =  string(getenv("CALIBRATIONROOT"))+ string("/Field/Map/sphenix3dbigmapxyz.root");  // default map from the calibration database
  //  G4MAGNET::magfield = "1.5"; // alternatively to specify a constant magnetic field, give a float number, which will be translated to solenoidal field in T, if string use as fieldmap name (including path)
  //  G4MAGNET::magfield_rescale = 1.;  // make consistent with expected Babar field strength of 1.4T
  G4MAGNET::magfield_rescale = 0.0;  // for zero field

  /////////////////
  // Pythia Decayer
  /////////////////
  // list of decay types in
  // $OFFLINE_MAIN/include/g4decayer/EDecayType.hh
  // default is All:
  // G4P6DECAYER::decayType = EDecayType::kAll;
  
  // Initialize the selected subsystems
  G4Init();
  
  //////////////////////-
  // GEANT4 Detector description
  //////////////////////-
  if (!Input::READHITS)
    {
      G4Setup();
    }
  

  //////////////////--
  // Detector Division
  //////////////////--

  if (Enable::INTT_CELL) Intt_Cells();

  const string kMask_dir = "/sphenix/tg/tg01/commissioning/INTT/work/genki/analysis/cosmic/tracking/results/";
  string mask_file = "run" + to_string( run ) + "_noisy_ch_list_takashi_format.txt";
  string mask_path = kMask_dir + mask_file;
  
  // "/sphenix/tg/tg01/commissioning/INTT/merged_root_files/cosmics_inttall-00026960-0000_event_base_ana.root"
  InttRawData *inttraw = new InttRawData("Inttraw", inputFile );
  inttraw->Verbosity( 0 );
  inttraw->SetRunNumber( run );

  if( use_mask )
    {
      cout << "INTT Mask file: " << mask_path << endl;
      inttraw->SetHotMapFile( mask_path.c_str() );
    }
  
  se->registerSubsystem(inttraw);
  
  //InttDummyData *inttdummy = new InttDummyData("InttDummy");
  //se->registerSubsystem(inttdummy);
  
  //////////////--
  // SVTX tracking
  //////////////--
  TrackingInit();
  if (Enable::INTT_CLUSTER) Intt_Clustering();

  //////////////////-
  // Global Vertexing
  //////////////////-
   // if (Enable::GLOBAL_RECO && Enable::GLOBAL_FASTSIM)
   // {
   //   cout << "You can only enable Enable::GLOBAL_RECO or Enable::GLOBAL_FASTSIM, not both" << endl;
   //   gSystem->Exit(1);
   // }
   // if (Enable::GLOBAL_RECO)
   // {
   //   Global_Reco();
   // }
   // else if (Enable::GLOBAL_FASTSIM)
   // {
   //   Global_FastSim();
   // }
    
  //////////////////////
  // Simulation evaluation
  //////////////////////
  // string outputroot = outputFile;
  // string remove_this = ".root";
  // size_t pos = outputroot.find(remove_this);
  // if (pos != string::npos)
  //   {
  //     outputroot.erase(pos, remove_this.length());
  //   }
  
  // if (Enable::USER) UserAnalysisInit();

  // InttAna *inttAna = new InttAna("Inttana");
  // inttAna->setBeamCenter(0.0, -0.5);
  //  inttAna->setBeamCenter(0.0,  0.0);
  //  se->registerSubsystem(inttAna);

  InttAnalysisCosmic* iac = new InttAnalysisCosmic( "name", outputFile );
   se->registerSubsystem( iac );
  
  //////////////////////
  // Standard QAs
  //////////////////////
  //  if (Enable::INTT_QA) Intt_QA();

  //////////////
  // Set up Input Managers
  //////////////

  InputManagers();
  //Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
  //in->Verbosity(Input::VERBOSITY);
  //se->registerInputManager(in);

  //  if (Enable::PRODUCTION)
  //  {
  //    Production_CreateOutputDir();
  //  }
  //
  //  if (Enable::DSTOUT)
  //  {
  //    string FullOutFile = DstOut::OutputDir + "/" + DstOut::OutputFile;
  //    Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", FullOutFile);
  //    if (Enable::DSTOUT_COMPRESS)
  //    {
  //      ShowerCompress();
  //      DstCompress(out);
  //    }
  //    se->registerOutputManager(out);
  //  }
  
  //////////////////////
  // Event processing //
  //////////////////////
  //  // if we use a negative number of events we go back to the command line here
  //  if (nEvents < 0)
  //  {
  //    return 0;
  //  }
  //  // if we run the particle generator and use 0 it'll run forever
  //  // for embedding it runs forever if the repeat flag is set
  //  if (nEvents == 0 && !Input::HEPMC && !Input::READHITS && INPUTEMBED::REPEAT)
  //  {
  //    cout << "using 0 for number of events is a bad idea when using particle generators" << endl;
  //    cout << "it will run forever, so I just return without running anything" << endl;
  //    return 0;
  //  }

  se->skip(skip);
  se->run(nEvents);

  ///////////////
  // QA output //
  ///////////////
  //if (Enable::QA) QA_Output(outputroot + "_qa.root");

  //////////
  // Exit //
  //////////
  //  CDBInterface::instance()->Print(); // print used DB files
  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  
  //  if (Enable::PRODUCTION)
  //  {
  //    Production_MoveOutput();
  //  }

  gSystem->Exit(0);
  return 0;
}
