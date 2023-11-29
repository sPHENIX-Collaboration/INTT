#ifndef CHANNEL_CLASSIFIER_BASE_H
#define CHANNEL_CLASSIFIER_BASE_H

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

class channel_classifier_base
{
public:
	channel_classifier_base();
	virtual ~channel_classifier_base();

	void adjust_hitrate(Double_t&, InttNameSpace::RawData_s const&) const;

	virtual int fill(std::string const&, std::string const& = "hitrate_tree");
	// Fill an internal member (e.g., TH1) with the adjusted hitrate
	//   1st argument is the name of the file to load
	//   2nd argument is the name of the tree to find

	virtual int fit();
	// Fit an internal member (e.g., a TF1) using the filled data member
	// The fitted pdf will be used to classify channels by hitrate

	virtual int write_hist(std::string const&);
	// Write the member histogram to file
	//   argument is the name of the file to save the plot to
	//   was used for debugging

	virtual int draw(std::string const&);
	// Create a TCanvas showing the fitted hitrate distribution
	//   argument is the name of the file to save the plot to
};

#endif//CHANNEL_CLASSIFIER_BASE_H
