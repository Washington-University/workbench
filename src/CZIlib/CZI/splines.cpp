//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************

#include "stdafx.h"
#include "splines.h"
#include <cmath>
#include "./eigen/Eigen"

using namespace std;
using namespace Eigen;

/*static*/std::vector<CSplines::Coefficients> CSplines::GetSplineCoefficients(int pointsCnt, std::function<void(int index, double* x, double* y)> getPoint)
{
	if (!getPoint || pointsCnt < 2)
	{
		throw invalid_argument("Not enough data points for spline fitting.");
	}

	int n = pointsCnt - 1;
	std::vector<CSplines::Coefficients> splineCoefficients;
	splineCoefficients.reserve(n);

	// Initialization of the matrix A and the right hand side (rhs) for the linear equation system A * x = rhs.
	// The solution x contains the coefficients b_1, ..., b_n-1. 
	// The coefficients b_0 and b_n are free variables, and we set b_0 = b_n = 0.
	// (b_n does not appear in a spline but it is necessary for the evaluation of a_(n-1) and c_(n-1)). 
	// Matrix indices: First index is row index, second index is column index.

	MatrixXd matrix; matrix.resize(n - 1, n - 1);
	matrix.setZero();
	VectorXd rhs; rhs.resize(n - 1);

	double pt1x, pt1y, pt0x, pt0y;
	getPoint(0, &pt0x, &pt0y);
	getPoint(1, &pt1x, &pt1y);
	double dx1, dy1;
	double dx2 = pt1x - pt0x;
	double dy2 = pt1y - pt0y;

	for (int i = 0; i < n - 1; i++)
	{
		dx1 = dx2;

		getPoint(i + 1, &pt0x, &pt0y);
		getPoint(i + 2, &pt1x, &pt1y);

		dx2 = pt1x - pt0x;

		dy1 = dy2;
		dy2 = pt1y - pt0y;

		// Diagonal entry
		matrix(i, i) = 2 * (dx1 + dx2);

		if (i != n - 2)
		{
			// Secondary diagonal entries
			matrix(i + 1, i) = dx2;
			matrix(i, i + 1) = dx2;
		}

		if (abs(dx1) < 0.001)
		{
			dx1 = 0.001;
		}

		if (abs(dx2) < 0.001)
		{
			dx2 = 0.001;
		}

		rhs[i] = 3 * ((dy2 / dx2) - (dy1 / dx1));
	}

	VectorXd resultVector = matrix.colPivHouseholderQr().solve(rhs);

	// Resolve the coefficients for the spline curve.
	double x_i = 0;
	for (int i = 0; i < n; i++)
	{
		double x_i_plus_1 = i < (n - 1) ? resultVector(i) : 0;
		Coefficients coeffs;
		getPoint(i, &pt0x, &pt0y);
		getPoint(i + 1, &pt1x, &pt1y);

		dx1 = pt1x - pt0x;
		dy1 = pt1y - pt0y;

		if (abs(dx1) < 0.0000001)
		{
			coeffs.a = 0;
			coeffs.c = 0;
		}
		else
		{
			coeffs.a = (x_i_plus_1 - x_i) / (3 * dx1);
			coeffs.c = (dy1 / dx1) - ((dx1 * (x_i_plus_1 + (2 * x_i))) / 3);
		}

		coeffs.b = x_i;
		coeffs.d = pt0y;
		splineCoefficients.push_back(coeffs);
		x_i = x_i_plus_1;
	}

	return splineCoefficients;
}

/*static*/double CSplines::CalculateSplineValue(double xPosition, int pointsCnt, std::function<void(int index, double* x)> getPoint, const  std::vector<Coefficients>& coefficients)
{
	// the polynomial number is always 4 in our case
	if (pointsCnt >= 4 + 2)
	{
		throw invalid_argument("The number of point intervals exceeds the polynomial number.");
	}

	// TODO: since the points are sorted for x (I'd think so...) we should be able to use a binary search here?
	int index = 0;
	double xPos_for_foundIndex;
	for (int i = 0; i < pointsCnt; i++)
	{
		double xPos_i;
		getPoint(i, &xPos_i);
		if (xPosition >= xPos_i)
		{
			index = i;
			xPos_for_foundIndex = xPos_i;
		}
	}

	xPosition -= xPos_for_foundIndex;

	return CalculateSplineValue(xPosition, coefficients.at(index));
}

/*static*/double CSplines::CalculateSplineValue(double xPosition, const CSplines::Coefficients& coeffs)
{
	const int n = 4;	// cubic spline, polynomial number is 4

	double y = 0;
	double xPower = 1;

	for (int i = n - 1; i >= 0; i--)
	{
		y += xPower * coeffs.Get(i);
		xPower *= xPosition;
	}

	return y;
}
