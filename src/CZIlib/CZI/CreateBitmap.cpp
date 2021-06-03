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
#include "bitmapData.h"
#include "Site.h"
#include "libCZI.h"

using namespace libCZI;

static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_JpgXr(ISubBlock* subBlk)
{
	auto dec = GetSite()->GetDecoder(ImageDecoderType::JPXR_JxrLib, nullptr);
	const void* ptr; size_t size;
	subBlk->DangerousGetRawData(ISubBlock::MemBlkType::Data, ptr, size);
	return dec->Decode(ptr, size);
}

static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_Uncompressed(ISubBlock* subBlk)
{
	size_t size;
	CSharedPtrAllocator sharedPtrAllocator(subBlk->GetRawData(ISubBlock::MemBlkType::Data, &size));

	// TODO: how can the stride be derived...? Add at least some more consistency checks
	auto sb = CBitmapData<CSharedPtrAllocator>::Create(
		sharedPtrAllocator,
		subBlk->GetSubBlockInfo().pixelType,
		subBlk->GetSubBlockInfo().physicalSize.w,
		subBlk->GetSubBlockInfo().physicalSize.h,
		subBlk->GetSubBlockInfo().physicalSize.w * CziUtils::GetBytesPerPel(subBlk->GetSubBlockInfo().pixelType));

	return sb;
}

std::shared_ptr<libCZI::IBitmapData> libCZI::CreateBitmapFromSubBlock(ISubBlock* subBlk)
{
	auto sbInfo = subBlk->GetSubBlockInfo();
	switch (sbInfo.mode)
	{
	case CompressionMode::JpgXr:
		return CreateBitmapFromSubBlock_JpgXr(subBlk);
	case CompressionMode::UnCompressed:
		return CreateBitmapFromSubBlock_Uncompressed(subBlk);
	default:	// silence warnings
		throw std::logic_error("The method or operation is not implemented.");
	}
}
