#ifndef RAW_DATA_DECDODER_C
#define RAW_DATA_DECDODER_C

#include <intt/InttRawDataDecoder.h>

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

void RawDataDecoder(std::string i_format, std::string o_format, int run_num, int server)
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
	se->Verbosity(Fun4AllBase::VERBOSITY_SOME);

	Fun4AllInputManager* in = new Fun4AllPrdfInputManager("PRDF");
	in->fileopen(i_filename.c_str());
	se->registerInputManager(in);

	SubsysReco* intt_decoder = new InttRawDataDecoder("INTTRAWDATADECODER");
	se->registerSubsystem(intt_decoder);

	Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", o_filename.c_str());
	se->registerOutputManager(out);

	se->run();
	se->End();

	delete se;

	exit(0);
}

#endif//INTT_RAW_DATA_DECDODER_C
