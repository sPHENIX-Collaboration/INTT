#ifndef INTTANA_H__
#define INTTANA_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <TTree.h>

/// Cluster/Calorimeter includes
#include <ffaobjects/EventHeaderv1.h>

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <trackbase/TrkrClusterv4.h>
//#include <trackbase/TrkrClusterContainerv3.h>
//#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>

/// ROOT includes
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TTree.h>
#include <Math/Vector3Dfwd.h>

/// C++ includes
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>
#include <iomanip>

//#include "InttEvent.h"
//#include "InttRawData.h"

#include "event.hh"

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TH1;
class TH2;
class TNtuple;
class TTree;
class InttRawData;

/// Definition of this analysis module class
class InttAnalysisCosmic : public SubsysReco
{
public:
  /// Constructor
  InttAnalysisCosmic(const std::string &name = "InttAnalysisCosmic", const std::string &filename = "output.root" );
  
  // Destructor
  virtual ~InttAnalysisCosmic();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
 
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

private:
  int event_counter_ = 0;
  
  //  InttRawData* _rawModule;

  std::string fname_;
  TFile* anafile_;
  // TH1*   h_dca2d_zero; 
  // TH2*   h2_dca2d_zero; 
  // TH2*   h2_dca2d_len; 
  // TH1*   h_zvtx;
  TNtuple*  h_ntp_clus; 
  //TTree*  h_t_clus; 
  TNtuple*  h_ntp_cluspair;
  
  TTree* tree_;
  event* ev_;
  //TH1* h_zvtxseed_;

  void Analysis( std::vector < TrkrCluster* >& clusters );

  void CleanVectors();
  
  template < class T >
  void CleanVector( std::vector < T* >& vec );
};

#endif
