#ifndef GETGEOM_H__
#define GETGEOM_H__

#include <fun4all/SubsysReco.h>

/// Class declarations for use in the analysis module
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;
class PHCompositeNode;
class RawClusterContainer;
class RawCluster;
class SvtxTrackMap;
class JetMap;
class GlobalVertex;
class PHHepMCGenEventMap;
class JetEvalStack;
class JetRecoEval;
class SvtxTrackEval;
class PHG4TruthInfoContainer;
class PHHepMCGenEvent;
class CaloTriggerInfo;
class JetTruthEval;
class SvtxEvalStack;
class JetEvalStack;

/// Definition of this analysis module class
class GetGeom : public SubsysReco
{
 public:
  /// Constructor
  GetGeom(const std::string &name = "GetGeom",
          const std::string &fname = "GetGeom.root");

  // Destructor
  virtual ~GetGeom();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

 private:
  /// String to contain the outfile name containing the trees
  std::string m_outfilename;

  /// TFile to hold the following TTrees and histograms
  TFile *m_outfile;
  TTree *m_clustertree;

  /**
   * Make variables for the relevant trees
   */

};

#endif
