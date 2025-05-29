#ifndef INTTCLUSANA_H__
#define INTTCLUSANA_H__

#include <fun4all/SubsysReco.h>
#include <trackbase/ActsGeometry.h>
//
#include <string>
#include <vector>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TH1;
class TH2;
class TNtuple;
class TTree;
class InttRawData;
class TrkrCluster;

/// Definition of this analysis module class
class InttClusAna : public SubsysReco
{
 public:
  /// Constructor
  InttClusAna(const std::string &name = "InttAna",
          const std::string &fname = "AnaTutorial.root");

  // Destructor
  virtual ~InttClusAna();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
 
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);


 private:

};

#endif
