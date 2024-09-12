#ifndef BCO_ANA_V1_H
#define BCO_ANA_V1_H

#include "intt_event_pool.h"

#include <Event/fileEventiterator.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>

#include <limits>
#include <map>
#include <string>
#include <vector>

typedef unsigned long long bco_t;
struct bco_comparator {
	static const bco_t BCO_MAX{bco_t{1}<<40};
	bool operator()(bco_t const&, bco_t const&);
};

class bco_ana_v1 : public intt_event_pool {
public:
	bco_ana_v1();
	virtual ~bco_ana_v1();

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
	};
	std::map<bco_t, bco_bin_s> m_bco_map;
	bco_t m_prev_bco{std::numeric_limits<bco_t>::max()};

	bco_comparator m_bco_less;

	TFile* m_file{nullptr};
	TTree* m_tree{nullptr};
	TH1I* m_hist{nullptr};
};

#endif//BCO_ANA_V1_H
