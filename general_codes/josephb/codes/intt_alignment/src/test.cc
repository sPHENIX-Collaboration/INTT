#include "foo/AlignTransform.h"

int main()
{
	AlignTransform a;
	a.Ang("z") = atan(1.0);
	a.SetTransformFromAngles();
	a.Print();
	std::cout << std::endl;

	AlignTransform b;
	b.Ang("z") = 2.0 / 3.0 * atan(1.0);
	b.SetTransformFromAngles();
	b.Print();
	std::cout << std::endl;

	a = a * b;
	a.SetAnglesFromTransform();
	a.Print();
	std::cout << std::endl;

	std::cout << a.Ang(0) << std::endl;
	std::cout << 5.0 / 3.0 * atan(1.0) << std::endl;

	return 0;
}
