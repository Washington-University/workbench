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
#include "SingleChannelTileAccessor.h"
#include "CziUtils.h"
#include "utilities.h"
#include "SingleChannelTileCompositor.h"
#include "Site.h"
#include <iterator> 
#include "bitmapData.h"

using namespace libCZI;
using namespace std;

CSingleChannelTileAccessor::CSingleChannelTileAccessor(std::shared_ptr<ISubBlockRepository> sbBlkRepository)
	: CSingleChannelAccessorBase(sbBlkRepository)
{
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CSingleChannelTileAccessor::Get(const libCZI::IntRect& roi, const IDimCoordinate* planeCoordinate, const Options* pOptions)
{
	// first, we need to determine the pixeltype, which we do from the repository
	libCZI::PixelType pixelType;
	bool b = this->TryGetPixelType(planeCoordinate, pixelType);
	if (b == false)
	{
		throw LibCZIAccessorException("Unable to determine the pixeltype.", LibCZIAccessorException::ErrorType::CouldntDeterminePixelType);
	}

	return this->Get(pixelType, roi, planeCoordinate, pOptions);
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CSingleChannelTileAccessor::Get(libCZI::PixelType pixeltype, const libCZI::IntRect& roi, const IDimCoordinate* planeCoordinate, const Options* pOptions)
{
	auto bmDest = GetSite()->CreateBitmap(pixeltype, roi.w, roi.h);
	this->InternalGet(roi.x, roi.y, bmDest.get(), planeCoordinate, pOptions);
	return bmDest;
}

/*virtual*/void CSingleChannelTileAccessor::Get(libCZI::IBitmapData* pDest, int xPos, int yPos, const IDimCoordinate* planeCoordinate, const Options* pOptions)
{
	this->InternalGet(xPos, yPos, pDest, planeCoordinate, pOptions);
}

void CSingleChannelTileAccessor::ComposeTiles(libCZI::IBitmapData* pBm, int xPos, int yPos, const std::vector<IndexAndM>& subBlocksSet, const ISingleChannelTileAccessor::Options& options)
{
	Compositors::ComposeSingleTileOptions composeOptions; composeOptions.Clear();
	composeOptions.drawTileBorder = options.drawTileBorder;
	Compositors::ComposeSingleChannelTiles(
		[&](int index, std::shared_ptr<libCZI::IBitmapData>& spBm, int& xPosTile, int& yPosTile)->bool
	{
		if (index < (int)subBlocksSet.size())
		{
			auto sb = this->sbBlkRepository->ReadSubBlock(subBlocksSet[index].index);
			spBm = sb->CreateBitmap();
			xPosTile = sb->GetSubBlockInfo().logicalRect.x;
			yPosTile = sb->GetSubBlockInfo().logicalRect.y;
			return true;
		}

		return false;
	},
		pBm,
		xPos,
		yPos,
		&composeOptions);
}

void CSingleChannelTileAccessor::InternalGet(int xPos, int yPos, libCZI::IBitmapData* pBm, const IDimCoordinate* planeCoordinate, const ISingleChannelTileAccessor::Options* pOptions)
{
	if (pOptions == nullptr)
	{
		ISingleChannelTileAccessor::Options options; options.Clear();
		this->InternalGet(xPos, yPos, pBm, planeCoordinate, &options);
		return;
	}

	this->CheckPlaneCoordinates(planeCoordinate);
	Clear(pBm, pOptions->backGroundColor);
	IntSize sizeBm = pBm->GetSize();
	IntRect roi{ xPos,yPos,(int)sizeBm.w,(int)sizeBm.h };
	std::vector<IndexAndM> subBlocksSet = this->GetSubBlocksSubset(roi, planeCoordinate, pOptions->sortByM);

	this->ComposeTiles(pBm, xPos, yPos, subBlocksSet, *pOptions);
}

std::vector<CSingleChannelTileAccessor::IndexAndM> CSingleChannelTileAccessor::GetSubBlocksSubset(const IntRect& roi, const IDimCoordinate* planeCoordinate, bool sortByM /*,libCZI::PixelType* pPixelTypeOfFirstFoundSubBlock=nullptr*/)
{
	// ok... for a first tentative, experimental and quick-n-dirty implementation, simply
	// get all subblocks by enumerating all
	std::vector<IndexAndM> subBlocksSet;
	this->GetAllSubBlocks(roi, planeCoordinate, [&](int index, int mIndex)->void {subBlocksSet.emplace_back(IndexAndM{ index,mIndex }); });
	if (sortByM == true)
	{
		// sort ascending-by-M-index (-> lowest M-index first, highest last)
		std::sort(subBlocksSet.begin(), subBlocksSet.end(), [](const IndexAndM& i1, const IndexAndM& i2)->bool {return i1.mIndex < i2.mIndex; });
	}

	return subBlocksSet;
}

void CSingleChannelTileAccessor::GetAllSubBlocks(const IntRect& roi, const IDimCoordinate* planeCoordinate, std::function<void(int index, int mIndex)> appender/*, libCZI::PixelType* pPixelTypeOfFirstFoundSubBlock*/)
{
	this->sbBlkRepository->EnumSubset(planeCoordinate, nullptr, true,
		[&](int idx, const SubBlockInfo& info)->bool
	{
		if (Utilities::DoIntersect(roi, info.logicalRect))
		{
			appender(idx, info.mIndex);
		}

		return true;
	});
}

