#include "SimpleDstCheck.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <trackbase/TrkrHitSetContainerv1.h>
#include <trackbase/TrkrClusterContainerv4.h>

#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>

SimpleDstCheck::SimpleDstCheck (
	std::string const& name
): SubsysReco(name) {
	// Do nothing
}

SimpleDstCheck::~SimpleDstCheck (
) {
	// Do nothing
}

int SimpleDstCheck::process_event (
	PHCompositeNode* top_node
) {
	auto trkr_hitset = findNode::getClass<TrkrHitSetContainer>(top_node, m_trkr_hitset_node_name);
	if (!trkr_hitset) return Fun4AllReturnCodes::ABORTRUN;

	auto trkr_cluster = findNode::getClass<TrkrClusterContainer>(top_node, m_trkr_cluster_node_name);
	if (!trkr_cluster) return Fun4AllReturnCodes::ABORTRUN;

	std::cout << "here" << std::endl;

	return Fun4AllReturnCodes::EVENT_OK;
}
