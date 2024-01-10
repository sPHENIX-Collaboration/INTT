#ifndef INTT_HIT_JB_H
#define INTT_HIT_JB_H

#include <cstdio>
#include <cstdint>
#include <cstddef>

#include <array>
#include <string>

#include <TObject.h>

class InttHitJb : public TObject
{
public:
	InttHitJb();
	virtual ~InttHitJb();

	// overrides of TObject functions
	Bool_t IsSortable() const override {return kTRUE;}

	Bool_t IsEqual(TObject const*) const override;
	Int_t Compare(TObject const*) const override;

	// class member functions
	void print() const;

	int get_ladder_z() const;
	int get_strip_row() const;
	int get_strip_col() const;

	// virtual methods
	virtual std::string version() const {return "InttHitJb";}

	// virtual get_* methods
	virtual uint32_t get_gtm_bco() const {return 0;}

	virtual uint32_t get_flx_bco() const {return 0;}
	virtual uint32_t get_flx_svr() const {return 0;}
	virtual uint32_t get_flx_chn() const {return 0;}

	virtual uint32_t get_chp() const {return 0;}
	virtual uint32_t get_chn() const {return 0;}
	virtual uint32_t get_adc() const {return 0;}

	// Virtual set_* methods
	virtual void set_gtm_bco(uint32_t const&) {}

	virtual void set_flx_bco(uint32_t const&) {}
	virtual void set_flx_svr(uint32_t const&) {}
	virtual void set_flx_chn(uint32_t const&) {}

	virtual void set_chp(uint32_t const&) {}
	virtual void set_chn(uint32_t const&) {}
	virtual void set_adc(uint32_t const&) {}

private:
	ClassDefOverride(InttHitJb, 1);
};

bool operator<(InttHitJb const&, InttHitJb const&);
bool operator==(InttHitJb const&, InttHitJb const&);

inline bool operator>(InttHitJb const& lhs, InttHitJb const& rhs) {return rhs < lhs;}
inline bool operator<=(InttHitJb const& lhs, InttHitJb const& rhs) {return !(lhs > rhs);}
inline bool operator>=(InttHitJb const& lhs, InttHitJb const& rhs) {return !(lhs < rhs);}
inline bool operator!=(InttHitJb const& lhs, InttHitJb const& rhs) {return !(lhs == rhs);}

#endif//INTT_HIT_JB_H
