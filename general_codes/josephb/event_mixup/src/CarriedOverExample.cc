#include "CarriedOverExample.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <ffarawobjects/InttRawHitContainerv2.h>
#include <ffarawobjects/InttRawHitv2.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h> // PHWHERE

#include <iomanip> // for setw
#include <sstream>

CarriedOverExample::CarriedOverExample(const std::string &name):
 SubsysReco(name)
{
  // Do nothing
}

CarriedOverExample::~CarriedOverExample()
{
  for (auto* rawhit_container : m_prev_rawhit_containers)
  {
    delete rawhit_container;
  }
}

int CarriedOverExample::Init(PHCompositeNode *topNode)
{
  // Do nothing
  return Fun4AllReturnCodes::EVENT_OK;
}

int CarriedOverExample::InitRun(PHCompositeNode *topNode)
{
  // Do nothing
  return Fun4AllReturnCodes::EVENT_OK;
}

int CarriedOverExample::process_event(PHCompositeNode *topNode)
{
  get_intt_rawhit_nodes(topNode);

  Print();

  // Increment our reference event counter
  ++m_event;
  return Fun4AllReturnCodes::EVENT_OK;
}

int CarriedOverExample::ResetEvent(PHCompositeNode *topNode)
{
  // Do nothing
  return Fun4AllReturnCodes::EVENT_OK;
}

int CarriedOverExample::EndRun(const int runnumber)
{
  // Do nothing
  return Fun4AllReturnCodes::EVENT_OK;
}

int CarriedOverExample::End(PHCompositeNode *topNode)
{
  // Do nothing
  return Fun4AllReturnCodes::EVENT_OK;
}

int CarriedOverExample::Reset(PHCompositeNode *topNode)
{
  // Do nothing
  return Fun4AllReturnCodes::EVENT_OK;
}

void CarriedOverExample::Print(const std::string &what) const
{
  std::cout
    << PHWHERE << "\n";
  for (unsigned int n = 0, N = m_prev_rawhit_containers.size(); n < N; ++n)
  {
    std::cout
      << "\tEvent: " << std::setw(8) << m_event - n
	  << "\tHits: " << std::setw(8) << m_prev_rawhit_containers[n]->get_nhits() << "\n"
      << std::flush;
  }
}

std::vector<std::string> const CarriedOverExample::m_intt_rawhit_container_node_names =
{
  // If all the rawhits have been grouped in a single node in your DSTs
  // "INTTRAWHIT",

  // Otherwise, by server:
  "INTTRAWHIT_0",
  "INTTRAWHIT_1",
  "INTTRAWHIT_2",
  "INTTRAWHIT_3",
  "INTTRAWHIT_4",
  "INTTRAWHIT_5",
  "INTTRAWHIT_6",
  "INTTRAWHIT_7",
};

void CarriedOverExample::get_intt_rawhit_nodes(PHCompositeNode *topNode)
{
  auto* tmp_rawhit_container = new InttRawHitContainerv2;

  std::stringstream verbosity_printout;
  verbosity_printout << PHWHERE << "\n";

  // Get hits from all the raw hit container nodes
  for (auto const& node_name : m_intt_rawhit_container_node_names)
  {
    auto* intt_rawhit_container = dynamic_cast<InttRawHitContainerv2*>(findNode::getClass<InttRawHitContainer>(topNode, node_name));
    if (!intt_rawhit_container) {
      std::cout
        << PHWHERE << "\n"
        << "\tCouldn't get node:\n"
        << "\t" << node_name << "\n"
        << "\tExiting\n"
        << std::flush;
      exit(1);
    }

    verbosity_printout << "\tGot node " << node_name;
	verbosity_printout << "\twith " << std::setw(4) << intt_rawhit_container->get_nhits() << " hits";
	if (intt_rawhit_container->get_nhits())
	{
      verbosity_printout << "\tleading BCO " << std::setw(10) << std::hex << intt_rawhit_container->get_hit(0)->get_bco() << std::dec;
	}
	verbosity_printout << "\n";

    for (unsigned int n = 0, N = intt_rawhit_container->get_nhits(); n < N; ++n)
    {
      auto* tmp_rawhit = tmp_rawhit_container->AddHit();
      *tmp_rawhit = *intt_rawhit_container->get_hit(n);
    }
  }

  // Note we are contain all the raw hits from all the rawhit nodes
  m_prev_rawhit_containers.insert(m_prev_rawhit_containers.begin(), tmp_rawhit_container);

  // Keep the program image size from growing too large
  for (unsigned int n = m_max_size, N = m_prev_rawhit_containers.size(); n < N; ++n)
  {
    delete m_prev_rawhit_containers[n];
    m_prev_rawhit_containers.pop_back();
  }

  // At this point, the contents of m_prev_rawhit_containers
  // are hit containers which have the hits from the past m_max_size events
  // m_prev_rawhit_containers[0] = this event's hits
  // m_prev_rawhit_containers[1] = prev event's hits
  // ...
  // m_prev_rawhit_containers[n] = hits from n events ago

  if (0 < Verbosity())
  {
    std::cout
      << verbosity_printout.str()
      << std::flush;
  }
}
