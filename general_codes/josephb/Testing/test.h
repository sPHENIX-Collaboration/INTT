#ifndef TEST_H
#define TEST_H

#include "foo.h"

#include <set>

#include <phool/PHObject.h>

class test : public PHObject {
public:
	typedef std::set<foo::bar, foo::bar_comparator> set_t;

	test();
	test(test const&);
	~test() override;

	void insert(foo::bar const&);
	bool contains(foo::bar const&) const;
	void clear();

	// ...
private:
	set_t m_set;

	ClassDefOverride(test, 1);

};

#endif//TEST_H
