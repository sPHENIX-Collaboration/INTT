#ifndef INTTSTREAMRECOF4A_H__
#define INTTSTREAMRECOF4A_H__

#include <fun4all/SubsysReco.h>
#include <TObject.h>

#include <set>
#include <string>

#include <intt/InttMapping.h>


/// Class declarations for use in the analysis module
class PHCompositeNode;

class InttRawHit;
class InttRawHitContainer;

class TH1;
class TH2;
class InttBadChannelMap;
class InttBCOMap;
class InttFeeMap;

/// Definition of this analysis module class
class InttStreamRecoF4A : public SubsysReco
{
 public:
  /*!
  @brief Constructor
  @param name Used to initialize Subsysreco
  @param fname It's assigned to fname_
  */
  InttStreamRecoF4A(const std::string &name = "InttStreamRecoF4A");

  // Destructor
  virtual ~InttStreamRecoF4A();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  //void SetBadChannelMap(const std::string& file);

  int  LoadHotChannelMapLocal(std::string const& filename);
  void SetBcoMap(const std::string& file);

 private:
   //InttBadChannelMap* m_badmap{nullptr};
   InttBCOMap*        m_bcomap{nullptr};
   InttFeeMap*        m_feemap{nullptr};

   typedef std::set<InttNameSpace::RawData_s, InttNameSpace::RawDataComparator> Set_t;
   Set_t m_HotChannelSet;

   int m_bcopeak[8][14]{0};

 protected:
   TH2* h_bcodiff[8]{nullptr};
   TH2* h_bco[8]{nullptr};
   TH2* h_hit[8]{nullptr};

   TH1* h_bco_felix[8]{nullptr}; // FPHX bco
   TH1* h_bco_all{nullptr};
   TH1* h_bcogl1diff_felix[8]{nullptr}; // bcofull - gl1bco for all hits

   TH1* h_bcoreco_diff[8]{nullptr};
   TH1* h_bcoreco_evt_diff[8]{nullptr};
   TH1* h_bcoreco_evt_diff_all{nullptr};

   TH1* h_bcorecointt_diff[8]{nullptr};
   TH1* h_bcointtgl1_diff{nullptr};

   TH1* h_bunch[8]{nullptr};
   TH1* h_bunch_all{nullptr};
   TH1* h_bunch_gl1{nullptr};
   TH1* h_bunch_all_trig{nullptr};
   TH1* h_bunch_gl1_trig{nullptr};

   TH1* h_bunch_strb[8]{nullptr};
   TH2* h_bunch_evt_bcodiff[8]{nullptr};
   TH2* h_bunch_bco[8]{nullptr};

   TH1* h_bcoprediff[8]{nullptr};
   TH1* h_bco_variation[8]{nullptr};
   TH1* h_bcogl1pre_diff{nullptr};


};

#endif
