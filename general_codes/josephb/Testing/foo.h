#ifndef FOO_H
#define FOO_H

#include <TObject.h>

class foo : TObject {
public:
	struct bar {
		int a = 0;
		int b = 0;
		int c = 0;
	};

	struct bar_comparator {
		bool operator()(bar const&, bar const&) const;
	};

private:
	ClassDefOverride(foo, 1);
};

#endif//FOO_H
