#ifndef PID_FILTER_H
#define PID_FILTER_H

#include "TmvaHelper.h"

#include <vector>

class TTree;

// For filtering combinatorial background from KFP signal data

class PidFilter {
public:
	PidFilter() = default;
	virtual ~PidFilter() {clean_members();}

	int verbosity() {return m_verbose;}
	int verbosity(int verbose) {return m_verbose = verbose;}

	void set_num_daughters(int);

	void set_mother_pdg_id(int);

	int branch(TmvaHelper&, TTree*);
	int eval();

private:
	void alloc_members();
	void clean_members();

	int m_mother_pdg_id{0};
	int m_num_daughters{0};

	int* m_track_true_id{};
	int** m_track_pdg_id{};

	std::vector<int>** m_true_track_history_pdg_id{};

	std::vector<float>** m_true_track_history_px{};
	std::vector<float>** m_true_track_history_py{};
	std::vector<float>** m_true_track_history_pz{};

	int m_verbose{0};
};

#endif//PID_FILTER_H
