#ifndef INTT_HIT_JB_VER_1_H
#define INTT_HIT_JB_VER_1_H

#include "InttHitJb.h"

#include <cstdint>
#include <cstddef>

#include <string>

#include <TObject.h>

class InttHitJbVer1 : public InttHitJb
{
public:
	typedef uint8_t BitField_t;

	InttHitJbVer1();
	InttHitJbVer1(InttHitJbVer1 const&);
	InttHitJbVer1(InttHitJb const* const&);
	virtual ~InttHitJbVer1();

	InttHitJbVer1(TBuffer&);
	void stream(TBuffer&) override;

	std::string version() const override;

	// class member functions
	BitField_t get_gtm_bco() const override;
	BitField_t get_flx_bco() const override;
	BitField_t get_flx_svr() const override;
	BitField_t get_flx_chn() const override;
	BitField_t get_chp() const override;
	BitField_t get_chn() const override;
	BitField_t get_adc() const override;

	void set_gtm_bco(BitField_t const&) override;
	void set_flx_bco(BitField_t const&) override;
	void set_flx_svr(BitField_t const&) override;
	void set_flx_chn(BitField_t const&) override;
	void set_chp(BitField_t const&) override;
	void set_chn(BitField_t const&) override;
	void set_adc(BitField_t const&) override;

	ClassDefOverride(InttHitJbVer1, 1);

protected:
	BitField_t m_gtm_bco;
	BitField_t m_flx_bco;
	BitField_t m_flx_svr;
	BitField_t m_flx_chn;
	BitField_t m_chp;
	BitField_t m_chn;
	BitField_t m_adc;

private:
};

#endif//INTT_HIT_JB_VER_1_H
