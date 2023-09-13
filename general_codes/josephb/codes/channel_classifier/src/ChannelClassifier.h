#ifndef CHANNEL_CLASSIFIER_H
#define CHANNEL_CLASSIFIER_H

#include <intt/InttMapping.h>

#include <cstdio>
#include <cmath>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <TFile.h>
#include <TTree.h>

#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooPoisson.h>

class ChannelClassifier
{
public:
	Int_t constexpr static NUM_CHANNELS = 56 * 52 * 128;

	Intt::Online_s static struct_from_hash(Int_t);

	Int_t static hash_from_struct(struct Intt::Online_s const&);
	Int_t static hash_from_struct(struct Intt::Offline_s const&);
	Int_t static hash_from_struct(struct Intt::RawData_s const&);

	void static print(Intt::Online_s const&);

protected:
	Long64_t hits[NUM_CHANNELS];

	TFile* current_file = nullptr;
	TTree* current_tree = nullptr;
};

#endif//CHANNEL_CLASSIFIER_H
