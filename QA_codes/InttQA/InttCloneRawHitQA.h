// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

#include "InttRawHitQA.h"

class InttCloneRawHitQA : public InttRawHitQA
{
private:
  std::vector < InttRawHit* > GetHits() override;
  // bool to check the clone hit 
  bool clone_hit_flag_[ kFelix_num_ ][ kFee_num_ ][ kChip_num_ ][ kChan_num_ ]{};

public:
  InttCloneRawHitQA(const std::string &name = "InttCloneRawHitQA", bool is_official=true );

  ~InttCloneRawHitQA() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  //virtual int InitRun(PHCompositeNode *topNode);

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  //int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  //int End(PHCompositeNode *topNode) override;

  /// Reset
  //int Reset(PHCompositeNode * /*topNode*/) override;

  //virtual void Print(const std::string &what = "ALL") const;
  
};

