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
	channel_classifier_ver_1();
	channel_classifier_ver_1(int const&, int const&);
	virtual ~channel_classifier_ver_1();

	int fill(std::string const&, std::string const&) override;
	int fit() override;
	int draw(std::string const&) override;

private:
	int m_num_sig = 5;
	int m_num_bins = 100;

	//std::unique_ptr<TH1> m_hist;
	TH1* m_hist = nullptr;
};

#endif//CHANNEL_CLASSIFIER_VER_1_H
