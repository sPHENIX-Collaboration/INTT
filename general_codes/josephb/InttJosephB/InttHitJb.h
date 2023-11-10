#ifndef INTT_HIT_JB_H
#define INTT_HIT_JB_H

#include <cstdint>

#include <TObject.h>

class InttHitJb : public TObject
{
public:
	struct BitField_s
	{
		uint64_t gtm_bco = 0;

		unsigned char flx_bco :	7; // range 0-127
		unsigned char flx_svr :	3; // range 0-7
		unsigned char flx_chn :	4; // range 0-13
		unsigned char chp :	5; // range 0-25
		unsigned char chn :	7; // range 0-127
		unsigned char adc :	3; // range 0-8
		//___________________+___
		//.....................29 bits (8 bytes)
	};

	InttHitJb();
	~InttHitJb();

	// Overrides of TObject functions
	Bool_t IsSortable() const {return kTRUE;}

	Bool_t IsEqual(TObject const*) const;
	Int_t Compare(TObject const*) const;

	// Class member functions
	void print() const;

	int get_ladder_z() const;
	int get_strip_row() const;
	int get_strip_col() const;

	// Trivial get_* methods
	uint64_t get_gtm_bco() const {return m_fields.gtm_bco;}

	uint8_t get_flx_bco() const {return m_fields.flx_bco;}
	uint8_t get_flx_svr() const {return m_fields.flx_svr;}
	uint8_t get_flx_chn() const {return m_fields.flx_chn;}

	uint8_t get_chp() const {return m_fields.chp;}
	uint8_t get_chn() const {return m_fields.chn;}
	uint8_t get_adc() const {return m_fields.adc;}

	// Trivial set_* methods
	void set_gtm_bco(uint64_t const& gtm_bco) {m_fields.gtm_bco = gtm_bco;}

	void set_flx_bco(uint8_t const& flx_bco) {m_fields.flx_bco = flx_bco;}
	void set_flx_svr(uint8_t const& flx_svr) {m_fields.flx_svr = flx_svr;}
	void set_flx_chn(uint8_t const& flx_chn) {m_fields.flx_chn = flx_chn;}

	void set_chp(uint8_t const& chp) {m_fields.chp = chp;}
	void set_chn(uint8_t const& chn) {m_fields.chn = chn;}
	void set_adc(uint8_t const& adc) {m_fields.adc = adc;}

protected:
	ClassDef(InttHitJb, 1);

private:
	BitField_s m_fields;
};

bool operator<(InttHitJb const&, InttHitJb const&);
bool operator==(InttHitJb const&, InttHitJb const&);

inline bool operator>(InttHitJb const& lhs, InttHitJb const& rhs) {return rhs < lhs;}
inline bool operator<=(InttHitJb const& lhs, InttHitJb const& rhs) {return !(lhs > rhs);}
inline bool operator>=(InttHitJb const& lhs, InttHitJb const& rhs) {return !(lhs < rhs);}
inline bool operator!=(InttHitJb const& lhs, InttHitJb const& rhs) {return !(lhs == rhs);}

#endif//INTT_HIT_JB_H
