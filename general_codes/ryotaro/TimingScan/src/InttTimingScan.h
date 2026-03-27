// Tell emacs that this is a C++ source
//  -*- C++ -*-.

// Fun4All analysis module for INTT timing scan, made by Ryotaro Koike 20250501
#ifndef INTT_TIMING_SCAN_H_20250501
#define INTT_TIMING_SCAN_H_20250501


#include "TH1D.h"
#include "TTree.h"
#include "TFile.h"

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>

#include <phool/getClass.h>
#include <phool/recoConsts.h>

#include <trackbase/InttDefs.h>
#include <trackbase/InttEventInfo.h>
#include <trackbase/InttEventInfov1.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSetContainerv1.h>
//#include <trackbase/TrkrHitSetContainerv2.h>
#include <trackbase/TrkrHitSetv1.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitv1.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrDefs.h>



#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>

#include <trackbase/InttDefs.h>
#include <trackbase/InttEventInfo.h>
#include <trackbase/InttEventInfov1.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSetContainerv1.h>
//#include <trackbase/TrkrHitSetContainerv2.h>
#include <trackbase/TrkrHitSetv1.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitv1.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrDefs.h>

#include <ffaobjects/FlagSavev1.h>
#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainerv2.h>


#include <intt/InttMapping.h>
#include <intt/InttDacMap.h>


class InttTimingScan : public SubsysReco 
{
  public:  
    InttTimingScan(const std::string &name, int run);

    ~InttTimingScan();

    // mandatory methods for Fun4All analysis module
    int Init(PHCompositeNode *topNode) override;
    int InitRun(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int ResetEvent(PHCompositeNode *topNode) override;
    int EndRun(const int runnumber) override;
    int End(PHCompositeNode *topNode) override;
    int Reset(PHCompositeNode *topNode) override; // Cheng-Wei のコードでは    int Reset(PHCompositeNode * /*topNode*/) override;  となっている。引数がおかしくない？
    void Print(const std::string &what) const override;
  
    // The return value represents <Event Counter, FPHX BCO>.
    // Will be used in the process_event
    std::vector< std::pair<uint16_t, int> > GetBcoEventCounter();
    std::vector<InttRawHit*> GetRawHitsWithoutClone();
    std::vector<InttRawHit*> GetRawHits();
    // The return value represents <hit information, ADC value>.
    // Will be used in the process_event
    std::vector< std::pair<InttNameSpace::Online_s, unsigned int> > GetHits(TrkrHitSetContainer::ConstRange hitsets);
    bool IsSame(InttRawHit* hit1, InttRawHit* hit2);

    enum class Mode { RawHitOnly, RawPlusTrkr };
    void set_mode(Mode m) { mode_ = m; };

    void set_hot_channel_map_file(std::string input) {file_path_to_hot_channel_map_ = input;};

    enum class DaqMode { Official, Local };
    void set_daq_mode(DaqMode daq_mode) { daq_mode_ = daq_mode;};

  private:
    TFile* output_file_;
    TTree* t_output_;
    TH1D* h_bco_diff_;
    TrkrHitSetContainer* node_trkrhitset_map_;
    InttRawHitContainerv2* node_inttrawhit_map_;
    uint16_t branch_bco_diff_;
    int run_;
    int my_event_counter_;

    Mode mode_ = Mode::RawHitOnly;
    int process_event_inttrawhit_only(PHCompositeNode *topNode);
    int process_event_raw_plus_trkrhit(PHCompositeNode *topNode);

    DaqMode daq_mode_ = DaqMode::Local;

    std::string node_name_intt_raw_hit_{"INTTRAWHIT"};

    std::string tree_name_hot_channel_{"Multiple"};
    std::string file_path_to_hot_channel_map_;
    std::map<std::string,std::string> hot_channel_map_;
    void PrepareHotChannelMap();

    void FillHist(InttRawHitContainerv2* container_inttrawhit);

};

#endif //INTT_TIMING_SCAN_H_20250501