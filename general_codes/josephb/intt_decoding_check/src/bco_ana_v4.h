#ifndef BCO_ANA_V4_H
#define BCO_ANA_V4_H

#include "intt_event_pool.h"

#include <Event/fileEventiterator.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>

#include <limits>
#include <map>
#include <string>
#include <vector>

class bco_ana_v4 : public intt_event_pool {
public:
	bco_ana_v4();
	virtual ~bco_ana_v4();

	int set_output_file(std::string const&) override;
	int write_output_file() override;
	int analyze(Packet*) override;

private:
	TFile* m_file{nullptr};
	TTree* m_tree{nullptr};

	static std::size_t const ARR_SIZE = 10000;
	int m_arr[ARR_SIZE] = {};

	int m_data_event{0};
	int m_cade_count[14] = {};
	int m_byte_count[14] = {};
};

#endif//BCO_ANA_V4_H
