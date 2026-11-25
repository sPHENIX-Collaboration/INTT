#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

// #include "InttHitCarryoverMonitor.h"
#include "/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/install/include/tutorial/InttHitCarryoverMonitor.h"

R__LOAD_LIBRARY(libInttHitCarryoverMonitor.so)

void Fun4All_HitCarryoverMonitor(int process_id, int run, int n_events, std::string file_path_to_hot_channel_map="/sphenix/user/jaein213/macros/inttcalib_fee/hotmap_cdb/hotmap_cdb_54280_100000_DST_1114.root", bool save_tree=false){
  Fun4AllServer* fun4all_server = Fun4AllServer::instance();

  std::vector<Fun4AllInputManager*> input_managers; 

  std::string job_index = std::to_string( process_id );
  int job_index_len = 5;
  job_index.insert(0, job_index_len - job_index.size(), '0');

  for (int i = 0; i < 8; i++) {
    std::string intt_file_name = Form("DST_STREAMING_EVENT_intt%d_run3auau_new_nocdbtag_v001-000%d-%s.root", i, run, job_index.c_str());
    // std::string intt_file_name = Form("DST_STREAMING_EVENT_intt%d_run3oo_new_nocdbtag_v001-000%d-%s.root", i, run, job_index.c_str());
    input_managers.push_back(new Fun4AllDstInputManager(Form("DST_track_%d",i)));
    input_managers.back() -> Verbosity(2);
    input_managers.back() -> AddFile( intt_file_name );
    fun4all_server->registerInputManager(input_managers.back());
  }

  // std::string intt_file_name = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2025/DST_beam_intt-00079244_special.root"; // = Form("DST_STREAMING_EVENT_intt%d_run3auau_new_nocdbtag_v001-000%d-%s.root", i, run, job_index.c_str());
  // input_managers.push_back(new Fun4AllDstInputManager(Form("DST_track")));
  // input_managers.back() -> Verbosity(2);
  // input_managers.back() -> AddFile( intt_file_name );
  // fun4all_server->registerInputManager(input_managers.back());

  InttHitCarryoverMonitor* module_carryover_monitor = new InttHitCarryoverMonitor("name", run, file_path_to_hot_channel_map, job_index, save_tree);

  std::string output_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d", run);
  std::string final_output_file_name = Form("%d-%s.root", run, job_index.c_str());
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  fun4all_server->registerSubsystem(module_carryover_monitor);
  fun4all_server->run(n_events);
  fun4all_server->End();
  
  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  // delete module_carryover_monitor;
  // for (int i=0; i<8; i++) {
  //   delete input_managers[i]; /* This will cause segmentaion fault. Probably because input_managers will be deleted automatically when we delete fun4all_server.
  // }
  delete fun4all_server;


}