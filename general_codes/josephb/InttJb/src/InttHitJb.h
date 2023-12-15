#ifndef INTT_HIT_JB_H
#define INTT_HIT_JB_H

#include <cstdint>
#include <cstddef>

#include <string>

#include <TBuffer.h>
#include <TObject.h>

class InttHitJb : public TObject
{
public:
	typedef uint8_t BitField_t;

	InttHitJb();
	InttHitJb(InttHitJb const&);
	virtual ~InttHitJb();

	InttHitJb(TBuffer&);
	virtual void stream(TBuffer&);

	void copy(InttHitJb const* const&);
	void print() const;

	virtual std::string version() const;

	// get_* methods
	virtual BitField_t get_gtm_bco() const;
	virtual BitField_t get_flx_bco() const;
	virtual BitField_t get_flx_svr() const;
	virtual BitField_t get_flx_chn() const;
	virtual BitField_t get_chp() const;
	virtual BitField_t get_chn() const;
	virtual BitField_t get_adc() const;

	// set_* methods
	virtual void set_gtm_bco(BitField_t const&);
	virtual void set_flx_bco(BitField_t const&);
	virtual void set_flx_svr(BitField_t const&);
	virtual void set_flx_chn(BitField_t const&);
	virtual void set_chp(BitField_t const&);
	virtual void set_chn(BitField_t const&);
	virtual void set_adc(BitField_t const&);

	ClassDefOverride(InttHitJb, 1);

protected:

private:
};

// operator overloading
struct InttHitComparatorJb
{
	bool operator()(InttHitJb const* const& lhs, InttHitJb const* const& rhs)
	{
		if(lhs->get_gtm_bco() != rhs->get_gtm_bco())return lhs->get_gtm_bco() < rhs->get_gtm_bco();
		if(lhs->get_flx_bco() != rhs->get_flx_bco())return lhs->get_flx_bco() < rhs->get_flx_bco();
		if(lhs->get_flx_svr() != rhs->get_flx_svr())return lhs->get_flx_svr() < rhs->get_flx_svr();
		if(lhs->get_flx_chn() != rhs->get_flx_chn())return lhs->get_flx_chn() < rhs->get_flx_chn();
		if(lhs->get_chp() != rhs->get_chp())return lhs->get_chp() < rhs->get_chp();
		if(lhs->get_chn() != rhs->get_chn())return lhs->get_chn() < rhs->get_chn();
		if(lhs->get_adc() != rhs->get_adc())return lhs->get_adc() < rhs->get_adc();

		return false;
	}
};

struct InttPosComparatorJb
{
	bool operator()(InttHitJb const* const& lhs, InttHitJb const* const& rhs) const
	{
		if(lhs->get_flx_svr() != rhs->get_flx_svr())return lhs->get_flx_svr() < rhs->get_flx_svr();
		if(lhs->get_flx_chn() != rhs->get_flx_chn())return lhs->get_flx_chn() < rhs->get_flx_chn();
		if(lhs->get_chp() != rhs->get_chp())return lhs->get_chp() < rhs->get_chp();
		if(lhs->get_chn() != rhs->get_chn())return lhs->get_chn() < rhs->get_chn();

		return false;
	}
};

#endif//INTT_HIT_JB_H
