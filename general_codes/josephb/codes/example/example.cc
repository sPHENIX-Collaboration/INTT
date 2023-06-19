#include <cstddef>
#include <cstdio>
#include <cstdint>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <intt/InttMapping.h>

void print(struct Intt::Online_s const&);

int main()
{
	printf("Hello World\n");

	struct Intt::Online_s online = (struct Intt::Online_s){.lyr = 0, .ldr = 1, .arm = 1, .chp = 2, .chn = 3};
	print(online);

	return EXIT_SUCCESS;
}

void print(struct Intt::Online_s const& s)
{
	printf("\tlyr:%4d\n", s.lyr);
	printf("\tldr:%4d\n", s.ldr);
	printf("\tarm:%4d\n", s.arm);
	printf("\tchp:%4d\n", s.chp);
	printf("\tchn:%4d\n", s.chn);
}
