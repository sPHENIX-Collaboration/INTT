#ifndef REFERENCE_C
#define REFERENCE_C

#include <cstddef>
#include <cstdio>
#include <cstdint>

#include <iostream>
#include <string>
#include <vector>

#include <intt/InttMapping.h>
R__LOAD_LIBRARY(libintt.so)

void Print(struct Intt::Online_s const&);

int Reference(std::string arg1, std::string arg2)
{
	std::cout << "arg0:\t" << "Reference.C" << std::endl;
	std::cout << "arg1:\t" << arg1 << std::endl;
	std::cout << "arg2:\t" << arg2 << std::endl;

	struct Intt::Online_s online = (struct Intt::Online_s){.lyr = 0, .ldr = 1, .arm = 1, .chp = 2, .chn = 3};
	Print(online);

	return EXIT_SUCCESS;
}

void Print(struct Intt::Online_s const& s)
{
	printf("\tlyr:%4d\n", s.lyr);
	printf("\tldr:%4d\n", s.ldr);
	printf("\tarm:%4d\n", s.arm);
	printf("\tchp:%4d\n", s.chp);
	printf("\tchn:%4d\n", s.chn);
}

#endif//REFERENCE_C
