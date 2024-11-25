#ifndef INTTANA_H__
#define INTTANA_H__

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

/// Definition of this analysis module class
class InttAna : public SubsysReco
{
 public:
  /// Constructor
  InttAna(const std::string &name = "InttAna",
          const std::string &fname = "AnaTutorial.root");

  // Destructor
  virtual ~InttAna();

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

  void setBeamCenter(float x=0, float y=0) {xbeam_=x; ybeam_=y;}

 private:
  InttRawData* _rawModule;

  std::string fname_;
  TFile* anafile_;
  TH1*   h_dca2d_zero; 
  TH2*   h2_dca2d_zero; 
  TH2*   h2_dca2d_len; 
  TH1*   h_zvtx;
  TNtuple*  h_ntp_clus; 
  //TTree*  h_t_clus; 
  TNtuple*  h_ntp_cluspair; 

  float xbeam_;
  float ybeam_;

  TH1* h_zvtxseed_;

};

#endif
