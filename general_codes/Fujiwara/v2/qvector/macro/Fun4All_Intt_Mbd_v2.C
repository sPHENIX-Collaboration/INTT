#include "Fun4All_Intt_Mbd_v2.hh"

#include <eventplaneinfo/Eventplaneinfo.h>
#include <eventplaneinfo/EventPlaneReco.h>

#include <ffamodules/CDBInterface.h>

R__LOAD_LIBRARY(libInttMbdv2.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libeventplaneinfo.so)
// R__LOAD_LIBRARY(libmbd.so)
// R__LOAD_LIBRARY(libEvtIDReporter.so)

/////////////////////////////////////////////////////////////////
// note : for clustering in Z axis
R__LOAD_LIBRARY(libintt.so)
/////////////////////////////////////////////////////////////////

void Intt_Clustering_new()
{
  cout<<"test test, can you see me here? "<<endl;

  int verbosity = std::max(Enable::VERBOSITY, Enable::INTT_VERBOSITY);
  Fun4AllServer* se = Fun4AllServer::instance();

  InttClusterizer* inttclusterizer = new InttClusterizer("InttClusterizer", G4MVTX::n_maps_layer, G4MVTX::n_maps_layer + G4INTT::n_intt_layer - 1);
  inttclusterizer->Verbosity(verbosity);
  // no Z clustering for Intt type 1 layers (we DO want Z clustering for type 0 layers)
  // turning off phi clustering for type 0 layers is not necessary, there is only one strip
  // per sensor in phi
  for (int i = G4MVTX::n_maps_layer; i < G4MVTX::n_maps_layer + G4INTT::n_intt_layer; i++)
  {
    if (G4INTT::laddertype[i - G4MVTX::n_maps_layer] == PHG4InttDefs::SEGMENTATION_PHI)
    {
      inttclusterizer->set_z_clustering(i, true);
    }
  }
  se->registerSubsystem(inttclusterizer);
}




void Fun4All_Intt_Mbd_v2( 
  int process_id = 20,
  int run_num = 54280,
  int nevents = 10,
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/mfuji/v2/sandbox/macro",
  string list_file_intt = "list_run54280_intt.txt",
  string list_file_calo = "list_run54280_epd.txt",
  bool apply_hot_channel = true,
  bool apply_bco_cut = true  
)
{

  gSystem->Load("libg4dst.so");
  
  ////////////////////////////////////////////////////////////////////////
  string run_type = "physics";
  int total_run_len = 5;
  string run_num_str = to_string( run_num );
  run_num_str.insert(0, total_run_len - run_num_str.size(), '0');
  
  ////////////////////////////////////////////////////////////////////////
  // Config for input/output files
  ////////////////////////////////////////////////////////////////////////

  string output_base = string("_") + run_type + "_intt-" + run_num_str;  
  string output_dst = "DST" + output_base;

  if (apply_hot_channel) output_dst += "_HotChannel";
  if (apply_bco_cut)     output_dst += "_BCOCut";
  output_dst += "_CaloCombined";

  
  string job_index = to_string( process_id );
  int total_index_len = 5;
  job_index.insert(0, total_index_len - job_index.size(), '0');
  output_dst += "_"+job_index;

  output_dst +=  ".root"; 

  Fun4AllServer* se = Fun4AllServer::instance();
  //se->Verbosity(5);

  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();
  
  Enable::CDB = true;
  const std::string& dbtag= "ProdA_2024";
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",54280);
  
  CDBInterface *cdb = CDBInterface::instance();
  cout <<"INTT_HotMap : "<< cdb->getUrl("INTT_HotMap") << endl;
  cout <<"INTT_BCOMAP : " <<cdb->getUrl("INTT_BCOMAP") << endl;
  cout <<"INTT_DACMAP : " <<cdb->getUrl("INTT_DACMAP") << endl;

  string cdb_hot_list = cdb->getUrl("INTT_HotMap");
  string cdb_bco = cdb->getUrl("INTT_BCOMAP");
  string cdbtree_name_dac = cdb->getUrl("INTT_DACMAP");

  //--input
  Fun4AllInputManager *in_calo = new Fun4AllDstInputManager("DST_calo");
  in_calo->Verbosity(2);
  // in_calo->AddFile( list_file_calo );
  in_calo->AddListFile( list_file_calo );
  //in_calo->Print("ALL");
  se->registerInputManager(in_calo);
  in_calo->Print("ALL");
  
  
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
  in->Verbosity(2);
  //in->AddFile( list_file_intt );
  in->AddListFile( list_file_intt ); 
  // in->AddListFile( list_file_calo );
  //in->Print("ALL");
  se->registerInputManager(in);
  in->Print("ALL");  

  //Enable::BBC = true;
  Enable::MVTX = true;
  Enable::INTT = true;
  Enable::TPC = true;
  Enable::MICROMEGAS = true;
  Enable::EPD = true;
  G4Init();
  G4Setup();

  TrackingInit();

  EventPlaneReco *epreco = new EventPlaneReco();
  epreco->set_sepd_epreco(true);
  se->registerSubsystem(epreco);
  
  //////////////////////////////////////
  
  InttCombinedRawDataDecoder* inttdecode = new InttCombinedRawDataDecoder();
  //  inttdecode->Verbosity(1);
  inttdecode->runInttStandalone( true );
  inttdecode->writeInttEventHeader( true );
  inttdecode->set_triggeredMode( true );
  
  if (apply_hot_channel) inttdecode->LoadHotChannelMapLocal( cdb_hot_list );

  if( apply_bco_cut ) // BCO diff selection to get only hits from triggered collision
    inttdecode->SetCalibBCO( cdb_bco, InttCombinedRawDataDecoder::FILE); 

  inttdecode->SetCalibDAC( cdbtree_name_dac, InttCombinedRawDataDecoder::FILE ); // not InttCombinedRawDataDecoder::CDB
  //inttdecode->set_fphxBcoFilter( fphx_bco );
  se->registerSubsystem( inttdecode );
  
  
  //////////////////////////////////////
  //Intt_Cells();
  Intt_Clustering();
  //Intt_Clustering_new(); // todo : for clustering in Z axis
  

  //////////////////MBD////////////////////
  // MbdReco *mbdreco = new MbdReco();
  // se->registerSubsystem(mbdreco);
  //////////////////MBD////////////////////  
  
   InttMbdv2* inttMbdv2 = new InttMbdv2("InttMbdv2", Form("%s/Trkr_%s",output_directory.c_str(), output_dst.c_str()), true);
  se->registerSubsystem(inttMbdv2);
  
  
  //output
  /*
  Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", output_directory + "/" + output_dst );
  se->registerOutputManager(out);
  //se->Print("ALL");
  */

  se->run(nevents);
  se->End();
  
  delete se;
}
