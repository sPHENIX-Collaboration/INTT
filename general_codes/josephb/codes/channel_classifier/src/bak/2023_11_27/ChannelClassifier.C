#ifndef CHANNEL_CLASSIFIER_C
#define CHANNEL_CLASSIFIER_C

#include <intt/InttMapping.h>
R__LOAD_LIBRARY(libintt.so)

void ChannelClassifier()
{
	std::cout << "Hello World" << std::endl;
	Intt::Online_s onl;

	onl.lyr = 0;
	onl.ldr = 0;
	onl.arm = 0;
	onl.chp = 0;
	onl.chn = 0;

	std::cout << "\t" << onl.lyr << std::endl;
	std::cout << "\t" << onl.ldr << std::endl;
	std::cout << "\t" << onl.arm << std::endl;
	std::cout << "\t" << onl.chp << std::endl;
	std::cout << "\t" << onl.chn << std::endl;
}

#endif//CHANNEL_CLASSIFIER_C
