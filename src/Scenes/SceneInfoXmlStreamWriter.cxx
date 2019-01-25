
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __SCENE_INFO_XML_STREAM_WRITER_DECLARE__
#include "SceneInfoXmlStreamWriter.h"
#undef __SCENE_INFO_XML_STREAM_WRITER_DECLARE__

#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "SceneInfo.h"
#include "WuQMacroGroupXmlStreamWriter.h"

using namespace caret;
    
/**
 * \class caret::SceneInfoXmlStreamWriter 
 * \brief XML stream writer for SceneInfo
 * \ingroup Scenes
 */

/**
 * Constructor.
 */
SceneInfoXmlStreamWriter::SceneInfoXmlStreamWriter()
: SceneInfoXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
SceneInfoXmlStreamWriter::~SceneInfoXmlStreamWriter()
{
}

/**
 * Write the given SceneInfo to the given xml stream writer
 *
 * @param xmlWriter
 *     The XML stream writer
 * @param sceneInfo
 *     The scene info
 * @param sceneInfoIndex
 *     Index of scene associated with the sceneInfo
 */
void
SceneInfoXmlStreamWriter::writeXML(QXmlStreamWriter* xmlWriter,
                                   const SceneInfo* sceneInfo,
                                   const int32_t sceneInfoIndex)
{
    CaretAssert(xmlWriter);
    CaretAssert(sceneInfo);
    CaretAssert(sceneInfoIndex >= 0);
    
    m_xmlWriter = xmlWriter;
    
    m_xmlWriter->writeStartElement(ELEMENT_SCENE_INFO);
    m_xmlWriter->writeAttribute(ATTRIBUTE_SCENE_INDEX,
                                QString::number(sceneInfoIndex));
    
    m_xmlWriter->writeTextElement(ELEMENT_NAME,
                                  sceneInfo->getName());
    
    m_xmlWriter->writeTextElement(ELEMENT_BALSA_SCENE_ID,
                                  sceneInfo->getBalsaSceneID());
    
    m_xmlWriter->writeTextElement(ELEMENT_DESCRIPTION,
                                  sceneInfo->getDescription());
    
    WuQMacroGroupXmlStreamWriter macroGroupXmlWriter;
    macroGroupXmlWriter.writeXml(xmlWriter,
                                 sceneInfo->getMacroGroup());
    
    writeImageElement(sceneInfo);
    
    m_xmlWriter->writeEndElement();
    
    m_xmlWriter = NULL;
}

/**
 * Write the image element
 *
 * @param sceneInfo
 *     The scene information
 */
void
SceneInfoXmlStreamWriter::writeImageElement(const SceneInfo* sceneInfo)
{
    QByteArray imageBytes;
    AString    imageFormat;
    sceneInfo->getImageBytes(imageBytes,
                             imageFormat);
    if (! imageBytes.isEmpty()) {
        const QByteArray base64ByteArray(imageBytes.toBase64());
        QString base64String = QString::fromLatin1(base64ByteArray.constData(),
                                                   base64ByteArray.size());
        m_xmlWriter->writeStartElement(ELEMENT_IMAGE);
        m_xmlWriter->writeAttribute(ATTRIBUTE_IMAGE_ENCODING, VALUE_ENCODING_BASE64);
        m_xmlWriter->writeAttribute(ATTRIBUTE_IMAGE_FORMAT, imageFormat);
        m_xmlWriter->writeCharacters(base64String);
        m_xmlWriter->writeEndElement();
    }
}

