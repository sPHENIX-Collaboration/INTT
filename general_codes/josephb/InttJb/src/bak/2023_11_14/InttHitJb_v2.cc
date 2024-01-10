#include "InttHitJb_v2.h"

#include <cstdio>

#include <iostream>

InttHitJb_v2::InttHitJb_v2()
{
	// Do nothing
}

InttHitJb_v2::~InttHitJb_v2()
{
	// Do nothing
}

std::array<InttHitJb_v2::BitParams_s, InttHitJb_v2::m_SIZE>
constexpr InttHitJb_v2::BIT = []
{
	std::array<BitParams_s, m_SIZE> bit {};

	bit[m_flx_bco] =	(BitParams_s){.range = 0x1f800000, .shift = 0x17}; // 7 bits
	bit[m_flx_svr] =	(BitParams_s){.range = 0x00700000, .shift = 0x14}; // 3 bits
	bit[m_flx_chn] =	(BitParams_s){.range = 0x000f0000, .shift = 0x10}; // 4 bits

	bit[m_chp] =		(BitParams_s){.range = 0x0000fc00, .shift = 0x0a}; // 6 bits
	bit[m_chn] =		(BitParams_s){.range = 0x000003f8, .shift = 0x03}; // 7 bits
	bit[m_adc] =		(BitParams_s){.range = 0x00000007, .shift = 0x00}; // 3 bits

	return bit;
}();

uint32_t
InttHitJb_v2::get_field(Field_e const& field) const
{
	return (m_bits & BIT[field].range) >> BIT[field].shift;
}

void
InttHitJb_v2::set_field(Field_e const& field, uint32_t const& value)
{
	m_bits |= BIT[field].range;
	m_bits &= (value << BIT[field].shift) & BIT[field].range;
}

/*
Bool_t
InttHitJb_v2::IsEqual(TObject const* other) const
{
	InttHitJb_v2 const* ptr = dynamic_cast<InttHitJb_v2 const*>(other);
	if(!ptr)return kFALSE;

	return *this == *ptr;
}

Int_t
InttHitJb_v2::Compare(TObject const* other) const
{
	InttHitJb_v2 const* ptr = dynamic_cast<InttHitJb_v2 const*>(other);
	if(!ptr)return -1;

	if(*this != *ptr)return *this < *ptr ? -1 : 1;

	return 0;
}

void
InttHitJb_v2::print() const
{
	printf("InttHitJb_v2\n");

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
InttHitJb_v2::get_ladder_z() const
{
	// here
	// 	Implement later
	return 0;
}

int
InttHitJb_v2::get_strip_row() const
{
	// here
	// 	Implement later
	return 0;
}

int
InttHitJb_v2::get_strip_col() const
{
	// here
	// 	Implement later
	return 0;
}

bool
operator==(InttHitJb_v2 const& lhs, InttHitJb_v2 const& rhs)
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
operator<(InttHitJb_v2 const& lhs, InttHitJb_v2 const& rhs)
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
*/
