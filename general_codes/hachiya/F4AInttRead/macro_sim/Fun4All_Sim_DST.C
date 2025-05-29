#ifndef FUN4ALL_INTT_ZVERTEX_C
#define FUN4ALL_INTT_ZVERTEX_C


////////////
#include <G4_ActsGeom.C>
#include <G4_Magnet.C>
#include <GlobalVariables.C>
//#include <QA.C>
#include <Trkr_Clustering.C>
//#include <Trkr_Reco_Cosmics.C>

//#include <Trkr_Reco.C>
//#include <Trkr_RecoInit.C>
//#include <G4Setup_sPHENIX.C>

#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

//#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <string>

#include <inttxyvertexfinder/InttZVertexFinder.h>
#include <inttxyvertexfinder/InttXYVertexFinder.h>

#include <inttread/InttAna.h>
#include <inttread/InttDisp.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libmvtx.so)
R__LOAD_LIBRARY(libintt.so)
R__LOAD_LIBRARY(libtpc.so)
R__LOAD_LIBRARY(libmicromegas.so)

R__LOAD_LIBRARY(libInttXYVertexFinder.so)
R__LOAD_LIBRARY(libinttread.so)
////////////

//--#include <intt/InttMapping.h>



Fun4AllServer* se = nullptr;


void Fun4All_Sim_DST(int nevents=5001 //1001, 
//       , std::string i_file = "SimDST_Upsilon.root"
//       , std::string i_file = "SimDST_Single_jpsi.root"
       , std::string i_file = "G4sPHENIX.root"
//       , std::string i_file = "SimDST_Single_pip_100.root"
//       , std::string i_file = "SimDST_Single_ep_100.root"
//       , std::string i_file = "SimDST_Single_k0s.root"
       , std::string o_file = "AnaTutorial_run21048.root"
       )
{
        gSystem->ListLibraries();

	se = Fun4AllServer::instance();
	//se->Verbosity(5);

        // just if we set some flags somewhere in this macro
        recoConsts *rc = recoConsts::instance();
        //rc->set_IntFlag("RUNNUMBER",40741);

        Enable::CDB = true;
        // global tag
        rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
        //
        // 64 bit timestamp
        rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  
	//--input
	Fun4AllInputManager *in = new Fun4AllDstInputManager("DST");
        in->fileopen(i_file.c_str());
	//in->Verbosity(2);
	se->registerInputManager(in);

        Enable::CDB = true;
        //rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
        //rc->set_uint64Flag("TIMESTAMP", 6);
        std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

        Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
        ingeo->AddFile(geofile);
        se->registerInputManager(ingeo);

        G4TPC::tpc_drift_velocity_reco = (8.0 / 1000) * 107.0 / 105.0;
        //G4MAGNET::magfield = "0.01";
        G4MAGNET::magfield_rescale = 1;
        ACTSGEOM::ActsGeomInit();
     


        InttAna *inttana = new InttAna("InttAna", "inttana.root");
        inttana->Verbosity(10);
        inttana->setBeamCenter( 0.0, 0.0);
//        inttana->setBeamCenter(-0.0188185, 0.198181); // 1st 10k version
        //inttana->setBeamCenter(-0.0188185, 0.1);
//        inttana->setInttRawData(inttraw);
        inttana->useSimVtx(true);
        se->registerSubsystem(inttana);

        InttDisp *disp = new InttDisp("InttDisp");
        disp->setAnaModule(inttana);
        se->registerSubsystem(disp);

	//output
	Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", o_file.c_str());
	//se->registerOutputManager(out);

	se->run(nevents);
//	se->End();

//	delete se;
}

#endif//FUN4ALL_INTT_RECO_C
