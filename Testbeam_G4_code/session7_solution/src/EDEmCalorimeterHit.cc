/// \file EDEmCalorimeterHit.cc
/// \brief Implementation of the EDEmCalorimeterHit class
//

#include "EDEmCalorimeterHit.hh"

#include <G4UnitsTable.hh>

#include <iomanip>

G4ThreadLocal G4Allocator<EDEmCalorimeterHit>* EDEmCalorimeterHitAllocator = 0;

EDEmCalorimeterHit::EDEmCalorimeterHit()
 : G4VHit(),
   fLayerNumber(-1),
   fEdep(0.)
{}

EDEmCalorimeterHit::~EDEmCalorimeterHit()
{}

EDEmCalorimeterHit::EDEmCalorimeterHit(const EDEmCalorimeterHit& /*right*/)
 : G4VHit() 
{}

const EDEmCalorimeterHit& 
EDEmCalorimeterHit::operator=(const EDEmCalorimeterHit& /*right*/)
{
  return *this;
}

int EDEmCalorimeterHit::operator==(const EDEmCalorimeterHit& /*right*/) const
{
  return 0;
}

void EDEmCalorimeterHit::Print()
{
  if ( fEdep > 0. ) {
    G4cout << "EmCalorimeter hit in layer: " << fLayerNumber 
           << "  Edep: " << std::setw(7) << G4BestUnit(fEdep,"Energy") << G4endl;
  }          
}
