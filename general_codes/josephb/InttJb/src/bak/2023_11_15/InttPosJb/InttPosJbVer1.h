#ifndef INTT_POS_JB_VER_1_H
#define INTT_POS_JB_VER_1_H

#include "InttPosJb.h"

#include <cstdint>
#include <cstddef>

#include <string>

#include <TObject.h>

class InttPosJbVer1 : public InttPosJb
{
public:
	using InttPosJb::BitField_t;

	struct BitField_s
	{
		BitField_t flx_svr :	3;
		BitField_t flx_chn :	4;
		BitField_t chp :	6;
		BitField_t chn :	7;
	};

	InttPosJbVer1();
	InttPosJbVer1(InttPosJb const&);
	~InttPosJbVer1() override;

	// method overrides
	std::string version() const override;

	// get_* method overrides
	BitField_t get_flx_svr() const override;
	BitField_t get_flx_chn() const override;
	BitField_t get_chp() const override;
	BitField_t get_chn() const override;

	// set_* method overrides
	void set_flx_svr(BitField_t const&) override;
	void set_flx_chn(BitField_t const&) override;
	void set_chp(BitField_t const&) override;
	void set_chn(BitField_t const&) override;

private:
	BitField_s m_fields;

	ClassDefOverride(InttPosJbVer1, 1);
};

#endif//INTT_POS_JB_VER_1_H
