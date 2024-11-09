#ifndef DEDX_BRANCHER_H
#define DEDX_BRANCHER_H

#include "TmvaHelper.h"

#include <string>
#include <map>

class TFile;
class TTree;
class TF1;

class DedxBrancher {
public:
	DedxBrancher() = default;
	virtual ~DedxBrancher() {clean_members();}

	int verbosity() {return m_verbose;}
	int verbosity(int verbose) {return m_verbose = verbose;}

	void set_num_daughters(int);

	int get_dedx_fits(std::string const&);
	int make_branch(TTree*);

	int branch(TmvaHelper&);
	int eval();

private:
	void alloc_members();
	void clean_members();

	int m_num_daughters{};

	int** m_track_pdg_id{};
	float** m_track_p{};

	float* m_track_dedx_fit{};

	TFile* m_file{};
	std::map<int, TF1*> m_fits;

	int m_verbose{0};
};

#endif//DEDX_BRANCHER_H
