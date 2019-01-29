
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

#define __SCENE_INFO_XML_STREAM_READER_DECLARE__
#include "SceneInfoXmlStreamReader.h"
#undef __SCENE_INFO_XML_STREAM_READER_DECLARE__

#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

#include "CaretAssert.h"
#include "SceneInfo.h"

using namespace caret;


    
/**
 * \class caret::SceneInfoXmlStreamReader 
 * \brief XML Stream Reader for SceneInfo
 * \ingroup Scenes
 */

/**
 * Constructor.
 */
SceneInfoXmlStreamReader::SceneInfoXmlStreamReader()
: SceneInfoXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
SceneInfoXmlStreamReader::~SceneInfoXmlStreamReader()
{
}

/**
 * Read the scene info.
 * If, after calling this method, xmlReader.hasError() return true,
 * there was an error reading the SceneInfo.
 *
 * @param xmlReader
 *     The XML stream reader
 * @param sceneInfo
 *     Read into this sceneInfo
 */
void
SceneInfoXmlStreamReader::readSceneInfo(QXmlStreamReader& xmlReader,
                                        SceneInfo* sceneInfo)
{
    CaretAssert(sceneInfo);
    if (sceneInfo == NULL) {
        return;
    }
    
    if (xmlReader.name() != ELEMENT_SCENE_INFO) {
        xmlReader.raiseError("First element is \""
                             + xmlReader.name()
                             + "\" but should be "
                             + ELEMENT_SCENE_INFO);
        return;
    }
    
    /*
     * Gets set when ending scene info directory element is read
     */
    bool endElementFound(false);

    while ( (! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == ELEMENT_NAME) {
                    sceneInfo->setName(xmlReader.readElementText());
                }
                else if (xmlReader.name() == ELEMENT_BALSA_SCENE_ID) {
                    sceneInfo->setBalsaSceneID(xmlReader.readElementText());
                }
                else if (xmlReader.name() == ELEMENT_DESCRIPTION) {
                    sceneInfo->setDescription(xmlReader.readElementText());
                }
                else if (xmlReader.name() == ELEMENT_IMAGE) {
                    const QXmlStreamAttributes atts = xmlReader.attributes();
                    const QString encodingName = atts.value(ATTRIBUTE_IMAGE_ENCODING).toString();
                    const QString formatName   = atts.value(ATTRIBUTE_IMAGE_FORMAT).toString();
                    sceneInfo->setImageFromText(xmlReader.readElementText(),
                                                encodingName,
                                                formatName);
                }
                else {
                    m_unexpectedXmlElements.insert(xmlReader.name().toString());
                    xmlReader.skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == ELEMENT_SCENE_INFO) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
    }
}

/**
 * @return Any unexpected elements that were found,
 * and ignored, while reading the SceneInfo XML.
 */
std::set<AString>
SceneInfoXmlStreamReader::getUnexpectedElements() const
{
    return m_unexpectedXmlElements;
}
