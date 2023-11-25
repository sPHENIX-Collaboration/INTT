#ifndef INTT_EVT_JB_VER_1_H
#define INTT_EVT_JB_VER_1_H

#include "InttEvtJb.h"
#include "InttHitJb.h"
#include "InttHitJbVer1.h"

#include <cstdint>
#include <cstddef>

#include <string>

#include <TBuffer.h>
#include <TObject.h>

class InttEvtJbVer1 : public InttEvtJb
{
public:
	using InttEvtJb::BitField_t;
	using InttEvtJb::Container_t;

	typedef InttHitJbVer1 HitVer_t;

	InttEvtJbVer1();
	InttEvtJbVer1(InttEvtJbVer1 const&);
	virtual ~InttEvtJbVer1();

	std::string version() const override;

	// class member functions
	BitField_t get_gtm_bco() const override;
	void set_gtm_bco(BitField_t const&) override;

	Container_t::const_iterator begin() const override;
	Container_t::const_iterator end() const override;

	void clear() override;
	void push_back(InttHitJb const* const&) override;
	void append_owned(InttHitJb* const&) override;

protected:

private:
	BitField_t m_gtm_bco;
	Container_t m_hits;

	ClassDefOverride(InttEvtJbVer1, 1);
};

#endif//INTT_EVT_JB_VER_1_H
