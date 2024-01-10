#ifndef INTT_EVT_JB_H
#define INTT_EVT_JB_H

#include "InttHitJb.h"

#include <cstdint>
#include <cstddef>

#include <string>

#include <TObject.h>

class InttEvtJb : public TObject
{
public:
	typedef uint64_t BitField_t;
	typedef std::vector<InttHitJb*> Container_t;

	InttEvtJb();
	InttEvtJb(InttEvtJb const&);
	virtual ~InttEvtJb();

	// class member functions
	void print() const;

	virtual std::string version() const;

	virtual BitField_t get_gtm_bco() const;
	virtual void set_gtm_bco(BitField_t const&);

	virtual Container_t::const_iterator begin() const;
	virtual Container_t::const_iterator end() const;

	virtual void clear();
	virtual void push_back(InttHitJb const* const&);
	virtual void append_owned(InttHitJb* const&);

	// virtual methods

protected:

private:

	ClassDefOverride(InttEvtJb, 1);
};

#endif//INTT_EVT_JB_H
