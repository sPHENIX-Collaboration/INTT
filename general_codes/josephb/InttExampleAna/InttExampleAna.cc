#include "InttExampleAna.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
InttExampleAna::InttExampleAna(const std::string &name):
 SubsysReco(name)
{
  std::cout << "InttExampleAna::InttExampleAna(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
InttExampleAna::~InttExampleAna()
{
  std::cout << "InttExampleAna::~InttExampleAna() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int InttExampleAna::Init(PHCompositeNode * /*topNode*/)
{
  std::cout << "InttExampleAna::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttExampleAna::InitRun(PHCompositeNode * /*topNode*/)
{
  std::cout << "InttExampleAna::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttExampleAna::process_event(PHCompositeNode * /*topNode*/)
{
  std::cout << "InttExampleAna::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttExampleAna::ResetEvent(PHCompositeNode * /*topNode*/)
{
  std::cout << "InttExampleAna::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttExampleAna::EndRun(const int runnumber)
{
  std::cout << "InttExampleAna::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttExampleAna::End(PHCompositeNode * /*topNode*/)
{
  std::cout << "InttExampleAna::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttExampleAna::Reset(PHCompositeNode * /*topNode*/)
{
 std::cout << "InttExampleAna::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void InttExampleAna::Print(const std::string &what) const
{
  std::cout << "InttExampleAna::Print(const std::string &what) const Printing info for " << what << std::endl;
}
