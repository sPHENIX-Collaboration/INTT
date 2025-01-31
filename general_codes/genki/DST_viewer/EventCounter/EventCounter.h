// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EVENTCOUNTER_H
#define EVENTCOUNTER_H

#include <string>
#include <iostream>
#include <iomanip>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>

class PHCompositeNode;

class EventCounter : public SubsysReco
{
 public:

  EventCounter(const std::string &name = "EventCounter");

  ~EventCounter() override;

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  int GetEventCounter(){ return event_counter_;};
  
 private:
  int event_counter_ = 0;
};

#endif // EVENTCOUNTER_H
