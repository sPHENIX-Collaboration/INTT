#include "DedxBrancher.h"

#include <TFile.h>
#include <TTree.h>
#include <TF1.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <boost/format.hpp>

void
DedxBrancher::set_num_daughters (
	int num_daughters
) {
	clean_members();
	m_num_daughters = num_daughters;
	alloc_members();
}

int
DedxBrancher::get_dedx_fits (
	std::string const& path
) {
	if (m_file) m_file->Close();
	m_file = TFile::Open(path.c_str());
	if (!m_file) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "\tFailed to open file:\n"
			<< "\t" << path << "\n"
			<< std::flush;
		return EXIT_FAILURE;
	}

	m_fits.insert({  211, dynamic_cast<TF1*>(m_file->Get("f_piband"))});
	m_fits.insert({ -211, dynamic_cast<TF1*>(m_file->Get("f_piminus_band"))});
	m_fits.insert({  321, dynamic_cast<TF1*>(m_file->Get("f_Kband"))});
	m_fits.insert({ -321, dynamic_cast<TF1*>(m_file->Get("f_Kminus_band"))});
	m_fits.insert({ 2212, dynamic_cast<TF1*>(m_file->Get("f_pband"))});
	m_fits.insert({-2212, dynamic_cast<TF1*>(m_file->Get("f_pbar_band"))});

	std::vector<int> missing_fits;
	for (auto const& [pid, func] : m_fits) {
		if (!func) missing_fits.push_back(pid);
	}

	if (missing_fits.size()) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "\tFailed to get fits for PIDs:\n";
		for (auto const& pid : missing_fits) std::cerr
			<< "\t" << pid << std::endl;
		std::cerr
			<< std::flush;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int
DedxBrancher::make_branch (
	TTree* tree
) {
	if (!tree) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "\tArgument 'tree' is null\n"
			<< std::flush;
		return EXIT_FAILURE;
	}

	std::vector<std::string> failed_branches;
	for (int n = 0; n < m_num_daughters; ++n) {
		std::string name;

		name = (boost::format("track_%d_dEdx_fit") % (n + 1)).str();
		if (!tree->Branch(name.c_str(), &(m_track_dedx_fit[n]))) failed_branches.push_back(name);
	}

	if (failed_branches.size()) {
		std::cerr
			<< __PRETTY_FUNCTION__ << " @ " << __FILE__ << ":" << __LINE__ << "\n"
			<< "\tFailed to set make branches\n";
		for (auto const& name : failed_branches) std::cerr
			<< "\t" << name << std::endl;
		std::cerr
			<< std::flush;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int
DedxBrancher::branch (
	TmvaHelper& tmva_helper
) {
	std::vector<std::string> missing_branches;
	for (int n = 0; n < m_num_daughters; ++n) {
		std::string name;

		name = (boost::format("track_%d_PDG_ID") % (n + 1)).str();
		if (!( m_track_pdg_id[n] = static_cast<int*>(tmva_helper.get_branch(name)) )) missing_branches.push_back(name);

		name = (boost::format("track_%d_p") % (n + 1)).str();
		if (!( m_track_p[n] = static_cast<float*>(tmva_helper.get_branch(name)) )) missing_branches.push_back(name);
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

int
DedxBrancher::eval (
) {
	for (int n = 0; n < m_num_daughters; ++n) {
		auto itr = m_fits.find(fabs(*m_track_pdg_id[n]));
		if (itr == m_fits.end()) {
			if (m_verbose) std::cout << __func__ << " bad pid" << std::endl;
			return EXIT_FAILURE;
		}

		float dedx_fit = itr->second->Eval(fabs(*m_track_p[n]));
		if (dedx_fit == 0.0) {
			if (m_verbose) std::cout << __func__ << " bad dedx val (n = " << n << " pid = " << fabs(*m_track_pdg_id[n]) << " p = " << fabs(*m_track_p[n]) << ")" << std::endl;
			return EXIT_FAILURE;
		}

		m_track_dedx_fit[n] = dedx_fit;
	}

	return EXIT_SUCCESS;
}

void
DedxBrancher::alloc_members (
) {
	m_track_pdg_id = new int*[m_num_daughters];
	m_track_p = new float*[m_num_daughters];

	m_track_dedx_fit = new float[m_num_daughters];
}

void
DedxBrancher::clean_members (
) {
	if (m_file) m_file->Close();

	delete[] m_track_pdg_id;
	delete[] m_track_p;

	delete[] m_track_dedx_fit;

	m_num_daughters = 0;
}
