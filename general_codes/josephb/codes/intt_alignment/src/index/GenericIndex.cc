#include "GenericIndex.h"

GenericIndex::GenericIndex(std::string s, int i, int l, int u)
{
	label = s;

	index = i;
	lower = l;
	upper = u;
}

GenericIndex::GenericIndex(const GenericIndex& b)
{
	label = b.label;

	index = b.index;
	lower = b.lower;
	upper = b.upper;
}

bool operator==(GenericIndex const& lhs, GenericIndex const& rhs)
{
	if(lhs.label != rhs.label)return false;

	if(lhs.index != rhs.index)return false;
	if(lhs.lower != rhs.lower)return false;
	if(lhs.upper != rhs.upper)return false;

	return true;
}

bool operator<(GenericIndex const& lhs, GenericIndex const& rhs)
{
	return lhs.index < rhs.index;
}

GenericIndex& GenericIndex::operator++()
{
	++index;

	return *this;
}

GenericIndex& GenericIndex::operator--()
{
	--index;

	return *this;
}

const GenericIndex GenericIndex::begin()
{
	GenericIndex b(this->label, this->lower, this->lower, this->upper);
	return b;
}

const GenericIndex GenericIndex::end()
{
	GenericIndex b(this->label, this->upper, this->lower, this->upper);
	return b;
}
