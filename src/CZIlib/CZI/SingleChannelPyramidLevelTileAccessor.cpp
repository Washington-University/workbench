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
#include "SingleChannelPyramidLevelTileAccessor.h"
#include "utilities.h"
#include "Site.h"

using namespace libCZI;
using namespace std;

CSingleChannelPyramidLevelTileAccessor::CSingleChannelPyramidLevelTileAccessor(std::shared_ptr<ISubBlockRepository> sbBlkRepository)
	: CSingleChannelAccessorBase(sbBlkRepository)
{
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CSingleChannelPyramidLevelTileAccessor::Get(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const ISingleChannelPyramidLayerTileAccessor::Options* pOptions)
{
	if (pOptions == nullptr) { Options opt; opt.Clear(); return this->Get(roi, planeCoordinate, pyramidInfo, &opt); }
	libCZI::PixelType pixelType;
	bool b = this->TryGetPixelType(planeCoordinate, pixelType);
	if (b == false)
	{
		throw LibCZIAccessorException("Unable to determine the pixeltype.", LibCZIAccessorException::ErrorType::CouldntDeterminePixelType);
	}

	return this->Get(pixelType, roi, planeCoordinate, pyramidInfo, pOptions);
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CSingleChannelPyramidLevelTileAccessor::Get(libCZI::PixelType pixeltype, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const libCZI::ISingleChannelPyramidLayerTileAccessor::Options* pOptions)
{
	if (pOptions == nullptr) { Options opt; opt.Clear(); return this->Get(pixeltype, roi, planeCoordinate, pyramidInfo, &opt); }
	int sizeOfPixel = CalcSizeOfPixelOnLayer0(pyramidInfo);
	IntSize sizeOfBitmap{ (std::uint32_t)(roi.w / sizeOfPixel),(std::uint32_t)(roi.h / sizeOfPixel) };
	if (sizeOfBitmap.w <= 0 || sizeOfBitmap.h <= 0)
	{
		// TODO
		throw runtime_error("error");
	}

	auto bmDest = GetSite()->CreateBitmap(pixeltype, sizeOfBitmap.w, sizeOfBitmap.h);
	this->InternalGet(bmDest.get(), roi.x, roi.y, sizeOfPixel, planeCoordinate, pyramidInfo, *pOptions);
	return bmDest;
}

/*virtual*/void CSingleChannelPyramidLevelTileAccessor::Get(libCZI::IBitmapData* pDest, int xPos, int yPos, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const Options* pOptions)
{
	if (pOptions == nullptr) { Options opt; opt.Clear(); this->Get(pDest, xPos, yPos, planeCoordinate, pyramidInfo, &opt); return; }
	int sizeOfPixel = CalcSizeOfPixelOnLayer0(pyramidInfo);
	this->InternalGet(pDest, xPos, yPos, sizeOfPixel, planeCoordinate, pyramidInfo, *pOptions);
}

void CSingleChannelPyramidLevelTileAccessor::InternalGet(libCZI::IBitmapData* pDest, int xPos, int yPos, int sizeOfPixelOnLayer0, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const Options& options)
{
	this->CheckPlaneCoordinates(planeCoordinate);
	Clear(pDest, options.backGroundColor);
	auto sizeBitmap = pDest->GetSize();
	auto subSet = GetSubBlocksSubset(IntRect{ xPos,yPos,(int)sizeBitmap.w*sizeOfPixelOnLayer0,(int)sizeBitmap.h*sizeOfPixelOnLayer0 }, planeCoordinate, pyramidInfo, options.sceneFilter.get());
	if (subSet.size()==0)
	{	// no subblocks were found in the requested plane/ROI, so there is nothing to do
		return;
	}

	auto byLayer = CalcByLayer(subSet, pyramidInfo.minificationFactor);
	// ok, now we just have to look at our requested pyramid-layer
	const auto& indices = byLayer.at(pyramidInfo.pyramidLayerNo).indices;

	// and now... copy...
	this->ComposeTiles(pDest, xPos, yPos, sizeOfPixelOnLayer0, (int)indices.size(), options,
		[&](int idx)->SbInfo
	{
		return subSet.at(indices.at(idx));
	});
}

void CSingleChannelPyramidLevelTileAccessor::ComposeTiles(libCZI::IBitmapData* bm, int xPos, int yPos, int sizeOfPixel, int bitmapCnt, const Options& options, std::function<SbInfo(int)> getSbInfo)
{
	Compositors::ComposeSingleTileOptions composeOptions; composeOptions.Clear();
	composeOptions.drawTileBorder = options.drawTileBorder;

	Compositors::ComposeSingleChannelTiles(
		[&](int index, std::shared_ptr<libCZI::IBitmapData>& spBm, int& xPosTile, int& yPosTile)->bool
	{
		if (index < bitmapCnt)
		{
			SbInfo sbinfo = getSbInfo(index);
			auto sb = this->sbBlkRepository->ReadSubBlock(sbinfo.index);
			spBm = sb->CreateBitmap();
			xPosTile = (sb->GetSubBlockInfo().logicalRect.x - xPos) / sizeOfPixel;
			yPosTile = (sb->GetSubBlockInfo().logicalRect.y - yPos) / sizeOfPixel;
			return true;
		}

		return false;
	},
		bm,
		0,
		0,
		&composeOptions);

}

libCZI::IntRect CSingleChannelPyramidLevelTileAccessor::CalcDestinationRectFromSourceRect(const libCZI::IntRect& roi, const PyramidLayerInfo& pyramidInfo)
{
	int p = CalcSizeOfPixelOnLayer0(pyramidInfo);
	int w = roi.w / p;
	int h = roi.h / p;
	return IntRect{ roi.x, roi.y, w, h };
}

libCZI::IntRect CSingleChannelPyramidLevelTileAccessor::NormalizePyramidRect(int x, int y, int w, int h, const PyramidLayerInfo& pyramidInfo)
{
	int p = this->CalcSizeOfPixelOnLayer0(pyramidInfo);
	return IntRect{ x,y,w*p,h*p };
}

/// <summary>	For the specified pyramid layer (and the pyramid type), calculate the size of a pixel on this
/// 			layer as measured by pixels on layer 0. </summary>
/// <param name="pyramidInfo">	Information describing the pyramid and the requested pyramid-layer. </param>
/// <returns>	The calculated size of pixel (in units of pixels on pyramid layer 0). </returns>
/*static*/int CSingleChannelPyramidLevelTileAccessor::CalcSizeOfPixelOnLayer0(const PyramidLayerInfo& pyramidInfo)
{
	int f = 1;
	for (int i = 0; i < pyramidInfo.pyramidLayerNo; ++i)
	{
		f *= pyramidInfo.minificationFactor;
	}

	return f;
}

std::map<int, CSingleChannelPyramidLevelTileAccessor::SbByLayer> CSingleChannelPyramidLevelTileAccessor::CalcByLayer(const std::vector<SbInfo>& sbinfos, int minificationFactor)
{
	std::map<int, CSingleChannelPyramidLevelTileAccessor::SbByLayer> result;
	for (size_t i = 0; i < sbinfos.size(); ++i)
	{
		const SbInfo& sbinfo = sbinfos.at(i);
		int pyrLayer = this->CalcPyramidLayerNo(sbinfo.logicalRect, sbinfo.physicalSize, minificationFactor);
		result[pyrLayer].indices.push_back((int)i);
	}

	return result;
}

int CSingleChannelPyramidLevelTileAccessor::CalcPyramidLayerNo(const libCZI::IntRect& logicalRect, const libCZI::IntSize& physicalSize, int minificationFactorPerLayer)
{
	double minFactor;
	if (physicalSize.w > physicalSize.h)
	{
		minFactor = double(logicalRect.w) / physicalSize.w;
	}
	else
	{
		minFactor = double(logicalRect.h) / physicalSize.h;
	}

	int minFactorInt = (int)round(minFactor);
	int f = 1;
	int layerNo = -1;
	for (int layer = 0;; layer++)
	{
		if (f >= minFactorInt)
		{
			layerNo = layer;
			break;
		}

		f *= minificationFactorPerLayer;
	}

	if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
	{
		stringstream ss;
		ss << "Logical=(" << logicalRect.x << "," << logicalRect.y << "," << logicalRect.w << "," << logicalRect.h << ") size=(" <<
			physicalSize.w << "," << physicalSize.h << ") minFactorPerLayer=" << minificationFactorPerLayer <<
			" minFact=" << minFactor << "[" << minFactorInt << "]" << " -> Layer:" << layerNo;
		GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss);
	}

	return layerNo;
}

std::vector<CSingleChannelPyramidLevelTileAccessor::SbInfo> CSingleChannelPyramidLevelTileAccessor::GetSubBlocksSubset(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const libCZI::IIndexSet* sceneFilter)
{
	std::vector<CSingleChannelPyramidLevelTileAccessor::SbInfo> sblks;
	this->GetAllSubBlocks(roi, planeCoordinate, sceneFilter,
		[&](const SbInfo& info)->void
	{
		sblks.emplace_back(info);
	});

	return sblks;
}

/// Enumerate all sub blocks on the specified plane which intersect with the specified ROI - irrespective of their zoom.
///
/// \param roi									   The roi.
/// \param planeCoordinate						   The plane coordinate.
/// \param appender								   The appender.
/// \param [in,out] pPixelTypeOfFirstFoundSubBlock If non-null, the pixel type of first found
/// sub block.
void CSingleChannelPyramidLevelTileAccessor::GetAllSubBlocks(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const libCZI::IIndexSet* sceneFilter, std::function<void(const SbInfo&)> appender)
{
	this->sbBlkRepository->EnumSubset(planeCoordinate, nullptr, false,
		[&](int idx, const SubBlockInfo& info)->bool
	{
		if (sceneFilter != nullptr)
		{
			int indexS;
			if (info.coordinate.TryGetPosition(DimensionIndex::S, &indexS) == true)
			{
				if (!sceneFilter->IsContained(indexS))
				{
					return true;
				}
			}
		}

		if (Utilities::DoIntersect(roi, info.logicalRect))
		{
			SbInfo sbinfo;
			sbinfo.logicalRect = info.logicalRect;
			sbinfo.physicalSize = info.physicalSize;
			sbinfo.mIndex = info.mIndex;
			sbinfo.index = idx;
			appender(sbinfo);
		}

		return true;
	});
}
