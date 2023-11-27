#include "channel_classifier.h"

channel_classifier::channel_classifier()
{
	m_hits = new Long64_t[NUM_CHANNELS];
	m_branches = {
		{"flx_svr",	new std::vector<Int_t>()},
		{"flx_chn",	new std::vector<Int_t>()},
		{"chp",		new std::vector<Int_t>()},
		{"chn",		new std::vector<Int_t>()},
	};
	Reset();
}

channel_classifier::~channel_classifier()
{
	delete[] m_hits;
	for(auto& itr: m_branches) {
		delete itr.second;
	}
}

void channel_classifier::Reset()
{
	m_evts = 0;
	for(int i = 0; i < NUM_CHANNELS; ++i) {
		m_hits[i] = 0;
	}
}

InttNameSpace::RawData_s channel_classifier::struct_from_hash(Int_t h)
{
	InttNameSpace::RawData_s s = (InttNameSpace::RawData_s) {
		.felix_server = 0,
		.felix_channel = 0,
		.chip = 0,
		.channel = 0
	};

	s.channel = h % 128;
	h /= 128;

	s.chip = h % 26;
	h /= 26;

	s.felix_channel = h % 14;
	h /= 14;

	s.felix_server = h % 8;

	return s;
}

Int_t channel_classifier::hash_from_struct(InttNameSpace::RawData_s const& s)
{
	Int_t h = 0;
	int b = 1;

	h += s.channel * b;
	b *= 128;

	h += s.chip * b;
	b *= 26;

	h += s.felix_channel * b;
	b *= 14;

	h += s.felix_server * b;

	return h;
}

Int_t channel_classifier::hash_from_struct(InttNameSpace::Online_s const& s)
{
	return hash_from_struct(InttNameSpace::ToRawData(s));
}

Int_t channel_classifier::hash_from_struct(InttNameSpace::Offline_s const& s)
{
	return hash_from_struct(InttNameSpace::ToRawData(s));
}

void channel_classifier::print(InttNameSpace::RawData_s const& s)
{
	printf("\t%7s: %3d\n", "flx_svr",	s.felix_server);
	printf("\t%7s: %3d\n", "flx_chn",	s.felix_channel);
	printf("\t%7s: %3d\n", "chp",		s.chip);
	printf("\t%7s: %3d\n", "chn",		s.channel);
}

int channel_classifier::append_file(std::string const& i_filename)
{
	int return_val = 0;
	std::stringstream output;

	output << "int channel_classifier::append_file(std::string const& i_filename)" << std::endl;
	output << "\twith 'i_filename' = '" << i_filename << "'" << std::endl;

	TFile* file = nullptr;
	TTree* tree = nullptr;
	InttNameSpace::RawData_s raw;

	if(!std::filesystem::exists(i_filename)) {
		output << "\tpath 'i_filename' does not exist" << std::endl;
		output << "file not appended" << std::endl;
		output << std::endl;

		return_val = 1;
		goto LABEL;
	}

	file = TFile::Open(i_filename.c_str(), "READ");
	if(!file) {
		output << "\tfailed to open file 'i_filename' as ROOT file" << std::endl;
		output << "file not appended" << std::endl;

		return_val = 1;
		goto LABEL;
	}
	output << "In file:" << std::endl;
	output << "\t" << i_filename << std::endl;

	tree = (TTree*)file->Get(i_treename.c_str());
	if(!tree) {
		output << "\tfailed to get TTree '" << i_treename << "' in file" << std::endl;
		output << "file not appended" << std::endl;

		return_val = 1;
		goto LABEL;
	}
	output << "In tree:" << std::endl;
	output << "\t" << i_treename << std::endl;

	for(auto& itr: m_branches) {
		if(!tree->GetBranch(itr.first.c_str())) {
			output << "\tfailed to get branch '" << itr.first << "'" << std::endl;
			return_val = 1;
		}
	}
	if(return_val)goto LABEL;

	for(auto& itr: m_branches) {
		tree->SetBranchAddress(itr.first.c_str(), &(itr.second));
	}

	m_evts += tree->GetEntriesFast();
	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		for(std::size_t s = 0; s < m_branches.begin()->second->size(); ++s) {
			raw.felix_server	= (*m_branches["flx_svr"])[s];
			raw.felix_channel	= (*m_branches["flx_chn"])[s];
			raw.chip		= (*m_branches["chp"])[s];
			raw.channel		= (*m_branches["chn"])[s];

			//if(n < 10 && s < 10) {
			//	std::cout << hash_from_struct(raw) << std::endl;
			//	print(raw);
			//	std::cout << std::endl;
			//}

			++(m_hits[hash_from_struct(raw)]);
		}
	}

	LABEL:
	output << std::ends;
	if(file)file->Close();
	if(return_val)std::cout << output.str();
	return return_val;
}

int channel_classifier::output_hits(std::string const& o_filename)
{
	int return_val = 0;
	std::stringstream output;

	output << "int channel_classifier::output_hits(std::string const& o_filename)" << std::endl;
	output << "\twith 'o_filename' = '" << o_filename << "'" << std::endl;

	Double_t hits;
	struct InttNameSpace::RawData_s raw;

	TFile* file = nullptr;
	TTree* tree = nullptr;

	file = TFile::Open(o_filename.c_str(), "RECREATE");
	if(!file) {
		output << "Could not create file:" << std::endl;
		output << "\t" << o_filename << std::endl;
		output << "Nothing written" << std::endl;
		return_val = 1;
		goto LABEL;
	}
	tree = new TTree(o_treename.c_str(), o_treename.c_str());

	tree->Branch("hits",	&hits);
	tree->Branch("flx_svr",	&(raw.felix_server));
	tree->Branch("flx_chn",	&(raw.felix_channel));
	tree->Branch("chp",	&(raw.chip));
	tree->Branch("chn",	&(raw.channel));

	// for(int i = 0; i < NUM_CHANNELS; ++i) {
	// 	if(m_hits[i] > 0) std::cout << i << "\t" << m_hits[i] << std::endl;
	// }

	for(int i = 0; i < NUM_CHANNELS; ++i) {
		hits = m_hits[i];
		hits /= m_evts;
		raw = struct_from_hash(i);
		tree->Fill();
	}

	LABEL:
	output << std::ends;
	if(tree)tree->Write();
	if(file)file->Write();
	if(file)file->Close();
	if(return_val)std::cout << output.str();
	return return_val;
}

int channel_classifier::hitrate_fit(std::string const&)
{
	// do nothing

	return 0;
}

int channel_classifier::classify(std::string const&)
{
	// do nothing

	return 0;
}
