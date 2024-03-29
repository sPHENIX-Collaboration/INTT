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

#include <inttbcofinder/InttBCOFinder.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libinttbcofinder.so)

void Fun4All_Intt_BCOFinder(int nevents = 10000,
                              std::string i_file = "/sphenix/tg/tg01/commissioning/INTT/work/jaein/DataProduction/combined-00020869.root",
                        //      std::string bco_file = "/sphenix/tg/tg01/commissioning/INTT/QA/bco_bcofull_difference/rootfile/2023/ladder_20888_3BCO.root",
                              std::string o_file = "bco_20869_test_fun4all.root",
                              bool adcflag = false
                              )
                              //nevents : # of event used to make hitmap
                              //i_file : merged root file. The file should include event_based TTree structure
                              //bco_file : input BCO file to apply the BCO cut BEFORE making HitMap
                              //o_file : output(hitmap) file
                              //bcoflag : flag to turn ON/OFF BCO cut // true : ON / false : OFF
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

        InttBCOFinder *inttbcofinder = new InttBCOFinder("inttbcofinder",
                                                i_file.c_str(),
                                                o_file.c_str(),
                                                nevents);
        inttbcofinder->ADCCut(adcflag);
//        intthitmap->SetBCOfile(bco_file.c_str());
        se->registerSubsystem(inttbcofinder);

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
