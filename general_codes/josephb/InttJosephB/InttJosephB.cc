#include "InttJosephB.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>

InttJosephB::InttJosephB(std::string const& name):
 SubsysReco(name)
{
  std::cout << "InttJosephB::InttJosephB(const std::string &name)" << std::endl;
}

InttJosephB::~InttJosephB()
{
  std::cout << "InttJosephB::~InttJosephB()" << std::endl;
}

int InttJosephB::Init(PHCompositeNode*)// *topNode)
{
  std::cout << "InttJosephB::Init(PHCompositeNode *topNode)" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttJosephB::InitRun(PHCompositeNode*)// *topNode)
{
  std::cout << "InttJosephB::InitRun(PHCompositeNode *topNode)" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttJosephB::process_event(PHCompositeNode*)// *topNode)
{
  std::cout << "InttJosephB::process_event(PHCompositeNode *topNode)" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttJosephB::ResetEvent(PHCompositeNode*)// *topNode)
{
  std::cout << "InttJosephB::ResetEvent(PHCompositeNode *topNode)" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttJosephB::EndRun(int const runnumber)
{
  std::cout << "InttJosephB::EndRun(const int runnumber)" << runnumber << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttJosephB::End(PHCompositeNode*)// *topNode)
{
  std::cout << "InttJosephB::End(PHCompositeNode *topNode)" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttJosephB::Reset(PHCompositeNode*)// *topNode)
{
  std::cout << "InttJosephB::Reset(PHCompositeNode *topNode)" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttJosephB::Print(std::string const& what) const
{
  std::cout << "InttJosephB::Print(const std::string &what)" << what << std::endl;
}
