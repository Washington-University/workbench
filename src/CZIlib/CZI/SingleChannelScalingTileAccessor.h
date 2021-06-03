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

#include <tuple>
#include <vector>
#include "CZIReader.h"
#include "libCZI.h"
#include "SingleChannelAccessorBase.h"

class CSingleChannelScalingTileAccessor : public CSingleChannelAccessorBase, public libCZI::ISingleChannelScalingTileAccessor
{
private:
	struct SbInfo
	{
		libCZI::IntRect			logicalRect;
		libCZI::IntSize			physicalSize;
		int						mIndex;
		int						index;

		float	GetZoom() const { return libCZI::Utils::CalcZoom(this->logicalRect, this->physicalSize); }
	};

public:
	explicit CSingleChannelScalingTileAccessor(std::shared_ptr<libCZI::ISubBlockRepository> sbBlkRepository);

public:	// interface ISingleChannelScalingTileAccessor
	libCZI::IntSize CalcSize(const libCZI::IntRect& roi, float zoom) const override;
	std::shared_ptr<libCZI::IBitmapData> Get(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::ISingleChannelScalingTileAccessor::Options* pOptions) override;
	std::shared_ptr<libCZI::IBitmapData> Get(libCZI::PixelType pixeltype, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::ISingleChannelScalingTileAccessor::Options* pOptions) override;
	void Get(libCZI::IBitmapData* pDest, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::ISingleChannelScalingTileAccessor::Options* pOptions) override;
private:
	static libCZI::IntSize InternalCalcSize(const libCZI::IntRect& roi, float zoom);

	std::vector<int> CreateSortByZoom(const std::vector<SbInfo>& sbBlks);
	std::vector<SbInfo> GetSubSet(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate);
	int GetIdxOf1stSubBlockWithZoomGreater(const std::vector<SbInfo>& sbBlks, const std::vector<int>& byZoom, float zoom);
	void ScaleBlt(libCZI::IBitmapData* bmDest, float zoom, const libCZI::IntRect&  roi, const SbInfo& sbInfo);

	void InternalGet(libCZI::IBitmapData* bmDest, const libCZI::IntRect&  roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::ISingleChannelScalingTileAccessor::Options& options);

	std::vector<int> DetermineInvolvedScenes(const libCZI::IntRect&  roi, const libCZI::IIndexSet* pSceneIndexSet);
	
	struct SubSetSortedByZoom
	{
		std::vector<SbInfo> subBlocks;
		std::vector<int>	sortedByZoom;
	};

	SubSetSortedByZoom GetSubSetSortedByZoom(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate);

	std::vector<std::tuple<int, SubSetSortedByZoom>> GetSubSetSortedByZoomPerScene(const std::vector<int>& scenes, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate);
	void Paint(libCZI::IBitmapData* bmDest, const libCZI::IntRect&  roi,const SubSetSortedByZoom& sbSetSortedByZoom, float zoom);
};
