#include "InttCombinedRawDataConverterEB.h"
#include <intt/InttMapping.h>

#include "InttEvent.h"

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>

#include <phool/getClass.h>
#include <phool/phool.h>

#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>

#include <cstdlib>    // for exit
#include <iostream>   // for operator<<, endl, bas...
#include <utility>    // for pair

class PHCompositeNode;

InttCombinedRawDataConverterEB::InttCombinedRawDataConverterEB(std::string const& name)
  : SubsysReco(name)
{
  // Do nothing
}

int InttCombinedRawDataConverterEB::SetOutputFile(std::string const& filename)
{
  if (filename.empty())
  {
    std::cout << "InttCombinedRawDataConverterEB::SetOutputFile(std:: string const& filename)" << std::endl;
    std::cout << "Argument \"filename\" is empty string" << std::endl;
    std::cout << "No output was written" << std::endl;

    return 1;
  }

  std::cout << "Will write to file:" << std::endl;
  std::cout << "\t" << filename << std::endl;

  file = TFile::Open(filename.c_str(), "RECREATE");
  if (tree)
  {
    tree->SetDirectory(file);
  }

  return 0;
}

int InttCombinedRawDataConverterEB::WriteOutputFile()
{
  if (!file)
  {
    std::cout << "InttCombinedRawDataConverterEB::WriteOputputFile()" << std::endl;
    std::cout << "Member \"file\" is uninitialized" << std::endl;
    std::cout << "Did you call SetOutputFile()?" << std::endl;
    std::cout << "No output was written" << std::endl;
    return 1;
  }

  if (!tree)
  {
    std::cout << "InttCombinedRawDataConverterEB::WriteOputputFile()" << std::endl;
    std::cout << "Member \"tree\" is uninitialized" << std::endl;
    std::cout << "Did you call SetOutputFile()?" << std::endl;
    std::cout << "No output was written" << std::endl;
    return 1;
  }

  file->cd();
  tree->Write();
  file->Write();
  file->Close();
  return 0;
}

int InttCombinedRawDataConverterEB::Init(PHCompositeNode* /*topNode*/)
{
  if(tree!=nullptr) delete tree;

  tree = new TTree("tree", "InttEvent");
  if (file)
  {
    tree->SetDirectory(file);
  }

  inttEvt = new InttEvent();

  tree->Branch("event", "InttEvent", &inttEvt, 8000, 99);

/*
  tree->Branch("n_evt", &n_evt);
  tree->Branch("num_hits", &num_hits);

  branches_i =
  {
     {"flx_svr", new std::vector<Int_t>()},
     {"flx_chn", new std::vector<Int_t>()},
     {"lyr",     new std::vector<Int_t>()},
     {"ldr",     new std::vector<Int_t>()},
     {"arm",     new std::vector<Int_t>()},
     {"chp",     new std::vector<Int_t>()},
     {"chn",     new std::vector<Int_t>()},

     {"flx_bco", new std::vector<Int_t>()},
     {"adc",     new std::vector<Int_t>()},
     {"amp",     new std::vector<Int_t>()},
  };

  branches_l =
  {
     {"gtm_bco", new std::vector<Long64_t>()},
  };

  branches_d =
  {
     //{"g_x",   new std::vector<Double_t>()},
     //{"g_y",   new std::vector<Double_t>()},
     //{"g_z",   new std::vector<Double_t>()},
  };

  for (auto& itr : branches_i)tree->Branch(itr.first.c_str(), &(itr.second));
  for (auto& itr : branches_l)tree->Branch(itr.first.c_str(), &(itr.second));
  for (auto& itr : branches_d)tree->Branch(itr.first.c_str(), &(itr.second));

  if (Verbosity() > 20)
  {
    std::cout << "int InttCombinedRawDataConverterEB::Init(PHCompositeNode* )" << std::endl;
  }
  if (Verbosity() > 20)
  {
    std::cout << "\tDone";
  }
*/

  return 0;
}

int InttCombinedRawDataConverterEB::InitRun(PHCompositeNode* /*topNode*/)
{
  n_evt = 0;
  for (auto& itr : branches_i)itr.second->clear();
  for (auto& itr : branches_l)itr.second->clear();
  for (auto& itr : branches_d)itr.second->clear();

  if (Verbosity() > 20)
  {
    std::cout << "int InttCombinedRawDataConverterEB::InitRun(PHCompositeNode* /*topNode*/)" << std::endl;
  }
  if (Verbosity() > 20)
  {
    std::cout << "\tDone";
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttCombinedRawDataConverterEB::process_event(PHCompositeNode* topNode)
{
  if (!tree)
  {
    return Fun4AllReturnCodes::DISCARDEVENT;
  }

  if (Verbosity() > 20)
  {
    std::cout << "int InttCombinedRawDataConverterEB::process_event(PHCompositeNode* topNode)" << std::endl;
  }
  if (Verbosity() > 20)
  {
    std::cout << "\t" << n_evt << std::endl;
  }

  InttRawHitContainer* inttcont = findNode::getClass<InttRawHitContainer>(topNode, m_InttRawNodeName);
  if (!inttcont)
  {
    std::cout << "int InttCombinedRawDataConverterEB::process_event(PHCompositeNode* topNode)" << std::endl;
    std::cout << "\t" << PHWHERE << " could not find node " << m_InttRawNodeName << std::endl;
    std::cout << "\tExiting" << std::endl;

    gSystem->Exit(1);
    exit(1);
  }

/*
  for (auto& itr : branches_i)itr.second->clear();
  for (auto& itr : branches_l)itr.second->clear();
  for (auto& itr : branches_d)itr.second->clear();
*/

  inttEvt->clear();
  int nHitTotal=0;

  //--for(int iladder=0; iladder<14; iladder++){
  //--  inttEvt->evtSeq = p->getEventNum(iladder);
  //--  inttEvt->bco    = p->getBCO(iladder);
  //--  if(inttEvt->evtSeq>=0) break;
  //--}
  //--if(debug)
  //--  cout<<" "<<inttEvt->evtSeq<<" "<<hex<<inttEvt->bco<<" "<<p->getBCO()<<dec<<endl;

  Intt::RawData_s raw;
  Intt::Online_s onl;
  for (unsigned int i = 0; i < inttcont->get_nhits(); i++)
  {
    InttRawHit* intthit = inttcont->get_hit(i);

    raw.felix_server  = Intt::FelixFromPacket(intthit->get_packetid());
    raw.felix_channel = intthit->get_fee();
    raw.chip          = intthit->get_chip_id();
    raw.channel       = intthit->get_channel_id();
    onl = Intt::ToOnline(raw);


    InttHit* hit = inttEvt->addHit();

    hit->pid = intthit->get_packetid();


    hit->adc     = intthit->get_adc();
    hit->ampl    = intthit->get_amplitude();
    hit->chip_id = intthit->get_chip_id();
    if(hit->chip_id>=27)  hit->chip_id -= 26;

    hit->module  = intthit->get_fee();
    hit->chan_id = intthit->get_channel_id();
    ////fem
    hit->bco      = intthit->get_FPHX_BCO();
    hit->bco_full = intthit->get_bco();
    //cout<<"chip : "<<hit->adc<<" "<<hit->chip_id<<" "<<chip<<" "<<hit->chip_id-chip<<" "<<hit->chan_id<<std::endl;

    //--hit->evt     = p->iValue(i, "EVTCTR");

    //hit->felix = p->getPacketID() % 100
    hit->roc = 2 * (intthit->get_packetid() - 3001);
    if(hit->module > 6) hit->roc++;
    hit->roc %= 8;
    hit->arm = (intthit->get_packetid() - 3001) / 4;

    if(0<=hit->module&&hit->module<14){
      hit->barrel = 0;
      hit->layer  = onl.lyr;
      hit->ladder = onl.ldr;
    } else {
      hit->barrel = 0;
      hit->layer  = 0;
      hit->ladder = 0;
      std::cout<<"module : "<<hit->module<<std::endl;
    }
    ////direction = (id / 100 + 1) % 2;

    hit->full_fphx = intthit->get_full_FPHX();
    hit->full_roc  = intthit->get_full_ROC();

    nHitTotal++;
  }

  //inttEvt->show();
  inttEvt->sort();
  //inttEvt->show();
  tree->Fill();




/*
  Intt::RawData_s raw;
  Intt::Online_s onl;
  std::map<std::tuple<int, int, int, int, int>, char> hits;
  for (unsigned int i = 0; i < inttcont->get_nhits(); i++)
  {
    InttRawHit* intthit = inttcont->get_hit(i);

    raw.felix_server = Intt::FelixFromPacket(intthit->get_packetid());
    raw.felix_channel = intthit->get_fee();
    raw.chip = intthit->get_chip_id();
    raw.channel = intthit->get_channel_id();
    onl = Intt::ToOnline(raw);

    std::tuple<int, int, int, int, int> tpl;
    std::get<0>(tpl) = onl.lyr;
    std::get<1>(tpl) = onl.ldr;
    std::get<2>(tpl) = onl.arm;
    std::get<3>(tpl) = onl.chp;
    std::get<4>(tpl) = onl.chn;

    if(hits.find(tpl) != hits.end())continue;
    hits[tpl] = 0;

    branches_i["flx_svr"]->push_back(raw.felix_server);
    branches_i["flx_chn"]->push_back(raw.felix_channel);

    branches_i["lyr"]->push_back(onl.lyr);
    branches_i["ldr"]->push_back(onl.ldr);
    branches_i["arm"]->push_back(onl.arm);
    branches_i["chp"]->push_back(onl.chp);
    branches_i["chn"]->push_back(onl.chn);

    branches_i["flx_bco"]->push_back(intthit->get_FPHX_BCO());
    branches_i["adc"]->push_back(intthit->get_adc());
    branches_i["amp"]->push_back(intthit->get_amplitude());

    branches_l["gtm_bco"]->push_back(intthit->get_bco());
  }

//  num_hits = branches_l.begin()->second->size();
  tree->Fill();
*/
  ++n_evt;

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttCombinedRawDataConverterEB::End(PHCompositeNode* /*topNode*/)
{
  WriteOutputFile();
  return Fun4AllReturnCodes::EVENT_OK;
}
