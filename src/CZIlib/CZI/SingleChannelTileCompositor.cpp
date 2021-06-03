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
#include "libCZI.h"
#include "SingleChannelTileCompositor.h"
#include "BitmapOperations.h"

using namespace libCZI;

/*static*/void CSingleChannelTileCompositor::Compose(libCZI::IBitmapData* dest, libCZI::IBitmapData* source, int x, int y, bool drawTileBorder)
{
	auto srcLck = ScopedBitmapLockerP(source);
	auto dstLck = ScopedBitmapLockerP(dest);
	CBitmapOperations::CopyOffsetedInfo info;
	info.xOffset = x;
	info.yOffset = y;
	info.srcPixelType = source->GetPixelType();
	info.srcPtr = srcLck.ptrDataRoi;
	info.srcStride = srcLck.stride;
	info.srcWidth = source->GetWidth();
	info.srcHeight = source->GetHeight();

	info.dstPixelType = dest->GetPixelType();
	info.dstPtr = dstLck.ptrDataRoi;
	info.dstStride = dstLck.stride;
	info.dstWidth = dest->GetWidth();
	info.dstHeight = dest->GetHeight();

	info.drawTileBorder = drawTileBorder;

	CBitmapOperations::CopyOffseted(info);
}

/*-----------------------------------------------------------------------------------------------*/

/*static*/void libCZI::Compositors::ComposeSingleChannelTiles(
	std::function<bool(int, std::shared_ptr<libCZI::IBitmapData>&, int&, int&)> getTiles,
	libCZI::IBitmapData* dest,
	int xPos,
	int yPos,
	const ComposeSingleTileOptions* pOptions)
{
	if (pOptions == nullptr)
	{
		ComposeSingleTileOptions options; options.Clear();
		ComposeSingleChannelTiles(getTiles, dest, xPos, yPos, &options);
		return;
	}

	for (int i = 0;; ++i)
	{
		int posXTile, posYTile;
		std::shared_ptr<libCZI::IBitmapData> src;
		bool b = getTiles(i, src, posXTile, posYTile);
		if (b != true) break;
		// TODO: check return values?
		CSingleChannelTileCompositor::Compose(dest, src.get(), posXTile - xPos, posYTile - yPos, pOptions->drawTileBorder);
	}
}
