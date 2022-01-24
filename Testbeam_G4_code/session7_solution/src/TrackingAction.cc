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
/// \file electromagnetic/TestEm1/src/TrackingAction.cc
/// \brief Implementation of the TrackingAction class

#include "TrackingAction.hh"
using namespace std;

TrackingAction::TrackingAction( EDPrimaryGeneratorAction* prim )
  :G4UserTrackingAction(),
   fPrimary(prim)
{
  // store the verbose level for debugging
  auto ui_manager = G4UImanager::GetUIpointer();
  verbose = ui_manager->GetCurrentIntValue( "/run/verbose", 1, true );

}

// void TrackingAction::Initialize()
// {
//   auto ui_manager = G4UImanager::GetUIpointer();
//   verbose = ui_manager->GetCurrentIntValue( "/run/verbose", 1, true );
// }

void TrackingAction::PreUserTrackingAction(const G4Track* track )
{

  //debug  
  const G4DynamicParticle* dynamic = track->GetDynamicParticle();
  G4double dynamCharge = dynamic->GetCharge();
  G4int occup          = dynamic->GetTotalOccupancy();
  G4double   pdgMass   = dynamic->GetParticleDefinition()->GetPDGMass();    
  G4double invarMass   = dynamic->Get4Momentum().m();  
  G4double dynamMass   = dynamic->GetMass();
  G4double dif1 = invarMass - pdgMass;
  G4double dif2 = dynamMass - pdgMass;

  //  if( verbose == 2 )
  if( false )
    {
      G4cout
	<< "Begin of track : charge= " <<  dynamCharge << "  occupancy= " << occup
	<< " pdgMass= " << G4BestUnit (pdgMass  , "Energy")    
	///    << "\n invarMass= " << G4BestUnit (invarMass, "Energy")
	///    << "   invar-pdg= " << G4BestUnit (dif1     , "Energy")
	<< "dynamMass= " << G4BestUnit (dynamMass, "Energy")
	<< "   dynam-pdg= " << G4BestUnit (dif2     , "Energy")
	<< G4endl;          
    }
}

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
  // how can I get it when given by use?
  auto ui_manager = G4UImanager::GetUIpointer();
  verbose = ui_manager->GetCurrentIntValue( "/tracking/verbose", 1, true );

  auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
  EDEventAction* event_action = (EDEventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
  
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  //auto track_volume = track->GetVolume(); // physical volume
  auto vertex_position = track->GetVertexPosition();
  
  const G4Step* step = track->GetStep();
  const G4DynamicParticle* particle = track->GetDynamicParticle();
  const G4ParticleDefinition* particle_def = track->GetParticleDefinition();
  //increase nb of processed tracks 
  //count nb of steps of this track
  G4int   nbSteps = track->GetCurrentStepNumber();

  TrackMC* track_mc = new TrackMC();
  track_mc->SetEventID( event->GetEventID() );
  track_mc->SetID( track->GetTrackID() );
  auto track_parent_id = track->GetParentID();
  track_mc->SetPrimary( track_parent_id == 0 ); // it works only #beam particle / event = 1 
  track_mc->SetQ( track->GetDefinition()->GetPDGCharge() );
  track_mc->SetName( particle_def->GetParticleName() );
  track_mc->SetTotalEnergy( track->GetTotalEnergy() );
  track_mc->SetKineticEnergy( track->GetKineticEnergy() );
  track_mc->SetTrackLength( track->GetTrackLength() );
  track_mc->SetTrackTime( track->GetLocalTime() );

  auto momentum = track->GetMomentum(); // G4ThreeVector
  track_mc->SetMomentum( momentum.x(), momentum.y(), momentum.z() );

  auto position = track->GetPosition(); // G4ThreeVector
  track_mc->SetPosition( position.x(), position.y(), position.z() );

  //  track_mc->Print( 2 );
  
  G4double rmin, dr, ds;
  G4int ih = 1;
    
  // //projected angle at exit  
  // G4double ux = direction.x(), uy = direction.y(), uz = direction.z();
  // G4double thetax = std::atan(ux/uz);
  // G4double thetay = std::atan(uy/uz);
      
  // //dN/dS at exit
  // G4double x = position.x(), y = position.y();
  // G4double r = std::sqrt(x*x + y*y);
  // ih = 2;
  // //dr = analysisManager->GetH1Width(ih);
  // rmin = ((int)(r/dr))*dr;
  // ds = twopi*(rmin + 0.5*dr)*dr;        
      
  // //d(N/cost)/dS at exit  
  // ih = 3;
  // dr = analysisManager->GetH1Width(ih);
  // rmin = ((int)(r/dr))*dr;
  // ds = twopi*(rmin + 0.5*dr)*dr;        
  
  // //vector of d(N/cost)/dS at exit
  
  // Run* run = static_cast<Run*>(
  // 			       G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  //  run->SumFluence(r, 1/uz);
  
  

  
  // Run* run = static_cast<Run*>(
  //            G4RunManager::GetRunManager()->GetNonConstCurrentRun()); 
    
  // if (track->GetDefinition()->GetPDGCharge() == 0.) {
  //   run->CountTraks0(1); 
  //   run->CountSteps0(nbSteps);
  
  // } else {
  //   run->CountTraks1(1); 
  //   run->CountSteps1(nbSteps);
  // }

  
  //true and projected ranges for primary particle
  if (track->GetTrackID() == 1) {
    //run->AddTrueRange(Trleng);
    const G4ThreeVector vertex = fPrimary->GetParticleGun()->GetParticlePosition();
    G4ThreeVector position = track->GetPosition() - vertex;      
    //run->AddProjRange(position.x());
    //run->AddTransvDev(position.y());
    //run->AddTransvDev(position.z());
      
    //    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    G4double beam_energy = track->GetVertexKineticEnergy();
    //cout << "Beam energy: " << beam_energy << endl;
    //EDEventAction::beam_energy = beam_energy;
    //    analysisManager->FillNtupleDColumn( 0, beam_energy ); // 0: Beam
    //analysisManager->FillH1(1,Trleng);
    //analysisManager->FillH1(2,(float)nbSteps);
    //    std::cout << beam_energy << endl;
  }
  //Print( track );

  if( verbose == 2 )
    {
      if( track_parent_id == 0 )
	{
	  //auto primary_track = track->Get
	  //Print( track );
	  auto primary_vertex = event->GetPrimaryVertex();
	  G4cout << "Beam vertex:" << G4endl;
	  primary_vertex->Print();


	}

  //     G4cout << "ID " << std::setw(3) << track_id << " "
  // 	     << "parent ID " << std::setw(3) << track_parent_id << " "
  // 	     << std::setw(4) << particle_def->GetParticleName() << " "
  // 	     << "E = " << std::setw(5) << std::setprecision(3) << track_energy << " MeV "
  // 	     << "Ekin = " << std::setw(5) << std::setprecision(3) << track->GetKineticEnergy() << " MeV "
  // 	     << "Q = " << std::setw(3) << charge << " "
  // 	     << std::setw(7) << std::setprecision(3) << track_time << " ns "
  // //auto track_volume = track->GetVolume(); // physical volume
  // 	     << "mom: ("
  // 	     << std::setw(9) << std::setprecision(6) << momentum.x() << ", "
  // 	     << std::setw(9) << std::setprecision(6) << momentum.y() << ", "
  // 	     << std::setw(9) << std::setprecision(6) << momentum.z()
  // 	     << ") " << G4endl << "\t\t\t"
  // 	     << "vertex pos: ("
  // 	     << std::setw(5) << std::setprecision(3) << vertex_position.x() << ", "
  // 	     << std::setw(5) << std::setprecision(3) << vertex_position.y() << ", "
  // 	     << std::setw(7) << std::setprecision(5) << vertex_position.z()
  // 	     << ") "
  // 	     << "pos: ("
  // 	     << std::setw(5) << std::setprecision(3) << position.x() << ", "
  // 	     << std::setw(5) << std::setprecision(3) << position.y() << ", "
  // 	     << std::setw(12) << std::setprecision(8) << position.z()
  // 	     << ") "
  // 	     // << "direction: ("
  // 	     // << std::setw(5) << std::setprecision(3) << direction.x() << ", "
  // 	     // << std::setw(5) << std::setprecision(3) << direction.y() << ", "
  // 	     // << std::setw(5) << std::setprecision(3) << direction.z()
  // 	     // << ")"
  // 	     << G4endl;


    }

  event_action->AddTrackMC( track_mc );
}

void TrackingAction::Print( const G4Track* track )
{
  G4int   nbSteps = track->GetCurrentStepNumber();
  G4double Trleng = track->GetTrackLength();

  const G4Step* step = track->GetStep();
  const G4DynamicParticle* particle = track->GetDynamicParticle();
  const G4ParticleDefinition* particle_def = track->GetParticleDefinition();

    //  G4ThreeVector vertex = fPrimary->GetParticleGun()->GetParticlePosition();
  // G4ThreeVector position = track->GetPosition() - vertex;      
  cout << setw(5)  << track->GetTrackID() << "->" // good
       << setw(10) << left << particle_def->GetParticleName() << "@" // good
       << "( " 
       << setw(5)  << track->GetVertexPosition().X << ", " // useless
       << setw(5)  << track->GetVertexPosition().Y << ", " // useless
       << setw(5)  << track->GetVertexPosition().Z << ") " // useless
    
       // << "( " 
       // << setw(5)  << track->GetPosition().X << ", " // useless
       // << setw(5)  << track->GetPosition().Y << ", " // useless
       // << setw(5)  << track->GetPosition().Z << ")  " // useless
    
       << setw(5)  << track->GetParentID() << "  " // good
    //       << setw(5)  << nbSteps << "  "
    //       << setw(10) << setprecision(2) << Trleng << "  "
    //       << setw(5)  << particle->GetMass() << " "
    //       << setw(3)  << particle->GetCharge() << "  "
    //       << setw(5)  << particle->GetTotalEnergy() << ",  " // useless
       << setw(5)  << track->GetVertexKineticEnergy() << "  " // good
       // << "( "
       // << setw(5)  << track->GetVertexMomentumDirection().X << ", "  // useless
       // << setw(5)  << track->GetVertexMomentumDirection().Y << ", "  // useless
       // << setw(5)  << track->GetVertexMomentumDirection().Z << ") "  // useless
    //       << setw(5)  << particle->GetTotalMomentum() << "  " // useless
    //       << setw(5)  << particle->GetKineticEnergy() << "  |  " // useless
    //       << setw(5)  << step->GetTotalEnergyDeposit()
       << endl;

  /*
  //debug  
  const G4DynamicParticle* dynamic = track->GetDynamicParticle();
  G4double dynamCharge = dynamic->GetCharge();
  G4int occup          = dynamic->GetTotalOccupancy();
  G4double   pdgMass   = dynamic->GetParticleDefinition()->GetPDGMass();    
  G4double invarMass   = dynamic->Get4Momentum().m();  
  G4double dynamMass   = dynamic->GetMass();
  G4double dif1 = invarMass - pdgMass;
  G4double dif2 = dynamMass - pdgMass;
  
  G4cout
    << "\n  End of track :"    
    << "\n    charge= " <<  dynamCharge << "  occupancy= " << occup
    << "\n   pdgMass= " << G4BestUnit (pdgMass  , "Energy")    
///    << "\n invarMass= " << G4BestUnit (invarMass, "Energy")
///    << "   invar-pdg= " << G4BestUnit (dif1     , "Energy")
    << "\n dynamMass= " << G4BestUnit (dynamMass, "Energy")
    << "   dynam-pdg= " << G4BestUnit (dif2     , "Energy")
    << G4endl;          
*/                       

}
