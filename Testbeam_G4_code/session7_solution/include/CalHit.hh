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
// $Id: CalHit.hh,v 1.9 2006/06/23 15:35:28 musat Exp $
// $Name: mokka-07-00 $
//
//-------------------------------------------------------
// CalHit : implements the calorimeter hits for all the
// calorimeter collections. Should be used with 
// all the different sensitive detectors, attached to the 
// calorimeters (Ecal and Hcal) sensitive volumes, for the 
// barrel and for the endcaps ones.
//
// History
// first implementation from Mokka08, 
// P. Mora de Freitas (mora@poly.in2p3.fr), July 2001
//
//-------------------------------------------------------

#ifndef CalHit_h
#define CalHit_h 1

#include "Control.hh"
#include "CGADefs.h"
#include "VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Circle.hh"
#include "PrimaryContribution.hh"
#include <map>

// PrimaryContribution_type maps the PID in the PrimaryContributions
// for this hit.
typedef std::map<G4int, PrimaryContribution*> PrimaryContribution_type;

class CalHit : public VHit
{
public:
  CalHit(
) : 
#ifdef LCIO_MODE
    VHit(LCIO::SIMCALORIMETERHIT),
#endif
    P(-1) 
  { code.id0 = code.id1 = 0; SetEnergy(0.0);}
  
  CalHit(G4int pP,G4int pS,G4int pM,G4int pI,G4int pJ,G4int pK,G4int GRZ,
	 G4double pX,G4double pY,G4double pZ,G4double pE,
	 G4int pPID,G4int pPDG,G4double pt,cell_ids pCode,float*sp=0) //	 G4int pPID,G4int pPDG,G4double pt=0)
    : 
#ifdef LCIO_MODE
    VHit(LCIO::SIMCALORIMETERHIT),
#endif
    P(pP),S(pS),M(pM),I(pI),J(pJ),K(pK),GRZone(GRZ),X(pX),Y(pY),Z(pZ)
    
  { code.id0 = pCode.id0; code.id1 = pCode.id1; SetEnergy(0.0);
    AddEdep(pPID, pPDG, pE, pt, sp);
  }
  
  ~CalHit();
  
  CalHit(const CalHit &right);
  const CalHit& operator=(const CalHit &right);
  int operator==(const CalHit &right) const
  { return ((code.id0 == right.code.id0)&&(code.id1 == right.code.id1)); }

  inline void *operator new(size_t);
  inline void operator delete(void *aCalHit);
  
  void Draw();
  void Print();

  void Save(FILE *oFile);

#ifdef LCIO_MODE
  virtual void Save(LCCollectionVec* aLCCollectionVec);
#endif

  G4bool Load(FILE *iFile);
  G4ThreeVector CellDim;
  
public:

  PrimaryContribution_type PrimaryContributions;

  inline G4int GetP() const {return P;} 
  inline G4int GetS() const {return S;} 
  inline G4int GetM() const {return M;} 

  inline G4int GetI() const {return I;} 
  inline G4int GetJ() const {return J;} 
  inline G4int GetK() const {return K;} 
  
  inline G4double GetX() const {return X;} 
  inline G4double GetY() const {return Y;} 
  inline G4double GetZ() const {return Z;} 

  inline G4int GetGRZone() const {return GRZone;} 
  
  void AddEdep(G4int pPID, G4int pPDG,
	       G4double de, G4double pt, float*sp=0 ); // G4double de, G4double pt=0 );

  inline G4bool testCell(cell_ids pCode) const
  { return ((code.id0 == pCode.id0)&&(code.id1 == pCode.id1)); }
  
protected:

  G4int P,S,M,I,J,K; // cell coordinates in the cells matrix
  G4int GRZone;
  // fg: this needs to be protected as we want  to have a different encoding in the subclasses
  cell_ids code;        // encoded cell id
                                                                                

private:
  G4double X,Y,Z;    // cell coordinates in space
  //VHit::Energy: total energy 
};

typedef G4THitsCollection<CalHit> HitsCollection;

extern G4Allocator<CalHit> CalHitAllocator;

inline void* CalHit::operator new(size_t)
{
  void *aCalHit;
  aCalHit = (void *) CalHitAllocator.MallocSingle();
  return aCalHit;
}

inline void CalHit::operator delete(void *aCalHit)
{
  CalHitAllocator.FreeSingle((CalHit*) aCalHit);
}

#endif

