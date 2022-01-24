#include "OutputManager.hh"

OutputManager::OutputManager()
:fRootFile(0), 
 fNtuple1(0), fNtuple2(0), 
 fEabs(0.), fEgap(0.) ,fLabs(0.), fLgap(0.)
{
      
  // histograms
  //for (G4int k=0; k<kMaxHisto; k++) fHisto[k] = 0;
    
  // ntuple
  fNtuple1 = 0;
  fNtuple2 = 0;
}

OutputManager::~OutputManager()
{
  //if (fRootFile) delete fRootFile;
  //  if (tf_output_) delete tf_output_;
}

void OutputManager::Book()
{ 
  // Creating a tree container to handle histograms and ntuples.
  // This tree is associated to an output file.
  //
  G4String fileName = "AnaEx02.root";
  /*
  fRootFile = new TFile(fileName,"RECREATE");
  if (! fRootFile) {
    G4cout << " OutputManager::Book :" 
           << " problem creating the ROOT TFile "
           << G4endl;
    return;
  }
  */
  // tf_output_ = new TFile( "test.root", "RECREATE" );
  // if (! tf_output_ ) {
  //   G4cout << " OutputManager::Book :" 
  //          << " problem creating the ROOT TFile "
  //          << G4endl;
  //   return;
  // }

  /*
  // id = 0
  fHisto[0] = new TH1D("EAbs", "Edep in absorber (MeV)", 100, 0., 800*CLHEP::MeV);
  // id = 1
  fHisto[1] = new TH1D("EGap", "Edep in gap (MeV)", 100, 0., 100*CLHEP::MeV);
  // id = 2
  fHisto[2] = new TH1D("LAbs", "trackL in absorber (mm)", 100, 0., 1*CLHEP::m);
  // id = 3
  fHisto[3] = new TH1D("LGap", "trackL in gap (mm)", 100, 0., 50*CLHEP::cm);

  for ( G4int i=0; i<kMaxHisto; ++i ) {
    if (! fHisto[i]) G4cout << "\n can't create histo " << i << G4endl;
  }  

  // create 1st ntuple
  fNtuple1 = new TTree("Ntuple1", "Edep");
  fNtuple1->Branch("Eabs", &fEabs, "Eabs/D");
  fNtuple1->Branch("Egap", &fEgap, "Egap/D");

  // create 2nd ntuple 
  fNtuple2 = new TTree("Ntuple2", "TrackL");
  fNtuple2->Branch("Labs", &fLabs, "Labs/D");
  fNtuple2->Branch("Lgap", &fLgap, "Lgap/D");

  
  tree_ = new TTree ( "tree", "tree" );
  // tree_->Branch( "event", &event_id_, "event/I" );
  // tree_->Branch( "tracks", &tracks_ );

    
  //G4cout << "\n----> Output file is open in " << fileName << G4endl;
}

void OutputManager::Save()
{ 
  /*
  if (! fRootFile) return;
  
  fRootFile->Write();       // Writing the histograms to the file
  fRootFile->Close();       // and closing the tree (and the file)
  */
  
  // if (! tf_output_)
  //   return;
  
  // tf_output_->Close();
  // G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;
}

void OutputManager::FillHisto(G4int ih, G4double xbin, G4double weight)
{
  // if (ih >= kMaxHisto) {
  //   G4cerr << "---> warning from OutputManager::FillHisto() : histo " << ih
  //          << " does not exist. (xbin=" << xbin << " weight=" << weight << ")"
  //          << G4endl;
  //   return;
  // }
  //  if  (fHisto[ih]) { fHisto[ih]->Fill(xbin, weight); }
}

void OutputManager::Normalize(G4int ih, G4double fac)
{
  // if (ih >= kMaxHisto) {
  //   G4cout << "---> warning from HistoManager::Normalize() : histo " << ih
  //          << " does not exist. (fac=" << fac << ")" << G4endl;
  //   return;
  // }
  //  if (fHisto[ih]) fHisto[ih]->Scale(fac);
}

void OutputManager::FillNtuple(G4double energyAbs, G4double energyGap,
                              G4double trackLAbs , G4double trackLGap )
{
  // fEabs = energyAbs;
  // fEgap = energyGap;
  // fLabs = trackLAbs;
  // fLgap = trackLGap;

  // if (fNtuple1) fNtuple1->Fill();
  // if (fNtuple2) fNtuple2->Fill();
}

void OutputManager::PrintStatistic()
{
  /*
  G4cout << "\n ----> print histograms statistic \n" << G4endl;
  for ( G4int i=0; i<kMaxHisto; ++i ) {
    TH1D* h1 = fHisto[i];
    const G4String name = h1->GetName();  

    G4String unitCategory;
    if (name[0] == 'E' ) unitCategory = "Energy"; 
    if (name[0] == 'L' ) unitCategory = "Length";

    G4cout << name
           << ": mean = " << G4BestUnit(h1->GetMean(), unitCategory) 
           << " rms = " << G4BestUnit(h1->GetRMS(), unitCategory ) 
           << G4endl;
  }
  */
  
}
