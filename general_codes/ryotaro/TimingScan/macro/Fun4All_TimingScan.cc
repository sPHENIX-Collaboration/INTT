#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include "InttTimingScan.h"

//#include <phool/recoConsts.h>
//#include <GlobalVariables.C>

R__LOAD_LIBRARY(libInttTimingScan.so)
R__LOAD_LIBRARY( libg4detectors.so )

void Fun4All_TimingScan(int process_id=0, int run=43417, int n_events=1000){

  std::string job_index = std::to_string( process_id );
  int job_index_len = 5;
  job_index.insert(0, job_index_len - job_index.size(), '0');

  Fun4AllServer* fun4all_server = Fun4AllServer::instance();

  Fun4AllInputManager* fun4all_input_manager = new Fun4AllDstInputManager("hoge");
  fun4all_input_manager->Verbosity(2);
  std::ostringstream run_padded_with0;
  run_padded_with0 << std::setw( 8 ) << std::setfill( '0' ) << run; // convert the run (say, 45751) to run_padded_with0 (= 00045751)
  std::string filename; 

  if (run == 66522) {
    filename = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2025/DST_physics_intt-%s_no_hot_special.root",run_padded_with0.str().c_str()); /* for run 66522 (it was taken in physics)*/
    fun4all_input_manager->AddFile(filename); 
    fun4all_server->registerInputManager(fun4all_input_manager);
  } else if (run == 81000) {
    filename = "/sphenix/u/yshojisek/work/repos/TrackingAnalysis/Silicon_MBD_Vertexing/Silicon_MBD_Comparisons/VertexCompare_run_81000/files/DST_SILIVTX.root";
    fun4all_input_manager->AddFile(filename); 
    fun4all_server->registerInputManager(fun4all_input_manager);
  } else if (run == 75519) {
    std::vector<Fun4AllInputManager*> input_managers;
    for (int i = 0; i < 8; i++) {
      // std::string intt_file_name = Form("DST_STREAMING_EVENT_intt%d_run3auau_new_nocdbtag_v001-000%d-%s.root", i, run, job_index.c_str());
      std::string intt_file_name = Form("DST_STREAMING_EVENT_intt%d_run3auau_new_nocdbtag_v001-000%d-00000.root", i, run);
      input_managers.push_back(new Fun4AllDstInputManager(Form("DST_track_%d",i)));
      input_managers.back() -> Verbosity(2);
      input_managers.back() -> AddFile( intt_file_name );
      fun4all_server->registerInputManager(input_managers.back());
    }
  } else if (run == 82400) {
    std::vector<Fun4AllInputManager*> input_managers;
    for (int i = 0; i < 8; i++) {
      std::string intt_file_name = Form("DST_STREAMING_EVENT_intt%d_run3oo_new_nocdbtag_v001-000%d-%s.root", i, run, job_index.c_str());
      input_managers.push_back(new Fun4AllDstInputManager(Form("DST_track_%d",i)));
      input_managers.back() -> Verbosity(2);
      input_managers.back() -> AddFile( intt_file_name );
      fun4all_server->registerInputManager(input_managers.back());
    }
  } else if (run == 82391) {
    filename = "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/jaein_decode_calib/test_DST_calib_intt-00082391_special_.root";
    fun4all_input_manager->AddFile(filename); 
    fun4all_server->registerInputManager(fun4all_input_manager);
  } else {
    filename = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2025/DST_beam_intt-%s_no_hot_special.root",run_padded_with0.str().c_str()); /* for other runs (they were taken in beams)*/
    fun4all_input_manager->AddFile(filename); 
    fun4all_server->registerInputManager(fun4all_input_manager);
  }


  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  //Enable::CDB = true;
  //  //  rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
  //rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024" );
  //rc->set_uint64Flag("TIMESTAMP", run);
  //rc->set_IntFlag("RUNNUMBER", run );

  InttTimingScan* module_timingscan = new InttTimingScan("name", run);
  module_timingscan->set_hot_channel_map_file("/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/jaein_decode_calib/cdb_00082391hot.root");
  module_timingscan->set_mode(InttTimingScan::Mode::RawHitOnly); /* RawHitOnly or RawPlusTrkr */
  module_timingscan->set_daq_mode(InttTimingScan::DaqMode::Official); /* Local or Official */
  // module_timingscan->set_mode(Mode::RawPlusTrkr);

  fun4all_server->registerSubsystem(module_timingscan);

  fun4all_server->run(n_events);
  fun4all_server->End();

  delete fun4all_server;

  gSystem->Exit(0);

}
