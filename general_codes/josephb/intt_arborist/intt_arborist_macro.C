#ifndef INTT_ARBORIST_MACRO_C
#define INTT_ARBORIST_MACRO_C

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4allraw/InputManagerType.h>
#include <fun4allraw/SingleInttPoolInput.h>

#include <intt/InttArborist.h>
#include <phool/recoConsts.h>

#include <filesystem>
#include <iostream>
#include <string>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libintt.so)

void
intt_arborist_macro (
	std::string const& i_format,
	std::string const& o_format,
	int run_num
) {
	Fun4AllServer* se = Fun4AllServer::instance();
	se->Verbosity(0);

	// Input Manager
	recoConsts* rc = recoConsts::instance();
	Fun4AllStreamingInputManager* in = new Fun4AllStreamingInputManager("Comb");

	bool no_input_files = true;
	for(int fee = 0; fee < 8; ++fee) {

		std::string i_filename = Form(i_format.c_str(), fee, run_num);
		if(!std::filesystem::exists(i_filename.c_str())) {
			std::cout << "Could not get file:" << std::endl;
			std::cout << "\t" << i_filename << std::endl;
			std::cout << "\t(Skipping)" << std::endl;
			std::cout << std::endl;

			continue;
		}

		std::cout << "Using .evt file:" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		std::cout << std::endl;

		SingleInttPoolInput* sngl = new SingleInttPoolInput("INTT_" + std::to_string(fee));
		sngl->AddFile(i_filename);
		in->registerStreamingInput(sngl, InputManagerType::INTT);

		no_input_files = false;
	}

	if(no_input_files) {
		std::cout << "No input files found" << std::endl;
		std::cout << "Exiting without running" << std::endl;

		delete se;
		gSystem->Exit(1);
		exit(1);
	}

	se->registerInputManager(in);

	std::string o_filename = Form(o_format.c_str(), run_num);
	std::cout << "Will write to output file:" << std::endl;
	std::cout << "\t" << o_filename << std::endl;
	std::cout << std::endl;

	// Analysis modules
	InttArborist* intt_arborist = new InttArborist();
	if(intt_arborist->CreateOutputFile(o_filename)) {
		delete intt_arborist;
		delete se;
		gSystem->Exit(1);
		exit(1);
	}
	se->registerSubsystem(intt_arborist);

	// Run
	se->run(3);
	se->End();

	delete intt_arborist;
	delete se;
	gSystem->Exit(1);
	exit(1);

}

#endif//ARBORIST_MACRO_C
