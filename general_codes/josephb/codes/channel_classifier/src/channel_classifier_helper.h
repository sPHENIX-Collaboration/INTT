#ifndef CHANNEL_CLASSIFIER_HELPER_H
#define CHANNEL_CLASSIFIER_HELPER_H

#include <intt/InttMapping.h>

#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdint>

#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <TFile.h>
#include <TTree.h>

class channel_classifier_helper
{
public:
	channel_classifier_helper();
	virtual ~channel_classifier_helper();

	Int_t constexpr static NUM_CHANNELS = 56 * 52 * 128;
	std::string i_treename = "prdf_tree";
	std::string o_treename = "hitrate_tree";

	void Reset();

	InttNameSpace::RawData_s static struct_from_hash(Int_t);
	Int_t static hash_from_struct(struct InttNameSpace::RawData_s const&);
	Int_t static hash_from_struct(struct InttNameSpace::Online_s const&);
	Int_t static hash_from_struct(struct InttNameSpace::Offline_s const&);

	void static print(InttNameSpace::RawData_s const&);

	int append_file(std::string const&);
	// parse output of a .root file (produced by CombinedRawDataConverter)
	//   and append hits to an internal array of hitrates
	//   argument is the input file name

	int output_hits(std::string const&);
	// normalize the internal hitrate array and write it to a .root file, with the total number of events
	//   argument is the output file name

protected:

private:
	Long64_t m_evts;
	Long64_t* m_hits;
	std::map<std::string, std::vector<Int_t>*> m_branches;
};

#endif//CHANNEL_CLASSIFIER_HELPER_H
