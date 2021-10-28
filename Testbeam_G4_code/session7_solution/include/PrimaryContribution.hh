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
// $Id: PrimaryContribution.hh,v 1.2 2004/06/23 08:44:59 frank Exp $
// $Name: mokka-07-00 $
//
//-------------------------------------------------------

#ifndef PrimaryContribution_h
#define PrimaryContribution_h 1

#include "globals.hh"
#include <map>

// PDGContribution_type, maps the PDG in the E contribution
// for a given primary
//typedef std::map<G4int, G4double> PDGContribution_type;
// fg: added time contribution
//typedef std::multimap<G4int, std::pair< G4double, G4double > > PDGContribution_type;

//gm: new definition to include step position needed by SDHcal group
typedef struct {
        G4double energy;
        G4double time;
        float *stepPosition;
} PDGEntry;
typedef std::multimap<G4int, PDGEntry > PDGContribution_type;

class PrimaryContribution
{
public:
  
  PrimaryContribution(G4int pPDG, G4double pE, G4double pt=0, float*sp=0);
  virtual ~PrimaryContribution(){ PDGContributions.clear();}

  PrimaryContribution(const PrimaryContribution &right);
  const PrimaryContribution& operator=(const PrimaryContribution &right);
  
  void AddEdep(G4int pPDG,G4double pE, G4double pt=0, float*sp=0 );

  G4double E;        // total energy 
  G4double time ;    // time of contribution

public:
  PDGContribution_type PDGContributions;

  G4double GetE() { return E; };
  void AddE(G4double de) { E += de; };

protected:
  bool _firstEdep ; 

};
#endif


