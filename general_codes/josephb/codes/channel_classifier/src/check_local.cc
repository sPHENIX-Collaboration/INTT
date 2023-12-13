#include "channel_classifier/base.h"
#include "channel_classifier/ver_1.h"

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>

std::string i_format = "/sphenix/user/jbertaux/Data/Debug/beam_intt_appended-%08d-0000.root";
std::string o_png_format = "/sphenix/user/jbertaux/Data/Debug/pngs/beam_intt_appended-%08d-0000.png";
std::string o_state_format = "/sphenix/user/jbertaux/Data/Debug/beam_intt_classified-%08d.root";

int main(int argc, char** argv) {
	typedef channel_classifier_ver_1 channel_classifier_t;
	static_assert(std::is_base_of<channel_classifier_base, channel_classifier_t>());

	if(argc != 2) {
		std::cout << "Expected at least one argument" << std::endl;
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

	char buff[256];
	snprintf(buff, sizeof(buff), i_format.c_str(), run_num);
	std::string i_filename = buff;
	if(!std::filesystem::exists(i_filename.c_str())) {
		std::cout << "Input file" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		std::cout << "Does not exist" << std::endl;
		std::cout << "Exiting" << std::endl;
		return 1;
	}

	channel_classifier_base* c = new channel_classifier_t();
	if(c->fill(i_filename))return 1;

	if(c->fit())return 1;

	snprintf(buff, sizeof(buff), o_png_format.c_str(), run_num);
	std::string o_png_filename = buff;
	if(c->draw(o_png_filename))return 1;

	snprintf(buff, sizeof(buff), o_state_format.c_str(), run_num);
	std::string o_state_filename = buff;
	if(c->classify(i_filename, o_state_filename))return 1;

	return 0;
}
