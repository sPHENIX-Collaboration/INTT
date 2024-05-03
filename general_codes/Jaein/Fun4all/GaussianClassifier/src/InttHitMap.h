#ifndef INTTHITMAP_H__
#define INTTHITMAP_H__

#include <fun4all/SubsysReco.h>
#include <TObject.h>
class PHCompositeNode;
class TFile;
class TTree;
class TH2D;
class InttEvent;

class InttHitMap : public SubsysReco
{
  public:
  /// @brief Constructor
  /// @param name Subsysreco
  /// @param fname input file(evt) name
  /// @param fname2 output file name
  /// @param nevents # of events processed
  InttHitMap(const std::string &name = "InttHitMap", const std::string &fname = "inttevent.root",const std::string &fname2 = "outputfile.root",int nevents=0);

  virtual ~InttHitMap();

  int Init(PHCompositeNode *);
  
  int InitRun(PHCompositeNode *);
  
  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);
  
  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  bool isBCOPeak(int felix,int ladder, int bco, Long64_t bcofull);
  void SetBCOcut(const bool flag){ isBCOcutON_ = flag; }
  int SetBCOfile(const char* bcofile);
  InttEvent* getInttEvent() { return inttEvt_; }
  
  bool isBCOcutON_ = false;
  std::string fname_;
  std::string bcofname_;
  std::string outfname_;
  TFile* inFile_ = nullptr;
  TFile* inBCOFile_ = nullptr;
  TFile* outFile_ = nullptr;
  TTree* tree_ = nullptr;
  InttEvent* inttEvt_;
  int nevents_;
  int ievent_;
  TH2D* h2_AllMap_[8][14];
  TH2D* h2_bco_cut_[8];
  bool IsCloneHit_[8][14][27][128];
};

#endif