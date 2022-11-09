/// \file EDEmCalorimeterHit.cc
/// \brief Implementation of the EDEmCalorimeterHit class
//

#include "EDEmCalorimeterHit.hh"

#include <G4UnitsTable.hh>

#include <iomanip>

G4ThreadLocal G4Allocator<EDEmCalorimeterHit>* EDEmCalorimeterHitAllocator = 0;

EDEmCalorimeterHit::EDEmCalorimeterHit()
 : G4VHit(),
   fLayerNumber_(-1),
   fEdep_(0.),
   xposition_( 0.0 ),
   yposition_( 0.0 ),
   zposition_( 0.0 ),
   track_angle_theta_( 0.0 ),
   track_angle_phi_( 0.0 )
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
  if ( fEdep_ > 0. ) {
    G4cout << "EmCalorimeter hit in layer: " << fLayerNumber_ 
           << "  Edep: " << std::setw(7) << G4BestUnit(fEdep_,"Energy") << G4endl;
  }
  
  G4cout << "TriggerHit, layer ID: "
	 << fLayerNumber_ << ", "    
	 << "Position: ("
	 << xposition_ << ", "
	 << yposition_ << ", "
	 << zposition_ << ")\t"
	 << "Edep: " << fEdep_
	 << G4endl;
}
