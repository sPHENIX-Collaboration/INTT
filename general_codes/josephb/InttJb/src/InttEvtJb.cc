#include "InttEvtJb.h"

#include <iostream>

InttEvtJb::InttEvtJb() : TObject()
{
	// do nothing
}

InttEvtJb::InttEvtJb(InttEvtJb const& other) : TObject(other)
{
	// do nothing
}

InttEvtJb::~InttEvtJb()
{
	// do nothing
}

void
InttEvtJb::print() const
{
	std::cout << "InttEvtJb" << std::endl;
}

std::string
InttEvtJb::version() const
{
	return "InttEvtJb";
}

InttEvtJb::BitField_t
InttEvtJb::get_gtm_bco() const
{
	return 0;
}

void
InttEvtJb::set_gtm_bco(BitField_t const&)
{
	// do nothing
}

InttEvtJb::Container_t::const_iterator
InttEvtJb::begin() const
{
	return Container_t().end();
}

InttEvtJb::Container_t::const_iterator
InttEvtJb::end() const
{
	return Container_t().end();
}

void
InttEvtJb::clear()
{
	// do nothing
}

void
InttEvtJb::push_back(InttHitJb const* const&)
{
	// do nothing
}

void
InttEvtJb::append_owned(InttHitJb* const&)
{
	// do nothing
}
