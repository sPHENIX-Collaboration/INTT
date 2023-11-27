#include "ChannelClassifier.h"

#include <cstdio>

int run_num = 20869;

std::string i_format = "/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt_combined-%08d-0000.root";
std::string o_format = "/sphenix/user/jbertaux/Data/channel_classifier/beam_intt_appended-%08d-0000.root";

int main()
{
	char buff[256];
	//ChannelClassifier c;


	snprintf(buff, sizeof(buff), i_format.c_str(), run_num);
	std::cout << buff << std::endl;

	//snprintf(buff, sizeof(buff), i_format.c_str(), run_num);
	//c.AppendFile(buff);

	//snprintf(buff, sizeof(buff), o_format.c_str(), run_num);
	//c.OutputHits(buff);

	//snprintf(buff, sizeof(buff), o_format.c_str(), run_num);
	//c.PoissonFit(buff);

	std::cout << "Done" << std::endl;

	return 0;
}
