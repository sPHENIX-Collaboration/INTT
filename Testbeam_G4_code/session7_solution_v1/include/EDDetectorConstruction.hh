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
/// \file EDDetectorConstruction.hh
/// \brief Definition of the EDDetectorConstruction class

#ifndef EDDetectorConstruction_h
#define EDDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
//extern G4double theoffset;
class G4VPhysicalVolume;


/// Detector construction class to define materials and geometry.

class EDDetectorConstruction : public G4VUserDetectorConstruction
{
  //void movement (G4double theoffset) {theoffset=35.;}
  //extern G4double theoffset =35;
  public:
    EDDetectorConstruction();
    //static double theoffset =35;
    virtual ~EDDetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();
  //   void Settheoffset( G4double theoffset) { fmovement = theoffset; }
  //   G4double Gettheoffset() const {return fmovement; }
  // private:
  // G4double fmovement;   
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

