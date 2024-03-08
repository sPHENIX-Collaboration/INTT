#ifndef COMBINED_RAW_DATA_CONVERTER_C
#define COMBINED_RAW_DATA_CONVERTER_C

#include <intt/InttMapping.h>
#include <intt/InttCombinedRawDataConverter.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <fun4allraw/SingleGl1PoolInput.h>
#include <fun4allraw/SingleInttPoolInput.h>
#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4allraw/Fun4AllEventOutputManager.h>

#include <phool/recoConsts.h>

#include <filesystem>
#include <iostream>
#include <string>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libintt.so)

void combined_raw_data_converter(
	std::string g_format,
	std::string i_format,
	std::string o_format,
	int run_num)
{
	std::cout << "combined_raw_data_converter:" << std::endl;
	std::cout << "\trunning..." << std::endl;

	Fun4AllServer* se = Fun4AllServer::instance();
	se->Verbosity(0);

	//input
	Fun4AllStreamingInputManager* in = new Fun4AllStreamingInputManager("Comb");

	int flag = 0;

	while(true)
	{
		std::string i_filename = Form(g_format.c_str(), run_num);
		if(!std::filesystem::exists(i_filename.c_str()))
		{
			std::cout << "Could not get file:" << std::endl;
			std::cout << "\t" << i_filename << std::endl;
			std::cout << "\t(Skipping)" << std::endl;
			std::cout << std::endl;

			flag = 1;
			break;
		}

		std::cout << "Using .evt file (GL1):" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		std::cout << std::endl;
		SingleGl1PoolInput* sngl = new SingleGl1PoolInput("GL1_0");
		sngl->AddFile(i_filename);
		in->registerStreamingInput(sngl, Fun4AllStreamingInputManager::enu_subsystem::GL1);

		break;
	}

	for(std::map<int, int>::const_iterator pid_itr = InttNameSpace::Packet_Id.begin(); pid_itr != InttNameSpace::Packet_Id.end(); ++pid_itr)
	{
		std::string i_filename = Form(i_format.c_str(), pid_itr->second, run_num);
		if(!std::filesystem::exists(i_filename.c_str()))
		{
			std::cout << "Could not get file:" << std::endl;
			std::cout << "\t" << i_filename << std::endl;
			std::cout << "\t(Skipping)" << std::endl;
			std::cout << std::endl;

			flag = 1;
			continue;
		}

		std::cout << "Using .evt file:" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		std::cout << std::endl;

		SingleInttPoolInput* sngl = new SingleInttPoolInput("INTT_" + std::to_string(pid_itr->second));
		sngl->AddFile(i_filename);
		in->registerStreamingInput(sngl, Fun4AllStreamingInputManager::enu_subsystem::INTT);
	}
	se->registerInputManager(in);

	if(flag)
	{
		std::cout << "Could not get all input files" << std::endl;
		std::cout << "Exiting" << std::endl;

		return 1;
	}

	std::string o_filename = Form(o_format.c_str(), run_num);
	std::cout << "Will write to output file:" << std::endl;
	std::cout << "\t" << o_filename << std::endl;

	//analysis
	InttCombinedRawDataConverter* intt_converter = new InttCombinedRawDataConverter();
	intt_converter->Verbosity(0);
	se->registerSubsystem(intt_converter);
	intt_converter->SetOutputFile(o_filename);

	//output
	se->run(10000);
	intt_converter->WriteOutputFile();
	se->End();

	std::cout << "combined_raw_data_converter:" << std::endl;
	std::cout << "\tdone..." << std::endl;

	delete se;
}

#endif//COMBINED_RAW_DATA_CONVERTER_C
