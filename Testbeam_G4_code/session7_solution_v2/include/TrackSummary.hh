//
//*******************************************************
//*                                                     *
//*                      Mokka                          * 
//*   - the detailed geant4 simulation for Tesla -      *
//*                                                     *
//* For more information about Mokka, please, go to the *
//*                                                     *
//*  polype.in2p3.fr/geant4/tesla/www/mokka/mokka.html  *
//*                                                     *
//* Mokka home page.                                    *
//*                                                     *
//*******************************************************
//
// $Id: TrackSummary.hh,v 1.9 2009/04/22 12:37:55 musat Exp $
// $Name: mokka-07-00 $
//
#ifndef TRACKSUMMARY_h
#define TRACKSUMMARY_h 1
  
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Allocator.hh" 
#include <vector>
#include <bitset>

#ifdef LCIO_MODE
#include "lcio.h"
#include "IMPL/MCParticleImpl.h" 
using namespace lcio ;
#endif

class G4Track;
class TrackSummary;
typedef std::vector<TrackSummary*> TrackSummaryVector;

class TrackSummary
{
public:
  
  // Constructor/destructor
  TrackSummary(const G4Track*,
	       G4bool,
	       G4bool BackScattering = false);

  ~TrackSummary(){}

  // new and delete for allocator usage
  inline void *operator new(size_t);
  inline void operator delete(void *);
  
  void update (const G4Track*);

  // Get/Set functions 
  G4double GetCharge() const
  { return charge; }

  G4ThreeVector GetEndPoint()  const
  {return endPoint;}

  G4double GetEnergy() const
//  {return energy;}
  {return 
	sqrt( momentum[0]*momentum[0] + momentum[1]*momentum[1] 
		+ momentum[2]*momentum[2] + mass*mass );}
  
  G4double GetMass() const
  {return mass;}
   
  G4int GetHepEvtStatus() const
  {return hepEvtStatus;}
  
  G4int GetSimulatorStatus() const
  { return _simstatus.to_ulong() ; }

  G4ThreeVector GetMomentum() const
  {return momentum;}

  TrackSummary* GetParent() const
  {return parent;}

  G4int GetPDG() const
  {return PDG;}

  const G4ThreeVector& GetVertex() const
  {return vertex;}

  G4int GetTrackID() const
  {return trackID;}

  G4int GetParentID() const
  {return parentTrackID;}

  G4double GetTrackLength() const
  {return trackLength;}

  G4bool GetToBeSaved() const
  {return toBeSaved;}

  void SetToBeSaved()
  { toBeSaved = true; }
  
  G4bool GetBackScattering() const
  {return backScattering;}

  void SetBackScattering()
  {  backScattering = true; }

  void SetParentToBeSaved();

#ifdef LCIO_MODE

  MCParticleImpl* GetMCParticle()
  { return theMCParticle;}

  void SetMCParticle(MCParticleImpl*  mcp) { theMCParticle = mcp ; }
  
  void BuildMCParticle();

#endif

private:

  TrackSummary* GetMyParent() const;
  
// Member data
  G4double charge;
  G4ThreeVector endPoint;
  G4double energy;
  G4double mass;
  G4int hepEvtStatus;
  std::bitset<32> _simstatus ;
  G4ThreeVector momentum;
  TrackSummary* parent;
  G4int PDG;
  G4ThreeVector vertex;

  G4bool toBeSaved;

  G4bool backScattering;
  G4int trackID;
  G4int parentTrackID;
  G4double trackLength;
#ifdef LCIO_MODE
  MCParticleImpl* theMCParticle;
  G4bool mcParticleIsUpToDate ;
#endif

};

extern G4Allocator<TrackSummary> TrackSummaryAllocator;

inline void* TrackSummary::operator new(size_t)
{
  void *aTrackSummary;
  aTrackSummary = (void *) TrackSummaryAllocator.MallocSingle();
  return aTrackSummary;
}

inline void TrackSummary::operator delete(void *aTrackSummary)
{
  TrackSummaryAllocator.FreeSingle((TrackSummary*) aTrackSummary);
}
#endif
