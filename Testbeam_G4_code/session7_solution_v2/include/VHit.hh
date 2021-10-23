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
//*    Mokka home page.                                 *
//*                                                     *
//*******************************************************
//
// $Id: VHit.hh,v 1.8 2006/06/23 15:35:28 musat Exp $
// $Name: mokka-07-00 $
//
//-------------------------------------------------------
// VHit : extends G4VHit interface to Mokka kernel 
//             requeriments.
//
// History
// first implementation P. Mora de Freitas, Feb 2002
//
//-------------------------------------------------------

#ifndef VHit_h
#define VHit_h 1

#include "Control.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include <stdio.h>

class VHit : public G4VHit
{
public:
  VHit(
#ifdef LCIO_MODE
       std::string aLCCollectionType
       )
    : theLCFlag(0)
  {
    if(aLCCollectionType != LCIO::SIMTRACKERHIT &&
       aLCCollectionType != LCIO::SIMCALORIMETERHIT)
      Control::Abort("VHit : bad LCCollectionType in constructor",
		MOKKA_OTHER_ERRORS);
    theLCCollectionType=aLCCollectionType;
  }
#else
  ) {}
#endif

  virtual ~VHit(){;};
  
#ifdef LCIO_MODE
  const VHit& operator=(const VHit &right)
  {
    theLCCollectionType=right.theLCCollectionType;
    theLCFlag=right.theLCFlag;
    return *this;
  }
#endif
  
  G4int operator==(const VHit &) const
  { return false; }
  
  virtual void Draw(){;}
  virtual void Print(){;}
  virtual void Save(FILE*){;}

#ifdef LCIO_MODE
  virtual void basic_Save(LCCollectionVec* aLCCollectionVec)
  {
    if(theLCCollectionType != aLCCollectionVec->getTypeName())
      Control::Abort("VHit::basicSave(): bad LCCollectionVec!",
		MOKKA_OTHER_ERRORS);
    Save(aLCCollectionVec);
  }
  
  virtual void Save(LCCollectionVec* )
  {
    Control::Abort("VHit::Save(LCCollectionVec*) NOT IMPLEMENTED!",
			MOKKA_ERROR_INCOMPLETE_DERIVED_CLASS);
  }

  std::string GetLCCollectionType() const
  {return theLCCollectionType;};

  LCFlagImpl GetLCFlag() const
  { return theLCFlag; }
#endif
  
  // Load(FILE *iFile) returns true while the read succeds
  virtual G4bool Load(FILE*) {return false;}
  virtual void setAsciiFlag(G4int aFlag) { theAsciiFlag = aFlag;}
  virtual G4int getAsciiFlag(void) { return theAsciiFlag;}
  virtual G4double GetEnergy() const {return Energy;}
  virtual void SetEnergy(G4double pEnergy) {Energy = pEnergy;}
  
protected:
  G4double Energy;
  G4int theAsciiFlag;
#ifdef LCIO_MODE
  std::string theLCCollectionType;
  LCFlagImpl theLCFlag;
#endif
};
typedef G4THitsCollection<VHit> VHitsCollection;
#endif


