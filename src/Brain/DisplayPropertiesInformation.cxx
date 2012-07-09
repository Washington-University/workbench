
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __DISPLAY_PROPERTIES_INFORMATION_DECLARE__
#include "DisplayPropertiesInformation.h"
#undef __DISPLAY_PROPERTIES_INFORMATION_DECLARE__

#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class DisplayPropertiesInformation 
 * \brief Display properties for information.
 */

/**
 * Constructor.
 */
DisplayPropertiesInformation::DisplayPropertiesInformation(Brain* brain)
: DisplayProperties(brain)
{
    m_contralateralIdentificationEnabled = false;
    m_volumeIdentificationEnabled = true;
    m_identificationSymbolColor = CaretColorEnum::GREEN;
    m_identificationContralateralSymbolColor = CaretColorEnum::BLUE;
    m_identifcationSymbolSize = 3.5;
    
    m_sceneAssistant->add("m_contralateralIdentificationEnabled",
                              &m_contralateralIdentificationEnabled);
    
    m_sceneAssistant->add("m_volumeIdentificationEnabled",
                              &m_volumeIdentificationEnabled);
    
    m_sceneAssistant->add("m_identifcationSymbolSize",
                              &m_identifcationSymbolSize);
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_identificationSymbolColor",
                                                                &m_identificationSymbolColor,
                                                                m_identificationSymbolColor);
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_identificationContralateralSymbolColor",
                                                                &m_identificationContralateralSymbolColor,
                                                                m_identificationContralateralSymbolColor);
}

/**
 * Destructor.
 */
DisplayPropertiesInformation::~DisplayPropertiesInformation()
{
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesInformation::reset()
{
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesInformation::update()
{
}

/**
 * @return Status of contralateral identification.
 */
bool 
DisplayPropertiesInformation::isContralateralIdentificationEnabled() const
{
    return m_contralateralIdentificationEnabled;
}
        
/**
 * Set status of contralateral identification.
 * @param 
 *    New status.
 */
void 
DisplayPropertiesInformation::setContralateralIdentificationEnabled(const bool enabled)
{
    m_contralateralIdentificationEnabled = enabled;
}

/**
 * @return Status of volume identification.
 */
bool 
DisplayPropertiesInformation::isVolumeIdentificationEnabled() const
{
    return m_volumeIdentificationEnabled;
}

/**
 * Set status of volume identification.
 * @param 
 *    New status.
 */
void 
DisplayPropertiesInformation::setVolumeIdentificationEnabled(const bool enabled)
{
    m_volumeIdentificationEnabled = enabled;
}

/**
 * @return The size of the identification symbol
 */
float 
DisplayPropertiesInformation::getIdentificationSymbolSize() const
{
    return m_identifcationSymbolSize;
}

/**
 * Set the size of the identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void 
DisplayPropertiesInformation::setIdentificationSymbolSize(const float symbolSize)
{
    m_identifcationSymbolSize = symbolSize;
}

/**
 * @return The color of the identification symbol.
 */
CaretColorEnum::Enum 
DisplayPropertiesInformation::getIdentificationSymbolColor() const
{
    return m_identificationSymbolColor;
}

/**
 * Set the color of the identification symbol.
 * @param color
 *    New color.
 */
void 
DisplayPropertiesInformation::setIdentificationSymbolColor(const CaretColorEnum::Enum color)
{
    m_identificationSymbolColor = color;
}

/**
 * @return The color of the contralateral identification symbol.
 */
CaretColorEnum::Enum 
DisplayPropertiesInformation::getIdentificationContralateralSymbolColor() const
{
    return m_identificationContralateralSymbolColor;
}

/**
 * Set the color of the contralateral identification symbol.
 * @param color
 *    New color.
 */
void 
DisplayPropertiesInformation::setIdentificationContralateralSymbolColor(const CaretColorEnum::Enum color)
{
    m_identificationContralateralSymbolColor = color;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
DisplayPropertiesInformation::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesInformation",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                      sceneClass);
    
//    sceneClass->addEnumeratedType<CaretColorEnum, CaretColorEnum::Enum>("m_identificationSymbolColor", 
//                                  m_identificationSymbolColor);
//    sceneClass->addEnumeratedType<CaretColorEnum, CaretColorEnum::Enum>("m_identificationContralateralSymbolColor", 
//                                  m_identificationContralateralSymbolColor);
//    sceneClass->addEnumeratedType("m_identificationSymbolColor", 
//                                  CaretColorEnum::toName(m_identificationSymbolColor));
//    sceneClass->addEnumeratedType("m_identificationContralateralSymbolColor", 
//                                  CaretColorEnum::toName(m_identificationContralateralSymbolColor));
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
DisplayPropertiesInformation::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                         sceneClass);
    
//    m_identificationSymbolColor = sceneClass->getEnumeratedTypeValue<CaretColorEnum, CaretColorEnum::Enum>("m_identificationSymbolColor",
//                                                                                                           CaretColorEnum::GREEN);
//    m_identificationContralateralSymbolColor = sceneClass->getEnumeratedTypeValue<CaretColorEnum, CaretColorEnum::Enum>("m_identificationContralateralSymbolColor",
//                                                                                                                        CaretColorEnum::BLUE);
//    m_identificationSymbolColor = CaretColorEnum::fromName(sceneClass->getEnumeratedTypeValue("m_identificationSymbolColor",
//                                                                                              CaretColorEnum::toName(CaretColorEnum::GREEN)),
//                                                           NULL);
//    m_identificationContralateralSymbolColor = CaretColorEnum::fromName(sceneClass->getEnumeratedTypeValue("m_identificationContralateralSymbolColor",
//                                                                                                           CaretColorEnum::toName(CaretColorEnum::BLUE)),
//                                                                        NULL);
    switch (sceneAttributes->getSceneType()) {
            
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
}

/**
 * Copy the display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void 
DisplayPropertiesInformation::copyDisplayProperties(const int32_t /*sourceTabIndex*/,
                                                    const int32_t /*targetTabIndex*/)
{
}

