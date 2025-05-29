#ifndef MYTIMER_H__
#define MYTIMER_H__

#include <fun4all/SubsysReco.h>
#include <phool/PHTimer.h>
#include <TObject.h>

#include <set>
#include <string>
#include <vector>



/// Class declarations for use in the analysis module
class PHCompositeNode;

class TFile;
class TTree;

/// Definition of this analysis module class
class MyTimer : public SubsysReco
{
 public:
  /*!
  @brief Constructor
  @param name Used to initialize Subsysreco
  @param fname It's assigned to fname_
  */
  MyTimer(const std::string &name = "MyTimer", const std::string &outfile="mytimer.root");

  // Destructor
  virtual ~MyTimer();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  void setNevents(const int nevt=1) {m_nevt = nevt; }


 protected:
   std::string m_outfile;

   PHTimer m_timer;

   int m_nevt;

};

#endif
