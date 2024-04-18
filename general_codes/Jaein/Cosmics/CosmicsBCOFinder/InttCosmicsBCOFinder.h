#ifndef INTTCOSMICSBCOFINDER_H__
#define INTTCOSMICSBCOFINDER_H__

#include <TObject.h>
#include <vector>
#include <filesystem>
#include <array>
#include <iostream>
#include <iomanip> // setw, setfill


#include <cdbobjects/CDBTTree.h>
#include <fun4all/SubsysReco.h>
R__LOAD_LIBRARY(libcdbobjects.so)
class PHCompositeNode;
class TFile;
class TTree;
class TH2D;

class InttCosmicsBCOFinder : public SubsysReco
{
  public:
  InttCosmicsBCOFinder(const std::string &name = "InttCosmicsBCOFinder", const std::string &fname = "outputfile.root",const std::string &fname2 = "cdbfile.root",int nevent = 10000);

  virtual ~InttCosmicsBCOFinder();

  int Init(PHCompositeNode *);
  
  int InitRun(PHCompositeNode *);
  
  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);
  bool IsADCcutON_ = false;
  bool WriteCDBTTree_ = false;
  bool WriteQAFile_ = false;
  void FindBCOPeak();
  void ADCCut(const bool flag) { IsADCcutON_ = flag; }
  void WriteCDBTTree(const bool flag) { WriteCDBTTree_ = flag; }
  void WriteQAFile(const bool flag) { WriteQAFile_ = flag; }
  int LoadHotChannelMapLocal(std::string const & = "INTT_HotChannelMap.root");
  std::string m_InttRawNodeName = "INTTRAWHIT";
  TFile *outFile_ = nullptr;
  TTree *tree_ = nullptr;
  CDBTTree *cdbttree_ = nullptr;
  int nevents_;
  int ievent_;
  std::string outfname_;
  std::string cdbname_;
  TH2D *h2_bco_ladder_[8];     // histogram for BCO alignment check half ladder by half ladder
  TH2D *h2_bco_ladder_cut_[8]; // histogram after BCO cuto
 private:
  typedef std::array<std::array<std::array<std::array<int, 128>, 26>, 14>, 8> ChanArray;

  ChanArray m_hot{};  // [FELIX_SERVER:8][FELIX_CHANNEL:14][CHIP:26][chan:128]

};
#endif