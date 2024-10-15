#include "TMVAHelper.h"

#include <RooRealVar.h>
#include <RooFormulaVar.h>
#include <TFile.h>

#include <iostream>
#include <fstream>

TTree*
TMVAHelper::get_tree (
	std::string const& file_name,
	std::string const& tree_name
) {
	if (file_name.empty()) {
		std::cerr << __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << std::endl;
		return nullptr;
	}
	TFile* file = TFile::Open(file_name.c_str(), "READ");
	if (!file) {
		std::cerr << __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
		          << "file: " << file_name << std::endl;
		return nullptr;
	}

	if (tree_name.empty()) {
		std::cerr << __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << std::endl;
		return nullptr;
	}
	TTree* tree = dynamic_cast<TTree*>(file->Get(tree_name.c_str()));
	if (!tree) {
		std::cerr << __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
		          << "file: " << file_name << "\n"
		          << "tree: " << tree_name << std::endl;
		return nullptr;
	}

	return tree;
}

int
TMVAHelper::read_file (
	std::string const& file_name,
	std::vector<std::string>& names
) {
	std::ifstream file(file_name, std::ios_base::in);
	if (!file.good()) {
		std::cerr << __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
		          << "file: " << file_name << std::endl;
		return EXIT_FAILURE;
	}

	names.clear();
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
TMVAHelper::read_branches (
	std::vector<std::string> const& branches_names
) {
	m_branches_names.clear();
	for (auto const& name : branches_names) {
		m_branches_names.push_back(name);
	}

	init_branches();
}

void
TMVAHelper::read_training (
	std::vector<std::string> const& training_names
) {
	m_training_names.clear();
	for (auto const& name : training_names) {
		m_training_names.push_back(name);
	}

	init_training();
}

int
TMVAHelper::read_branches (
	std::string const& branches_file_name
) {
	if (read_file(branches_file_name, m_branches_names)) return EXIT_FAILURE;

	init_branches();
	return EXIT_SUCCESS;
}

int
TMVAHelper::read_training (
	std::string const& training_file_name
) {
	if (read_file(training_file_name, m_training_names)) return EXIT_FAILURE;

	init_training();
	return EXIT_SUCCESS;
}

void
TMVAHelper::init_branches (
) {
	m_branches_map.clear();
	m_branches_args.Clear();
	for (auto const& name : m_branches_names) {
		m_branches_map[name] = 0.0;
		m_branches_args.addOwned ( *new RooRealVar (
			name.c_str(), name.c_str(),
			0.0,
			-std::numeric_limits<Float_t>::max(), std::numeric_limits<Float_t>::max()
		) );
	}
}

void
TMVAHelper::init_training (
) {
	m_training_map.clear();
	m_training_args.Clear();
	for (auto const& name : m_training_names) {
		m_training_map[name] = 0.0;
		std::size_t pos = name.find(":=");
		m_training_args.addOwned ( *new RooFormulaVar (
			name.c_str(),
			pos == std::string::npos ? name.c_str() : name.substr(pos + 2).c_str(),
			m_branches_args, kFALSE
		) );
	}
}

int
TMVAHelper::branch (
	TTree* tree
) {
	for (auto& [name, val] : m_branches_map) {
		if (!tree->GetBranch(name.c_str())) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			return EXIT_FAILURE;
		}
		tree->SetBranchAddress(name.c_str(), &(m_branches_map[name]));
	}

	return EXIT_SUCCESS;
}

void
TMVAHelper::branch (
	TMVA::DataLoader* dataloader
) const {
	for (auto const& name : m_training_names) {
		dataloader->AddVariable(name.c_str());
	}
}

void
TMVAHelper::branch (
	TMVA::Reader* reader
) {
	for (auto& [name, val] : m_training_map) {
		reader->AddVariable(name.c_str(), &val);
	}
}

int
TMVAHelper::eval (
) {
	for (auto const& [name, val] : m_branches_map) {
		if (!(val == val)) return EXIT_FAILURE; // IEEE NaN filtering
		dynamic_cast<RooRealVar&>(m_branches_args[name]).setVal(val);
	}

	for (auto& [name, val] : m_training_map) {
		if (!(val == val)) return EXIT_FAILURE; // IEEE NaN filtering
		val = dynamic_cast<RooFormulaVar&>(m_training_args[name]).getValV();
	}

	return EXIT_SUCCESS;
}

void
TMVAHelper::show (
) const {
	for (auto const& [name, val] : m_branches_map) {
		std::cout << "\t" << name << ": " << val << "\t";
	}
	std::cout << std::endl;

	for (auto const& [name, val] : m_training_map) {
		std::cout << "\t" << name << ": " << val << "\t";
	}
	std::cout << std::endl;
}

