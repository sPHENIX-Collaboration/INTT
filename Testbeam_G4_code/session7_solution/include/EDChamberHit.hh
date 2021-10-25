//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file EDChamberHit.hh
/// \brief Definition of the EDChamberHit class
//

#ifndef EDChamberHit_h
#define EDChamberHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class EDChamberHit : public G4VHit
{
  public:
    EDChamberHit();
    virtual ~EDChamberHit();
    EDChamberHit(const EDChamberHit& right);
    const EDChamberHit& operator=(const EDChamberHit& right);
    int operator==(const EDChamberHit &right) const;

    inline void* operator new(size_t);
    inline void  operator delete(void* hit);

    virtual void Print();
    virtual void Draw();

    // setter methods
    void SetEncoderID(G4int upordown, G4int silicon_type,G4int zposition, G4int xposition, G4int yposition )
    {
        fupordown=upordown;
        fsilicon_type=silicon_type;
        fzposition=zposition;
        fxposition=xposition;
        fyposition=yposition;
        
    }
    void AddEdep(G4double edep)       { fEdep += edep; }

    G4int Getupordown()const { return fupordown; }
    G4int Getsilicon_type()const { return fsilicon_type; }
    G4int Getzposition()const { return fzposition; }
    G4int Getxposition() const { return fxposition; }
    G4int Getyposition ()const { return fyposition; }
    G4double GetEnergyDeposit() const { return fEdep; }


    void SetLayerNumber(G4int number) { fLayerNumber = number; }
    void SetTime(G4double time)       { fTime = time; }
    void SetPosition(G4ThreeVector position) { fPosition = position; }

    // getter methods
    G4int          GetLayerNumber() const { return fLayerNumber;}
    G4double       GetTime() const        { return fTime; }
    G4ThreeVector  GetPosition() const    { return fPosition; } 

  private:
    // data members
    G4int fupordown;
    G4int fsilicon_type;
    G4int fzposition;
    G4int fxposition;
    G4int fyposition;
    G4double fEdep;

    G4int          fLayerNumber;
    G4double       fTime;
    G4ThreeVector  fPosition;
};

typedef G4THitsCollection<EDChamberHit> EDChamberHitsCollection;

extern G4ThreadLocal G4Allocator<EDChamberHit>* EDChamberHitAllocator;

inline void* EDChamberHit::operator new(size_t)
{
  if (! EDChamberHitAllocator)
        EDChamberHitAllocator = new G4Allocator<EDChamberHit>;
  return (void*)EDChamberHitAllocator->MallocSingle();
}

inline void EDChamberHit::operator delete(void* hit)
{
  EDChamberHitAllocator->FreeSingle((EDChamberHit*) hit);
}

#endif


