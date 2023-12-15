#ifndef INTT_POS_JB_H
#define INTT_POS_JB_H

#include <cstdint>
#include <cstddef>

#include <string>

#include <TObject.h>

class InttPosJb : public TObject
{
public:
	typedef uint32_t BitField_t;

	InttPosJb();
	InttPosJb(InttPosJb const&);
	virtual ~InttPosJb();

	// overrides of TObject functions
	Bool_t IsSortable() const override;
	Bool_t IsEqual(TObject const*) const override;
	Int_t Compare(TObject const*) const override;

	// class member functions
	void print() const;

	BitField_t get_ladder_z() const;
	BitField_t get_strip_row() const;
	BitField_t get_strip_col() const;

	// virtual methods
	virtual std::string version() const;

	// virtual get_* methods
	virtual BitField_t get_flx_svr() const;
	virtual BitField_t get_flx_chn() const;
	virtual BitField_t get_chp() const;
	virtual BitField_t get_chn() const;

	// virtual set_* methods
	virtual void set_flx_svr(BitField_t const&);
	virtual void set_flx_chn(BitField_t const&);
	virtual void set_chp(BitField_t const&);
	virtual void set_chn(BitField_t const&);

private:
	ClassDefOverride(InttPosJb, 1);
};

// operator overloading
bool operator<(InttPosJb const&, InttPosJb const&);
bool operator==(InttPosJb const&, InttPosJb const&);

inline bool operator>(InttPosJb const& lhs, InttPosJb const& rhs) {return rhs < lhs;}
inline bool operator<=(InttPosJb const& lhs, InttPosJb const& rhs) {return !(lhs > rhs);}
inline bool operator>=(InttPosJb const& lhs, InttPosJb const& rhs) {return !(lhs < rhs);}
inline bool operator!=(InttPosJb const& lhs, InttPosJb const& rhs) {return !(lhs == rhs);}

#endif//INTT_POS_JB_H
