#ifndef INTTBCORECO_H__
#define INTTBCORECO_H__

#include <fun4all/SubsysReco.h>
#include <TObject.h>

#include <set>
#include <string>
#include <vector>

#include <intt/InttMapping.h>


/// Class declarations for use in the analysis module
class PHCompositeNode;

class InttRawHit;
class InttRawHitContainer;

class TFile;
class TTree;
class TH1;
class TH2;
class InttBadChannelMap;
class InttBCOMap;
class InttFeeMap;

/// Definition of this analysis module class
class InttBcoReco : public SubsysReco
{
 public:
  /*!
  @brief Constructor
  @param name Used to initialize Subsysreco
  @param fname It's assigned to fname_
  */
  InttBcoReco(const std::string &name = "InttStreamReco", const std::string &outfile="hdiffout.root");

  // Destructor
  virtual ~InttBcoReco();

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

   std::vector<InttRawHit*> m_event_bco[128]{};

 protected:
   std::string m_outfile;
   TFile* m_froot;
   TTree* m_t_evtgl1;
  
   int      m_evt_gl1;
   uint64_t m_bco_gl1;
   int      m_bunch_gl1;

   TH2* h_bcodiff[8]{nullptr};
   TH2* h_bco[8]{nullptr};
   TH2* h_hit[8]{nullptr};

   TH1* h_bco_felix[8]{nullptr};
   TH1* h_bco_all{nullptr};

   //TH1* h_bcodiff_gl1strb[8]{nullptr};

};

#endif
