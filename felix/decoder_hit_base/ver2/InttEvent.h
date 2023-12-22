#ifndef __INTTEVENT_H__
#define __INTTEVENT_H__

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

//#include "TTree.h"
//#include "Riostream.h"
#include "TClonesArray.h"
#include "TObject.h"
#include "TClonesArray.h"

using namespace std;

class InttHit : public TObject {
public:
  InttHit();
  virtual ~InttHit(){};
  //    InttHit(int Pid);

  void copy(InttHit& hit);
  void show(bool explanation_flag=false);
  bool parameterCheck();
  void setCloneFlag( bool flag ){ is_clone = flag; };
  void setNoiseFlag( bool flag ){ is_noise = flag; };
  static bool is_first( const InttHit* hit1, const InttHit* hit2 );
  static bool is_equal( const InttHit* hit1, const InttHit* hit2 );
  
  virtual Bool_t IsEqual(const TObject *obj) const;
  virtual Bool_t IsEqualNoAdc(const TObject *obj) const;
  virtual Bool_t IsSortable() const;
  virtual Int_t  Compare(const TObject* obj) const;
  
public:
  int pid;

  int adc;
  int ampl;
  int chip_id;
  //fpga
  int module;
    
  int chan_id;
  //fem
    
  int bco;
  Long64_t bco_full;
    
  int evt;
    
  int roc;
  int barrel;
  int layer;
  int ladder;
  int arm;
    
  int full_fphx;
  int full_roc;

  bool is_clone = false;
  bool is_noise = false;
  
  //  protected:
#ifdef __CINT__
  ClassDef(InttHit, 1)
#endif // __CINT__
};

class InttEvent : public TObject {
public:
  InttEvent();
  virtual ~InttEvent();

  InttHit* addHit( TClonesArray* hit_array, InttHit* hit );
  InttHit* addHit();
  void     clear();
  void     copy(InttEvent* org);

  int      getNHits();
  int      getNHits( bool without_clone, bool without_noise );
  int      getNHitsNoClone();
  int      getNHitsNoNoise();
  int      getNHitsNoCloneNoise();
  InttHit* getHit(const int ihit);

  // int      getNHits( bool with_clone = true, bool with_noise = true );
  InttHit* getHit(const int ihit, bool without_clone, bool without_noise);
  InttHit* getHitTemp(const int ihit, bool without_clone, bool without_noise);

  void     show();
  void     showList();
  void     showList( bool without_clone, bool without_noise );
  void     showListNoClone();
  void     sort();

  //protected:
  int           evtSeq;
  Long64_t      bco;
  int           fNhits;
  int           fNhits_no_clone;
  int           fNhits_no_noise;
  int           fNhits_no_clone_noise;
  
  TClonesArray* fhitArray;
  TClonesArray* fhitArray_no_clone;
  TClonesArray* fhitArray_no_noise;
  
  string noisy_map;
  vector < InttHit* > noise_hits;

  void checkCloneHit();
  void checkNoiseHit();

  bool setNoisyMap( string noisy_map_ );
  void showNoisyChannels();

#ifdef __CINT
  ClassDef(InttEvent, 2)
#endif // __CINT__

};

#endif
