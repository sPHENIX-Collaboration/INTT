#ifndef INTT_HIT_JB_v1_H
#define INTT_HIT_JB_v1_H

#include "InttHitJb.h"

#include <cstdio>
#include <cstdint>
#include <cstddef>

#include <array>
#include <string>

#include <TObject.h>

class InttHitJb_v1 : public InttHitJb
{
public:
	typedef unsigned int BitField_t;
	struct BitField_s
	{
		BitField_t gtm_bco :	7;

		BitField_t flx_bco :	7;
		BitField_t flx_svr :	3;
		BitField_t flx_chn :	4;

		BitField_t chp :	6;
		BitField_t chn :	7;
		BitField_t adc :	3;
	};

	InttHitJb_v1();
	~InttHitJb_v1() override;

	// method overrides
	std::string version() const override {return "InttHitJb_v1";}

	// get_* method overrides
	uint32_t get_gtm_bco() const override {return m_fields.gtm_bco;}

	uint32_t get_flx_bco() const override {return m_fields.flx_bco;}
	uint32_t get_flx_svr() const override {return m_fields.flx_svr;}
	uint32_t get_flx_chn() const override {return m_fields.flx_chn;}

	uint32_t get_chp() const override {return m_fields.chp;}
	uint32_t get_chn() const override {return m_fields.chn;}
	uint32_t get_adc() const override {return m_fields.adc;}

	// set_* method overrides
	void set_gtm_bco(uint32_t const& gtm_bco) override {m_fields.gtm_bco = gtm_bco;}

	void set_flx_bco(uint32_t const& flx_bco) override {m_fields.flx_bco = flx_bco;}
	void set_flx_svr(uint32_t const& flx_svr) override {m_fields.flx_svr = flx_svr;}
	void set_flx_chn(uint32_t const& flx_chn) override {m_fields.flx_chn = flx_chn;}

	void set_chp(uint32_t const& chp) override {m_fields.chp = chp;}
	void set_chn(uint32_t const& chn) override {m_fields.chn = chn;}
	void set_adc(uint32_t const& adc) override {m_fields.adc = adc;}

private:
	BitField_s m_fields;

	ClassDefOverride(InttHitJb_v1, 1);
};

#endif//INTT_HIT_JB_v1_H
