#include <cstddef>
#include <cstdio>
#include <cstdint>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <intt/InttMapping.h>
#include <phparameter/PHParameters.h>

void print(struct InttNameSpace::Online_s const&);

int main()
{
	printf("Hello World\n");

	struct InttNameSpace::Online_s onl = (struct InttNameSpace::Online_s) {
		.lyr = 0,
		.ldr = 1,
		.arm = 1,
		.chp = 2,
		.chn = 3
	};

	print(onl);

	char buff[256];
	InttNameSpace::Offline_s ofl = (struct InttNameSpace::Offline_s) {
		.layer = 0,
		.ladder_phi = 0,
		.ladder_z = 0,
		.strip_x = 0,
		.strip_y = 0
	};


	PHParameters my_pars("my_pars");
	my_pars.set_name("INTT");
	for(ofl.strip_x = 0; ofl.strip_x < 5; ++ofl.strip_x) {
		snprintf(buff, sizeof(buff), "INTT_%02d_%01d_%03d_%03d",
			ofl.ladder_phi,
			ofl.ladder_z,
			ofl.strip_y, // strip_z
			ofl.strip_x // strip_phi
		);
		my_pars.set_int_param(buff, 1);
	}
	my_pars.WriteToFile("xml");

	return EXIT_SUCCESS;
}

void print(struct InttNameSpace::Online_s const& s)
{
	printf("\tlyr:%4d\n", s.lyr);
	printf("\tldr:%4d\n", s.ldr);
	printf("\tarm:%4d\n", s.arm);
	printf("\tchp:%4d\n", s.chp);
	printf("\tchn:%4d\n", s.chn);
}
