#include "InttHitJb.h"

#include <cstdio>

#include <TBuffer.h>

InttHitJb::InttHitJb() : TObject()
{
	// do nothing
}

InttHitJb::InttHitJb(InttHitJb const& other) : TObject(other)
{
	// do nothing
}

InttHitJb::~InttHitJb()
{
	// do nothing
}

InttHitJb::InttHitJb(TBuffer&)
{
	// do nothing
}

void
InttHitJb::stream(TBuffer&)
{
	// do nothing
}

void
InttHitJb::copy(InttHitJb const* const& other)
{
	set_gtm_bco(other->get_gtm_bco());
	set_flx_bco(other->get_flx_bco());
	set_flx_svr(other->get_flx_svr());
	set_flx_chn(other->get_flx_chn());
	set_chp(other->get_chp());
	set_chn(other->get_chn());
	set_adc(other->get_adc());
}

void
InttHitJb::print() const
{
	printf("\t%7s: %10x\n", "gtm_bco", get_gtm_bco());
	printf("\t%7s: %10x\n", "flx_bco", get_flx_bco());

	printf("\t%7s: %10u\n", "flx_svr", get_flx_svr());
	printf("\t%7s: %10u\n", "flx_chn", get_flx_chn());
	printf("\t%7s: %10u\n", "chp", get_chp());
	printf("\t%7s: %10u\n", "chn", get_chn());

	printf("\t%7s: %10u\n", "adc", get_adc());

	printf("\n");
}

std::string
InttHitJb::version() const 
{
	return "InttHitJb";
}

// virtual get_* methods
InttHitJb::BitField_t
InttHitJb::get_gtm_bco() const 
{
	return 0;
}

InttHitJb::BitField_t
InttHitJb::get_flx_bco() const
{
	return 0;
}

InttHitJb::BitField_t
InttHitJb::get_flx_svr() const
{
	return 0;
}

InttHitJb::BitField_t
InttHitJb::get_flx_chn() const
{
	return 0;
}

InttHitJb::BitField_t
InttHitJb::get_chp() const
{
	return 0;
}

InttHitJb::BitField_t
InttHitJb::get_chn() const
{
	return 0;
}

InttHitJb::BitField_t
InttHitJb::get_adc() const
{
	return 0;
}

// virtual set_* methods
void
InttHitJb::set_gtm_bco(BitField_t const&)
{
	// do nothing
}

void
InttHitJb::set_flx_bco(BitField_t const&)
{
	// do nothing
}

void
InttHitJb::set_flx_svr(BitField_t const&)
{
	// do nothing
}

void
InttHitJb::set_flx_chn(BitField_t const&)
{
	// do nothing
}

void
InttHitJb::set_chp(BitField_t const&)
{
	// do nothing
}

void
InttHitJb::set_chn(BitField_t const&)
{
	// do nothing
}

void
InttHitJb::set_adc(BitField_t const&)
{
	// do nothing
}
