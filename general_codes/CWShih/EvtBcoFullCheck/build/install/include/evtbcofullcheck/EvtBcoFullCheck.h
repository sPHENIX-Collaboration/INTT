// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EVTBCOFULLCHECK_H
#define EVTBCOFULLCHECK_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>   // for PHIODataNode
#include <phool/PHNodeIterator.h> // for PHNodeIterator
#include <phool/PHObject.h>       // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/recoConsts.h>


#include <ffarawobjects/Gl1Packetv2.h>


#include <limits>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2.h>


class PHCompositeNode;
class Gl1Packet;


class TH1;
class TH2;
class TTree;
class TCanvas;
class TString;

class EvtBcoFullCheck : public SubsysReco
{
 public:

  EvtBcoFullCheck(
    const std::string &name = "EvtBcoFullCheck",
    const int process_id_in = 0,
    const int runnumber_in = 0,
    const std::string output_directory_in = "output.root",
    const bool MBDNS_trigger_require_tag_in = true,
    const int  trigger_MBDvtxZ_cm_in = 10 // note : cm
  );

  ~EvtBcoFullCheck() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  std::string GetOutputFileName() {return output_filename;}

 private:
    int process_id;
    int runnumber;
    std::string output_directory;
    bool MBDNS_trigger_require_tag;
    int trigger_MBDvtxZ_cm; 

    std::string output_filename;

    std::vector<long long> GL1_BcoFull_vec;

    long long eID_count;

    // note : -------------------------------- for output ------------------------------
    TFile * file_out;
    std::map<std::string,TH1D*> h1_bcofull_map;
    int InitHist();

    // note : -------------------------------- for gl1 and trigger selection ------------------------------
    Gl1Packet * p_gl1;
    std::string m_gl1NodeName = "GL1Packet";

    long long live_trigger_decimal; 
    std::map<int,int>  live_trigger_map;

    long long scaled_trigger_decimal; 
    std::map<int,int>  scaled_trigger_map;
 
    static std::map<int, int> prepare_trigger_vec(long long trigger_input); 


    const int MBDNS_VtxZ10cm_Id = 12;
    const int MBDNS_VtxZ30cm_Id = 13;

};

#endif // EVTBCOFULLCHECK_H
