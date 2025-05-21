#ifndef MACRO_C
#define MACRO_C

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllServer.h>
R__LOAD_LIBRARY(libfun4all.so)

#include <carriedoverexample/CarriedOverExample.h>
R__LOAD_LIBRARY(libCarriedOverExample.so)

#include <boost/format.hpp>

void macro (
	int num_events = 30,
	std::string const& list_file_format = "intt%d.list"
) {
	// Server
	Fun4AllServer* se = Fun4AllServer::instance();
	se->Verbosity(1);

	// Input
	for (int i = 0; i < 8; ++i) {
		std::string dst_name = (boost::format("DSTIN_INTT%d") % i).str();
		Fun4AllDstInputManager* in = new Fun4AllDstInputManager(dst_name);
		std::string list_file_name = (boost::format(list_file_format) % i).str();
		if (!std::filesystem::exists(list_file_name)) {
			std::cout
				<< "Expected list file:\n"
				<< "\t" << list_file_name << "\n"
				<< "Not found\n"
				<< "Exiting\n"
				<< std::flush;
			exit(1);
		}
		in->AddListFile(list_file_name);
		se->registerInputManager(in);
	}

	// Modules
	CarriedOverExample* carried_over_example = new CarriedOverExample;
	carried_over_example->Verbosity(1);
	se->registerSubsystem(carried_over_example);

	// Run
	std::cout << "Running" << std::endl;
	se->run(num_events);

	std::cout << "Macro done" << std::endl;
	se->End();

	delete se;
	gSystem->Exit(1);
	exit(1);
}

#endif // MACRO_C
