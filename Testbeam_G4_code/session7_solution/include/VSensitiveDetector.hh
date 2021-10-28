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
// $Id: VSensitiveDetector.hh,v 1.7 2006/03/01 14:13:30 musat Exp $
// $Name: mokka-07-00 $
//
//-------------------------------------------------------
// VSensitiveDetector : extends G4VSensitiveDetector interface to Mokka kernel 
//             requeriments.
//
// History
// first implementation P. Mora de Freitas, Feb 2002
//
//-------------------------------------------------------

#ifndef VSensitiveDetector_h
#define VSensitiveDetector_h 1

#include "globals.hh"
#include "G4VSensitiveDetector.hh"
#include "G4ThreeVector.hh"
#include <stdio.h>
#include "Control.hh"
#include "VEncoder.hh"
#include "CGADefs.h"

class VSensitiveDetector : public G4VSensitiveDetector
{
public:
  VSensitiveDetector(G4String SDname) 
    : G4VSensitiveDetector(SDname), theEncoder(0){}

  virtual ~VSensitiveDetector(){}
  
  virtual void LoadEvent(FILE* )
  {
    G4cout << "Warning, sensitive detector "
	   << SensitiveDetectorName 
	   << " has not yet a LoadEvent(File* ) method" << G4endl;
  }

  virtual void setID(G4int index) { id = index;}

  virtual G4int getID(void) { return id;}

  virtual G4ThreeVector GetCellCenter(G4int, G4int, G4int,
		      G4int, G4int, G4int) {
			
	  G4cout << "GetCellCenter was not implemented for sensitive detector "
		  << GetName() << G4endl;

	  return G4ThreeVector(-99999, -99999, -99999);
  }
  
  virtual cell_ids GetCellIndex(double , double , double ,
	int & , double =0, double =0, double =1) {

	  G4cout << "GetCellIndex was not implemented for sensitive detector "
		  << GetName() << G4endl;

          cell_ids theIndex;
	  theIndex.id0=theIndex.id1=-99999;

	  return theIndex;
  }

  VEncoder* getEncoder(void) { return theEncoder;}

protected:
  VEncoder *theEncoder;

private:
  G4int id;
};
#endif


