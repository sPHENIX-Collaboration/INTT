#ifndef CHANNEL_CLASSIFIER_VER_1_H
#define CHANNEL_CLASSIFIER_VER_1_H

#include "base.h"

#include <intt/InttMapping.h>

#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdio>

#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TTree.h>

class channel_classifier_ver_1 : public channel_classifier_base
{
public:
	using channel_classifier_base::status;

	channel_classifier_ver_1();
	channel_classifier_ver_1(int const&, int const&);
	virtual ~channel_classifier_ver_1();

	void adjust_hitrate(Double_t&, InttNameSpace::RawData_s const&) const override;
	int fill(std::string const&, std::string const&) override;
	int fit() override;
	int classify(std::string const&, std::string const&, std::string const&, std::string const&) override;
	int write_hist(std::string const&) override;
	int draw(std::string const&) override;

	enum Params_e : char {
		GOOD_INNER_NORM,
		GOOD_INNER_AVG,
		GOOD_INNER_SIG,

		HALF_INNER_NORM,
		HALF_INNER_AVG,
		HALF_INNER_SIG,

		COLD_INNER_NORM,
		COLD_INNER_SHIFT,
		COLD_INNER_WIDTH,

		HOT_INNER_NORM,
		HOT_INNER_SHIFT,
		HOT_INNER_WIDTH,


		GOOD_OUTER_NORM,
		GOOD_OUTER_AVG,
		GOOD_OUTER_SIG,

		HALF_OUTER_NORM,
		HALF_OUTER_AVG,
		HALF_OUTER_SIG,

		COLD_OUTER_NORM,
		COLD_OUTER_SHIFT,
		COLD_OUTER_WIDTH,

		HOT_OUTER_NORM,
		HOT_OUTER_SHIFT,
		HOT_OUTER_WIDTH,

		NUM_PAR
	};

	static Double_t good_inner_pdf(Double_t*, Double_t*);
	static Double_t half_inner_pdf(Double_t*, Double_t*);
	static Double_t cold_inner_pdf(Double_t*, Double_t*);
	static Double_t hot_inner_pdf(Double_t*, Double_t*);

	static Double_t good_outer_pdf(Double_t*, Double_t*);
	static Double_t half_outer_pdf(Double_t*, Double_t*);
	static Double_t cold_outer_pdf(Double_t*, Double_t*);
	static Double_t hot_outer_pdf(Double_t*, Double_t*);

protected:
	void init_gauss(TF1*, char);
	void init_left_exp(TF1*, char);
	void init_right_exp(TF1*, char);

private:
	int m_num_sig = 5;
	int m_num_bins = 100;

	std::string m_hist_name = "hist";
	TH1D* m_hist = nullptr;
	TH1D* m_inner_hist = nullptr;
	TH1D* m_outer_hist = nullptr;

	std::string m_fit_name = "fit";
	TF1* m_fit = nullptr;

	struct term_s {
		TF1* tf1 = nullptr;
		Double_t(*func)(Double_t*, Double_t*) = nullptr;
		void(channel_classifier_ver_1::*init)(TF1*, char) = nullptr;
		Double_t min = -FLT_MAX;
		Double_t max = FLT_MAX;
		int begin = 0;
		int end = 0;
		status state;
	};
	typedef std::map<std::string, term_s> map_t;
	map_t m_terms;
	void init_terms();
};

#endif//CHANNEL_CLASSIFIER_VER_1_H
