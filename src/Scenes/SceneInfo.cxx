
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
 * @param imageThumbnailBytes
 *    Byte array containing the image file.
 * @param imageFormat 
 *    Format of the image (jpg, ppm, etc.).
 */
void
SceneInfo::setImageThumbnailBytes(const QByteArray& imageThumbnailBytes,
                                  const AString& imageFormat)
{
    m_imageThumbnailBytes  = imageThumbnailBytes;
    m_imageThumbnailFormat = imageFormat;
}

/**
 * Get bytes containing the thumbnail image.
 *
 * @param imageThumbnailBytesOut
 *    Byte array containing the image file.
 * @param imageFormatOut
 *    Format of the image (jpg, ppm, etc.).
 */
void
SceneInfo::getImageThumbnailBytes(QByteArray& imageThumbnailBytesOut,
                                  AString& imageFormatOut) const
{
    imageThumbnailBytesOut = m_imageThumbnailBytes;
    imageFormatOut         = m_imageThumbnailFormat;
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
    
    xmlWriter.writeElementCharacters(SceneXmlElements::SCENE_INFO_NAME_TAG,
                                     m_sceneName);
    
    xmlWriter.writeElementCharacters(SceneXmlElements::SCENE_INFO_DESCRIPTION_TAG,
                                       m_sceneDescription);
    
    writeSceneInfoImage(xmlWriter,
                        SceneXmlElements::SCENE_INFO_IMAGE_THUMBNAIL_TAG,
                        m_imageThumbnailBytes,
                        m_imageThumbnailFormat);
    
    
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
        QString base64String(imageBytes.toBase64());
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
 * 
 */
void
SceneInfo::setImageThumbnailFromText(const AString& text,
                               const AString& encoding,
                               const AString& imageFormat)
{
    m_imageThumbnailBytes.clear();
    m_imageThumbnailFormat = "";
    
    if ( ! text.isEmpty()) {
        if (encoding == SceneXmlElements::SCENE_INFO_ENCODING_BASE64_NAME) {
            m_imageThumbnailBytes = QByteArray::fromBase64(text.toAscii());
            m_imageThumbnailFormat = imageFormat;
        }
        else {
            CaretLogSevere("Invalid encoding ("
                           + encoding
                           + ") for scene thumbnail image.");
        }
    }
}





