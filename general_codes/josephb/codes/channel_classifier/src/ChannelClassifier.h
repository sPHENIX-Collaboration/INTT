#ifndef CHANNEL_CLASSIFIER_H
#define CHANNEL_CLASSIFIER_H

#include <intt/InttMapping.h>

#include <cstdio>
#include <cmath>

#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <TFile.h>
#include <TTree.h>

//#include <RooRealVar.h>
//#include <RooDataSet.h>
//#include <RooPoisson.h>

class ChannelClassifier
{
public:
	Int_t constexpr static NUM_CHANNELS = 56 * 52 * 128;

	Intt::Online_s static struct_from_hash(Int_t);

	Int_t static hash_from_struct(struct Intt::Online_s const&);
	Int_t static hash_from_struct(struct Intt::Offline_s const&);
	Int_t static hash_from_struct(struct Intt::RawData_s const&);

	void static print(Intt::Online_s const&);

	int AppendFile(std::string const&);
	int PoissonFit(Long64_t*);

protected:
	std::string treename = "prdf_tree";
	std::string outtreename = "channel_tree";

	//I originally wanted to implement some clever multithreading
	//So I broke up the reading into threadable functions that read different subsets
	//of the same (large) TTree, but ROOT is written by a pack of drooling fucking monkeys
	//and I can't use it without CLANG complaining that a bunch of static asserts failing
	//at runtime
	//
	//This is most likely because they manage file browsing a global level instead of wrapping
	//it in class instances, so you can't have multiple independent readers in the same program
	//
	//Embarrassing levels of mental defficiency for software written for particle physics
	//that would have massive data files, I guess we're stuck with ~30 minute analysis codes
	//that could be reduced by a factor of the number of threads you can throw at it
	int const NUM_THREADS = 1;
	void static AppendHits(std::string const&, std::string const&, Long64_t*, int, int);
};

#endif//CHANNEL_CLASSIFIER_H
