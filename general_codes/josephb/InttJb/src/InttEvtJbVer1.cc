#include "InttEvtJbVer1.h"
#include "InttHitJbVer1.h"

#include <iostream>

InttEvtJbVer1::InttEvtJbVer1() : InttEvtJb()
{
	set_gtm_bco(0);
	m_hits.clear();
}

InttEvtJbVer1::InttEvtJbVer1(InttEvtJbVer1 const& other) : InttEvtJb(other)
{
	set_gtm_bco(other.get_gtm_bco());

	clear();
	for(auto hit : other.m_hits) {
		m_hits.push_back(new InttHitJbVer1(hit));
	}
}


InttEvtJbVer1::~InttEvtJbVer1()
{
	clear();
}

InttEvtJbVer1::BitField_t
InttEvtJbVer1::get_gtm_bco() const
{
	return m_gtm_bco;
}

void
InttEvtJbVer1::Streamer(TBuffer& b)
{
	if(b.IsReading()) {
		b.ReadVersion();
		TObject::Streamer(b);

		b >> m_gtm_bco;

		uint32_t N = 0; b >> N; clear();
		for(uint32_t n = 0; n < N; ++n) {
			m_hits.push_back(new HitVer_t(b));
		}
	} else {
		b.WriteVersion(InttEvtJbVer1::IsA());
		TObject::Streamer(b);

		b << m_gtm_bco;

		uint32_t N = m_hits.size();b << N;
		for(auto hit : m_hits) {
			hit->stream(b);
		}
	}
}

std::string
InttEvtJbVer1::version() const
{
	return "InttEvtJbVer1";
}

void
InttEvtJbVer1::set_gtm_bco(BitField_t const& gtm_bco)
{
	 m_gtm_bco = gtm_bco;
}

InttEvtJbVer1::Container_t::const_iterator
InttEvtJbVer1::begin() const
{
	return m_hits.begin();
}

InttEvtJbVer1::Container_t::const_iterator
InttEvtJbVer1::end() const
{
	return m_hits.end();
}

void
InttEvtJbVer1::clear()
{
	for(auto hit : m_hits) {
		delete hit;
	}
	m_hits.clear();
}

void
InttEvtJbVer1::push_back(InttHitJb const* const& hit)
{
	m_hits.push_back(new HitVer_t(hit));
}

void
InttEvtJbVer1::append_owned(InttHitJb* const& hit)
{
	if(!dynamic_cast<HitVer_t*>(hit)) {
		std::cerr << "InttEvtJbVer1::append_owned(InttHitJb* const& hit)" << std::endl;
		std::cerr << "\tAgrument \"hit\" has the wrong dynamic type" << std::endl;

		// change this line when HitVer_t is changed
		std::cerr << "\t(Does not inherit from InttHitJbVer1)" << std::endl;

		std::cerr << std::endl;
		std::cerr << "You will need to rewrite you code such that" << std::endl;
		std::cerr << "new hits are allocated as the correct dynamic type" << std::endl;

		exit(1);

		return;
	}

	m_hits.push_back(hit);
}
