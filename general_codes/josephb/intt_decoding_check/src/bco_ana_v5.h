#ifndef BCO_ANA_V5_H
#define BCO_ANA_V5_H

#include "intt_event_pool.h"

#include <Event/fileEventiterator.h>

#include <limits>
#include <map>
#include <string>
#include <vector>

class TFile;
class TH1;

class bco_ana_v5 : public intt_event_pool {
public:
	bco_ana_v5();
	virtual ~bco_ana_v5();

	int set_output_file(std::string const&) override;
	int write_output_file() override;
	int analyze(Packet*) override;

private:
	TFile* m_file{nullptr};
	TH1* m_hist[14]={};

};

#endif//BCO_ANA_V4_H
