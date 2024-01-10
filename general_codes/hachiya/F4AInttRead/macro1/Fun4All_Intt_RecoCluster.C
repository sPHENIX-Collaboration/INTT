#ifndef FUN4ALL_INTT_READDST_C
#define FUN4ALL_INTT_READDST_C

#include <intt/InttMapping.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <inttread/InttSaveTree.h>

#include <phool/recoConsts.h>

#include <filesystem>
#include <iostream>
#include <string>

#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <G4Setup_sPHENIX.C>

#include <inttread/InttRawData.h>
#include <inttread/InttSaveTree.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libinttread.so)

void Fun4All_Intt_RecoCluster(int nevents=0, 
                       std::string i_file = "/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00021048-0000_event_base_ana.root",
                       std::string o_file = "AnaTutorial_run21048.root", 
                       std::string dac_file = "dac_21048.txt", 
                       std::string bco_file = "bco_21048.txt", 
                       bool        adc7flag = true, 
                       int run_num=0)
{
	Fun4AllServer* se = Fun4AllServer::instance();
	//se->Verbosity(5);

        // just if we set some flags somewhere in this macro
        recoConsts *rc = recoConsts::instance();

        Enable::CDB = true;
        // global tag
        rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
        //
        // 64 bit timestamp
        rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  
	//--input
	Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
	in->Verbosity(2);
	se->registerInputManager(in);
     
 //
        //Enable::BBC = true;
        //Enable::PIPE = true;
        //Enable::BEAMPIPE = true;
        Enable::MVTX = true;
        Enable::INTT = true;
        Enable::TPC = true;
        Enable::MICROMEGAS = true;
        G4Init();
        G4Setup();

        TrackingInit();

        //////////////////////////////////////
       
	InttRawData *inttraw = new InttRawData("Inttraw",
          //  "/sphenix/lustre01/sphnxpro/commissioning/INTT/tmp/beam_inttall-00020444-0000_event_base_ana.root" // FF
          //  "/sphenix/lustre01/sphnxpro/commissioning/INTT/tmp/beam_inttall-00020864-0000_event_base_ana.root"
          //  "/sphenix/lustre01/sphnxpro/commissioning/INTT/tmp/beam_inttall-00020869-0000_event_base_ana.root" // ZF
          //  "/sphenix/lustre01/sphnxpro/commissioning/INTT/tmp/beam_inttall-00020885-0000_event_base_ana.root" // ZF
          //  "/sphenix/lustre01/sphnxpro/commissioning/INTT/tmp/beam_inttall-00020766-0000_event_base_ana.root"
          //  "/sphenix/lustre01/sphnxpro/commissioning/INTT/tmp/beam_inttall-00023613-0000_event_base_ana.root" // FF
          //"/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00020708-0000_event_base_ana.root"
          //"/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00020869-0000_event_base_ana.root"
          //--"/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00021018-0000_event_base_ana.root"
          //"/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00021019-0000_event_base_ana.root"
          //"/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00021029-0000_event_base_ana.root"
          //"/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00021037-0000_event_base_ana.root"
          //"/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/InttEventSync/beam_inttall-00021048-0000_event_base_ana.root"
          i_file.c_str()
          );
	//inttraw->SetHotMapFile("hotchannel_run25566.txt");
	inttraw->SetDACfile(dac_file.c_str());
	inttraw->SetBCOfile(bco_file.c_str());
	inttraw->SetAdc7RemovalFlag(adc7flag);
	se->registerSubsystem(inttraw);
  



        //////////////////////////////////////

        //Intt_Cells();
        Intt_Clustering();

        //
        InttSaveTree *savetree = new InttSaveTree("InttSaveTree", o_file.c_str());
        savetree->setInttRawData(inttraw);
        se->registerSubsystem(savetree);

	//output
	//Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", o_file.c_str());
	//se->registerOutputManager(out);

	se->run(nevents);
	se->End();

	delete se;
}

#endif//FUN4ALL_INTT_RECO_C
