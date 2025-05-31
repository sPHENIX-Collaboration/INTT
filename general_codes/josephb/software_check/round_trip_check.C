#ifndef ROUND_TRIP_CHECK_C
#define ROUND_TRIP_CHECK_C

#include <intt/InttMapping.h>
R__LOAD_LIBRARY(libintt.so)

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
print (
	InttNameSpace::Online_s const& online
) {
	std::cout << (boost::format (
		"Online_s { .lyr = %1d, .ldr = %2d, .arm = %2d, .chp = %2d, .chn = %3d }")
		 % online.lyr % online.ldr % online.arm % online.chp % online.chn).str();
}

void
print (
	InttNameSpace::Offline_s const& offline
) {
	std::cout << (boost::format (
		"Online_s { .layer = %1d, .ladder_phi = %2d, .ladder_z = %2d, .strip_x = %2d, .strip_y = %3d }")
		% offline.layer % offline.ladder_phi % offline.ladder_z % offline.strip_x % offline.strip_y).str();
}

void
round_trip_check (
) {
	for (int felix_server = 0; felix_server < 8; ++felix_server) {
		for (int felix_channel = 0; felix_channel < 14; ++felix_channel) {
			for (int chip = 0; chip < 26; ++chip) {
				for (int channel = 0; channel < 128; ++channel) {

					InttNameSpace::RawData_s rawdata;
					rawdata.felix_server = felix_server;
					rawdata.felix_channel = felix_channel;
					rawdata.chip = chip;
					rawdata.channel = channel;

					InttNameSpace::Online_s online = InttNameSpace::ToOnline(rawdata);
					InttNameSpace::Offline_s offline = InttNameSpace::ToOffline(rawdata);

					print(rawdata); std::cout << std::endl;
					print(online); std::cout << std::endl;
					print(offline); std::cout << std::endl;

					// Checks
					if (InttNameSpace::ToRawData(online) != rawdata) {
						std::cout << "Problem on line " << __LINE__ << std::endl;
					}

					if (InttNameSpace::ToRawData(offline) != rawdata) {
						std::cout << "Problem on line " << __LINE__ << std::endl;
					}

					if (InttNameSpace::ToOnline(offline) != online) {
						std::cout << "Problem on line " << __LINE__ << std::endl;
					}

					if (InttNameSpace::ToOffline(online) != offline) {
						std::cout << "Problem on line " << __LINE__ << std::endl;
					}
				}
			}
		}
	}
}

#endif//ROUND_TRIP_CHECK_C
