#ifndef INTTSAVETREE_H__
#define INTTSAVETREE_H__

#include <fun4all/SubsysReco.h>
#include <string>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TH1;
class TH2;
class TNtuple;
class TTree;
class InttRawData;

class InttEvent;
class InttOfflineEvent;
class InttOfflineClusterList;

/// Definition of this analysis module class
class InttSaveTree : public SubsysReco
{
 public:
  /// Constructor
  InttSaveTree(const std::string &name = "InttSaveTreeAna",
               const std::string &fname = "AnaTutorial.root");

  // Destructor
  virtual ~InttSaveTree();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
 
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);


  void setInttRawData(InttRawData *rawModule) { _rawModule = rawModule; }

  void readRawHit(PHCompositeNode *);

 private:
  InttRawData* _rawModule;

  std::string fname_;
  TFile* anafile_;
  TTree* tree_; 

  InttEvent*              inttEvtCp_;
  InttOfflineEvent*       inttOfflineEvt_;
  InttOfflineClusterList* inttOfflineCls_;
  InttOfflineClusterList* emcOfflineCls_;
  InttOfflineClusterList* mvtxOfflineCls_;

};

#endif
