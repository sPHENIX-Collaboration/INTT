#ifndef BCO_ANA_V2_H
#define BCO_ANA_V2_H

#include "intt_event_pool.h"

#include <Event/fileEventiterator.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>

#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

typedef unsigned long long bco_t;

class bco_ana_v2 : public intt_event_pool {
public:
	bco_ana_v2();
	virtual ~bco_ana_v2();

	int set_output_file(std::string const&) override;
	int write_output_file() override;
	int analyze(Packet*) override;

private:
	static bco_t constexpr m_BCO_PROJ = (bco_t{1}<<40) - (bco_t{1}<<26);

	struct bco_bin_s {
		bco_t min{std::numeric_limits<bco_t>::max()};
		bco_t max{0};
		int min_evt{std::numeric_limits<int>::max()};
		int max_evt{0};
		std::size_t count{0};
		std::set<bco_t> bco_set;
	};
	std::map<bco_t, bco_bin_s> m_bco_map[14];

	TFile* m_file{nullptr};
	TTree* m_tree{nullptr};
};

#endif//BCO_ANA_V2_H
