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
#include <cmath>
#include "libCZI.h"
#include "Site.h"
#include "BitmapOperations.h"
#include "splines.h"
#include "IndexSet.h"
#include "MD5Sum.h"

using namespace libCZI;
using namespace std;

/*static*/char Utils::DimensionToChar(libCZI::DimensionIndex dim)
{
	switch (dim)
	{
	case	DimensionIndex::Z:return 'Z';
	case 	DimensionIndex::C:return 'C';
	case 	DimensionIndex::T:return 'T';
	case 	DimensionIndex::R:return 'R';
	case 	DimensionIndex::S:return 'S';
	case 	DimensionIndex::I:return 'I';
	case 	DimensionIndex::H:return 'H';
	case 	DimensionIndex::V:return 'V';
	case 	DimensionIndex::B:return 'B';
	default:return '?';
	}
}

/*static*/libCZI::DimensionIndex Utils::CharToDimension(char c)
{
	switch (c)
	{
	case 'z':case'Z':
		return	DimensionIndex::Z;
	case 'c':case'C':
		return DimensionIndex::C;
	case 't':case'T':
		return 	DimensionIndex::T;
	case 'r':case'R':
		return 	DimensionIndex::R;
	case 's':case'S':
		return 	DimensionIndex::S;
	case 'i':case'I':
		return 	DimensionIndex::I;
	case 'h':case'H':
		return 	DimensionIndex::H;
	case 'v':case'V':
		return 	DimensionIndex::V;
	case 'b':case'B':
		return 	DimensionIndex::B;
	default:
		return DimensionIndex::invalid;
	}
}

/*static*/int Utils::CalcMd5SumHash(libCZI::IBitmapData* bm, std::uint8_t* ptrHash, int hashSize)
{
	return CBitmapOperations::CalcMd5Sum(bm, ptrHash, hashSize);
}

/*static*/int Utils::CalcMd5SumHash(const void* ptrData, size_t sizeData, std::uint8_t* ptrHash, int hashSize)
{
	if (ptrHash == nullptr) { return 16; }
	if (hashSize < 16)
	{
		throw invalid_argument("argument 'hashsize' must be >= 16");
	}

	CMd5Sum md5sum;
	md5sum.update(ptrData, sizeData);
	md5sum.complete();
	md5sum.getHash((char*)ptrHash);
	return 16;
}

static double CalcSplineValue(double x, const std::vector<libCZI::IDisplaySettings::SplineData>& splineData)
{
	if (x < 0 || x>1)
	{
		throw invalid_argument("out of range");
	}

	size_t index = 0;
	for (size_t i = 0; i < splineData.size(); ++i)
	{
		if (x > splineData.at(i).xPos)
		{
			index = i;
		}
	}

	double xPosNormalized = x - splineData.at(index).xPos;

	return CSplines::CalculateSplineValue(xPosNormalized, splineData.at(index).coefficients);
}

/*static*/std::vector<std::uint8_t> Utils::Create8BitLookUpTableFromSplines(int tableElementCnt, float blackPoint, float whitePoint, const std::vector<libCZI::IDisplaySettings::SplineData>& splineData)
{
	std::vector<std::uint8_t> lut; lut.reserve(tableElementCnt);

	// TODO - look into rounding
	int blackVal = (int)(tableElementCnt*blackPoint);
	int whiteVal = (int)(tableElementCnt*whitePoint);

	for (int i = 0; i < blackVal; ++i)
	{
		lut.emplace_back(0);
	}

	for (int i = blackVal; i < whiteVal; ++i)
	{
		double x = (i - blackVal) / double(whiteVal - blackVal - 1);
		double s = CalcSplineValue(x, splineData);
		int is = (int)(s * 255);
		std::uint8_t value = (is < 0 ? 0 : is>255 ? 255 : (std::uint8_t)is);
		lut.emplace_back(value);
	}

	for (int i = whiteVal; i < tableElementCnt; ++i)
	{
		lut.emplace_back(255);
	}

	return lut;
}

/// <summary>
/// Gets the parameter for the toe slope adjustment function.
/// </summary>
/// <remarks>
/// The Toe Slope adjustment uses a slightly adjusted version of the gamma function to evaluate the display image.
/// The adjusted version has the advantage that its slope at the origin, i.e. for x = 0, doesn't equal infinity.
/// The formula for this looks like y = ((ax + 1)**G - 1) / ((a + 1)**G - 1), where the parameter "a" depends on the gamma value.
/// Additionally, we choose the slope of 1/(G**3) for x = 0. 
/// This yields the iteration formula that is used in the method:
/// a = ((a+1)**G - 1)/(G**4).
/// </remarks>
/// <param name="gamma">The gamma value.</param>
/// <returns>The parameter necessary for the toe slope adjustment function formula.</returns>
template <typename tFloat>
tFloat GetParameterForToeSlopeAdjustment(tFloat gamma)
{
	const double GammaTolerance = (tFloat)0.0001;
	if (abs(gamma - 0.5) < GammaTolerance)
	{
		return 224;
	}
	else if (abs(gamma - (tFloat)0.45) < GammaTolerance)
	{
		// Optimization for frequently used gamma value.
		return (tFloat)287.806332841221;
	}
	else
	{
		tFloat start = 224;
		tFloat result = start;

		tFloat gamma2 = gamma * gamma;
		tFloat factor = 1 / (gamma2 * gamma2);

		const tFloat ResultTolerance = (tFloat)0.000001;
		const int MaxIterationCount = 200;

		for (int i = 0; i < MaxIterationCount; i++)
		{
			start = result;
			result = factor * (pow(start + 1, gamma) - 1);

			if (abs(start - result) < ResultTolerance)
			{
				break;
			}
		}

		return result;
	}
}

template <typename tFloat>
std::vector<std::uint8_t> InternalCreate8BitLookUpTableFromGamma(int tableElementCnt, tFloat blackPoint, tFloat whitePoint, tFloat gamma)
{
	std::vector<std::uint8_t> lut; lut.reserve(tableElementCnt);

	int lowOut = (int)(blackPoint * tableElementCnt);
	int highOut = (int)(whitePoint * tableElementCnt);

	for (int i = 0; i < lowOut; ++i)
	{
		lut.emplace_back(0);
	}

	if (gamma < 1.0f)
	{
		// If gamma < 1, use toe slope to avoid slope of infinity for x = 0.
		// Toe slope adjustment formula: y = ((ax + 1)**G - 1) / ((a + 1)**G - 1)
		tFloat a = GetParameterForToeSlopeAdjustment(gamma);
		tFloat denumeratorToeSlope = (std::pow(a + 1, gamma) - 1);

		for (int i = lowOut; i < highOut; ++i)
		{
			tFloat x = (i - lowOut) / tFloat(highOut - lowOut - 1);

			tFloat val = 255 * (pow(a * x + 1, gamma) - 1) / denumeratorToeSlope;
			if (val > 255) { val = 255; }
			else if (val < 0) { val = 0; }

			std::uint8_t val8 = (std::uint8_t)val;
			lut.emplace_back(val8);
		}
	}
	else
	{
		for (int i = lowOut; i < highOut; ++i)
		{
			tFloat x = (i - lowOut) / tFloat(highOut - lowOut - 1);
			tFloat val = 255 * pow(x, gamma);
			if (val > 255) { val = 255; }
			else if (val < 0) { val = 0; }

			std::uint8_t val8 = (std::uint8_t)val;
			lut.emplace_back(val8);
		}
	}

	for (int i = highOut; i < tableElementCnt; ++i)
	{
		lut.emplace_back(255);
	}

	return lut;
}

/*static*/std::vector<std::uint8_t> Utils::Create8BitLookUpTableFromGamma(int tableElementCnt, float blackPoint, float whitePoint, float gamma)
{
	return InternalCreate8BitLookUpTableFromGamma(tableElementCnt, blackPoint, whitePoint, gamma);
}

/*static*/std::vector<libCZI::IDisplaySettings::SplineData> Utils::CalcSplineDataFromPoints(int pointCnt, std::function< std::tuple<double, double>(int idx)> getPoint)
{
	auto coeffs = CSplines::GetSplineCoefficients(
		pointCnt + 2,
		[&](int index, double* px, double* py)->void
	{
		if (index == 0)
		{
			if (px != nullptr) { *px = 0; }
			if (py != nullptr) { *py = 0; }
		}
		else if (index == pointCnt + 1)
		{
			if (px != nullptr) { *px = 1; }
			if (py != nullptr) { *py = 1; }
		}
		else
		{
			auto pt = getPoint(index - 1);
			if (px != nullptr) { *px = get<0>(pt); }
			if (py != nullptr) { *py = get<1>(pt); }
		}
	});

	std::vector<libCZI::IDisplaySettings::SplineData> splineData; splineData.reserve(coeffs.size());

	for (int i = 0; i < (int)coeffs.size(); ++i)
	{
		double xCoord = (i == 0) ? 0. : get<0>(getPoint(i - 1));
		IDisplaySettings::SplineData spD{ xCoord, coeffs.at(i) };
		splineData.push_back(spD);
	}

	return splineData;
}

/*static*/std::shared_ptr<libCZI::IBitmapData> Utils::NearestNeighborResize(libCZI::IBitmapData* bmSrc, int dstWidth, int dstHeight)
{
	auto bmDest = GetSite()->CreateBitmap(bmSrc->GetPixelType(), dstWidth, dstHeight);
	CBitmapOperations::NNResize(bmSrc, bmDest.get());
	return bmDest;
}

/*static*/std::shared_ptr<libCZI::IBitmapData > Utils::NearestNeighborResize(libCZI::IBitmapData* bmSrc, int dstWidth, int dstHeight, const DblRect& roiSrc, const DblRect& roiDest)
{
	auto bmDest = GetSite()->CreateBitmap(bmSrc->GetPixelType(), dstWidth, dstHeight);
	CBitmapOperations::Fill(bmDest.get(), { 0,0,0 });

	ScopedBitmapLockerSP lckDest{ bmDest };
	ScopedBitmapLockerP lckSrc{ bmSrc };
	CBitmapOperations::NNResizeInfo2Dbl resizeInfo;
	resizeInfo.srcPtr = lckSrc.ptrDataRoi;
	resizeInfo.srcStride = lckSrc.stride;
	resizeInfo.srcWidth = bmSrc->GetWidth();
	resizeInfo.srcHeight = bmSrc->GetHeight();
	resizeInfo.srcRoiX = roiSrc.x;
	resizeInfo.srcRoiY = roiSrc.y;
	resizeInfo.srcRoiW = roiSrc.w;
	resizeInfo.srcRoiH = roiSrc.h;
	resizeInfo.dstPtr = lckDest.ptrDataRoi;
	resizeInfo.dstStride = lckDest.stride;
	resizeInfo.dstWidth = bmDest->GetWidth();
	resizeInfo.dstHeight = bmDest->GetHeight();
	resizeInfo.dstRoiX = roiDest.x;
	resizeInfo.dstRoiY = roiDest.y;
	resizeInfo.dstRoiW = roiDest.w;
	resizeInfo.dstRoiH = roiDest.h;

	CBitmapOperations::NNSCale2(bmSrc->GetPixelType(), bmDest->GetPixelType(), resizeInfo);

	return bmDest;
}

/*static*/const char* Utils::PixelTypeToInformalString(libCZI::PixelType pxltp)
{
	switch (pxltp)
	{
	case PixelType::Invalid:			return "invalid";
	case PixelType::Gray8:				return "gray8";
	case PixelType::Gray16:				return "gray16";
	case PixelType::Gray32Float:		return "gray32float";
	case PixelType::Bgr24:				return "bgr24";
	case PixelType::Bgr48:				return "bgr48";
	case PixelType::Bgr96Float:			return "bgr96float";
	case PixelType::Bgra32:				return "bgra32";
	case PixelType::Gray64ComplexFloat: return "gray64complexfloat";
	case PixelType::Bgr192ComplexFloat: return "bgr192complexfloat";
	case PixelType::Gray32:				return "gray32";
	case PixelType::Gray64Float:		return "gray64float";
	}

	return "illegal value";
}

/*static*/const char* Utils::CompressionModeToInformalString(libCZI::CompressionMode compressionMode)
{
	switch (compressionMode)
	{
	case CompressionMode::UnCompressed:
		return "uncompressed";
	case CompressionMode::Jpg:
		return "jpg";
	case CompressionMode::JpgXr:
		return "jpgxr";
	case CompressionMode::Invalid:
		return "invalid";
	}

	return "illegal value";
}

/*static*/std::string Utils::DimCoordinateToString(const libCZI::IDimCoordinate* coord)
{
	stringstream ss;
	for (int i = (int)(libCZI::DimensionIndex::MinDim); i <= (int)(libCZI::DimensionIndex::MaxDim); ++i)
	{
		int value;
		if (coord->TryGetPosition((libCZI::DimensionIndex)i, &value))
		{
			ss << DimensionToChar((libCZI::DimensionIndex)i) << value;
		}
	}

	return ss.str();
}

/*static*/std::shared_ptr<libCZI::IIndexSet> Utils::IndexSetFromString(const std::wstring& s)
{
	return std::make_shared<CIndexSet>(s);
	/*std::vector<wstring> parts;
	Utilities::Split(s, L',', [&](const std::wstring str)->bool {parts.push_back(str); });*/

	/*const std::wregex expression(L"([+-]*\s*(?:\d+|inf))\s*-\s*([+-]*\s*(?:\d+|inf))");*/

	class CIndexTest :public IIndexSet
	{
	public:
		bool IsContained(int index) const {
			return index == 2 || index == 3 ? true : false;
		}
	};

	return std::make_shared<CIndexTest>();
}

/*static*/libCZI::PixelType  Utils::TryDeterminePixelTypeForChannel(libCZI::ISubBlockRepository* repository, int channelIdx)
{
	SubBlockInfo info;
	bool b = repository->TryGetSubBlockInfoOfArbitrarySubBlockInChannel(channelIdx, info);
	if (b == false)
	{
		return PixelType::Invalid;
	}

	return info.pixelType;
}
