
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __SCENE_INFO_DECLARE__
#include "SceneInfo.h"
#undef __SCENE_INFO_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneXmlElements.h"
#include "XmlAttributes.h"
#include "XmlWriter.h"
using namespace caret;


    
/**
 * \class caret::SceneInfo 
 * \brief Contains information about a scene.
 * \ingroup Scenes
 */

/**
 * Constructor.
 */
SceneInfo::SceneInfo()
: CaretObject()
{
    
}

SceneInfo::SceneInfo(const SceneInfo& rhs) : CaretObject()
{
    m_sceneName = rhs.m_sceneName;
    m_sceneDescription = rhs.m_sceneDescription;
    m_balsaSceneID = rhs.m_balsaSceneID;
    m_imageFormat = rhs.m_imageFormat;
    m_imageBytes = rhs.m_imageBytes;
}

/**
 * Destructor.
 */
SceneInfo::~SceneInfo()
{
}

/**
 * @return name of scene
 */
AString
SceneInfo::getName() const
{
    return m_sceneName;
}

/**
 * Set name of scene
 * @param sceneName
 *    New value for name of scene
 */
void
SceneInfo::setName(const AString& sceneName)
{
    m_sceneName = sceneName;
}

/**
 * @return description of scene
 */
AString
SceneInfo::getDescription() const
{
    return m_sceneDescription;
}

/**
 * Set description of scene
 * @param sceneDescription
 *    New value for description of scene
 */
void
SceneInfo::setDescription(const AString& sceneDescription)
{
    m_sceneDescription = sceneDescription;
}

/**
 * Set bytes containing the thumbnail image.  
 *
 * @param imageBytes
 *    Byte array containing the image file.
 * @param imageFormat 
 *    Format of the image (jpg, ppm, etc.).
 */
void
SceneInfo::setImageBytes(const QByteArray& imageBytes,
                                  const AString& imageFormat)
{
    m_imageBytes  = imageBytes;
    m_imageFormat = imageFormat;
}

/**
 * Get bytes containing the thumbnail image.
 *
 * @param imageBytesOut
 *    Byte array containing the image file.
 * @param imageFormatOut
 *    Format of the image (jpg, ppm, etc.).
 */
void
SceneInfo::getImageBytes(QByteArray& imageBytesOut,
                                  AString& imageFormatOut) const
{
    imageBytesOut = m_imageBytes;
    imageFormatOut         = m_imageFormat;
}

/**
 * @return true if the scene contains an image, else false.
 *
 * The image is considered valid if the image bytes are not empty and,
 * if not empty, it is assumed that the image is valid.
 */
bool
SceneInfo::hasImage() const
{
    if (m_imageBytes.isEmpty()) {
        return false;
    }
    
    return true;
}

/**
 * @return The BALSA Scene ID.
 */
AString
SceneInfo::getBalsaSceneID() const
{
    return m_balsaSceneID;
}

/**
 * Set the BALSA Scene ID.
 *
 * @param balsaSceneID
 *     The BALSA scene ID.
 */
void
SceneInfo::setBalsaSceneID(const AString& balsaSceneID)
{
    m_balsaSceneID = balsaSceneID;
}

/**
 * Write the scene info element.
 *
 * @param sceneInfo
 *     The scene info element that is written.
 * @param sceneInfoIndex
 *     The index for the scene info.
 */
void
SceneInfo::writeSceneInfo(XmlWriter& xmlWriter,
                          const int32_t sceneInfoIndex) const
{
    XmlAttributes attributes;
    attributes.addAttribute(SceneXmlElements::SCENE_INFO_INDEX_ATTRIBUTE,
                            sceneInfoIndex);
    
    xmlWriter.writeStartElement(SceneXmlElements::SCENE_INFO_TAG,
                                  attributes);
    
    xmlWriter.writeElementCData(SceneXmlElements::SCENE_INFO_NAME_TAG,
                                     m_sceneName);
    
    xmlWriter.writeElementCData(SceneXmlElements::SCENE_INFO_BALSA_SCENE_ID_TAG,
                                m_balsaSceneID);
    
    xmlWriter.writeElementCData(SceneXmlElements::SCENE_INFO_DESCRIPTION_TAG,
                                       m_sceneDescription);
    
    writeSceneInfoImage(xmlWriter,
                        SceneXmlElements::SCENE_INFO_IMAGE_TAG,
                        m_imageBytes,
                        m_imageFormat);
    
    /*
     * End class element.
     */
    xmlWriter.writeEndElement();
}

/**
 * Write an image to the scene info.
 *
 * @param xmlWriter
 *    The XML writer.
 * @param xmlTag
 *    Tag for the image.
 * @param imageBytes
 *    Bytes containing the image.
 * @param imageFormat
 *    Format of the image.
 *
 */
void
SceneInfo::writeSceneInfoImage(XmlWriter& xmlWriter,
                               const AString& xmlTag,
                                    const QByteArray& imageBytes,
                                    const AString& imageFormat) const
{
    if (imageBytes.length() > 0) {
        //QString base64String(imageBytes.toBase64());
        const QByteArray base64ByteArray(imageBytes.toBase64());
        QString base64String = QString::fromAscii(base64ByteArray.constData(),
                                                  base64ByteArray.size());
        XmlAttributes attributes;
        attributes.addAttribute(SceneXmlElements::SCENE_INFO_IMAGE_ENCODING_ATTRIBUTE,
                                SceneXmlElements::SCENE_INFO_ENCODING_BASE64_NAME);
        attributes.addAttribute(SceneXmlElements::SCENE_INFO_IMAGE_FORMAT_ATTRIBUTE,
                                imageFormat);
        
        xmlWriter.writeStartElement(xmlTag,
                                      attributes);
        
        xmlWriter.writeCharacters(base64String);
        
        xmlWriter.writeEndElement();
    }
}

/**
 * Set an image form text.
 *
 * @param text
 *     Text containing the image.
 * @param encoding
 *     Encoding of the image data.
 * @param imageFormat
 *     Format of the image.
 */
void
SceneInfo::setImageFromText(const AString& text,
                               const AString& encoding,
                               const AString& imageFormat)
{
    m_imageBytes.clear();
    m_imageFormat = "";
    
    if ( ! text.isEmpty()) {
        if (encoding == SceneXmlElements::SCENE_INFO_ENCODING_BASE64_NAME) {
            m_imageBytes = QByteArray::fromBase64(text.toLatin1());
            m_imageFormat = imageFormat;
        }
        else {
            CaretLogSevere("Invalid encoding ("
                           + encoding
                           + ") for scene thumbnail image.");
        }
    }
}





