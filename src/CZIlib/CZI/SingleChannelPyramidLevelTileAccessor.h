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

class CSingleChannelPyramidLevelTileAccessor : public CSingleChannelAccessorBase, public libCZI::ISingleChannelPyramidLayerTileAccessor
{
private:
	struct SbInfo
	{
		libCZI::IntRect			logicalRect;
		libCZI::IntSize			physicalSize;
		int						mIndex;
		int						index;
	};

	struct SbByLayer
	{
		//int		layer;
		std::vector<int> indices;
	};

public:
	explicit CSingleChannelPyramidLevelTileAccessor(std::shared_ptr<libCZI::ISubBlockRepository> sbBlkRepository);

public:	// interface ISingleChannelTileAccessor
	std::shared_ptr<libCZI::IBitmapData> Get(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const Options* pOptions) override;

	std::shared_ptr<libCZI::IBitmapData> Get(libCZI::PixelType pixeltype, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const libCZI::ISingleChannelPyramidLayerTileAccessor::Options* pOptions) override;

	void Get(libCZI::IBitmapData* pDest, int xPos, int yPos, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const Options* pOptions) override;
private:
	libCZI::IntRect CalcDestinationRectFromSourceRect(const libCZI::IntRect& roi, const PyramidLayerInfo& pyramidInfo);

	libCZI::IntRect NormalizePyramidRect(int x, int y, int w, int h, const PyramidLayerInfo& pyramidInfo);

	static int CalcSizeOfPixelOnLayer0(const PyramidLayerInfo& pyramidInfo);

	std::map<int,SbByLayer> CalcByLayer(const std::vector<SbInfo>& sbinfo, int minificationFactor);

	std::vector<SbInfo> GetSubBlocksSubset(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo,const libCZI::IIndexSet* sceneFilter);

	void GetAllSubBlocks(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, const libCZI::IIndexSet* sceneFilter, std::function<void(const SbInfo& info)> appender);

	int CalcPyramidLayerNo(const libCZI::IntRect& logicalRect, const libCZI::IntSize& physicalSize, int minificationFactor);

	void ComposeTiles(libCZI::IBitmapData* bm, int xPos, int yPos, int sizeOfPixel, int bitmapCnt, const Options& options, std::function<SbInfo(int)> getSbInfo);

	void InternalGet(libCZI::IBitmapData* pDest, int xPos, int yPos, int sizeOfPixelOnLayer0, const libCZI::IDimCoordinate* planeCoordinate, const PyramidLayerInfo& pyramidInfo, const Options& options);

};

