#ifndef TMVA_HELPER_H
#define TMVA_HELPER_H

#include <RtypesCore.h>
#include <TTree.h>

#include <RooArgSet.h>

#include <TMVA/DataLoader.h>
#include <TMVA/Reader.h>

#include <map>
#include <string>
#include <vector>

class TMVAHelper {
public:
	// Gets a TTree from a file, while doing normal error checking
	static TTree* get_tree (std::string const&, std::string const&);

	// Reads the lines of a file into a reference vector, while pruning comments and empty lines
	static int read_file (std::string const&, std::vector<std::string>&);

	// For initializing members
	void read_branches (std::vector<std::string> const&);
	void read_training (std::vector<std::string> const&);
	void read_cuts     (std::vector<std::string> const&);
	int  read_branches (std::string const&);
	int  read_training (std::string const&);
	int  read_cuts     (std::string const&);

	// For modifying other objects with class members
	int  branch (TTree*);
	void branch (TMVA::DataLoader*) const;
	void branch (TMVA::Reader*);

	// To computing the training variables from the branches variables
	int  eval ();
	void show () const;

private:
	void init_branches();
	void init_training();
	void init_cuts();

	std::vector<std::string> m_branches_names;
	std::vector<std::string> m_training_names;
	std::vector<std::string> m_cuts_names;

	std::map<std::string, Float_t> m_branches_map;
	std::map<std::string, Float_t> m_training_map;
	std::map<std::string, Float_t> m_cuts_map;

	RooArgSet m_branches_args;
	RooArgSet m_training_args;
	RooArgSet m_cuts_args;
};

#endif// TMVA_HELPER_H

