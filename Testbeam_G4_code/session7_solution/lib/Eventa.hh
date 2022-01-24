#pragma once

#include "TObject.h"
#include "TrackMC.hh"

typedef int HitMC;

class Eventa : public TObject
{
private:
  int eventa_id_;
  vector < TrackMC* > tracks_MC_;
  vector < HitMC* > hits_MC_; // should be HitMC class
  void PrintLine( string header, string contents, int total_num, char padding_word = ' ' );
  
public:
  Eventa();
  Eventa( int id );
  ~Eventa();
  
  void AddTrackMC( TrackMC* track );

  int      GetNTrackMC(){ return tracks_MC_.size();};
  TrackMC* GetTrackMC( int id );
  
  int      GetNHitMC(){ return hits_MC_.size();};
  //  HitMC GetHitMC( int id );
  
  void Print( int level= 0 );
  ClassDef( Eventa, 1 );

};
