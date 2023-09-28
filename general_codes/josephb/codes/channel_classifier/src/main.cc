#include "ChannelClassifier.h"

#include <cstdio>

int main()
{
	printf("\n");
	printf("Hello World\n");
	printf("\n");

	ChannelClassifier c;

	Intt::Online_s s;
	while(true)
	{
		Int_t h = ChannelClassifier::hash_from_struct(s);
		Intt::Online_s t = ChannelClassifier::struct_from_hash(h);
		if(t != s)
		{
			printf("\n\nOof:\n");
			ChannelClassifier::print(s);
			printf("\th:%12d\n", h);
			ChannelClassifier::print(t);

			break;
		}

		if(++s.chn < 128)continue;
		s.chn = 0;

		if(++s.chp < 26)continue;
		s.chp = 0;

		if(++s.arm < 2)continue;
		s.arm = 0;

		if(++s.ldr < ((s.lyr < 2) ? 12 : 16))continue;
		s.ldr = 0;

		if(++s.lyr < 3)continue;
		s.lyr = 0;

		break;
	}


	c.AppendFile("/sphenix/tg/tg01/commissioning/INTT/root_files/beam_intt_combined-00020445-0000.root");

	std::cout << "Done" << std::endl;

	return 0;
}
