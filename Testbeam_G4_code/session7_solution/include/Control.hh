// *********************************************************
// *                         Mokka                         *
// *    -- A detailed Geant 4 simulation for the ILC --    *
// *                                                       *
// *  polywww.in2p3.fr/geant4/tesla/www/mokka/mokka.html   *
// *********************************************************
//
// $Id: Control.hh,v 1.60 2009/05/13 08:22:57 steve Exp $
// $Name: mokka-07-00 $

#ifndef CONTROL_h
#define CONTROL_h 1
  
#ifndef MOKKA_DEBUG
#define NDEBUG 1
#endif

#include "globals.hh"
#include <vector>
#include <time.h>  
class  VSubDetectorDriver;

#ifdef LCIO_MODE
#include "lcio.h"
  
#include "IO/LCWriter.h"
#include "EVENT/LCIO.h"
#include "DATA/LCFloatVec.h"
  
#include "IMPL/LCEventImpl.h" 
#include "IMPL/LCRunHeaderImpl.h" 
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/MCParticleImpl.h" 
#include "IMPL/LCFlagImpl.h" 
#include "IMPL/LCTOOLS.h"
#include <map>

using namespace lcio;

 

typedef std::map<G4int, MCParticle*> MCParticleMap_type;
typedef std::multimap<std::string,std::pair<std::string,std::string> > LCIOEventParameterMap ;

#endif // LCIO_MODE

#include "G4Version.hh"
#define G4_VERSION_GE( VERSION ) ( G4VERSION_NUMBER >= VERSION )


//fg: define a helper exception function to make Mokka compatible with geant4 9.5
#if G4_VERSION_GE( 950 )
inline void G4Exception(const std::string &message) {

  G4Exception ( message.c_str() ,"MokkaError",FatalException,"we are sorry ..." );

  //void G4Exception(const char* issure, const char* errorCode, G4ExceptionSeverity severity, const char* comments) 
} 
#endif


enum
  {
    TPC = 0, // TPC and inner tracker detectors don't use it.
    ECALENDCAPMINUS,
    ECALBARREL,
    ECALENDCAPPLUS,
    
    HCALENDCAPMINUS,
    HCALBARREL,
    HCALENDCAPPLUS,
    
    PROTOMODULE,
    HODOSCOPE,

    // tbeam module ids --JM 01-14-04
    TBDCH,
    TBSCINT,
    TBHCAL,
    TBECAL,
    TBCATCHER,

    // Guard-ring ids --GM 02-24-06
    GRENDCAPMINUS,
    GRBARREL,
    GRENDCAPPLUS,

    // TB finger counter
    TBFINGCOUNT,

    // TB muon counter
    TBMUONCOUNT,

    // TB veto counter
    TBVETOCOUNT,

    // TB cerenkov counter
    TBCERENKOV,

    // TB vacuum pipe
    TBVACPIPE
  } ;

enum
  {
    MOKKA_EXIT_SUCCESS = 0,
    MOKKA_OTHER_ERRORS,
    MOKKA_ERROR_WRONG_USAGE,
    MOKKA_ERROR_NO_CONNECTION_TO_DATABASE,
    MOKKA_ERROR_DATABASE_SELECT_ERROR,
    MOKKA_ERROR_BAD_DATABASE_PARAMETERS,
    MOKKA_ERROR_MYSQL_SCRIPT_NOT_FOUND,
    MOKKA_ERROR_BAD_GENERATOR_FILENAME,
    MOKKA_ERROR_STDHEP_FILE_NOT_FOUND,
    MOKKA_ERROR_STDHEP_FILE_RAN_OUT_OF_EVENTS,
    MOKKA_ERROR_CANNOT_READ_STDHEP_FILE,
    MOKKA_ERROR_CANNOT_OPEN_GUINEA_PIG_INPUT_FILE,
    MOKKA_ERROR_GUINEA_PIG_INPUT_FILE_ENDED,
    MOKKA_ERROR_CANNOT_OPEN_LCIO_FILE,
    MOKKA_ERROR_CANNOT_WRITE_TO_LCIO_FILE,
    MOKKA_ERROR_CANNOT_WRITE_TO_LCIO_EVENT,
    MOKKA_ERROR_INCOMPLETE_DERIVED_CLASS,
    MOKKA_ERROR_BAD_SENSITIVE_DETECTOR_NAME,
    MOKKA_ERROR_CANNOT_CREATE_GLOBAL_PARAMETER,
    MOKKA_ERROR_CANNOT_FIND_GLOBAL_PARAMETER,
    MOKKA_ERROR_BAD_GLOBAL_PARAMETERS,
    MOKKA_ERROR_MODEL_NOT_FOUND,
    MOKKA_ERROR_DETECTOR_SETUP_NOT_FOUND,
    MOKKA_ERROR_DETECTOR_CONCEPT_NOT_FOUND,
    MOKKA_ERROR_SUBDETECTOR_NOT_FOUND,
    MOKKA_ERROR_SUBDETECTOR_CONSTRUCTION_FAILED,
    MOKKA_ERROR_CANNOT_BUILD_MATERIAL,
    MOKKA_ERROR_RESTART_MODE_NOT_POSSIBLE,
    MOKKA_ERROR_PHYSICS_LIST_NOT_FOUND,
    MOKKA_ERROR_WRONG_LCIO_VERSION
  } ;

//fg: introduce a namespace to avoid name clashes (e.g. NONE is used in geant4)
namespace mokka {
  typedef enum
    {
      NONE = 1,
      ASCII_FILES,
      LCIO_FILE
    } PERSISTENCY_MODE;
}

class Visu;
class G4Event;
class G4UIdirectory;
class ControlMessenger ;
class G4Step;

#include "TrackSummary.hh"
#include "limits.h"
#include "CGAGeometryEnvironment.hh"

enum GeometryOperations
  {
    ADD,
    REMOVE,
    NEW
  };
struct GeometryEdition
{
  GeometryEdition(GeometryOperations Op,
		  G4String subDetectorName,
		  G4int buildOrder,
		  G4String driverName="",
		  G4String dbName="")
    : _Op (Op), _subDetectorName(subDetectorName),
      _buildOrder(buildOrder), _driverName(driverName), _dbName(dbName) {}
  GeometryOperations _Op;
  G4String _subDetectorName;
  G4int _buildOrder;
  G4String _driverName;
  G4String _dbName;
};

class Control
{

  friend class ControlMessenger ; // needed to set Control::MCCutInRange with steering

public:

  // The Control has to be a singleton class.
  // To get access to the Control the user 
  // must call the Control::GetControl()
  // static method.
  static Control* GetControl();

  G4int GetPIDForCalHit(G4Step *aStep);

  void ResetPIDForCalHit(G4int newPID)
  { 
    FirstPIDInCal = INT_MAX; 
    primaryId=newPID;
  }

  G4int GetPIDForTrkHit(G4Step *aStep);

  void ResetPIDForTrkHit()
  {
    FirstPIDInTrk = INT_MAX;
  }

  TrackSummaryVector* GetTrackedtracks()
  {
    return trackedtracks;
  }

  virtual ~Control();

  //----------------------------------------------
  // Public control parameters
  //----------------------------------------------
  static G4bool   BATCH_MODE;
  static G4String LOGFILEPATH;
  static G4String LASTEVTFILEPATH;

  static G4String DBHOST;
  static G4String USER;
  static G4String DBPASSWD;

  static G4String MODELS_DBNAME;
  static G4String MATERIALS_DBNAME;

  static G4String DETECTOR_MODEL;
  static G4String DETECTOR_CONCEPT;
  static G4String DETECTOR_SETUP;
 
  static G4String SUB_DETECTOR;

  static G4bool   DUMPG3;
  static G4String IntialMacroFile;

  static mokka::PERSISTENCY_MODE PersistencyMode;
  static G4String OutDirName;

  static G4String LCIOWRITEMODE ;

#ifdef LCIO_MODE
  static G4String LCIOFILENAME;
  static LCWriter* lcWrt;
  static LCEventImpl* lcEvt;
  static LCRunHeaderImpl* lcRunHdr ;
  static LCCollectionVec* lcMCVec;
  static MCParticleMap_type MCParticleMap;
  typedef MCParticleMap_type::value_type PIDMC;
  static LCIOEventParameterMap lcioEvtParamMap ;
#endif

  static G4bool   USE_OLD_HEPLCIO;
  static G4bool   LCIOWriteCompleteHepEvt;
  static G4bool   LCIOWriteParentsForBackscatter;
  static G4bool   SavingTrajectories;
  static G4bool   SavingPrimaries;
  static G4bool   RESTART;
  static G4bool   SKIP;
  static G4bool   VISUMODE;

  static G4int    SYNCEVT;
  static G4String PhytiaFileName; 

  static G4String PhysicsListName; 

  static G4double ActiveRangeCut;
  static G4double RadiatorRangeCut;
  static G4double PCBRangeCut;
  static G4double RangeCut;
  static G4double BFactor;
  
  static G4double TPCCut;
  
  //SJA: Add control over TrackingPhysicsList
  static G4bool   TrackingPhysicsListMSOn ;
  static G4bool   TrackingPhysicsListELossOn ;
  
  static G4double TPCLowPtCut;
  static G4bool   TPCLowPtStepLimit;
  static G4bool   TPCLowPtStoreMCPForHits;
  static G4double TPCLowPtMaxStepLength;
  static G4double TPCLowPtMaxHitSeparation;

  static G4int    stepNumber;
  static G4int    primaryId;
  static G4bool   TrackingPrimary;

  // global fields to turn around Geant4 strange behavior
  // when filling G4Track in the begin of Track. 
  // Filled by SteppingAction at first step and used
  // by TrackSummary::update method.
  static G4ThreeVector VertexPosition;
  static G4double VertexEnergy;
  static G4ThreeVector VertexMomentum;

  static Visu     *VisuManager;

  static G4bool   LCIODetailedShowerMode ;

  static G4bool   FixStdHepLeptonFSR  ;

  static G4double ConfigAngle;

  static G4double LorentzTransformationAngle;

  static G4double PrimaryVertexSpreadZ;

  static CGASetupParameters *globalModelParameters;
 
  static G4int PrintLevel;
  
  static G4int RandomSeed;
  static G4bool RandomSeedSetViaCommandLine;
  
  static G4int PairParticlesPerEvent ;

  static G4bool   LCIOStorePosition ;

  static std::vector<GeometryEdition*> GeometryEditions;

  static std::vector<G4String> lcioStoreTRKHitMomentum;

  static std::vector<G4String> detailedHitsStoring;

  static G4int DataRunNumber;
  static G4bool DataRunNumberSetViaCommandLine;
  
  static G4int mcRunNumber;
  static G4bool mcRunNumberSetViaCommandLine;

  static G4String ConfDataTag;

  static G4double UserDeltaIntersection;
  static G4double UserDeltaOneStep;
  static G4double G4DefaultDeltaIntersection;
  static G4double G4DefaultDeltaOneStep;

  static G4bool   ModelOpened ;
  static G4bool   EditGeometry ;

  static G4String MokkaVersionName;

#ifdef MOKKA_GEAR
  static G4String mokkaGearFileName ;
  static G4String mokkaGearMergeSource ;
#endif

  static G4bool RunAborted;
  static G4String InputFileGeneratorWarning;

  static G4String PDGFile;
  static clock_t Start_t;
  static clock_t End_t; 
protected:
  //fg20040226 need to make this static protected for steering files
//   static G4double MCCutInRange;
//   static G4double MCCutInEnergy;

public:
  void setup(int argc, char** argv);
  static void Log(const char * message);
  static void Abort(const char * message, int errorCode);

  // Begin/End event management
  void BeginOfEventAction(const G4Event* anEvent);
  void EndOfEventAction(const G4Event* anEvent);

  static std::vector<VSubDetectorDriver*> DETECTOR_DRIVERS;

  inline void SetDrawFlag(G4bool val)
  { drawFlag = val; };
  
  void LoadEvent(G4int EventNumber);

private:
  
  // The Control has to be a singleton class.
  // To get access to the Control the user 
  // must call the Control::GetControl()
  // static method.
  Control();

  void Usage(const char *name,const char *str);
  void SaveTracks(const G4Event* anEvent);
  void LoadTracks(G4Event* anEvent);

  static G4bool drawFlag;
  static Control* theControl;

  ControlMessenger * theControlMessenger;

  TrackSummaryVector* trackedtracks;
  G4int FirstPIDInCal;
  G4int FirstPIDInTrk;
};
#endif
