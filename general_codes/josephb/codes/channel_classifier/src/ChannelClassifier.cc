#include "ChannelClassifier.h"

Intt::Online_s ChannelClassifier::struct_from_hash(Int_t h)
{
	Intt::Online_s s = (Intt::Online_s){.lyr = 0, .ldr = 0, .arm = 0, .chp = 0, .chn = 0};

	s.chn = h % 128;
	h /= 128;

	s.chp = h % 26;
	h /= 26;

	s.arm = h % 2;
	h /= 2;

	s.ldr = h;
	while(true)
	{
		if(s.ldr < 12)break;
		s.ldr -= 12;
		++s.lyr;

		if(s.ldr < 12)break;
		s.ldr -= 12;
		++s.lyr;

		if(s.ldr < 16)break;
		s.ldr -= 16;
		++s.lyr;

		break;
	}

	return s;
}

Int_t ChannelClassifier::hash_from_struct(Intt::Online_s const& s)
{
	Int_t h = 0;
	int b = 1;

	h += s.chn * b;
	b *= 128;

	h += s.chp * b;
	b *= 26;

	h += s.arm * b;
	b *= 2;

	h += s.ldr * b;
	while(true)
	{
		if(s.lyr < 1)break;
		h += 12 * b;

		if(s.lyr < 2)break;
		h += 12 * b;

		if(s.lyr < 3)break;
		h += 16 * b;

		break;
	}

	return h;
}

Int_t ChannelClassifier::hash_from_struct(Intt::Offline_s const& s)
{
	return hash_from_struct(Intt::ToOnline(s));
}

Int_t ChannelClassifier::hash_from_struct(Intt::RawData_s const& s)
{
	return hash_from_struct(Intt::ToOnline(s));
}

void ChannelClassifier::print(Intt::Online_s const& s)
{
	printf("lyr: %2d ldr: %2d arm: %1d chp: %2d chn: %3d\n", s.lyr, s.ldr, s.arm, s.chp, s.chn);
}
