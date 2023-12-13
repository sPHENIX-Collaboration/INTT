#ifndef INTT_UNPACKER_JB_C
#define INTT_UNPACKER_JB_C

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <fun4allraw/SingleInttInput.h>
#include <fun4allraw/Fun4AllEvtInputPoolManager.h>
#include <fun4allraw/Fun4AllEventOutputManager.h>

#include <intt_josephb/InttUnpackerJb.h>

#include <phool/recoConsts.h>

#include <filesystem>
#include <iostream>
#include <string>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libintt_josephb.so)

void InttUnpackerMacroJb(std::string i_format, std::string o_format, int run_num)
{
	Fun4AllServer* se = Fun4AllServer::instance();
	se->Verbosity(0);

	// Input Manager
	Fun4AllEvtInputPoolManager* in = new Fun4AllEvtInputPoolManager("Comb");

	bool no_input_files = true;
	for(int flx_svr = 0; flx_svr < 8; ++flx_svr)
	{
		std::string i_filename = Form(i_format.c_str(), flx_svr, run_num);
		if(!std::filesystem::exists(i_filename.c_str()))
		{
			std::cout << "Could not get file:" << std::endl;
			std::cout << "\t" << i_filename << std::endl;
			std::cout << "\t(Skipping)" << std::endl;
			std::cout << std::endl;

			continue;
		}

		std::cout << "Using .evt file:" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		std::cout << std::endl;

		SingleInttInput* sngl = new SingleInttInput("INTT_" + std::to_string(flx_svr));
		sngl->AddFile(i_filename);
		in->registerStreamingInput(sngl, Fun4AllEvtInputPoolManager::enu_subsystem::INTT);

		no_input_files = false;
	}
	se->registerInputManager(in);
	if(no_input_files)
	{
		std::cout << "No input files found" << std::endl;
		std::cout << "Exiting without running" << std::endl;

		delete se;
		gSystem->Exit(1);
		exit(1);
	}

	// Analysis modules
	InttUnpackerJb* intt_unpacker = new InttUnpackerJb();
	intt_unpacker->Verbosity(1); // Change the verbosity if you need
	se->registerSubsystem(intt_unpacker);

	std::string o_filename = Form(o_format.c_str(), run_num);
	intt_unpacker->SetOutputFile(o_filename);
	std::cout << "Will write to output file:" << std::endl;
	std::cout << "\t" << o_filename << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	// Run
	se->run(2);
	se->End();

	delete se;
	gSystem->Exit(1);
	exit(1);
}

#endif//INTT_UNPACKER_JB_C
