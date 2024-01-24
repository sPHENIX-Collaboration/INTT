#ifndef INTTRAWDATA_H__
#define INTTRAWDATA_H__

#include <fun4all/SubsysReco.h>
#include <InttHotMap.h>
#include <TObject.h>

#include <set>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TTree;
class InttEvent;

/// Definition of this analysis module class
class InttRawData : public SubsysReco
{
 public:
  /*!
  @brief Constructor
  @param name Used to initialize Subsysreco
  @param fname It's assigned to fname_
  */
  InttRawData(const std::string &name = "InttRawData",
              const std::string &fname = "inttevent.root");

  // Destructor
  virtual ~InttRawData();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  void SetHotMapFile(const char* filename) { hotfilename_ = std::string(filename); }

  void SetDACfile(const char* dacfile){ dacfile_ = std::string(dacfile); }
  void SetBCOfile(const char* bcofile){ bcofile_ = std::string(bcofile); }
  void SetAdc7RemovalFlag(const bool flag){ isAdc7Removal_ = flag; }

  InttEvent* getInttEvent(){ return inttEvt_; }

  struct OfflineRawdata{
    int layer;
    int ladder_z;
    int ladder_phi;
    int bco;
    int chip;
    int chan;
    int adc;

    void set(int Layer, int Ladder_z, int Ladder_phi, int Bco, int Chip, int Chan, int Adc){
      layer       = Layer;
      ladder_z    = Ladder_z;
      ladder_phi  = Ladder_phi;
      bco         = Bco;
      chip        = Chip;
      chan        = Chan;
      adc         = Adc;
    }
  };

  // 
  bool checkBCOBg(int felix, int ladder, int bco, Long64_t bcofull);

  /// The path to the input file. The file must have TTree "tree".
  std::string fname_;
  std::string hotfilename_;

  InttHotMap hotmap_;

  TFile*      inFile_ = nullptr;
  TTree*      tree_;
  InttEvent*  inttEvt_;
  int         ievent_;
  std::string dacfile_;
  std::string bcofile_;
  bool        isAdc7Removal_;

  int           dacpar_[8]; // 8 different thresholds
  std::set<int> bcopar_[8]; // 8 felix

};

#endif
