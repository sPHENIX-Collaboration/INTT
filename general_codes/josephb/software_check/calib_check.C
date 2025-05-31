#ifndef CALIB_CHECK
#define CALIB_CHECK

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllServer.h>
R__LOAD_LIBRARY(libfun4all.so)

#include <intt/InttBCOMap.h>
#include <intt/InttBadChannelMap.h>
#include <intt/InttMapping.h>
R__LOAD_LIBRARY(libintt.so)

#include <inttcalib/InttCalib.h>
R__LOAD_LIBRARY(libinttcalib.so)

#include <phool/recoConsts.h>
R__LOAD_LIBRARY(libphool.so)

#include <boost/format.hpp>

void
print (
	InttNameSpace::RawData_s const& rawdata
) {
	std::cout << (boost::format (
		"RawData_s { .felix_server = %1d, .felix_channel = %2d, .chip = %2d, .channel = %3d }")
		% rawdata.felix_server % rawdata.felix_channel % rawdata.chip % rawdata.channel).str();
}

void
calib_check (
	int n_events = 0,
	std::string const& dst_in_path = "dstout.root"
) {
	/// Run InttCalib with Fun4All
	gSystem->Load("libg4dst");

	auto rc = recoConsts::instance();
	rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
	rc->set_IntFlag("RUNNUMBER", 53877);

	Fun4AllServer *se = Fun4AllServer::instance();
	se->Verbosity(1);

	Fun4AllDstInputManager *in = new Fun4AllDstInputManager("DSTIN");
	in->fileopen(dst_in_path);
	se->registerInputManager(in);

	InttCalib* intt_calib = new InttCalib;
	intt_calib->SetHotMapCdbFile("hotmap_cdb.root");
	intt_calib->SetBcoMapCdbFile("bcomap_cdb.root");
	se->registerSubsystem(intt_calib);

	se->run(n_events);
	se->End();

	/// Check the produced calibrations after Fun4All
	InttBCOMap bco_map;
	bco_map.LoadFromFile("bcomap_cdb.root");

	InttBadChannelMap hot_map;
	hot_map.Load("hotmap_cdb.root");

	uint64_t bco_full = 0x0u; // The implementation of InttBCOMap only takes the difference, so this can be any arbitrary value
	InttNameSpace::RawData_s raw;
	for (raw.felix_server = 0; raw.felix_server < 8; ++raw.felix_server) {
		for (raw.felix_channel = 0; raw.felix_channel < 14; ++raw.felix_channel) {
			for (raw.chip = 0; raw.chip < 26; ++raw.chip) {
				for (raw.channel = 0; raw.channel < 128; ++raw.channel) {
					print(raw); std::cout << std::endl;
					std::cout << " hot: " << hot_map.IsBad(raw) << std::endl;
					for (int bco = 0; bco < 128; ++bco) {
						std::cout << "\tbco: " << hot_map.IsBad(raw, bco_full, bco) << std::endl;
					}
				}
			}
		}
	}
}

#endif//CALIB_CHECK
