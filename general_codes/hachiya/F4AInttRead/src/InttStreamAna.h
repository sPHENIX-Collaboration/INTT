#ifndef INTTSTREAMANA_H__
#define INTTSTREAMANA_H__

#include <fun4all/SubsysReco.h>
#include <TObject.h>

#include <set>
#include <string>

/// Class declarations for use in the analysis module
class PHCompositeNode;

class InttRawHit;
class InttRawHitContainer;

class TFile;
class TTree;
class TH1;
class TH2;

/// Definition of this analysis module class
class InttStreamAna : public SubsysReco
{
 public:
  /*!
  @brief Constructor
  @param name Used to initialize Subsysreco
  @param fname It's assigned to fname_
  */
  InttStreamAna(const std::string &name = "InttStreamAna");

  // Destructor
  virtual ~InttStreamAna();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  void SetGl1File(const std::string& sfname){ m_gl1_fname = sfname; }

 protected:
   std::string m_gl1_fname;

   TFile*    m_gl1file{nullptr};
   TTree*    m_t_evtgl1{nullptr};
   int       m_evt_gl1;
   ULong64_t m_bco_gl1;
   int       m_bunch_gl1;

   int       m_gl1_ievent{0};


   TH2* h_bco[8]{nullptr};
   TH1* h_bco_all{nullptr};

   TH1* h_bcoreco_diff[8]{nullptr};
   TH1* h_bcoreco_evt_diff[8]{nullptr};

   TH1* h_bunch[8]{nullptr};
   TH1* h_bunch_all{nullptr};
   TH1* h_bunch_gl1{nullptr};

   TH1* h_bunch_strb[8]{nullptr};
   TH2* h_bunch_evt_bcodiff[8]{nullptr};
   TH2* h_bunch_bco[8]{nullptr};

   TH1* h_bcoprediff[8]{nullptr};
};

#endif
