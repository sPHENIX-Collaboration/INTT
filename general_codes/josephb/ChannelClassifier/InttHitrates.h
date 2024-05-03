// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTHITRATES_H
#define INTTHITRATES_H

#include <map>
#include <set>
#include <string>

#include <intt/InttMapping.h>
#include <fun4all/SubsysReco.h>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

class PHCompositeNode;

class InttHitrates : public SubsysReco {
public:
	InttHitrates(std::string const& = "InttHitrates");
	~InttHitrates() override;
	
	int Init(PHCompositeNode*) override;
	int InitRun(PHCompositeNode*) override;
	int process_event(PHCompositeNode*) override;
	int ResetEvent(PHCompositeNode*) override;
	int EndRun(const int runnumber) override;
	int End(PHCompositeNode*) override;
	int Reset(PHCompositeNode*) override;
	
	void Print(std::string const& = "ALL") const override;

	void set_output_file(std::string const& = "");

protected:

private:
	typedef std::map<InttNameSpace::RawData_s, Long64_t, InttNameSpace::RawDataComparator> map_t;
	typedef std::set<InttNameSpace::RawData_s, InttNameSpace::RawDataComparator> set_t;

	std::string m_InttRawNodeName = "INTTRAWHIT";

	Long64_t m_num_events = 0;
	map_t m_hitrates;

	TFile* m_file = nullptr;
	TTree* m_tree = nullptr;
};

#endif//INTTHITRATES_H
