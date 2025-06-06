#pragma once

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

//#include <inttread/InttSaveTree.h>

#include <phool/recoConsts.h>

#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <G4Setup_sPHENIX.C>

#include <intt/InttCombinedRawDataDecoder.h>
#include <intt/InttMapping.h>

//#include <inttread/InttRawData.h>
//#include <inttread/InttSaveTree.h>

#include "functions.hh"
#include "constant_values.hh"

#include <InttMbdv2/InttMbdv2.h>

// #include <intt/InttClusterizer.h>

// #include <mbd/MbdReco.h>

// #include <evtidreporter/EvtIDReporter.h>

// R__LOAD_LIBRARY(libfun4all.so)
// R__LOAD_LIBRARY(libfun4allraw.so)
// R__LOAD_LIBRARY(libffarawmodules.so)


/////////////////////////////////////////////////////////////////
// note : for clustering in Z axis
#include <GlobalVariables.C>

#include <G4_ActsGeom.C>
#include <G4_TrkrVariables.C>
#include <intt/InttClusterizer.h>
/////////////////////////////////////////////////////////////////


