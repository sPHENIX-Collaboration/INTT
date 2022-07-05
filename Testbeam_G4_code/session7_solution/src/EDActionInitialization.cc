/// \file EDActionInitialization.cc
/// \brief Implementation of the EDActionInitialization class

#include "EDActionInitialization.hh"

EDActionInitialization::EDActionInitialization( INTTMessenger* INTT_mess, EDDetectorConstruction* fDetConstruction_arg )
  : G4VUserActionInitialization(),
    fDetConstruction( fDetConstruction_arg ),
    INTT_mess_( INTT_mess )
{}

EDActionInitialization::~EDActionInitialization()
{}

void EDActionInitialization::BuildForMaster() const
{

  OutputManager* output = new OutputManager();
  
  SetUserAction(
		new EDRunAction( INTT_mess_,
				new EDPrimaryGeneratorAction( INTT_mess_ ),
				new EDEventAction( INTT_mess_ ),
				output
				)
		);
}

void EDActionInitialization::Build() const
{
  auto pga = new EDPrimaryGeneratorAction( INTT_mess_ );  
  SetUserAction( pga );

  OutputManager* output = new OutputManager();
  auto event = new EDEventAction( INTT_mess_ );
  SetUserAction( event );
  SetUserAction(new EDRunAction( INTT_mess_, pga, event, output ) );
  SetUserAction(new TrackingAction( pga ) );
  SetUserAction( new SteppingAction(fDetConstruction) );
}  
