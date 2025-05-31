#ifndef FUN4ALL_CHECK_C
#define FUN4ALL_CHECK_C

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllServer.h>
R__LOAD_LIBRARY(libfun4all.so)

#include <simpledstcheck/SimpleDstCheck.h>
R__LOAD_LIBRARY(libSimpleDstCheck.so)

void
Fun4All_Check (
	// std::string const& dst_in_path = "G4sPHENIX.root",
	std::string const& dst_in_path = "dstout.root",
	int n_events = 0
) {
	gSystem->Load("libg4dst");

	Fun4AllServer *se = Fun4AllServer::instance();
	se->Verbosity(1);

	Fun4AllDstInputManager *in = new Fun4AllDstInputManager("DSTIN");
	in->fileopen(dst_in_path);
	se->registerInputManager(in);

	SimpleDstCheck* simple_dst_check = new SimpleDstCheck;
	se->registerSubsystem(simple_dst_check);

	se->run(n_events);
	se->End();
}

#endif//FUN4ALL_CHECK_C
