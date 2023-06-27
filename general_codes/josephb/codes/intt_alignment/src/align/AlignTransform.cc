#include "AlignTransform.h"

AlignTransform::AlignTransform()
{
	//Do nothing
}

AlignTransform::AlignTransform(const AlignTransform& o)
{
	int i = 0;
	int j = 0;

	for(i = 0; i < 3; ++i)
	{
		angles[i] = o.angles[i];
	}

	for(i = 0; i < 4; ++i)
	{
		for(j = 0; j < 4; ++j)
		{
			align_transform[i][j] = o.align_transform[i][j];
		}
	}

	angle_convention = o.angle_convention;
}

AlignTransform operator*(AlignTransform& lhs, AlignTransform& rhs)
{
	int i = 0;
	int j = 0;
	int k = 0;

	AlignTransform t;

	for(i = 0; i < 4; ++i)
	{
		for(j = 0; j < 4; ++j)
		{
			t[i][j] = 0.0;
			for(k = 0; k < 4; ++k)t[i][j] += (lhs[i][k]) * (rhs[k][j]);
		}
	}

	return t;
}

double* AlignTransform::operator[](const int& i)
{
	return align_transform[i];
}

AlignTransform AlignTransform::Inverse()
{
	AlignTransform lhs = *this;
	AlignTransform rhs;

	int i = 0;
	int j = 0;
	int k = 0;

	double d = 0.0;

	for(j = 0; j < 4; ++j)
	{
		//find a pivot
		d = 0.0;
		for(i = j; i < 4; ++i)
		{
			if(std::abs(lhs[i][j]) > d)
			{
				d = std::abs(lhs[i][j]);
				k = i;
			}
		}

		//Check for singularity
		if(d == 0.0)std::cout << "singular" << std::endl;

		//Pivot was found on row k; exchanges rows j and k to move it to diagonal
		for(i = 0; i < 4; ++i)
		{
			d = lhs[j][i];
			lhs[j][i] = lhs[k][i];
			lhs[k][i] = d;

			d = rhs[j][i];
			rhs[j][i] = rhs[k][i];
			rhs[k][i] = d;
		}

		//Normalize the pivot row
		d = 1.0 / lhs[j][j];
		for(i = 0; i < 4; ++i)
		{
			lhs[j][i] *= d;
			rhs[j][i] *= d;
		}

		//Upwards and downwards row elimination
		for(i = 0; i < 4; ++i)
		{
			if(i == j)continue;

			d = -lhs[i][j];

			for(k = 0; k < 4; ++k)
			{
				lhs[i][k] += d * lhs[j][k];
				rhs[i][k] += d * rhs[j][k];
			}
		}
	}

	return rhs;
}

double& AlignTransform::Ang(const int& i)
{
	return angles[i];
}

double& AlignTransform::Pos(const int& i)
{
	return align_transform[i][3];
}

int AlignTransform::SetTransformFromAngles()
{
	return angle_convention->SetTransformFromAngles(align_transform, angles);
}

int AlignTransform::SetAnglesFromTransform()
{
	return angle_convention->SetAnglesFromTransform(align_transform, angles);
}

void AlignTransform::Reset()
{
	int i = 0;
	int j = 0;

	for(i = 0; i < 4; ++i)
	{
		for(j = 0; j < i; ++j)
		{
			align_transform[i][j] = 0.0;
		}
		align_transform[i][i] = 1.0;
		for(j = i + 1; j < 4; ++j)
		{
			align_transform[i][j] = 0.0;
		}
	}

	for(i = 0; i < 3; ++i)
	{
		angles[i] = 0.0;
	}

}

void AlignTransform::PrintRow(const int& i, std::string s)
{
	int j = 0;
	while(true)
	{
		printf(s.c_str(), align_transform[i][j]);

		if(++j >= 4)break;

		printf("\t");
	}
}

void AlignTransform::Print(std::string s)
{
	int i = 0;
	while(true)
	{
		PrintRow(i, s);
		printf("\n");

		if(++i >= 4)break;
	}
}
