#include "ChannelClassifier.h"

#include <cstdio>

int main()
{
	ChannelClassifier c;

	//c.AppendFile("/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt_combined-00023046-0000.root");
	//c.OutputHits("local.root");
	c.PoissonFit("local.root");

	std::cout << "Done" << std::endl;

	return 0;
}
