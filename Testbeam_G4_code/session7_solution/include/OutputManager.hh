#ifndef OutputManager_h
#define OutputManager_h 1

#include <CLHEP/Units/SystemOfUnits.h>
#include "G4UnitsTable.hh"
#include "globals.hh"

#include <TH1D.h>
#include <TFile.h>
#include <TTree.h>

class TFile;
class TTree;
class TH1D;

const G4int kMaxHisto = 4;

class OutputManager
{
public:
  OutputManager();
  ~OutputManager();
   
  void Book();
  void Save();

  void FillHisto(G4int id, G4double bin, G4double weight = 1.0);
  void Normalize(G4int id, G4double fac);    

  void FillNtuple(G4double energyAbs, G4double energyGap,
		  G4double trackLAbs, G4double trackLGap);
    
  void PrintStatistic();
        
private:
  TFile*   fRootFile;
  TH1D*    fHisto[kMaxHisto];            
  TTree*   fNtuple1;    
  TTree*   fNtuple2;    
  TFile*   tf_output_;
  TTree*   tree_;
  
  G4double fEabs;
  G4double fEgap;
  G4double fLabs;
  G4double fLgap;
};

#endif

