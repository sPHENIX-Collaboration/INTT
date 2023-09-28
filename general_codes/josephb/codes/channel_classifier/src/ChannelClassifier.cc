#include "ChannelClassifier.h"

Intt::Online_s ChannelClassifier::struct_from_hash(Int_t h)
{
	Intt::Online_s s = (Intt::Online_s){.lyr = 0, .ldr = 0, .arm = 0, .chp = 0, .chn = 0};

	s.chn = h % 128;
	h /= 128;

	s.chp = h % 26;
	h /= 26;

	s.arm = h % 2;
	h /= 2;

	s.ldr = h;
	while(true)
	{
		if(s.ldr < 12)break;
		s.ldr -= 12;
		++s.lyr;

		if(s.ldr < 12)break;
		s.ldr -= 12;
		++s.lyr;

		if(s.ldr < 16)break;
		s.ldr -= 16;
		++s.lyr;

		break;
	}

	return s;
}

Int_t ChannelClassifier::hash_from_struct(Intt::Online_s const& s)
{
	Int_t h = 0;
	int b = 1;

	h += s.chn * b;
	b *= 128;

	h += s.chp * b;
	b *= 26;

	h += s.arm * b;
	b *= 2;

	h += s.ldr * b;
	while(true)
	{
		if(s.lyr < 1)break;
		h += 12 * b;

		if(s.lyr < 2)break;
		h += 12 * b;

		if(s.lyr < 3)break;
		h += 16 * b;

		break;
	}

	return h;
}

Int_t ChannelClassifier::hash_from_struct(Intt::Offline_s const& s)
{
	return hash_from_struct(Intt::ToOnline(s));
}

Int_t ChannelClassifier::hash_from_struct(Intt::RawData_s const& s)
{
	return hash_from_struct(Intt::ToOnline(s));
}

void ChannelClassifier::print(Intt::Online_s const& s)
{
	printf("lyr: %2d ldr: %2d arm: %1d chp: %2d chn: %3d\n", s.lyr, s.ldr, s.arm, s.chp, s.chn);
}

int ChannelClassifier::AppendFile(std::string const& filename)
{
	int return_val = 0;
	std::stringstream output;

	output << "int ChannelClassifier::AppendFile(std::string const& filename)" << std::endl;
	output << "\twith 'filename' = '" << filename << "'" << std::endl;

	TFile* file = nullptr;
	TTree* tree = nullptr;
	Intt::Online_s onl;

	std::map<std::string, std::vector<Int_t>*> branches =
	{
		{"lyr",	nullptr},
		{"ldr",	nullptr},
		{"arm",	nullptr},
		{"chp",	nullptr},
		{"chn",	nullptr},
	};

	if(!std::filesystem::exists(filename))
	{
		output << "\tpath 'filename' does not exist" << std::endl;
		output << "file not appended" << std::endl;
		output << std::endl;

		return_val = 1;
		goto LABEL;
	}

	file = TFile::Open(filename.c_str(), "READ");
	if(!file)
	{
		output << "\tfailed to open file 'filename' as ROOT file" << std::endl;
		output << "file not appended" << std::endl;

		return_val = 1;
		goto LABEL;
	}
	output << "In file:" << std::endl;
	output << "\t" << filename << std::endl;

	tree = (TTree*)file->Get(treename.c_str());
	if(!tree)
	{
		output << "\tfailed to get TTree '" << treename << "' in file" << std::endl;
		output << "file not appended" << std::endl;

		return_val = 1;
		goto LABEL;
	}
	output << "In tree:" << std::endl;
	output << "\t" << treename << std::endl;

	for(auto& itr: branches)
	{
		if(!tree->GetBranch(itr.first.c_str()))
		{
			output << "\tfailed to get branch '" << itr.first << "'" << std::endl;
			return_val = 1;
		}
	}
	if(return_val)goto LABEL;

	LABEL:
	output << std::ends;

	if(file)file->Close();

	if(return_val)
	{
		std::cout << output.str();
		return return_val;
	}

	Long64_t** hit_array = new Long64_t*[NUM_THREADS];
	std::vector<std::thread> threads;
	for(int i = 0; i < NUM_THREADS; ++i)
	{
		hit_array[i] = new Long64_t[NUM_CHANNELS];
		threads.push_back(std::thread(ChannelClassifier::AppendHits, filename, treename, hit_array[i], NUM_THREADS, i));
	}
	for(auto& thr: threads)
	{
		thr.join();
	}

	Long64_t* hits = new Long64_t[NUM_CHANNELS];
	for(int i = 0; i < NUM_CHANNELS; ++i)
	{
		hits[i] = 0;
		for(int j = 0; j < NUM_THREADS; ++j)
		{
			hits[i] += hit_array[j][i];
		}
	}

	for(int i = 0; i < NUM_THREADS; ++i)
	{
		delete[] hit_array[i];
	}
	delete[] hit_array;

	return_val = PoissonFit(hits);
	delete[] hits;

	return return_val;
}

void ChannelClassifier::AppendHits(std::string const& filename, std::string const& treename, Long64_t* hits, int num_threads, int modulus)
{
	Intt::Online_s onl;

	std::map<std::string, std::vector<Int_t>*> branches =
	{
		{"lyr",	new std::vector<Int_t>()},
		{"ldr",	new std::vector<Int_t>()},
		{"arm",	new std::vector<Int_t>()},
		{"chp",	new std::vector<Int_t>()},
		{"chn",	new std::vector<Int_t>()},
	};

	TFile* file = TFile::Open(filename.c_str(), "READ");
	TTree* tree = (TTree*)file->Get(treename.c_str());

	for(auto& itr: branches)
	{
		tree->SetBranchAddress(itr.first.c_str(), &(itr.second));
	}

	for(Long64_t n = 0; n < NUM_CHANNELS; ++n)
	{
		hits[n] = 0;
	}

	for(Long64_t n = modulus; n < tree->GetEntriesFast(); n+=num_threads)
	{
		if(n > 5)break;

		tree->GetEntry(n);
		if(modulus == 0)std::cout << "Multiplicity: " << branches.begin()->second->size() << std::endl;

		for(std::size_t s = 0; s < branches.begin()->second->size(); ++s)
		{
			if(s > 5)break;

			onl.lyr = (*branches["lyr"])[s];
			onl.ldr = (*branches["ldr"])[s];
			onl.arm = (*branches["arm"])[s];
			onl.chp = (*branches["chp"])[s];
			onl.chn = (*branches["chn"])[s];

			++hits[hash_from_struct(onl)];

			if(modulus == 0)print(onl);
		}
	}

	for(auto& itr: branches)
	{
		if(itr.second)delete itr.second;
	}

	file->Close();
}

int ChannelClassifier::PoissonFit(Long64_t* hit_array)
{
	struct Intt::Online_s onl;
	Long64_t hits;

	TTree* tree = new TTree(outtreename.c_str(), outtreename.c_str());

	tree->Branch("lyr", &(onl.lyr));
	tree->Branch("ldr", &(onl.ldr));
	tree->Branch("arm", &(onl.arm));
	tree->Branch("chp", &(onl.chp));
	tree->Branch("chn", &(onl.chn));

	tree->Branch("hits", &hits);

	for(int i = 0; i < NUM_CHANNELS; ++i)
	{
		onl = struct_from_hash(i);
		hits = hit_array[i];

		tree->Fill();
	}

	tree->Print();

	return 0;
}

