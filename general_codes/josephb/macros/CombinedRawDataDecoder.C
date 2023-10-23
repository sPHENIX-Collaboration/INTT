#ifndef COMBINED_RAW_DATA_DECODER_C
#define COMBINED_RAW_DATA_DECODER_C

#include <intt/InttMapping.h>
#include <intt/InttCombinedRawDataDecoder.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <fun4allraw/SingleInttInput.h>
#include <fun4allraw/Fun4AllEvtInputPoolManager.h>
#include <fun4allraw/Fun4AllEventOutputManager.h>

#include <phool/recoConsts.h>

#include <filesystem>
#include <iostream>
#include <string>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libintt.so)

void CombinedRawDataDecoder(std::string i_format, std::string o_format, int run_num)
{
	Fun4AllServer* se = Fun4AllServer::instance();
	se->Verbosity(0);

	//input
	Fun4AllEvtInputPoolManager* in = new Fun4AllEvtInputPoolManager("Comb");
	for(std::map<int, int>::const_iterator pid_itr = Intt::Packet_Id.begin(); pid_itr != Intt::Packet_Id.end(); ++pid_itr)
	{
		std::string i_filename = Form(i_format.c_str(), pid_itr->second, run_num);
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

		SingleInttInput* sngl = new SingleInttInput("INTT_" + std::to_string(pid_itr->second));
		sngl->AddFile(i_filename);
		in->registerStreamingInput(sngl, Fun4AllEvtInputPoolManager::enu_subsystem::INTT);
	}
	se->registerInputManager(in);

	std::string o_filename = Form(o_format.c_str(), run_num);
	std::cout << "Will write to output file:" << std::endl;
	std::cout << "\t" << o_filename << std::endl;

	//analysis
	InttCombinedRawDataDecoer* intt_decoder = new InttCombinedRawDataDecoder();
	intt_decoder->Verbosity(0);
	se->registerSubsystem(intt_decoder);

	//output
	se->run();
	se->End();

	delete se;
}

#endif//COMBINED_RAW_DATA_DECODER_C
