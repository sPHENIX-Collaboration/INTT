#ifndef RAW_DATA_CONVERTER_C
#define RAW_DATA_CONVERTER_C

#include <intt/InttRawDataConverter.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/recoConsts.h>

#include <filesystem>
#include <iostream>
#include <string>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libintt.so)

void RawDataConverter(std::string i_format, std::string o_format, int run_num, int server)
{
	std::string i_filename = Form(i_format.c_str(), server, run_num);
	std::string o_filename = Form(o_format.c_str(), server, run_num);

	std::cout << "Using input file:" << std::endl;
	std::cout << "\t" << i_filename << std::endl;
	std::cout << "Will write to output file:" << std::endl;
	std::cout << "\t" << o_filename << std::endl;

	if(!std::filesystem::exists(i_filename.c_str()))
	{
		std::cout << "Input file:" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		std::cout << "Does not exist" << std::endl;
		std::cout << "Exiting" << std::endl;

		exit(1);
	}

	Fun4AllServer* se = Fun4AllServer::instance();

	//input
	Fun4AllInputManager* in = new Fun4AllPrdfInputManager("PRDF");
	in->fileopen(i_filename.c_str());
	se->registerInputManager(in);

	//ana
	InttRawDataConverter* intt_converter = new InttRawDataConverter();
	se->registerSubsystem(intt_converter);
	intt_converter->SetOutputFile(o_filename);

	//output
	se->run();
	intt_converter->WriteOutputFile();
	se->End();

	delete se;
}

#endif//INTT_RAW_DATA_CONVERTER_C
