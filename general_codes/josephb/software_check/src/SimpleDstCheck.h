// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SIMPLEDSTCHECK_H
#define SIMPLEDSTCHECK_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;

class SimpleDstCheck : public SubsysReco
{
 public:

  SimpleDstCheck(const std::string &name = "SimpleDstCheck");
  ~SimpleDstCheck() override;

  int process_event(PHCompositeNode*) override;

 private:
  std::string m_trkr_hitset_node_name{"TRKR_HITSET"};
  std::string m_trkr_cluster_node_name{"TRKR_CLUSTER"};
};

#endif // SIMPLEDSTCHECK_H
