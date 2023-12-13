#include "channel_classifier_helper.h"

#include <cstdio>

#include <filesystem>
#include <string>

std::string o_format = "/sphenix/user/jbertaux/Data/Debug/beam_intt_appended-%08d-0000.root";

int main(int argc, char** argv) {
	if(argc != 3) {
		std::cout << "Expected two arguments" << std::endl;
		std::cout << "Exiting" << std::endl;
		return 1;
	}

	int run_num = -1;
	try {
		run_num = std::stoi(argv[1]);
	} catch (std::invalid_argument& e) {
		std::cout << "Could not cast argument" << std::endl;
		std::cout << "\t" << argv[1] << std::endl;
		std::cout << "As int (expected this positional argument as run number)" <<std::endl;
		std::cout << e.what() << std::endl;
		std::cout << "Exiting" << std::endl;
		return 1;
	} catch (std::exception& e) {
		std::cout << "Other std::stoi failure" << std::endl;
		std::cout << e.what() << std::endl;
		return 1;
	}

	channel_classifier_helper c;

	char buff[256];
	snprintf(buff, sizeof(buff), o_format.c_str(), run_num);
	if(!std::filesystem::exists(buff)) {
		std::cout << "Output file" << std::endl;
		std::cout << "\t" << buff << std::endl;
		std::cout << "Does not exist prior to classification" << std::endl;
		if(!std::filesystem::exists(argv[2])) {
			std::cout << "Nor does the original converted .root file," << std::endl;
			std::cout << "\t" << argv[2] << std::endl;
			std::cout << "The wrapper would've created this, probably a problem with the converter or in writing the file to disk" << std::endl;
			return 1;
		}
		std::cout << "Creating it..." << std::endl;
		c.append_file(argv[2]);
		c.output_hits(buff);
		std::cout << "...done" << std::endl;
	}

	std::cout << ".exe:" << std::endl;
	std::cout << "\tDone" << std::endl;

	return 0;
}
