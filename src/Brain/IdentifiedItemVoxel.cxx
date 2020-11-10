
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

#define __IDENTIFIED_ITEM_VOXEL_DECLARE__
#include "IdentifiedItemVoxel.h"
#undef __IDENTIFIED_ITEM_VOXEL_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SessionManager.h"

using namespace caret;


    
/**
 * \class caret::IdentifiedItemVoxel 
 * \brief Identified voxel.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
IdentifiedItemVoxel::IdentifiedItemVoxel()
: IdentifiedItem()
{
    initializeMembers();
}
    
/**
 * Constructor.
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 * @param xyz
 *    XYZ of voxel
 */
IdentifiedItemVoxel::IdentifiedItemVoxel(const AString& simpleText,
                                         const AString& formattedText,
                                         const float xyz[3])
: IdentifiedItem(simpleText,
                 formattedText)
{
    initializeMembers();
    
    m_xyz[0] = xyz[0];
    m_xyz[1] = xyz[1];
    m_xyz[2] = xyz[2];
}

/**
 * Destructor.
 */
IdentifiedItemVoxel::~IdentifiedItemVoxel()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
IdentifiedItemVoxel::IdentifiedItemVoxel(const IdentifiedItemVoxel& obj)
: IdentifiedItem(obj)
{
    initializeMembers();
    this->copyHelperIdentifiedItemVoxel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
IdentifiedItemVoxel&
IdentifiedItemVoxel::operator=(const IdentifiedItemVoxel& obj)
{
    if (this != &obj) {
        IdentifiedItem::operator=(obj);
        this->copyHelperIdentifiedItemVoxel(obj);
    }
    return *this;    
}

/**
 * Initialize members of this class.
 */
void
IdentifiedItemVoxel::initializeMembers()
{
    m_xyz[0]       = 0.0;
    m_xyz[1]       = 0.0;
    m_xyz[2]       = 0.0;
    m_symbolRGB[0] = 0;
    m_symbolRGB[1] = 0;
    m_symbolRGB[1] = 0;
    m_symbolSize   = 0.0;
    m_identificationSymbolSizeType = IdentificationSymbolSizeTypeEnum::MILLIMETERS;

    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->addArray("m_xyz", m_xyz, 3, 0.0);
    m_sceneAssistant->addArray("m_symbolRGB", m_symbolRGB, 3, 0);
    m_sceneAssistant->add("m_symbolSize", &m_symbolSize);
    m_sceneAssistant->add<IdentificationSymbolSizeTypeEnum, IdentificationSymbolSizeTypeEnum::Enum>("m_identificationSymbolSizeType",
                                                                                                    &m_identificationSymbolSizeType);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
IdentifiedItemVoxel::copyHelperIdentifiedItemVoxel(const IdentifiedItemVoxel& obj)
{
    m_xyz[0] = obj.m_xyz[0];
    m_xyz[1] = obj.m_xyz[1];
    m_xyz[2] = obj.m_xyz[2];
    
    m_symbolRGB[0] = obj.m_symbolRGB[0];
    m_symbolRGB[1] = obj.m_symbolRGB[1];
    m_symbolRGB[2] = obj.m_symbolRGB[2];
    
    m_symbolSize = obj.m_symbolSize;
    m_identificationSymbolSizeType = obj.m_identificationSymbolSizeType;
}

/**
 * Get the coordinates of the identified voxel.
 * 
 * @param xyzOut
 *    Output with coordinates of voxel.
 */
void
IdentifiedItemVoxel::getXYZ(float xyzOut[3]) const
{
    xyzOut[0] = m_xyz[0];
    xyzOut[1] = m_xyz[1];
    xyzOut[2] = m_xyz[2];
}

/**
 * @return Is this item valid?  Typically only used when restoring
 * from scene.  True if the symbol size is greater than zero,
 * else false.
 */
bool
IdentifiedItemVoxel::isValid() const
{
    if (m_symbolSize > 0.0) {
        return true;
    }
    
    return true;
}


/**
 * @return The color for the symbol's identification symbol.
 */
const float*
IdentifiedItemVoxel::getSymbolRGB() const
{
    return m_symbolRGB;
}

/**
 * Get color for the identification symbol.
 *
 * @param rgbaOut
 *    RGBA ranging 0 to 255.
 */
void
IdentifiedItemVoxel::getSymbolRGBA(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_symbolRGB[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_symbolRGB[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_symbolRGB[2] * 255.0);
    rgbaOut[3] = 255;
}

/**
 * @return  The size of the symbol.
 */
float
IdentifiedItemVoxel::getSymbolSize() const
{
    return m_symbolSize;
}

/**
 * Set the color for the identification symbol.
 *
 * @param rgb
 *   Red, green, blue color components for identification system.
 */
void
IdentifiedItemVoxel::setSymbolRGB(const float* rgb)
{
    m_symbolRGB[0] = rgb[0];
    m_symbolRGB[1] = rgb[1];
    m_symbolRGB[2] = rgb[2];
}

/**
 * Set the size of the symbol.
 *
 * @param symbolSize
 *    Size of the symbol.
 */
void
IdentifiedItemVoxel::setSymbolSize(const float symbolSize)
{
    m_symbolSize = symbolSize;
}

/**
 * @param The identification symbol size type
 */
IdentificationSymbolSizeTypeEnum::Enum
IdentifiedItemVoxel::getIdentificationSymbolSizeType() const
{
    return m_identificationSymbolSizeType;
}

/**
 * Set the identification size type
 * @param sizeType
 *    The new size type
 */
void
IdentifiedItemVoxel::setIdentificationSymbolSizeType(const IdentificationSymbolSizeTypeEnum::Enum sizeType)
{
    m_identificationSymbolSizeType = sizeType;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
IdentifiedItemVoxel::toString() const
{
    const AString s = (IdentifiedItem::toString()
                       + ", m_xyz=" + AString::fromNumbers(m_xyz, 3, ", "));
    return s;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
IdentifiedItemVoxel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentifiedItemVoxel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    /*
     * Save data in parent class.
     */
    saveMembers(sceneAttributes,
                sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
IdentifiedItemVoxel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_identificationSymbolSizeType = IdentificationSymbolSizeTypeEnum::MILLIMETERS;
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    /*
     * Restores data in parent class.
     */
    restoreMembers(sceneAttributes,
                   sceneClass);
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

