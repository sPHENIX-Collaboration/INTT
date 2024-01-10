#ifndef INTT_HIT_JB_v2_H
#define INTT_HIT_JB_v2_H

#include "InttHitJb.h"

#include <cstdio>
#include <cstdint>
#include <cstddef>

#include <array>
#include <string>

#include <TObject.h>

class InttHitJb_v2 : public InttHitJb
{
public:
	enum Field_e : uint8_t
	{
		m_flx_bco,
		m_flx_svr,
		m_flx_chn,

		m_chp,
		m_chn,
		m_adc,

		m_SIZE
	};

	struct BitParams_s
	{
		uint32_t range = 0;
		uint8_t shift = 0;
	};

	std::array<BitParams_s, m_SIZE> static const BIT;

	InttHitJb_v2();
	~InttHitJb_v2() override;

	// method overrides
	std::string version() const override {return "InttHitJb_v2";}

	// get_* method overrides
	uint32_t get_gtm_bco() const override {return m_gtm_bco;}

	uint32_t get_flx_bco() const override {return get_field(m_flx_bco);}
	uint32_t get_flx_svr() const override {return get_field(m_flx_svr);}
	uint32_t get_flx_chn() const override {return get_field(m_flx_chn);}

	uint32_t get_chp() const override {return get_field(m_chp);}
	uint32_t get_chn() const override {return get_field(m_chn);}
	uint32_t get_adc() const override {return get_field(m_adc);}

	// set_* method overrides
	void set_gtm_bco(uint32_t const& gtm_bco) override {m_gtm_bco = gtm_bco;}

	void set_flx_bco(uint32_t const& flx_bco) override {set_field(m_flx_bco, flx_bco);}
	void set_flx_svr(uint32_t const& flx_svr) override {set_field(m_flx_svr, flx_svr);}
	void set_flx_chn(uint32_t const& flx_chn) override {set_field(m_flx_chn, flx_chn);}

	void set_chp(uint32_t const& chp) override {set_field(m_chp, chp);}
	void set_chn(uint32_t const& chn) override {set_field(m_chn, chn);}
	void set_adc(uint32_t const& adc) override {set_field(m_adc, adc);}

protected:
	uint32_t get_field(Field_e const&) const;
	void set_field(Field_e const&, uint32_t const&);

private:
	uint8_t m_gtm_bco = 0;
	uint32_t m_bits = 0;

	ClassDefOverride(InttHitJb_v2, 1);
};

// bool operator<(InttHitJb_v2 const&, InttHitJb_v2 const&);
// bool operator==(InttHitJb_v2 const&, InttHitJb_v2 const&);
// 
// inline bool operator>(InttHitJb_v2 const& lhs, InttHitJb_v2 const& rhs) {return rhs < lhs;}
// inline bool operator<=(InttHitJb_v2 const& lhs, InttHitJb_v2 const& rhs) {return !(lhs > rhs);}
// inline bool operator>=(InttHitJb_v2 const& lhs, InttHitJb_v2 const& rhs) {return !(lhs < rhs);}
// inline bool operator!=(InttHitJb_v2 const& lhs, InttHitJb_v2 const& rhs) {return !(lhs == rhs);}

#endif//INTT_HIT_JB_v2_H
