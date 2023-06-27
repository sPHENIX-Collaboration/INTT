#ifndef ALIGN_TRANSFORM_H
#define ALIGN_TRANSFORM_H

#include "AngleConvention.h"
#include "TaitBryanZYX.h"

#include <cstdlib>
#include <memory>
#include <type_traits>

#include <string>
#include <iostream>

class AlignTransform
{
private:
	std::shared_ptr<AngleConvention> angle_convention = std::static_pointer_cast<AngleConvention>(std::make_shared<TaitBryanZYX>());
	double align_transform[4][4] = {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}};
	double angles[3] = {0.0, 0.0, 0.0};

public:
	AlignTransform();
	AlignTransform(const AlignTransform&);

	friend AlignTransform operator*(AlignTransform&, AlignTransform&);
	double* operator[](const int&);
	AlignTransform Inverse();

	double& Ang(const int&);
	double& Pos(const int&);

	int SetTransformFromAngles();
	int SetAnglesFromTransform();

	void Reset();

	void PrintRow(const int&, std::string= "%+10.4f");
	void Print(std::string = "%+10.4f");

	template <typename T>
	AlignTransform()
	{
		SetConvention<T>();
	}

	template <typename T>
	int SetConvention()
	{
		if(!(std::is_base_of<AngleConvention, T>::value))return 1;

		angle_convention = std::static_pointer_cast<AngleConvention>(std::make_shared<T>());

		return 0;
	}
};

#endif
