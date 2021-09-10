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
#include "SingleChannelScalingTileAccessor.h"
#include "utilities.h"
#include "BitmapOperations.h"
#include "Site.h"

using namespace libCZI;
using namespace std;

CSingleChannelScalingTileAccessor::CSingleChannelScalingTileAccessor(std::shared_ptr<ISubBlockRepository> sbBlkRepository)
	: CSingleChannelAccessorBase(sbBlkRepository)
{
}

/*virtual*/libCZI::IntSize CSingleChannelScalingTileAccessor::CalcSize(const libCZI::IntRect& roi, float zoom) const
{
	return InternalCalcSize(roi, zoom);
}

/*virtual*/ std::shared_ptr<libCZI::IBitmapData> CSingleChannelScalingTileAccessor::Get(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::ISingleChannelScalingTileAccessor::Options* pOptions)
{
	if (pOptions == nullptr) { Options opt; opt.Clear(); return this->Get(roi, planeCoordinate, zoom, &opt); }
	libCZI::PixelType pixelType;
	bool b = this->TryGetPixelType(planeCoordinate, pixelType);
	if (b == false)
	{
		throw LibCZIAccessorException("Unable to determine the pixeltype.", LibCZIAccessorException::ErrorType::CouldntDeterminePixelType);
	}

	return this->Get(pixelType, roi, planeCoordinate, zoom, pOptions);
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CSingleChannelScalingTileAccessor::Get(libCZI::PixelType pixeltype, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::ISingleChannelScalingTileAccessor::Options* pOptions)
{
	if (pOptions == nullptr) { Options opt; opt.Clear(); return this->Get(pixeltype, roi, planeCoordinate, zoom, &opt); }
	IntSize sizeOfBitmap = InternalCalcSize(roi, zoom);
	auto bmDest = GetSite()->CreateBitmap(pixeltype, sizeOfBitmap.w, sizeOfBitmap.h);
	this->InternalGet(bmDest.get(), roi, planeCoordinate, zoom, *pOptions);
	return bmDest;
}

/*virtual*/void CSingleChannelScalingTileAccessor::Get(libCZI::IBitmapData* pDest, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::ISingleChannelScalingTileAccessor::Options* pOptions)
{
	if (pOptions == nullptr) { Options opt; opt.Clear(); return this->Get(pDest, roi, planeCoordinate, zoom, &opt); }

	IntSize sizeOfBitmap = InternalCalcSize(roi, zoom);
	if (sizeOfBitmap.w != pDest->GetWidth() || sizeOfBitmap.h != pDest->GetHeight())
	{
		stringstream ss;
		ss << "The specified bitmap has a size of " << pDest->GetWidth() << "*" << pDest->GetHeight() << ", whereas the expected size is " << sizeOfBitmap.w << "*" << sizeOfBitmap.h << ".";
		throw invalid_argument(ss.str().c_str());
	}

	this->InternalGet(pDest, roi, planeCoordinate, zoom, *pOptions);
}

// ----------------------------------------------------------------------------------------------------------------------

/*static*/libCZI::IntSize CSingleChannelScalingTileAccessor::InternalCalcSize(const libCZI::IntRect& roi, float zoom)
{
	return IntSize{ (uint32_t)(roi.w*zoom),(uint32_t)(roi.h*zoom) };
}

void CSingleChannelScalingTileAccessor::ScaleBlt(libCZI::IBitmapData* bmDest, float /*zoom*/, const libCZI::IntRect&  roi, const SbInfo& sbInfo)
{
	// calculate the intersection of the with the subblock (logical rect) and the destination
	auto intersect = Utilities::Intersect(sbInfo.logicalRect, roi);

	double roiSrcTopLeftX = double(intersect.x - sbInfo.logicalRect.x) / sbInfo.logicalRect.w;
	double roiSrcTopLeftY = double(intersect.y - sbInfo.logicalRect.y) / sbInfo.logicalRect.h;
	double roiSrcBttmRightX = double(intersect.x + intersect.w - sbInfo.logicalRect.x) / sbInfo.logicalRect.w;
	double roiSrcBttmRightY = double(intersect.y + intersect.h - sbInfo.logicalRect.y) / sbInfo.logicalRect.h;

	double destTopLeftX = double(intersect.x - roi.x) / roi.w;
	double destTopLeftY = double(intersect.y - roi.y) / roi.h;
	double destBttmRightX = double(intersect.x + intersect.w - roi.x) / roi.w;
	double destBttmRightY = double(intersect.y + intersect.h - roi.y) / roi.h;

	DblRect srcRoi{ roiSrcTopLeftX ,roiSrcTopLeftY,roiSrcBttmRightX - roiSrcTopLeftX ,roiSrcBttmRightY - roiSrcTopLeftY };
	DblRect dstRoi{ destTopLeftX ,destTopLeftY,destBttmRightX - destTopLeftX ,destBttmRightY - destTopLeftY };

	srcRoi.x *= sbInfo.physicalSize.w;
	srcRoi.y *= sbInfo.physicalSize.h;
	srcRoi.w *= sbInfo.physicalSize.w;
	srcRoi.h *= sbInfo.physicalSize.h;

	dstRoi.x *= bmDest->GetWidth();
	dstRoi.y *= bmDest->GetHeight();
	dstRoi.w *= bmDest->GetWidth();
	dstRoi.h *= bmDest->GetHeight();

	auto sb = this->sbBlkRepository->ReadSubBlock(sbInfo.index);
	if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
	{
		stringstream ss;
		ss << "   bounds: " << Utils::DimCoordinateToString(&sb->GetSubBlockInfo().coordinate);
		GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss);
	}

	auto spBm = sb->CreateBitmap();

	CBitmapOperations::NNResize(spBm.get(), bmDest, srcRoi, dstRoi);
}

int CSingleChannelScalingTileAccessor::GetIdxOf1stSubBlockWithZoomGreater(const std::vector<SbInfo>& sbBlks, const std::vector<int>& byZoom, float zoom)
{
	// now, skip until the zoom of the subBlock is greater than the specified zoom
	for (size_t i = 0; i < byZoom.size(); ++i)
	{
		if (sbBlks.at(byZoom.at(i)).GetZoom() >= zoom)
		{
			return (int)i;
		}
	}

	return -1;
}

/// <summary>	
/// Create an vector with indices (into the specified vector with SubBlock-infos) so that the indices give 
/// the items sorted by their "zoom"-factor. (A zoom of "1" means that the subblock is on layer-0). Subblocks of
/// a higher pyramid-layer are at the end of the list.
/// </summary>
/// <remarks>	
/// TODO: within a pyramid-layer, the subblocks should be sorted according to their M-index. The problem is (once again...)
/// to define a "pyramid-layer". Since, currently, only layer-0 may have overlapping tiles, it is only relevant for layer-0,
/// and layer-0 can be easily defined by physical_size==logical_size.
/// </remarks>
/// <param name="sbBlks">	The vector of subblock-infos for which to create the sorted indices. </param>
/// <returns>	A vector with indices which give the subblocks sorted by their zoom (biggest zoom first). </returns>
std::vector<int> CSingleChannelScalingTileAccessor::CreateSortByZoom(const std::vector<SbInfo>& sbBlks)
{
	std::vector<int> byZoom;
	byZoom.reserve(sbBlks.size());
	for (size_t i = 0; i < sbBlks.size(); ++i) { byZoom.emplace_back((int)i); }
	std::sort(byZoom.begin(), byZoom.end(), [&](const int i1, const int i2)->bool {return sbBlks.at(i1).GetZoom() < sbBlks.at(i2).GetZoom(); });
	return byZoom;
}

std::vector<CSingleChannelScalingTileAccessor::SbInfo> CSingleChannelScalingTileAccessor::GetSubSet(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate)
{
	std::vector<SbInfo> sblks;
	this->sbBlkRepository->EnumSubset(planeCoordinate, &roi, false,
		[&](int idx, const SubBlockInfo& info)->bool
	{
		SbInfo sbinfo;
		sbinfo.logicalRect = info.logicalRect;
		sbinfo.physicalSize = info.physicalSize;
		sbinfo.mIndex = info.mIndex;
		sbinfo.index = idx;
		sblks.push_back(sbinfo);
		return true;
	});

	return sblks;
}

void CSingleChannelScalingTileAccessor::InternalGet(libCZI::IBitmapData* bmDest, const libCZI::IntRect&  roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::ISingleChannelScalingTileAccessor::Options& options)
{
	this->CheckPlaneCoordinates(planeCoordinate);
	Clear(bmDest, options.backGroundColor);
	std::vector<int> scenesInvolved = this->DetermineInvolvedScenes(roi,options.sceneFilter.get());

	if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
	{
		stringstream ss;
		ss << "SingleChannelScalingTileAccessor -> Plane: " << Utils::DimCoordinateToString(planeCoordinate) << " Requested ROI: " << roi << " Zoom: " << zoom;
		GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss);
#ifdef WORKBENCH_CZI_MOD
        ss.str("");
#else
        /*
         * Appear to be clearing the string stream by swapping with empty stream
         */
		std::stringstream().swap(ss);
#endif
		if (scenesInvolved.empty())
		{
			ss << " scenes involved: none found (=either no scenes in repository or no overlap at all)";
		}
		else
		{
			ss << " scenes involved: ";
			bool isFirst = true;
			for (auto it : scenesInvolved)
			{
				if (!isFirst)
				{
					ss << ", ";
				}

				ss << it;
				isFirst = false;
			}
		}

		GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss);
	}


	if (scenesInvolved.size() <= 1)
	{
		// we only have to deal with a single scene (or: the document does not include a scene-dimension at all)
		auto sbSetsortedByZoom = this->GetSubSetSortedByZoom(roi, planeCoordinate);
		this->Paint(bmDest, roi, sbSetsortedByZoom, zoom);
	}
	else
	{
		auto sbSetSortedByZoomPerScene = this->GetSubSetSortedByZoomPerScene(scenesInvolved, roi, planeCoordinate);
		for (const auto& it : sbSetSortedByZoomPerScene)
		{
			this->Paint(bmDest, roi, get<1>(it), zoom);
		}
	}
}

void CSingleChannelScalingTileAccessor::Paint(libCZI::IBitmapData* bmDest, const libCZI::IntRect&  roi, const SubSetSortedByZoom& sbSetSortedByZoom, float zoom)
{
	int idxOf1stSSubBlockOfZoomGreater = this->GetIdxOf1stSubBlockWithZoomGreater(sbSetSortedByZoom.subBlocks, sbSetSortedByZoom.sortedByZoom, zoom);
	if (idxOf1stSSubBlockOfZoomGreater < 0)
	{
		// this means that we would need to overzoom (i.e. the requested zoom is less than the lowest level we find in the subblock-repository)
		// TODO: this requires special consideration, for the time being -> bail out
		// ...we end up here e. g. when lowest level does not cover all the range, so - this is not
		//    something where we want to throw an excpetion
		//throw LibCZIAccessorException("Overzoom not supported", LibCZIAccessorException::ErrorType::Unspecified);
		return;
	}

	std::vector<int>::const_iterator it = sbSetSortedByZoom.sortedByZoom.cbegin();
	std::advance(it, idxOf1stSSubBlockOfZoomGreater);

	float startZoom = sbSetSortedByZoom.subBlocks.at(*it).GetZoom();

	for (; it != sbSetSortedByZoom.sortedByZoom.cend(); ++it)
	{
		const SbInfo& sbInfo = sbSetSortedByZoom.subBlocks.at(*it);

		// as an interim solution (in fact... this seems to be a rather good solution...), stop when we arrive at subblocks with a zoom-level about twice that what we started with
		if (sbInfo.GetZoom() >= startZoom * 1.9)
		{
			break;
		}

		if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
		{
			stringstream ss;
			ss << " Drawing subblock: idx=" << sbInfo.index << " Log.: " << sbInfo.logicalRect << " Phys.Size: " << sbInfo.physicalSize;
			GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss);
		}

		this->ScaleBlt(bmDest, zoom, roi, sbInfo);
	}
}

/// <summary>	Using the specified ROI, determine the scenes it intersects with. If the
/// 			subblock-repository does not contain an "S-dimension" we return an empty result.</summary>
/// <param name="roi">	The ROI. </param>
/// <param name="pSceneIndexSet"> Set of the scenes which are "allowed". May be null, in which case all scenes are considered "allowed". </param>
/// <returns>	A vector with the scene indices that the specified ROI intersects with. </returns>
std::vector<int> CSingleChannelScalingTileAccessor::DetermineInvolvedScenes(const libCZI::IntRect&  roi,const libCZI::IIndexSet* pSceneIndexSet)
{
	SubBlockStatistics statistics = this->sbBlkRepository->GetStatistics();
	if (statistics.sceneBoundingBoxes.empty())
	{
		return std::vector<int>();
	}

	std::vector<int> result;
	for (auto it = statistics.sceneBoundingBoxes.cbegin(); it != statistics.sceneBoundingBoxes.cend(); ++it)
	{
		// check if the scene is part of the "scene index set" (if this is specified)
		if (pSceneIndexSet == nullptr || pSceneIndexSet->IsContained(it->first))
		{
			if (it->second.boundingBox.IntersectsWith(roi))
			{
				result.push_back(it->first);
			}
		}
	}

	return result;
}

CSingleChannelScalingTileAccessor::SubSetSortedByZoom CSingleChannelScalingTileAccessor::GetSubSetSortedByZoom(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate)
{
	SubSetSortedByZoom result;
	result.subBlocks = this->GetSubSet(roi, planeCoordinate);
	result.sortedByZoom = this->CreateSortByZoom(result.subBlocks);
	return result;
}

std::vector<std::tuple<int, CSingleChannelScalingTileAccessor::SubSetSortedByZoom>> CSingleChannelScalingTileAccessor::GetSubSetSortedByZoomPerScene(const vector<int>& scenes, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate)
{
	std::vector<std::tuple<int, CSingleChannelScalingTileAccessor::SubSetSortedByZoom>> result;
	CDimCoordinate coord(planeCoordinate);
	for (const auto& sceneIdx : scenes)
	{
		SubSetSortedByZoom sbset;

		// we explicitely set the S-coordinate here so that we only get subblocks of this scene
		// TODO: we need to look into what is supposed to happen if the user passed in a scene-index
		//       I guess the natural thing would be to consider only the specified scene
		coord.Set(DimensionIndex::S, sceneIdx);
		sbset.subBlocks = this->GetSubSet(roi, &coord);
		sbset.sortedByZoom = this->CreateSortByZoom(sbset.subBlocks);
		result.emplace_back(make_tuple(sceneIdx, sbset));
	}

	return result;
}
