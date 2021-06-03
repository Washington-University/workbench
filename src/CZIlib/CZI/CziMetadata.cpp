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
#include "CziMetadata.h"
#include "CziMetadataDocumentInfo.h"

using namespace pugi;
using namespace libCZI;


CCziMetadata::CCziMetadata(libCZI::IMetadataSegment* pMdSeg) 
{

	const void* ptrData; size_t size;
	pMdSeg->DangerousGetRawData(IMetadataSegment::MemBlkType::XmlMetadata, ptrData, size);
	xml_parse_result r = this->doc.load_buffer(ptrData, size, pugi::parse_default, encoding_utf8);
}

/*virtual*/std::string CCziMetadata::GetXml()
{
	static pugi::char_t Indent[] = PUGIXML_TEXT(" ");

	std::ostringstream stream;
	xml_writer_stream writer(stream);
	this->doc.save(writer, Indent, format_default, encoding_utf8);
	stream.flush();
	return stream.str();
}

const pugi::xml_document& CCziMetadata::GetXmlDoc() const
{
	return this->doc;
}

/*virtual*/std::shared_ptr<ICziMultiDimensionDocumentInfo> CCziMetadata::GetDocumentInfo()
{
	return std::make_shared<CCziMetadataDocumentInfo>(this->shared_from_this());
}

