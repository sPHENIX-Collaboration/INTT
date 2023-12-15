#include "InttHitJbVer1.h"

#include <cstdio>

#include <TBuffer.h>

InttHitJbVer1::InttHitJbVer1() : InttHitJb()
{
	set_gtm_bco(0);
	set_flx_bco(0);
	set_flx_svr(0);
	set_flx_chn(0);
	set_chp(0);
	set_chn(0);
	set_adc(0);
}

InttHitJbVer1::InttHitJbVer1(InttHitJbVer1 const& other) : InttHitJb(other)
{
	set_gtm_bco(other.get_gtm_bco());
	set_flx_bco(other.get_flx_bco());
	set_flx_svr(other.get_flx_svr());
	set_flx_chn(other.get_flx_chn());
	set_chp(other.get_chp());
	set_chn(other.get_chn());
	set_adc(other.get_adc());
}

InttHitJbVer1::InttHitJbVer1(InttHitJb const* const& other)
{
	set_gtm_bco(other->get_gtm_bco());
	set_flx_bco(other->get_flx_bco());
	set_flx_svr(other->get_flx_svr());
	set_flx_chn(other->get_flx_chn());
	set_chp(other->get_chp());
	set_chn(other->get_chn());
	set_adc(other->get_adc());
}

InttHitJbVer1::~InttHitJbVer1()
{
	// do nothing
}

InttHitJbVer1::InttHitJbVer1(TBuffer& b)
{
	b >> m_gtm_bco;
	b >> m_flx_bco;
	b >> m_flx_svr;
	b >> m_flx_chn;
	b >> m_chp;
	b >> m_chn;
	b >> m_adc;
}

void
InttHitJbVer1::stream(TBuffer& b)
{
	b << m_gtm_bco;
	b << m_flx_bco;
	b << m_flx_svr;
	b << m_flx_chn;
	b << m_chp;
	b << m_chn;
	b << m_adc;
}

std::string
InttHitJbVer1::version() const 
{
	return "InttHitJbVer1";
}

// virtual get_* methods
InttHitJbVer1::BitField_t
InttHitJbVer1::get_gtm_bco() const 
{
	return m_gtm_bco;
}

InttHitJbVer1::BitField_t
InttHitJbVer1::get_flx_bco() const
{
	return m_flx_bco;
}

InttHitJbVer1::BitField_t
InttHitJbVer1::get_flx_svr() const
{
	return m_flx_svr;
}

InttHitJbVer1::BitField_t
InttHitJbVer1::get_flx_chn() const
{
	return m_flx_chn;
}

InttHitJbVer1::BitField_t
InttHitJbVer1::get_chp() const
{
	return m_chp;
}

InttHitJbVer1::BitField_t
InttHitJbVer1::get_chn() const
{
	return m_chn;
}

InttHitJbVer1::BitField_t
InttHitJbVer1::get_adc() const
{
	return m_adc;
}

// virtual set_* methods
void
InttHitJbVer1::set_gtm_bco(BitField_t const& gtm_bco)
{
	m_gtm_bco = gtm_bco;
}

void
InttHitJbVer1::set_flx_bco(BitField_t const& flx_bco)
{
	m_flx_bco = flx_bco;
}

void
InttHitJbVer1::set_flx_svr(BitField_t const& flx_svr)
{
	m_flx_svr = flx_svr;
}

void
InttHitJbVer1::set_flx_chn(BitField_t const& flx_chn)
{
	m_flx_chn = flx_chn;
}

void
InttHitJbVer1::set_chp(BitField_t const& chp)
{
	m_chp = chp;
}

void
InttHitJbVer1::set_chn(BitField_t const& chn)
{
	m_chn = chn;
}

void
InttHitJbVer1::set_adc(BitField_t const& adc)
{
	m_adc = adc;
}
