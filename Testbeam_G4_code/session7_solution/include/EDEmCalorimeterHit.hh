/// \file EDEmCalorimeterHit.hh
/// \brief Definition of the EDEmCalorimeterHit class
//

#ifndef EDEmCalorimeterHit_h
#define EDEmCalorimeterHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Step.hh"

class EDEmCalorimeterHit : public G4VHit
{
public:
  EDEmCalorimeterHit();
  virtual ~EDEmCalorimeterHit();
  EDEmCalorimeterHit(const EDEmCalorimeterHit& right);
  const EDEmCalorimeterHit& operator=(const EDEmCalorimeterHit& right);
  int operator==(const EDEmCalorimeterHit& right) const;

  inline void* operator new(size_t);
  inline void  operator delete(void* hit);

  virtual void Print();

  // add setter/getter methods
  void SetLayerNumber(G4int number) { fLayerNumber_ = number; }
  void SetPosition( G4double x, G4double y, G4double z ){
    xposition_ = x;
    yposition_ = y;
    zposition_ = z;
  }

  void SetTiming( G4double timing_arg ){ timing_ = timing_arg; };
  void SetTrackAngles( G4double theta, G4double phi ){
    track_angle_theta_ = theta;
    track_angle_phi_ = phi;
  }
  
  void AddEdep(G4double edep)       { fEdep_ += edep; }

  G4int    GetLayerNumber() const { return fLayerNumber_;}
  G4double GetEdep() const        { return fEdep_; }
  G4double GetEnergyDeposit() const { return fEdep_; }

  G4double		Getxposition()		const	{ return xposition_; }
  G4double		Getyposition()		const	{ return yposition_; }
  G4double		Getzposition()		const	{ return zposition_; }
  G4double              GetTiming()             const   { return timing_; };
  G4double              GetTrackAngleTheta()    const   { return track_angle_theta_;};
  G4double              GetTrackAnglePhi()    const   { return track_angle_phi_;};

private:
      
  // add data members
  G4int     fLayerNumber_;
  G4double  fEdep_;
  
  G4double xposition_;
  G4double yposition_;
  G4double zposition_;
  G4double edep_total_;
  G4double timing_;
  
  G4double track_angle_theta_;
  G4double track_angle_phi_;
  
};

typedef G4THitsCollection<EDEmCalorimeterHit> EDEmCalorimeterHitsCollection;

extern G4ThreadLocal G4Allocator<EDEmCalorimeterHit>* EDEmCalorimeterHitAllocator;

inline void* EDEmCalorimeterHit::operator new(size_t)
{
  if (! EDEmCalorimeterHitAllocator)
    EDEmCalorimeterHitAllocator = new G4Allocator<EDEmCalorimeterHit>;
  return (void*)EDEmCalorimeterHitAllocator->MallocSingle();
}

inline void EDEmCalorimeterHit::operator delete(void* hit)
{
  EDEmCalorimeterHitAllocator->FreeSingle((EDEmCalorimeterHit*) hit);
}

#endif


