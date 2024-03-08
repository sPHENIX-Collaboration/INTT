#include "foo.h"

bool foo::bar_comparator::operator() (
	bar const& lhs,
	bar const& rhs
) const {
	if(lhs.a != rhs.a)return lhs.a < rhs.a;
	if(lhs.b != rhs.b)return lhs.b < rhs.b;
	if(lhs.c != rhs.c)return lhs.c < rhs.c;

	return false;
}
