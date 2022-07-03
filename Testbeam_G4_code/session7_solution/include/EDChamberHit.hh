/// \file EDChamberHit.hh
/// \brief Definition of the EDChamberHit class
//

#ifndef EDChamberHit_h
#define EDChamberHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"

#include "INTT_misc.hh"

class EDChamberHit : public G4VHit
{
public:
  EDChamberHit();
  virtual ~EDChamberHit();
  EDChamberHit(const EDChamberHit& right);

  const EDChamberHit& operator=(const EDChamberHit& right);

  int operator==(const EDChamberHit &right) const;
  inline void* operator new(size_t);
  inline void  operator delete(void* hit);

  virtual void Print();
  void PrintEvent();
  virtual void Draw();

  // setter methods
  void SetEncoderID(G4int upordown, G4int silicon_type,G4int zposition, G4int xposition, G4int yposition )
  {
    upordown_ = upordown;
    silicon_type_ = silicon_type;
    zposition_ = zposition;
    xposition_ = xposition;
    yposition_ = yposition;
        
  }

  void AddEdep(G4double edep)       { edep_total_ += edep; }

  // getter methods for hit information
  G4int		Getupordown()		const	{ return upordown_; }
  G4int		Getsilicon_type()	const	{ return silicon_type_; }
  G4int		Getzposition()		const	{ return zposition_; }
  G4int		Getxposition()		const	{ return xposition_; }
  G4int		Getyposition ()		const	{ return yposition_; }
  G4double	GetEnergyDeposit()	const	{ return edep_total_; }
  G4int         GetLayerNumber()	const	{ return layerNumber_;}
  G4double      GetTime()		const   { return hit_time_; }
  G4ThreeVector GetPosition()		const   { return position_; } 

  // getter methods for the track&step information
  G4Track*	GetTrack(){ return track_;};
  G4Step*	GetStepIn(){ return step_in_;};
  G4Step*	GetStepOut(){ return step_out_;};
  G4Step*	GetStepMax(){ return step_max_;};
  
  // methods to set private hit variables
  void SetLayerNumber(G4int number)		{ layerNumber_ = number; }
  void SetTime(G4double time)			{ hit_time_ = time; }
  void SetPosition(G4ThreeVector position)	{ position_ = position; }
  void SetEdepStep( G4double edep )		{ edep_step_ = edep;};
  
  // methods to set private step variables
  void SetTrack( G4Track* tr )		{ track_ = tr;};
  void SetStepIn( G4Step* step )	{ step_in_ = step;};
  void SetStepOut( G4Step* step )	{ step_out_ = step;};
  void SetStepMax( G4Step* step )	{ step_max_ = step;};
  G4double GetEdepStep()		{ return edep_step_;};
  void SetBeamIn( bool flag ){ is_beam_in_ = flag;};
  void SetBeamOut( bool flag ){ is_beam_out_ = flag;};

  // methods to check the status
  bool IsBeamIn(){ return is_beam_in_;};
  bool IsBeamOut(){ return is_beam_out_;};
  
  void AddStepID( G4int id ){ step_IDs_.push_back( id );}; // there is no step ID... ...
  std::vector < G4int > GetStepIDs() const { return step_IDs_;};

private:
  // data members
  G4int upordown_;
  G4int silicon_type_;
  G4int xposition_;
  G4int yposition_;
  G4int zposition_;
  G4double hit_time_;
  G4double edep_total_;
  G4double edep_step_;

  // track
  G4Track* track_;
  
  // step information
  G4Step* step_in_;
  G4Step* step_out_;
  G4Step* step_max_; // a step deposited maximum energy
  
  G4int          layerNumber_;
  G4ThreeVector  position_;

  bool is_beam_in_ = false;
  bool is_beam_out_ = false;
  std::vector < G4int > step_IDs_;

  void PrintStep( G4Step* step ); //
};

typedef G4THitsCollection < EDChamberHit > EDChamberHitsCollection;

extern G4ThreadLocal G4Allocator<EDChamberHit>* EDChamberHitAllocator;

inline void* EDChamberHit::operator new(size_t)
{
  if (! EDChamberHitAllocator)
    EDChamberHitAllocator = new G4Allocator<EDChamberHit>;
  
  return (void*)EDChamberHitAllocator->MallocSingle();
}

inline void EDChamberHit::operator delete(void* hit)
{
  EDChamberHitAllocator->FreeSingle((EDChamberHit*) hit);
}

#endif


