#include "TmvaHelper.h"

#include <RooRealVar.h>
#include <RooFormulaVar.h>
#include <TFile.h>

#include <iostream>
#include <fstream>

#include <boost/format.hpp>

TTree*
TmvaHelper::get_tree (
	std::string const& file_name,
	std::string const& tree_name
) {
	if (file_name.empty()) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< std::flush;
		return nullptr;
	}
	TFile* file = TFile::Open(file_name.c_str(), "READ");
	if (!file) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "\tfile: " << file_name << "\n"
			<< std::flush;
		return nullptr;
	}

	if (tree_name.empty()) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< std::flush;
		return nullptr;
	}
	TTree* tree = dynamic_cast<TTree*>(file->Get(tree_name.c_str()));
	if (!tree) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "\tfile: " << file_name << "\n"
			<< "\ttree: " << tree_name << "\n"
			<< std::flush;
		return nullptr;
	}

	return tree;
}

int
TmvaHelper::read_file (
	std::string const& file_name,
	std::vector<std::string>& names
) {
	std::ifstream file(file_name, std::ios_base::in);
	if (!file.good()) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "file: " << file_name << "\n"
			<< std::flush;
		return EXIT_FAILURE;
	}

	for (std::string line; std::getline(file, line);) {
		for (std::size_t pos; (pos = line.find("#")) != std::string::npos;) {
			line = line.substr(0, pos);
		}

		if (line.empty()) continue;

		names.push_back(line);
	}

	return EXIT_SUCCESS;
}

void
TmvaHelper::read_branches (
	std::vector<std::string> const& branches_names
) {
	for (auto const& name : branches_names) {
		m_branches_names.push_back(name);
	}

	init_branches();
}

void
TmvaHelper::read_training (
	std::vector<std::string> const& training_names
) {
	for (auto const& name : training_names) {
		m_training_names.push_back(name);
	}

	init_training();
}

void
TmvaHelper::read_cuts (
	std::vector<std::string> const& cut_names
) {
	for (auto const& name : cut_names) {
		m_cuts_names.push_back(name);
	}

	init_cuts();
}

int
TmvaHelper::read_branches (
	std::string const& branches_file_name
) {
	if (read_file(branches_file_name, m_branches_names)) return EXIT_FAILURE;

	init_branches();
	return EXIT_SUCCESS;
}

int
TmvaHelper::read_training (
	std::string const& training_file_name
) {
	if (read_file(training_file_name, m_training_names)) return EXIT_FAILURE;

	init_training();
	return EXIT_SUCCESS;
}

int
TmvaHelper::read_cuts (
	std::string const& cuts_file_name
) {
	if (read_file(cuts_file_name, m_cuts_names)) return EXIT_FAILURE;

	init_cuts();
	return EXIT_SUCCESS;
}

void
TmvaHelper::init_branches (
) {
	m_branches_map_f.clear();
	m_branches_map_i.clear();
	m_branches_map_u.clear();

	m_branches_args.Clear();
	for (auto const& name : m_branches_names) {

		std::size_t pos = name.find("/");
		std::string n = pos == std::string::npos ? name : name.substr(0,  pos);
		std::string t = pos == std::string::npos ? "F" : name.substr(pos + 1); // Float_t by default

		if (t == "F") { 
			m_branches_map_f[n] = 0.0;
		} else if (t == "I") {
			m_branches_map_i[n] = 0;
		} else if (t == "U") {
			m_branches_map_u[n] = 0;
		} else {
			std::cerr
				<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
				<< "\tBranch " << n << " specifies untreated type '" << t << "'\n"
				<< "\t\t" << name << "\n"
				<< std::flush;
			continue;
		}

		m_branches_args.addOwned ( *new RooRealVar (
			n.c_str(), n.c_str(), 0.0,
			-std::numeric_limits<Float_t>::max(), std::numeric_limits<Float_t>::max()
		) );
	}
}

void
TmvaHelper::init_training (
) {
	m_training_map.clear();
	m_training_args.Clear();
	for (auto const& name : m_training_names) {

		std::size_t pos = name.find(":=");
		std::string f = pos == std::string::npos ? name : name.substr(pos + 2);

		m_training_map[name] = 0.0;
		m_training_args.addOwned ( *new RooFormulaVar (
			name.c_str(), f.c_str(),
			m_branches_args, kFALSE
		) );
	}
}

void
TmvaHelper::init_cuts (
) {
	m_cuts_map.clear();
	m_cuts_args.Clear();
	for (auto const& name : m_cuts_names) {
		m_cuts_map[name] = 0.0;
		m_cuts_args.addOwned ( *new RooFormulaVar (
			name.c_str(), name.c_str(),
			m_branches_args, kFALSE
		) );
	}
}

int
TmvaHelper::branch (
	TTree* tree
) {
	if (!tree) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "\tArgument 'tree' is null\n"
			<< std::flush;
		return EXIT_FAILURE;
	}

	std::vector<std::string> missing_branches;
	for (auto& [name, val] : m_branches_map_f) {
		if (tree->SetBranchAddress(name.c_str(), &val) != TTree::kMatch) missing_branches.push_back(name);
	}

	for (auto& [name, val] : m_branches_map_i) {
		if (tree->SetBranchAddress(name.c_str(), &val) != TTree::kMatch) missing_branches.push_back(name);
	}

	for (auto& [name, val] : m_branches_map_u) {
		if (tree->SetBranchAddress(name.c_str(), &val) != TTree::kMatch) missing_branches.push_back(name);
	}

	if (missing_branches.size()) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "\tFailed to set branch addresses\n";
		for (auto const& name : missing_branches) std::cerr
			<< "\t" << name << std::endl;
		std::cerr
			<< std::flush;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void
TmvaHelper::branch (
	TMVA::DataLoader* dataloader
) const {
	for (auto const& name : m_training_names) {
		dataloader->AddVariable(name.c_str());
	}

	boost::format no_nan("%s == %s");

	for (auto const& [name, val] : m_branches_map_f) {
		TCut cut = (no_nan % name % name).str().c_str();
		dataloader->AddCut(cut, "Signal");
		dataloader->AddCut(cut, "Background");
	}

	// for (auto const& [name, val] : m_training_map) {
	// 	TCut cut = (no_nan % name % name).str().c_str();
	// 	dataloader->AddCut(cut, "Signal");
	// 	dataloader->AddCut(cut, "Background");
	// }

	for (auto const& name : m_cuts_names) {
		TCut cut = name.c_str();
		dataloader->AddCut(cut, "Signal");
		dataloader->AddCut(cut, "Background");
	}
}

void
TmvaHelper::branch (
	TMVA::Reader* reader
) {
	for (auto& name : m_training_names) {
		if (m_training_map.find(name) == m_training_map.end()) continue;
		reader->AddVariable(name.c_str(), &(m_training_map[name]));
	}
}

void
TmvaHelper::make_branches (
	TTree* tree
) {
	for (auto& [name, val] : m_branches_map_f) {
		tree->Branch(name.c_str(), &val);
	}

	for (auto& [name, val] : m_branches_map_i) {
		tree->Branch(name.c_str(), &val);
	}

	for (auto& [name, val] : m_branches_map_u) {
		tree->Branch(name.c_str(), &val);
	}
}

void*
TmvaHelper::get_branch (
	std::string const& name
) {
	if (m_branches_map_f.find(name) != m_branches_map_f.end()) return static_cast<void*>(&m_branches_map_f[name]);
	if (m_branches_map_i.find(name) != m_branches_map_i.end()) return static_cast<void*>(&m_branches_map_i[name]);
	if (m_branches_map_u.find(name) != m_branches_map_u.end()) return static_cast<void*>(&m_branches_map_u[name]);
	return nullptr;
}

int
TmvaHelper::eval (
) {
	for (auto const& [name, val] : m_branches_map_f) {
		if (!(val == val)) return EXIT_FAILURE; // IEEE NaN filtering
		dynamic_cast<RooRealVar&>(m_branches_args[name]).setVal(val);
	}
	for (auto const& [name, val] : m_branches_map_i) {
		dynamic_cast<RooRealVar&>(m_branches_args[name]).setVal(val);
	}
	for (auto const& [name, val] : m_branches_map_u) {
		dynamic_cast<RooRealVar&>(m_branches_args[name]).setVal(val);
	}

	for (auto& [name, val] : m_training_map) {
		val = dynamic_cast<RooFormulaVar&>(m_training_args[name]).getValV();
		if (!(val == val)) return EXIT_FAILURE; // IEEE NaN filtering
	}

	for (auto& [name, val] : m_cuts_map) {
		val = dynamic_cast<RooFormulaVar&>(m_cuts_args[name]).getValV();
		if (val == 0) return EXIT_FAILURE; // Doesn't pass cut criteria
	}

	return EXIT_SUCCESS;
}

void
TmvaHelper::show (
) const {
	std::cout << __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << std::endl;
	for (auto const& [name, val] : m_branches_map_f) {
		std::cout << "\t" << name << ": " << val << std::endl;
	}
	for (auto const& [name, val] : m_branches_map_i) {
		std::cout << "\t" << name << ": " << val << std::endl;
	}
	for (auto const& [name, val] : m_branches_map_u) {
		std::cout << "\t" << name << ": " << val << std::endl;
	}

	for (auto const& [name, val] : m_training_map) {
		std::cout << "\t" << name << ": " << val << std::endl;
	}

	for (auto const& [name, val] : m_cuts_map) {
		std::cout << "\t" << name << ": " << val << std::endl;
	}
	std::cout << std::endl;
}

