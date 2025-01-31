#include "Fun4All_Intt_HitMapEvt.hh"

R__LOAD_LIBRARY(libINTTHitMapEvt.so)
// R__LOAD_LIBRARY(libdNdEtaINTT.so)
// R__LOAD_LIBRARY(libmbd.so)
// R__LOAD_LIBRARY(libEvtIDReporter.so)

std::vector<std::tuple<long long, int, int>> PrepareCheckHL(std::string directory_in)
{
  TFile * file_in = TFile::Open(directory_in.c_str());

  if (file_in == nullptr)
  {
    std::cout << "Error: file_in can not be opened" << std::endl;
    exit(1);
  }

  TTree * tree_in = (TTree*)file_in->Get("tree");
  long long in_INTT_bco_full;
  int in_FELIX;
  int in_FELIX_ch;
  int in_chip_id;
  int in_nHits;

  tree_in->SetBranchAddress("INTT_bco_full", &in_INTT_bco_full);
  tree_in->SetBranchAddress("FELIX", &in_FELIX);
  tree_in->SetBranchAddress("FELIX_ch", &in_FELIX_ch);
  tree_in->SetBranchAddress("chip_id", &in_chip_id);
  tree_in->SetBranchAddress("nHits", &in_nHits);

  std::vector<std::tuple<long long, int, int>> out_vec;

  for (int i = 0; i < tree_in->GetEntries(); i++)
  {
    tree_in->GetEntry(i);
    std::tuple<long long, int, int> temp_tuple = std::make_tuple(in_INTT_bco_full, in_FELIX, in_FELIX_ch);
    out_vec.push_back(temp_tuple);
  }

  return out_vec;

}

void Fun4All_Intt_HitMapEvt(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string input_HLcheck_file = "aaa.root", // note: full directory
  
  // todo : modify here
  string hot_channel_full_file_directory = "/sphenix/user/jaein213/macros/inttcalib/hotmap_cdb_1031/hotmap_run_00054280.root",
  int bco_diff_peak = 55,
  
  bool apply_hot_channel = false,
  bool apply_bco_diff = false,
  bool hitQA_check = true,
  bool clone_hit_remove_BCO = true,
  bool setADCinZaxis_tag = true
)
{

  //  gSystem->ListLibraries();
  TStopwatch* watch = new TStopwatch();
  watch->Start();
  
  Fun4AllServer* se = Fun4AllServer::instance();

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
  in->Verbosity(2);
  in->AddFile( input_directory + "/" + input_filename); 
  // in->AddListFile( input_directory + "/" + input_filename );
  se->registerInputManager(in);

  INTTHitMapEvt * IHM = new INTTHitMapEvt (
    "INTTHitMapEvt",
    process_id,
    run_num,
    output_directory,
    apply_hot_channel,
    hot_channel_full_file_directory,
    apply_bco_diff,
    bco_diff_peak,
    hitQA_check,
    clone_hit_remove_BCO,
    setADCinZaxis_tag
  );

  // IHM -> SetCheckEventBCOFULLs(checked_event);
  IHM -> SetCheckEventBCOFULLsHalfLadder(PrepareCheckHL(input_HLcheck_file));
  
  string final_output_file_name = IHM->GetOutputFileName();

  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  se->registerSubsystem(IHM);

  se->run(nevents);
  se->End();

  watch->Stop();
  cout << "----------------------------------" << endl;
  cout << "Real time: " << watch->RealTime() << endl;
  cout << "CPU time:  " << watch->CpuTime() << endl;  

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  delete se;






  // ////////////////////////////////////////////////////////////////////////
  // string run_type = "physics";
  // string run_num_str = string( "000" ) + to_string( run_num );

  // string list_file = string( "list_run" ) + to_string( run_num ) + ".txt";
  // string list_file_calo = "list_run54280_calo.txt";
  
  // ////////////////////////////////////////////////////////////////////////
  // // Config for input/output files
  // ////////////////////////////////////////////////////////////////////////

  // // output_base format: {run_tpye}_intt_{run number}
  // // For example, cosmics_intt_01234567
  // string output_base = string("_") + run_type + "_intt-" + run_num_str;  
  // string output_dst = "DST" + output_base;

  // if (apply_hot_channel) output_dst += "_HotChannel";
  // if (apply_bco_cut)     output_dst += "_BCOCut";
  // output_dst += "_CaloCombined";
  // output_dst +=  ".root"; 

  // // string  cdb_hot_list = kIntt_cdb_dir + "cdb_49737_special.root";
  // // cdb_hot_list = kIntt_cdb_dir + "cdb_00050377_special.root";
  // // cout << "Hot channel CDB is forced to be " << cdb_hot_list  << endl;
  // string  cdb_hot_list = "/sphenix/user/jaein213/macros/inttcalib/hotmap_cdb/hotmap_run_000"+to_string(run_num)+".root";

  // // string cdb_bco = kIntt_cdb_dir + "bco_diff_map/"
  // //   + "cdb_bco_" + run_num_str + "_official" + ".root";
  
  // string cdb_bco = "/sphenix/user/jaein213/macros/inttcalib/bcomap_cdb/bcomap_run_000"+to_string(run_num)+".root"; 
  
  // string cdbtree_name_dac = kIntt_cdb_dir
  //   + "dac_map/"
  //   //  + "cdb_intt_dac_30_45_60_90_120_150_180_210.root";
  //   + "cdb_intt_dac_35_45_60_90_120_150_180_210_streaming.root";
  
  // Fun4AllServer* se = Fun4AllServer::instance();
  // //se->Verbosity(5);

  // // just if we set some flags somewhere in this macro
  // recoConsts *rc = recoConsts::instance();
  
  // Enable::CDB = true;
  // rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // // 64 bit timestamp
  // rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  // rc->set_IntFlag("RUNNUMBER", run_num );
  
  // cout<<"test test, CDB_GLOBALTAG: "<<CDB::global_tag<<endl;

  // //--input
  // Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
  // in->Verbosity(2);
  // in->AddListFile( list_file ); 
  // // in->AddListFile( list_file_calo );
  // se->registerInputManager(in);

  // Fun4AllInputManager *in_calo = new Fun4AllDstInputManager("DST_calo");
  // in_calo->Verbosity(2);
  // in_calo->AddListFile( list_file_calo );
  // se->registerInputManager(in_calo);
     
  // //Enable::BBC = true;
  // Enable::MVTX = true;
  // Enable::INTT = true;
  // Enable::TPC = true;
  // Enable::MICROMEGAS = true;
  // G4Init();
  // G4Setup();

  // TrackingInit();

  // //////////////////////////////////////
  // InttCombinedRawDataDecoder* inttdecode = new InttCombinedRawDataDecoder();
  // //  inttdecode->Verbosity(1);
  // inttdecode->runInttStandalone( true );
  // inttdecode->writeInttEventHeader( true );
  // inttdecode->set_triggeredMode( true );
  

  // if (apply_hot_channel) inttdecode->LoadHotChannelMapLocal( cdb_hot_list );

  // if( apply_bco_cut ) // BCO diff selection to get only hits from triggered collision
  //   inttdecode->SetCalibBCO( cdb_bco, InttCombinedRawDataDecoder::FILE); 

  // inttdecode->SetCalibDAC( cdbtree_name_dac, InttCombinedRawDataDecoder::FILE ); // not InttCombinedRawDataDecoder::CDB
  // //inttdecode->set_fphxBcoFilter( fphx_bco );
  // se->registerSubsystem( inttdecode );
  
  // //////////////////////////////////////
  // //Intt_Cells();
  // Intt_Clustering();


  // //////////////////MBD////////////////////
  // // MbdReco *mbdreco = new MbdReco();
  // // se->registerSubsystem(mbdreco);
  // //////////////////MBD////////////////////
  


  // // EvtIDReporter * evtridreporter = new EvtIDReporter("evtridreporter", 4000);
  // // se->registerSubsystem(evtridreporter);

  // dNdEtaINTT * dndetaintt = new dNdEtaINTT("dndetaintt", Form("%s/Trkr_%s",output_directory.c_str(), output_dst.c_str()), true);
  // se->registerSubsystem(dndetaintt);


  // //output
  // Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", output_directory + "/" + output_dst );
  // se->registerOutputManager(out);

  // se->run(nevents);
  // se->End();

  // cout << "CDB (hot channel) " << cdb_hot_list << endl;
  // cout << "CDB (BCO diff) " << cdb_bco << endl;
  // cout << "CDB (DAC map)    " << cdbtree_name_dac << endl;
  // cout << "FPHX BCO: " << fphx_bco << endl;
  // cout << "Output: " << output_dst << endl;

  // watch->Stop();
  // cout << "----------------------------------" << endl;
  // cout << "Real time: " << watch->RealTime() << endl;
  // cout << "CPU time:  " << watch->CpuTime() << endl;  
  // delete se;
}
