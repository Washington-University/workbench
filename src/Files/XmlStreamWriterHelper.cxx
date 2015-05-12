
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __XML_STREAM_WRITER_HELPER_DECLARE__
#include "XmlStreamWriterHelper.h"
#undef __XML_STREAM_WRITER_HELPER_DECLARE__

#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "GiftiMetaData.h"
#include "GiftiXmlElements.h"

using namespace caret;


    
/**
 * \class caret::XmlStreamWriterHelper 
 * \brief Helper class for using QXmlStreamWriter
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param filename
 *    Name of file that is being read using the QXmlStreamWriter
 * @param stream
 *    The QXmlStreamWriter
 */
XmlStreamWriterHelper::XmlStreamWriterHelper(const QString& filename,
                                             QXmlStreamWriter* stream)
: CaretObject(),
m_filename(filename),
m_stream(stream)
{
    CaretAssert(stream);
}


/**
 * Destructor.
 */
XmlStreamWriterHelper::~XmlStreamWriterHelper()
{
}

/**
 * Write the given metadata using the XML stream writer.
 *
 * @param metadata
 *     Metadata that will be written to the XML stream.
 */
void
XmlStreamWriterHelper::writeMetaData(const GiftiMetaData* metadata)
{
    CaretAssert(metadata);
    
    m_stream->writeStartElement(GiftiXmlElements::TAG_METADATA);
    
    const std::map<AString, AString> mdMap = metadata->getAsMap();
    for (std::map<AString, AString>::const_iterator iter = mdMap.begin();
         iter != mdMap.end();
         iter++) {
        m_stream->writeStartElement(GiftiXmlElements::TAG_METADATA_ENTRY);
        m_stream->writeTextElement(GiftiXmlElements::TAG_METADATA_NAME, iter->first);
        m_stream->writeTextElement(GiftiXmlElements::TAG_METADATA_VALUE, iter->second);
        m_stream->writeEndElement();
    }

    m_stream->writeEndElement();
}

