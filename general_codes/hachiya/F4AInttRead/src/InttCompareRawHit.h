#ifndef INTTCOMPARERAWHIT_H__
#define INTTCOMPARERAWHIT_H__

#include <fun4all/SubsysReco.h>
#include <TObject.h>

#include <set>

/// Class declarations for use in the analysis module
class PHCompositeNode;

class InttRawHit;
class InttRawHitContainer;

/// Definition of this analysis module class
class InttCompareRawHit : public SubsysReco
{
 public:
  /*!
  @brief Constructor
  @param name Used to initialize Subsysreco
  @param fname It's assigned to fname_
  */
  InttCompareRawHit(const std::string &name = "InttCompareRawHit");

  // Destructor
  virtual ~InttCompareRawHit();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);


 protected:
  bool Compare(InttRawHitContainer* rawmap1, InttRawHitContainer* rawmap2);
  bool Compare(InttRawHit* raw1, InttRawHit* raw2);

  template<typename T> bool compare(const T& val1, const T& val2, std::string label);

};

#endif
