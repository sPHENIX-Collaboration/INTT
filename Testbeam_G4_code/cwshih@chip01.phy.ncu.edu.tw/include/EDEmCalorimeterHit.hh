/// \file EDEmCalorimeterHit.hh
/// \brief Definition of the EDEmCalorimeterHit class
//

#ifndef EDEmCalorimeterHit_h
#define EDEmCalorimeterHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

class EDEmCalorimeterHit : public G4VHit
{
  public:
    EDEmCalorimeterHit();
    virtual ~EDEmCalorimeterHit();
    EDEmCalorimeterHit(const EDEmCalorimeterHit& right);
    const EDEmCalorimeterHit& operator=(const EDEmCalorimeterHit& right);
    int operator==(const EDEmCalorimeterHit& right) const;

    inline void* operator new(size_t);
    inline void  operator delete(void* hit);

    virtual void Print();

    // add setter/getter methods
    void SetLayerNumber(G4int number) { fLayerNumber = number; }
    void AddEdep(G4double edep)       { fEdep += edep; }

    G4int    GetLayerNumber() const { return fLayerNumber;}
    G4double GetEdep() const        { return fEdep; }
    G4double GetEnergyDeposit() const { return fEdep; }
  private:
    // add data members
    G4int     fLayerNumber;
    G4double  fEdep;
};

typedef G4THitsCollection<EDEmCalorimeterHit> EDEmCalorimeterHitsCollection;

extern G4ThreadLocal G4Allocator<EDEmCalorimeterHit>* EDEmCalorimeterHitAllocator;

inline void* EDEmCalorimeterHit::operator new(size_t)
{
  if (! EDEmCalorimeterHitAllocator)
        EDEmCalorimeterHitAllocator = new G4Allocator<EDEmCalorimeterHit>;
  return (void*)EDEmCalorimeterHitAllocator->MallocSingle();
}

inline void EDEmCalorimeterHit::operator delete(void* hit)
{
  EDEmCalorimeterHitAllocator->FreeSingle((EDEmCalorimeterHit*) hit);
}

#endif


