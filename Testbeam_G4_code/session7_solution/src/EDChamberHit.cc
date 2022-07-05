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
    upordown_(-1),
    silicon_type_(-1),
    xposition_(-1),
    yposition_(-1),
    zposition_(-1),
    hit_time_( 0.0 ),
    edep_total_(0.),
    edep_step_(0.),
    track_( nullptr ),
    step_in_( nullptr ),
    step_out_( nullptr ),
    step_max_( nullptr ),
    is_beam_in_( false ),
    is_beam_out_( false )
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
  G4cout << ""  << upordown_  << "\t"
         << "(x, y, z) = ("
	 << std::setw(3) << xposition_ << ", "
	 << std::setw(3) << yposition_ << ", "
	 << std::setw(3) << zposition_ << "),  "
         << "type " << silicon_type_ << "  "
	 << "dE " << std::setw(5) << std::setprecision(3) << edep_total_ << " MeV, "
	 << "timing "  << std::setw(5) << std::setprecision(3) << hit_time_ << " || step info: "

    // step info
	 << std::setw(5) << track_->GetTrackID() << ", "
	 << std::setw(8) << track_->GetParticleDefinition()->GetParticleSubType() << ", "
	 << std::setw(6) << std::setprecision(4) << track_->GetTotalEnergy() << ", "
	 << std::setw(10) << std::setprecision(7) << track_->GetGlobalTime() << ", "
	 << std::setw(5) << std::setprecision(3) << this->GetEdepStep()
    //<< 
    //	 << std::setw()
          << G4endl; 
}


void EDChamberHit::PrintEvent()
{  
  G4cout << ""  << upordown_  << "\t"
         << "(x, y, z) = ("
	 << std::setw(3) << xposition_ << ", "
	 << std::setw(3) << yposition_ << ", "
	 << std::setw(3) << zposition_ << "),  "
         << "type " << silicon_type_ << "  "
	 << "dE " << std::setw(5) << std::setprecision(3) << edep_total_ << " MeV, "
	 << "timing "  << std::setw(5) << std::setprecision(3) << hit_time_ << G4endl;
  G4cout << "Is beam? " << INTT_misc::red << IsBeamIn() << INTT_misc::close << G4endl;
  
  G4cout << "Step info: " << G4endl;
  G4cout << "step_in:" << G4endl;
  this->PrintStep( step_in_ );
  
  G4cout << "step_out:" << G4endl;
  this->PrintStep( step_out_ );
  
  G4cout << "step_max:" << G4endl;
  this->PrintStep( step_max_ );
  
}

void EDChamberHit::PrintStep( G4Step* step )
{
  if( step == nullptr )
    G4cout << "nullptr" << G4endl;
  else
    //G4cout << std::setw(5) << step->GetStepID() << ", "
    G4cout << std::setw(8) << step->GetTrack()->GetParticleDefinition()->GetParticleSubType() << ", "
	   << std::setw(6) << std::setprecision(4) << step->GetDeltaEnergy() << ", "
	   << std::setw(10) << std::setprecision(7) << step->GetTrack()->GetGlobalTime() + step->GetDeltaTime() << ", "
	   << G4endl;  
}

void EDChamberHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(position_);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);

    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }

}
