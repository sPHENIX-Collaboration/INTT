#include "InttPosJbVer1.h"

InttPosJbVer1::InttPosJbVer1() : InttPosJb()
{
	// do nothing
}

InttPosJbVer1::InttPosJbVer1(InttPosJb const& other) : InttPosJb(other)
{
	// do nothing
}

InttPosJbVer1::~InttPosJbVer1()
{
	// do nothing
}

// method overrides
std::string InttPosJbVer1::version() const
{
	return "InttPosJbVer1";
}

// get_* method overrides
InttPosJb::BitField_t
InttPosJbVer1::get_flx_svr() const 
{
	return m_fields.flx_svr;
}

InttPosJb::BitField_t
InttPosJbVer1::get_flx_chn() const
{
	return m_fields.flx_chn;
}

InttPosJb::BitField_t
InttPosJbVer1::get_chp() const
{
	return m_fields.chp;
}

InttPosJb::BitField_t
InttPosJbVer1::get_chn() const
{
	return m_fields.chn;
}

// set_* method overrides
void
InttPosJbVer1::set_flx_svr(BitField_t const& flx_svr)
{
	m_fields.flx_svr = flx_svr;
}

void
InttPosJbVer1::set_flx_chn(BitField_t const& flx_chn)
{
	m_fields.flx_chn = flx_chn;
}

void
InttPosJbVer1::set_chp(BitField_t const& chp)
{
	m_fields.chp = chp;
}

void
InttPosJbVer1::set_chn(BitField_t const& chn)
{
	m_fields.chp = chp;
}
