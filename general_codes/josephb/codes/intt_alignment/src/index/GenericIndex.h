#ifndef GENERIC_INDEX_H
#define GENERIC_INDEX_H

#include <string>

class GenericIndex
{
protected:
public:
	int lower = 0;
	int index = 0;
	int upper = 0;

	std::string label;

public:
	GenericIndex(std::string, int, int, int);
	GenericIndex(const GenericIndex&);

	friend bool operator==(GenericIndex const&, GenericIndex const&);
	friend bool operator!=(GenericIndex const& lhs, GenericIndex const& rhs){return !(lhs == rhs);}

	friend bool operator<(GenericIndex const&, GenericIndex const&);
	friend bool operator>(GenericIndex const& lhs, GenericIndex const& rhs){return rhs < lhs;}
	friend bool operator<=(GenericIndex const& lhs, GenericIndex const& rhs){return !(lhs > rhs);}
	friend bool operator>=(GenericIndex const& lhs, GenericIndex const& rhs){return !(lhs < rhs);}

	GenericIndex& operator++();
	GenericIndex& operator--();

	const GenericIndex begin();
	const GenericIndex end();

	GenericIndex(std::string s, int l, int u) : GenericIndex(s, l, l, u){};
};

#endif
