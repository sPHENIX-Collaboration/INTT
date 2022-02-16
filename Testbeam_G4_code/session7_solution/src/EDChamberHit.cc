/// \file EDChamberHit.cc
/// \brief Implementation of the EDChamberHit class
//

#include "EDChamberHit.hh"

#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4SystemOfUnits.hh"

G4ThreadLocal G4Allocator<EDChamberHit>* EDChamberHitAllocator = 0;

EDChamberHit::EDChamberHit()
  : G4VHit(),
    fupordown(-1),
    fsilicon_type(-1),
    fzposition(-1),
    fxposition(-1),
    fyposition(-1),
    fEdep(0.)
{}

EDChamberHit::~EDChamberHit()
{}

EDChamberHit::EDChamberHit(const EDChamberHit& /*right*/)
 : G4VHit() 
{}

const EDChamberHit& EDChamberHit::operator=(const EDChamberHit& /*right*/)
{
  return *this;
}

int EDChamberHit::operator==(const EDChamberHit& right) const
{
  return ( this == &right ) ? 1 : 0;
}

void EDChamberHit::Print()
{  
  G4cout << "up or down "  << fupordown  << "\t"
         << "(x, y, z) = ("  << fxposition << ", "  << fyposition << ", "  << fzposition << "),\t"
         << "sensor type " << fsilicon_type << "\t"
	 << "energy depo " << std::setw(5) << std::setprecision(3) << fEdep << "\t"
	 << "timing "  << std::setw(5) << std::setprecision(3) << fTime << "\t"
          << G4endl; 
}

void EDChamberHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(fPosition);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);

    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }

}
