#pragma once

class EventResidual
{
  
public:

  EventResidual( int num, double chan0, double chan1, double chan2 );
  EventResidual( TGraph* g );

  double GetChannel( int i ){ return chan_id_[i];};
  double GetChannelTracking( int i ){ return track_->Eval( i );};
  TF1* GetTrack(){ return track_;};
  TGraph* GetGraph(){ return g_;};

  double GetResidual( int ladder )
  {
    return this->GetChannel(ladder)  - this->GetChannelTracking( ladder );
  };
  
  void Print();
  
private:
  TF1* track_;
  double chan_id_[3];
  TGraph* g_;
  
  void Init();
  
};

#ifdef __CINT__
#include "EventResidual.cc"
#endif
