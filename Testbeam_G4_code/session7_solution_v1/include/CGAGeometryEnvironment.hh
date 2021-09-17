//
//*******************************************************
//*                                                     *
//*                      Mokka                          * 
//* For more information about Mokka, please, go to the *
//*                                                     *
//*  polype.in2p3.fr/geant4/tesla/www/mokka/mokka.html  *
//*                                                     *
//* Mokka home page.                                    *
//*                                                     *
//*******************************************************
//
// $Id: CGAGeometryEnvironment.hh,v 1.4 2005/07/26 07:48:14 mora Exp $
// $Name: mokka-07-00 $
//
#ifndef CGAGeometryEnvironment_h
#define CGAGeometryEnvironment_h 1

#include "globals.hh"
#include <map>

typedef std::map < G4String, G4String > CGASetupParameters;

class CGAGeometryEnvironment
{
public:
  CGAGeometryEnvironment(G4String DBName, 
			 G4String DetectorConceptName,
			 CGASetupParameters *SetupParameters,
			 CGASetupParameters *GlobalParameters); 

  virtual ~CGAGeometryEnvironment();

  // operator= and copy constructors
  CGAGeometryEnvironment(const CGAGeometryEnvironment &right);

  const CGAGeometryEnvironment& operator=(const CGAGeometryEnvironment &right);

  // public access methods
  const G4String GetDBName() const { return _DBName;}
  void SetDBName(G4String aDBName) {_DBName = aDBName;}

  const G4String GetDetectorConcept () const {return _DetectorConcept;}

  G4String GetParameterAsString (G4String PName,G4String s="") const;
  G4double GetParameterAsDouble (G4String PName,G4double d=0)const;
  G4int GetParameterAsInt (G4String PName,G4int i=0) const;
  void CreateGlobalParameter(G4String PName, G4String PValue);


private:
  G4String _DBName;
  G4String _DetectorConcept;
  CGASetupParameters *_SetupParameters;
  CGASetupParameters *_GlobalParameters;
};


#endif


