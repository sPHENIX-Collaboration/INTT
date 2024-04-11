#ifndef FUN4ALL_INTT_BCOFINDER_C
#define FUN4ALL_INTT_BCOFINDER_C

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
#include <inttcosmicshotmap/InttCosmicsHotMap.h>
#include "functions.hh"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libinttcosmicshotmap.so)
void Fun4All_Intt_HitMap(int run_num = 38554, int nevents = 10000,
std::string in_file = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/cosmic/NEW_DST_creation/test_DST_cosmics_intt_00038554_10000events.root")
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
        Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
        in->Verbosity(2);
        in->fileopen(in_file);
        se->registerInputManager(in);
        //
        int nevents_hitmap = nevents;
        std::string hitmap_out_file = "hitmap_run" + to_string(run_num) + ".root";
        InttCosmicsHotMap *intthitmap = new InttCosmicsHotMap("inttcosmicshotmap",
                                                              hitmap_out_file.c_str(),
                                                              nevents_hitmap);
        intthitmap->SetBCOcut(false);
        // std::string bco_input_file = "/sphenix/tg/tg01/commissioning/INTT/QA/bco_bcofull_difference/rootfile/2023/ladder_20869_3BCOcut.root";
        // intthitmap->SetBCOFile(bco_input_file.c_str());
        intthitmap->SetFeeMapFile("InttFeeMap.root");
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

#endif // FUN4ALL_INTT_BCOFINDER.C
