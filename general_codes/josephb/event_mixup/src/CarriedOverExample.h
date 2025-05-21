// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CARRIEDOVEREXAMPLE_H
#define CARRIEDOVEREXAMPLE_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class InttRawHitContainerv2;

class CarriedOverExample : public SubsysReco
{
 public:

  CarriedOverExample(const std::string &name = "CarriedOverExample");

  ~CarriedOverExample() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode*) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode*) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode*) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode*) override;

  /// Called at the end of each run.
  int EndRun(const int) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode*) override;

  /// Reset
  int Reset(PHCompositeNode*) override;

  void Print(std::string const& = "ALL") const override;

 private:
  void get_intt_rawhit_nodes(PHCompositeNode*);

  int m_max_size{4}; // Store up to m_max_size consecutive events
  int m_event{0};

  static std::vector<std::string> const m_intt_rawhit_container_node_names;
  std::vector<InttRawHitContainerv2*> m_prev_rawhit_containers;
};

#endif // CARRIEDOVEREXAMPLE_H
