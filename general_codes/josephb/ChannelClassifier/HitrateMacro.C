#ifndef INTTHITRATES_C
#define INTTHITRATES_C

#include <intt/InttMapping.h>
#include <intthitrates/InttHitrates.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>

#include <fun4allraw/SingleGl1PoolInput.h>
#include <fun4allraw/SingleInttPoolInput.h>
#include <fun4allraw/Fun4AllStreamingInputManager.h>

#include <phool/recoConsts.h>

#include <climits>

#include <filesystem>
#include <iostream>
#include <string>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libintt.so)
R__LOAD_LIBRARY(libInttHitrates.so)

void HitrateMacro(
	int run_num = 20444,
	int bco_range = 2
) {
	std::string i_format = "/sphenix/lustre01/sphnxpro/commissioning/INTT/beam/beam_intt%d-%08d-0000.evt";
	std::string g_format = "/sphenix/lustre01/sphnxpro/commissioning/GL1/beam/GL1_beam_gl1daq-%08d-0000.prdf";
	std::string o_format = "/sphenix/user/jbertaux/Data/Debug/hitrates-run-%08d-bco-range-%08d.root";

	Fun4AllServer* se = Fun4AllServer::instance();
	se->Verbosity(0);

	// input
	bool flag = false;
	Fun4AllStreamingInputManager* in = new Fun4AllStreamingInputManager("Comb");
	for (
		std::map<int, int>::const_iterator pid_itr = InttNameSpace::Packet_Id.begin();
		pid_itr != InttNameSpace::Packet_Id.end();
		++pid_itr
	) {
		std::string i_filename = Form (
			i_format.c_str(),
			pid_itr->second,
			run_num
		);

		if( !std::filesystem::exists(i_filename.c_str()) ) {
			std::cout << "Could not get file:" << std::endl;
			std::cout << "\t'" << i_filename << "'" << std::endl;
			std::cout << "\t(Skipping)" << std::endl;
			std::cout << std::endl;
			flag = true;

			continue;
		}

		std::cout << "Using input file:" << std::endl;
		std::cout << "\t'" << i_filename << "'" << std::endl;
		std::cout << std::endl;

		SingleInttPoolInput* sngl = new SingleInttPoolInput (
			"INTT_" + std::to_string(pid_itr->second)
		);
		sngl->AddFile(i_filename);
		sngl->SetNegativeBco(1);
		sngl->SetBcoRange(bco_range);
		in->registerStreamingInput (
			sngl,
			Fun4AllStreamingInputManager::INTT
		);
	}

	while( true ) {
		std::string g_filename = Form (
			g_format.c_str(),
			run_num
		);

		if( !std::filesystem::exists(g_filename.c_str()) ) {
			std::cout << "Could not get file:" << std::endl;
			std::cout << "\t'" << g_filename << "'" << std::endl;
			std::cout << "\t(Skipping)" << std::endl;
			std::cout << std::endl;

			flag = true;
			break;
		}

		std::cout << "Using input file:" << std::endl;
		std::cout << "\t'" << g_filename << "'" << std::endl;
		std::cout << std::endl;

		SingleGl1PoolInput* sngl = new SingleGl1PoolInput("GL1_0");
		sngl->AddFile(g_filename);
		in->registerStreamingInput (
			sngl,
			Fun4AllStreamingInputManager::GL1
		);

		break;
	}

	if( flag ) {
		std::cout << "Could not get all files" << std::endl;
		std::cout << "Exiting" << std::endl;
		delete se;

		return;
	}

	se->registerInputManager(in);

	// analysis
	InttHitrates* intt_hitrates = new InttHitrates();
	se->registerSubsystem(intt_hitrates);
	intt_hitrates->set_output_file(Form (
		o_format.c_str(),
		run_num,
		bco_range
	));
	std::cout << "Writing to:" << std::endl;
	std::cout << "\t" << Form (
		o_format.c_str(),
		run_num
	) << std::endl;

	// running
	se->run();
	se->End();
	delete se;
}

#endif//INTTHITRATES_C
