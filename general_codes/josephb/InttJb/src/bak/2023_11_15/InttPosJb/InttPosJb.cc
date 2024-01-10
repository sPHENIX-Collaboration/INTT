#include "InttPosJb.h"

#include <cstdio>

InttPosJb::InttPosJb()
{
	set_flx_svr(0);
	set_flx_chn(0);
	set_chp(0);
	set_chn(0);
}

InttPosJb::InttPosJb(InttPosJb const& other)
{
	set_flx_svr(other.get_flx_svr());
	set_flx_chn(other.get_flx_chn());
	set_chp(other.get_chp());
	set_chn(other.get_chn());
}

InttPosJb::~InttPosJb()
{
	// do nothing
}

// overrides of TObject functions
Bool_t
InttPosJb::IsSortable() const
{
	return kTRUE;
}
Bool_t
InttPosJb::IsEqual(TObject const* other) const
{
	InttPosJb const* ptr = dynamic_cast<InttPosJb const*>(other);
	if(!ptr)return kFALSE;

	return *this == *ptr;
}

Int_t
InttPosJb::Compare(TObject const* other) const
{
	InttPosJb const* ptr = dynamic_cast<InttPosJb const*>(other);
	if(!ptr)return -1;

	if(*this != *ptr)return *this < *ptr ? -1 : 1;

	return 0;
}

// class member functions
void
InttPosJb::print() const
{
	printf("%s\n", version().c_str());

	printf("\t%7s: %10u\n", "flx_svr", get_flx_svr());
	printf("\t%7s: %10u\n", "flx_chn", get_flx_chn());
	printf("\t%7s: %10u\n", "chp", get_chp());
	printf("\t%7s: %10u\n", "chn", get_chn());

	printf("\n");
}

InttPosJb::BitField_t
InttPosJb::get_ladder_z() const
{
	// here
	// 	Implement later
	return 0;
}

InttPosJb::BitField_t
InttPosJb::get_strip_row() const
{
	// here
	// 	Implement later
	return 0;
}

InttPosJb::BitField_t
InttPosJb::get_strip_col() const
{
	// here
	// 	Implement later
	return 0;
}

// virtual methods
std::string InttPosJb::version() const
{
	return "InttPosJb";
}

// virtual get_* methods
InttPosJb::BitField_t
InttPosJb::get_flx_svr() const 
{
	return 0;
}

InttPosJb::BitField_t
InttPosJb::get_flx_chn() const
{
	return 0;
}

InttPosJb::BitField_t
InttPosJb::get_chp() const
{
	return 0;
}

InttPosJb::BitField_t
InttPosJb::get_chn() const
{
	return 0;
}

// virtual set_* methods
void
InttPosJb::set_flx_svr(BitField_t const&)
{
	// do nothing
}

void
InttPosJb::set_flx_chn(BitField_t const&)
{
	// do nothing
}

void
InttPosJb::set_chp(BitField_t const&)
{
	// do nothing
}

void
InttPosJb::set_chn(BitField_t const&)
{
	// do nothing
}

// operator overloading
bool
operator==(InttPosJb const& lhs, InttPosJb const& rhs)
{
	if(lhs.get_flx_svr() != rhs.get_flx_svr())return false;
	if(lhs.get_flx_chn() != rhs.get_flx_chn())return false;

	if(lhs.get_chp() != rhs.get_chp())return false;
	if(lhs.get_chn() != rhs.get_chn())return false;

	return true;
}

bool
operator<(InttPosJb const& lhs, InttPosJb const& rhs)
{
	if(lhs.get_flx_svr() != rhs.get_flx_svr())return lhs.get_flx_svr() < rhs.get_flx_svr();
	if(lhs.get_flx_chn() != rhs.get_flx_chn())return lhs.get_flx_chn() < rhs.get_flx_chn();

	if(lhs.get_chp() != rhs.get_chp())return lhs.get_chp() < rhs.get_chp();
	if(lhs.get_chn() != rhs.get_chn())return lhs.get_chn() < rhs.get_chn();

	return false;
}
