#ifndef INTTSKIPEVENT_H__
#define INTTSKIPEVENT_H__

#include <fun4all/SubsysReco.h>
#include <TObject.h>

#include <set>

/// Class declarations for use in the analysis module
class PHCompositeNode;

class InttRawHit;
class InttRawHitContainer;

/// Definition of this analysis module class
class InttSkipEvent : public SubsysReco
{
 public:
  /*!
  @brief Constructor
  @param name Used to initialize Subsysreco
  @param fname It's assigned to fname_
  */
  InttSkipEvent(const std::string &name = "InttSkipEvent");

  // Destructor
  virtual ~InttSkipEvent();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);


 protected:

};

#endif
