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

#include <phool/recoConsts.h>

#include <filesystem>
#include <iostream>
#include <string>

#include <G4Setup_sPHENIX.C>

#include <intthitmap/InttHitMap.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libintthitmap.so)

void Fun4All_Intt_HitMap(int nevents = 100,
                              std::string i_file = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/beam_inttall-00020869-0000_event_base_ana.root",
                              std::string bco_file = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/BCOFinder/Felix/rootfile/ladder_20869_3BCO.root",
                              std::string o_file = "hitmap_test_20869.root",
                              bool bcoflag = true
                              )
{
        Fun4AllServer *se = Fun4AllServer::instance();
        // se->Verbosity(5);

        // just if we set some flags somewhere in this macro
        recoConsts *rc = recoConsts::instance();

        Enable::CDB = true;
        // global tag
        rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
        //
        // 64 bit timestamp
        rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);

        //--input
        Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
        in->Verbosity(2);
        se->registerInputManager(in);

        //
        // Enable::BBC = true;
        // Enable::PIPE = true;
        // Enable::BEAMPIPE = true;
        /*    Enable::MVTX = true;
           Enable::INTT = true;
           Enable::TPC = true;
           Enable::MICROMEGAS = true;
           G4Init();
           G4Setup(); 

        TrackingInit(); */

        InttHitMap *intthitmap = new InttHitMap("intthitmap",
                                                i_file.c_str(),
                                                o_file.c_str(),
                                                nevents);
        intthitmap->SetBCOcut(bcoflag);
        intthitmap->SetBCOfile(bco_file.c_str());
        se->registerSubsystem(intthitmap);

        //////////////////////////////////////

        // Intt_Cells();

        //

        // output
        // Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", o_file.c_str());
        // se->registerOutputManager(out);

        se->run(nevents);
        se->End();

        delete se;
}

#endif // FUN4ALL_INTT_RECO_C
