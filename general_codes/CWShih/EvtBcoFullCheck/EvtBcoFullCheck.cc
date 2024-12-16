//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in EvtBcoFullCheck.h.
//
// EvtBcoFullCheck(const std::string &name = "EvtBcoFullCheck")
// everything is keyed to EvtBcoFullCheck, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// EvtBcoFullCheck::~EvtBcoFullCheck()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int EvtBcoFullCheck::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int EvtBcoFullCheck::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int EvtBcoFullCheck::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int EvtBcoFullCheck::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int EvtBcoFullCheck::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int EvtBcoFullCheck::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int EvtBcoFullCheck::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void EvtBcoFullCheck::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "EvtBcoFullCheck.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
EvtBcoFullCheck::EvtBcoFullCheck(
  const std::string &name,
  const int process_id_in,
  const int runnumber_in,
  const std::string output_directory_in,
  const bool MBDNS_trigger_require_tag_in,
  const int  trigger_MBDvtxZ_cm_in
  ):
  SubsysReco(name),
  process_id(process_id_in),
  runnumber(runnumber_in),
  output_directory(output_directory_in),
  MBDNS_trigger_require_tag(MBDNS_trigger_require_tag_in),
  trigger_MBDvtxZ_cm(trigger_MBDvtxZ_cm_in)
{
  std::cout << "EvtBcoFullCheck::EvtBcoFullCheck(const std::string &name) Calling ctor" << std::endl;

  if (trigger_MBDvtxZ_cm != 10 && trigger_MBDvtxZ_cm != 30) {
    std::cout<<"EvtBcoFullCheck::EvtBcoFullCheck - trigger_MBDvtxZ_cm is not 10 or 30, please check the input"<<std::endl;
    exit(1);
  }

  std::string job_index = std::to_string( process_id );
  int job_index_len = 5;
  job_index.insert(0, job_index_len - job_index.size(), '0');

  std::string runnumber_str = std::to_string( runnumber );
  int runnumber_str_len = 8;
  runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');


  output_filename = "EvtBcoFullCheck";
  output_filename += (MBDNS_trigger_require_tag) ? "_MBDNSTrig" : "";
  output_filename += (MBDNS_trigger_require_tag) ? Form("vtxZ%dcm",trigger_MBDvtxZ_cm) : "";
  output_filename += Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str());

  file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()),"RECREATE");

  h1_bcofull_map.clear();
  InitHist();

  GL1_BcoFull_vec.clear();

  eID_count = 0;

}

int EvtBcoFullCheck::InitHist()
{
  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_200000",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_200000",
      "h1_ThisBcoFull_PreviousBcoFull_0_200000;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      200,
      0,
      200000
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_100000",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_100000",
      "h1_ThisBcoFull_PreviousBcoFull_0_100000;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      200,
      0,
      100000
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_50000",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_50000",
      "h1_ThisBcoFull_PreviousBcoFull_0_50000;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      200,
      0,
      50000
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_25000",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_25000",
      "h1_ThisBcoFull_PreviousBcoFull_0_25000;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      200,
      0,
      25000
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_10000",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_10000",
      "h1_ThisBcoFull_PreviousBcoFull_0_10000;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      200,
      0,
      10000
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_5000",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_5000",
      "h1_ThisBcoFull_PreviousBcoFull_0_5000;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      200,
      0,
      5000
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_2500",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_2500",
      "h1_ThisBcoFull_PreviousBcoFull_0_2500;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      250,
      0,
      2500
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_1000",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_1000",
      "h1_ThisBcoFull_PreviousBcoFull_0_1000;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      200,
      0,
      1000
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_500",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_500",
      "h1_ThisBcoFull_PreviousBcoFull_0_500;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      250,
      0,
      500
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_300",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_300",
      "h1_ThisBcoFull_PreviousBcoFull_0_300;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      150,
      0,
      300
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_200",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_200",
      "h1_ThisBcoFull_PreviousBcoFull_0_200;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      200,
      0,
      200
    )
  ));

  h1_bcofull_map.insert(std::make_pair(
    "h1_ThisBcoFull_PreviousBcoFull_0_150",
    new TH1D(
      "h1_ThisBcoFull_PreviousBcoFull_0_150",
      "h1_ThisBcoFull_PreviousBcoFull_0_150;This_Gl1BcoFull - Pre_Gl1BcoFull;Entries",
      150,
      0,
      150
    )
  ));

  for (auto &pair : h1_bcofull_map)
  {
      std::string YTitle = pair.second -> GetYaxis() -> GetTitle();

      std::string YTitle_post;

      if (YTitle.find("Entries") != std::string::npos) // note : found the (Entries)
      {
          YTitle_post = Form("Entries  (/%.2f)", pair.second -> GetBinWidth(1));
          pair.second -> GetYaxis() -> SetTitle(YTitle_post.c_str());
      }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
EvtBcoFullCheck::~EvtBcoFullCheck()
{
  std::cout << "EvtBcoFullCheck::~EvtBcoFullCheck() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int EvtBcoFullCheck::Init(PHCompositeNode *topNode)
{
  std::cout << "EvtBcoFullCheck::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EvtBcoFullCheck::InitRun(PHCompositeNode *topNode)
{
  std::cout << "EvtBcoFullCheck::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EvtBcoFullCheck::process_event(PHCompositeNode *topNode)
{
  // std::cout << "EvtBcoFullCheck::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  if (eID_count % 1000 == 0) {
    std::cout << "EvtBcoFullCheck::process_event - processing event " << eID_count << std::endl;
  }

  p_gl1 = findNode::getClass<Gl1Packetv2>(topNode, m_gl1NodeName); // note : for the selfgen DST, it may be the "GL1RAWHIT"

  if (!p_gl1)
  {
      std::cout << "In EvtBcoFullCheck::process_event, No GL1RAWHIT node found" << std::endl;
      exit(1);
  }

  live_trigger_decimal = p_gl1->lValue(0,"LiveVector");
  live_trigger_map = EvtBcoFullCheck::prepare_trigger_vec(live_trigger_decimal);

  scaled_trigger_decimal = p_gl1->lValue(0,"ScaledVector");
  scaled_trigger_map = EvtBcoFullCheck::prepare_trigger_vec(scaled_trigger_decimal);


  if (MBDNS_trigger_require_tag)
  {
    if (trigger_MBDvtxZ_cm == 10 && live_trigger_map.find(MBDNS_VtxZ10cm_Id) == live_trigger_map.end()){

      GL1_BcoFull_vec.push_back(-999);

      eID_count += 1;
      return Fun4AllReturnCodes::EVENT_OK;
    }
    
    if (trigger_MBDvtxZ_cm == 30 && live_trigger_map.find(MBDNS_VtxZ30cm_Id) == live_trigger_map.end()){

      GL1_BcoFull_vec.push_back(-999);      

      eID_count += 1;
      return Fun4AllReturnCodes::EVENT_OK;
    }
  }


  GL1_BcoFull_vec.push_back(p_gl1->getBCO());

  eID_count += 1;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EvtBcoFullCheck::ResetEvent(PHCompositeNode *topNode)
{
  // std::cout << "EvtBcoFullCheck::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EvtBcoFullCheck::EndRun(const int runnumber)
{
  std::cout << "EvtBcoFullCheck::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;

  long long previous_bcofull;

  for (long long i = 0; i <  Long64_t(GL1_BcoFull_vec.size()); i++)
  {
    if (i == 0) {
      previous_bcofull = GL1_BcoFull_vec[i];
      continue;
    }

    if (previous_bcofull != -999 && GL1_BcoFull_vec[i] != -999)
    {
      for (auto &pair : h1_bcofull_map){
        pair.second->Fill(GL1_BcoFull_vec[i] - previous_bcofull);
      }
    }



    previous_bcofull = GL1_BcoFull_vec[i];
    
  }

  file_out -> cd();
  for (auto &pair : h1_bcofull_map){
    pair.second -> Write();
  }

  file_out -> Close();


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EvtBcoFullCheck::End(PHCompositeNode *topNode)
{
  std::cout << "EvtBcoFullCheck::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int EvtBcoFullCheck::Reset(PHCompositeNode *topNode)
{
 std::cout << "EvtBcoFullCheck::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void EvtBcoFullCheck::Print(const std::string &what) const
{
  std::cout << "EvtBcoFullCheck::Print(const std::string &what) const Printing info for " << what << std::endl;
}

std::map<int,int> EvtBcoFullCheck::prepare_trigger_vec(long long trigger_input)
{
    std::bitset<64> trigger_input_bitset(trigger_input);
    std::vector<int> output_vec; output_vec.clear();
    std::map<int,int> output_map; output_map.clear();

    for (unsigned int i=0; i<64; i++)
	{
	    if (trigger_input_bitset.test(i))
	    {
            output_vec.push_back(i);
	    }
	}

  for (int ele : output_vec){
    output_map[ele] = ele;
  }



    return output_map;

}