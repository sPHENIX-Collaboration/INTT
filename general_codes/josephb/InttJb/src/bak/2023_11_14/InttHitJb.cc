#include "InttHitJb.h"

#include <cstdio>

#include <iostream>

InttHitJb::InttHitJb()
{
	// Do nothing
}

InttHitJb::~InttHitJb()
{
	// Do nothing
}

Bool_t
InttHitJb::IsEqual(TObject const* other) const
{
	InttHitJb const* ptr = dynamic_cast<InttHitJb const*>(other);
	if(!ptr)return kFALSE;

	return *this == *ptr;
}

Int_t
InttHitJb::Compare(TObject const* other) const
{
	InttHitJb const* ptr = dynamic_cast<InttHitJb const*>(other);
	if(!ptr)return -1;

	if(*this != *ptr)return *this < *ptr ? -1 : 1;

	return 0;
}

void
InttHitJb::print() const
{
	printf("InttHitJb\n");

	printf("\t%7s: 0x%010X\n", "gtm_bco", get_gtm_bco());

	printf("\t%7s: %10u\n", "flx_bco", get_flx_bco());
	printf("\t%7s: %10u\n", "flx_svr", get_flx_svr());
	printf("\t%7s: %10u\n", "flx_chn", get_flx_chn());

	printf("\t%7s: %10u\n", "chp", get_chp());
	printf("\t%7s: %10u\n", "chn", get_chn());
	printf("\t%7s: %10u\n", "adc", get_adc());

	printf("\n");
}

int
InttHitJb::get_ladder_z() const
{
	// here
	// 	Implement later
	return 0;
}

int
InttHitJb::get_strip_row() const
{
	// here
	// 	Implement later
	return 0;
}

int
InttHitJb::get_strip_col() const
{
	// here
	// 	Implement later
	return 0;
}

bool
operator==(InttHitJb const& lhs, InttHitJb const& rhs)
{
	// Ignore timing and adc information when doing comparison
	// 	It is more useful to only compare by hit position
	// 	(e.g. clone hits compared by position, hitrates compared by position)

	// if(lhs.get_gtm_bco() != rhs.get_gtm_bco())return false;

	// if(lhs.get_flx_bco() != rhs.get_flx_bco())return false;
	if(lhs.get_flx_svr() != rhs.get_flx_svr())return false;
	if(lhs.get_flx_chn() != rhs.get_flx_chn())return false;

	if(lhs.get_chp() != rhs.get_chp())return false;
	if(lhs.get_chn() != rhs.get_chn())return false;
	// if(lhs.get_adc() != rhs.get_adc())return false;

	return true;
}

bool
operator<(InttHitJb const& lhs, InttHitJb const& rhs)
{
	// Ignore timing and adc information when doing comparison
	// 	It is more useful to only compare by hit position
	// 	(e.g. clone hits compared by position, hitrates compared by position)

	// if(lhs.get_gtm_bco() != rhs.get_gtm_bco())return lhs.get_gtm_bco() < rhs.get_gtm_bco();

	// if(lhs.get_flx_bco() != rhs.get_flx_bco())return lhs.get_flx_bco() < rhs.get_flx_bco();
	if(lhs.get_flx_svr() != rhs.get_flx_svr())return lhs.get_flx_svr() < rhs.get_flx_svr();
	if(lhs.get_flx_chn() != rhs.get_flx_chn())return lhs.get_flx_chn() < rhs.get_flx_chn();

	if(lhs.get_chp() != rhs.get_chp())return lhs.get_chp() < rhs.get_chp();
	if(lhs.get_chn() != rhs.get_chn())return lhs.get_chn() < rhs.get_chn();
	// if(lhs.get_adc() != rhs.get_adc())return lhs.get_adc() < rhs.get_adc();

	return false;
}
