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

#pragma once

#include "CZIReader.h"
#include "libCZI.h"
#include "SingleChannelAccessorBase.h"

class CSingleChannelTileAccessor : public CSingleChannelAccessorBase, public libCZI::ISingleChannelTileAccessor
{
public:
	explicit CSingleChannelTileAccessor(std::shared_ptr<libCZI::ISubBlockRepository> sbBlkRepository);

public:	// interface ISingleChannelTileAccessor
	std::shared_ptr<libCZI::IBitmapData> Get(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const libCZI::ISingleChannelTileAccessor::Options* pOptions) override;
	std::shared_ptr<libCZI::IBitmapData> Get(libCZI::PixelType pixeltype, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const Options* pOptions) override;
	void Get(libCZI::IBitmapData* pDest, int xPos, int yPos, const libCZI::IDimCoordinate* planeCoordinate, const Options* pOptions) override;
private:
	void InternalGet(int xPos, int yPos, libCZI::IBitmapData* pBm, const libCZI::IDimCoordinate* planeCoordinate, const libCZI::ISingleChannelTileAccessor::Options* pOptions);
	//std::shared_ptr<libCZI::IBitmapData> InternalGet(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const ISingleChannelTileAccessor::Options* pOptions);
	void GetAllSubBlocks(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, std::function<void(int index, int mIndex)> appender/*, libCZI::PixelType* pPixelTypeOfFirstFoundSubBlock*/);

	struct IndexAndM
	{
		int index;
		int mIndex;
	};

	std::vector<CSingleChannelTileAccessor::IndexAndM> GetSubBlocksSubset(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, bool sortByM/*, libCZI::PixelType* pPixelTypeOfFirstFoundSubBlock = nullptr*/);
	void ComposeTiles(libCZI::IBitmapData* pBm, int xPos, int yPos, const std::vector<IndexAndM>& subBlocksSet, const libCZI::ISingleChannelTileAccessor::Options& options);
};
