
#include "EventCounter.h"


EventCounter::EventCounter(const std::string &name):
 SubsysReco(name)
{

}

EventCounter::~EventCounter()
{

}

int EventCounter::Init(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventCounter::InitRun(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventCounter::process_event(PHCompositeNode *topNode)
{
  event_counter_++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int EventCounter::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventCounter::EndRun(const int runnumber)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventCounter::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventCounter::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void EventCounter::Print(const std::string &what) const
{

}
