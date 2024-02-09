#include "test.h" 

test::test (
) {
	// Do nothing
}

test::test (
	test const& o
) {
	m_set.clear();
	for(auto i : o.m_set) {
		m_set.insert(i);
	}
}

test::~test (
) {
	// Do nothing
}

void test::insert (
	foo::bar const& b
) {
	m_set.insert(b);
}

bool test::contains (
	foo::bar const& b
) const {
	return m_set.find(b) != m_set.end();
}

void test::clear (
) {
	m_set.clear();
}
