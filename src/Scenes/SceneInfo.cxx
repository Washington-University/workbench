
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
#include "SceneInfoXmlStreamBase.h"
#include "XmlAttributes.h"
#include "XmlUtilities.h"
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
: CaretObjectTracksModification()
{
}

SceneInfo::SceneInfo(const SceneInfo& rhs) : CaretObjectTracksModification()
{
    m_sceneName = rhs.m_sceneName;
    m_sceneDescription = rhs.m_sceneDescription;
    m_balsaSceneID = rhs.m_balsaSceneID;
    m_imageFormat = rhs.m_imageFormat;
    m_imageBytes = rhs.m_imageBytes;
    m_workbenchInfo = rhs.m_workbenchInfo;
}

/**
 * Destructor.
 */
SceneInfo::~SceneInfo()
{
}

/**
 * @return True if this scene info is modified
 */
bool
SceneInfo::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    
    return false;
}

/**
 * @return Is this instance modified?
 */
void
SceneInfo::clearModified()
{
    CaretObjectTracksModification::clearModified();
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
    if (sceneName != m_sceneName) {
        m_sceneName = sceneName;
        setModified();
    }
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
    if (sceneDescription != m_sceneDescription) {
        m_sceneDescription = sceneDescription;
        setModified();
    }
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
    if ((imageBytes != m_imageBytes)
        || (imageFormat != m_imageFormat)) {
        m_imageBytes  = imageBytes;
        m_imageFormat = imageFormat;
    }
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
    imageBytesOut  = m_imageBytes;
    imageFormatOut = m_imageFormat;
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
 * @return Info about Workbench info that created this scene
 */
AString
SceneInfo::getWorkbenchInfo() const
{
    return m_workbenchInfo;
}

void
SceneInfo::setWorkbenchInfo(const AString& workbenchInfo)
{
    if (workbenchInfo != m_workbenchInfo) {
        m_workbenchInfo = workbenchInfo;
        setModified();
    }
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
    if (balsaSceneID != m_balsaSceneID) {
        m_balsaSceneID = balsaSceneID;
        setModified();
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
        if (encoding == SceneInfoXmlStreamBase::VALUE_ENCODING_BASE64) {
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





