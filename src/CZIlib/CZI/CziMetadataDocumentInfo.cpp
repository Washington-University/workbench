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
#include "CziMetadataDocumentInfo.h"
#include "CziDimensionInfo.h"
#include "CziDisplaySettings.h"
#include "utilities.h"

using namespace libCZI;
using namespace std;

CCziMetadataDocumentInfo::CCziMetadataDocumentInfo(std::shared_ptr<CCziMetadata> md)
	: metadata(md)
{
	this->ParseDimensionInfo();
}

/*virtual*/libCZI::GeneralDocumentInfo CCziMetadataDocumentInfo::GetGeneralDocumentInfo() const
{
	GeneralDocumentInfo generalDocumentInfo;
	auto np = this->GetNode(L"Metadata/Information/Document");
	if (!np.empty())
	{
		auto n = GetNodeRelativeFromNode(np, L"UserName");
		if (!n.empty())
		{
			generalDocumentInfo.userName = n.text().as_string();
		}

		n = GetNodeRelativeFromNode(np, L"Title");
		if (!n.empty())
		{
			generalDocumentInfo.title = n.text().as_string();
		}

		n = GetNodeRelativeFromNode(np, L"Description");
		if (!n.empty())
		{
			generalDocumentInfo.description = n.text().as_string();
		}

		n = GetNodeRelativeFromNode(np, L"Comment");
		if (!n.empty())
		{
			generalDocumentInfo.comment = n.text().as_string();
		}

		n = GetNodeRelativeFromNode(np, L"Rating");
		if (!n.empty())
		{
			generalDocumentInfo.rating = n.text().as_int();
		}

		n = GetNodeRelativeFromNode(np, L"CreationDate");
		if (!n.empty())
		{
			generalDocumentInfo.creationDateTime = n.text().as_string();
		}

		n = GetNodeRelativeFromNode(np, L"Keywords");
		if (!n.empty())
		{
			generalDocumentInfo.keywords = n.text().as_string();
		}

		n = GetNodeRelativeFromNode(np, L"Name");
		if (!n.empty())
		{
			generalDocumentInfo.name = n.text().as_string();
		}
	}

	return generalDocumentInfo;
}

/*virtual*/libCZI::ScalingInfo CCziMetadataDocumentInfo::GetScalingInfo() const
{
	ScalingInfo scalingInfo;
	auto np = this->GetNode(L"Metadata/Scaling");
	if (np.empty())
	{
		return scalingInfo;
	}

	auto nodeScalingValue = np.select_single_node(L"Items/Distance[@Id='X']/Value");
	if (!nodeScalingValue.node().empty())
	{
		scalingInfo.scaleX = nodeScalingValue.node().text().as_double();
	}

	nodeScalingValue = np.select_single_node(L"Items/Distance[@Id='Y']/Value");
	if (!nodeScalingValue.node().empty())
	{
		scalingInfo.scaleY = nodeScalingValue.node().text().as_double();
	}

	nodeScalingValue = np.select_single_node(L"Items/Distance[@Id='Z']/Value");
	if (!nodeScalingValue.node().empty())
	{
		scalingInfo.scaleZ = nodeScalingValue.node().text().as_double();
	}

	return scalingInfo;
}

/*virtual*/void CCziMetadataDocumentInfo::EnumDimensions(std::function<bool(libCZI::DimensionIndex)> enumDimensions)
{
	for (const auto& it : this->dimensions)
	{
		if (enumDimensions(it.first) == false)
		{
			break;
		}
	}
}

/*virtual*/std::shared_ptr<libCZI::IDimensionInfo> CCziMetadataDocumentInfo::GetDimensionInfo(libCZI::DimensionIndex dim)
{
	const auto it = this->dimensions.find(dim);
	if (it == this->dimensions.cend())
	{
		// TODO: or should we throw an exception?
		return std::shared_ptr<libCZI::IDimensionInfo>();
	}

	switch (dim)
	{
	case DimensionIndex::C:
	case DimensionIndex::T:
	case DimensionIndex::Z:
	case DimensionIndex::B:

		// TODO: not sure about these... should we report them here...?
	case DimensionIndex::R:
	case DimensionIndex::S:
	case DimensionIndex::I:
	case DimensionIndex::H:
	case DimensionIndex::V:
		return std::make_shared<CCziDimensionInfo>(this->metadata, it->second);
	default:
		throw std::logic_error("unknwon dimension, not implemented");
	}
}

/*virtual*/std::shared_ptr<libCZI::IDisplaySettings> CCziMetadataDocumentInfo::GetDisplaySettings() const
{
	auto dsplSetting = this->GetNode(L"Metadata/DisplaySetting");
	if (dsplSetting.empty())
	{
		// TODO - exception or empty object?
		return std::shared_ptr<libCZI::IDisplaySettings>();
	}

	return CCziDisplaySettings::CreateFromXml(dsplSetting);
}

void CCziMetadataDocumentInfo::ParseDimensionInfo()
{
	static const struct
	{
		DimensionIndex index;
		const wchar_t* start, *size;
	} DimAndNodeNames[] =
	{
		{ DimensionIndex::Z,L"StartZ",L"SizeZ"},
		{ DimensionIndex::C,L"StartC",L"SizeC" },
		{ DimensionIndex::T,L"StartT",L"SizeT" },
		{ DimensionIndex::R,L"StartR",L"SizeR" },
		{ DimensionIndex::S,L"StartS",L"SizeS" },
		{ DimensionIndex::I,L"StartI",L"SizeI" },
		{ DimensionIndex::H,L"StartH",L"SizeH" },
		{ DimensionIndex::V,L"StartV",L"SizeV" },
		{ DimensionIndex::B,L"StartB",L"SizeB" }
	};

	auto np = this->GetNode(L"Metadata/Information/Image");
	if (!np.empty())
	{
		for (int i = 0; i < sizeof(DimAndNodeNames) / sizeof(DimAndNodeNames[0]); ++i)
		{
			auto nStart = GetNodeRelativeFromNode(np, DimAndNodeNames[i].start);
			auto nSize = GetNodeRelativeFromNode(np, DimAndNodeNames[i].size);
			if (!nStart.empty() || !nSize.empty())
			{
				int start = 0, size = 1;
				if (!nStart.empty())
				{
					start = nStart.text().as_int();
				}

				if (!nSize.empty())
				{
					size = nSize.text().as_int();
				}

				this->dimensions[DimAndNodeNames[i].index] = DimensionAndStartSize{ DimAndNodeNames[i].index ,start,size };
			}
		}
	}
}

pugi::xml_node CCziMetadataDocumentInfo::GetNode(const wchar_t* path) const
{
	return CCziMetadataDocumentInfo::GetNodeRelativeFromNode(this->metadata->GetXmlDoc().child(L"ImageDocument"), path);
}

/*static*/pugi::xml_node CCziMetadataDocumentInfo::GetNodeRelativeFromNode(pugi::xml_node node, const wchar_t* path)
{
	Utilities::Split(wstring(path), L'/',
		[&](const std::wstring str)->bool
	{
		node = node.child(str.c_str());
		return !node.empty();
	});

	return node;
}