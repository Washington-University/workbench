
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __LABEL_DRAWING_PROPERTIES_DECLARE__
#include "LabelDrawingProperties.h"
#undef __LABEL_DRAWING_PROPERTIES_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::LabelDrawingProperties 
 * \brief File properties for labels.
 * \ingroup Files
 */

/**
 * Constructor.
 */
LabelDrawingProperties::LabelDrawingProperties()
: CaretObject()
{
    initializeInstance();
}

/**
 * Destructor.
 */
LabelDrawingProperties::~LabelDrawingProperties()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
LabelDrawingProperties::LabelDrawingProperties(const LabelDrawingProperties& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    initializeInstance();
    copyHelper(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
LabelDrawingProperties&
LabelDrawingProperties::operator=(const LabelDrawingProperties& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelper(obj);
    }
    return *this;
}

/**
 * Initialize an instance of this class.
 */
void
LabelDrawingProperties::initializeInstance()
{
    m_drawingType          = LabelDrawingTypeEnum::DRAW_FILLED;
    m_outlineColor         = CaretColorEnum::BLACK;
    m_drawMedialWallFilled = true;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<LabelDrawingTypeEnum, LabelDrawingTypeEnum::Enum>("m_drawingType",
                                                                            &m_drawingType);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_outlineColor",
                                                                &m_outlineColor);
    m_sceneAssistant->add("m_drawMedialWallFilled",
                          &m_drawMedialWallFilled);
}


/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
LabelDrawingProperties::copyHelper(const LabelDrawingProperties& obj)
{
    m_drawingType          = obj.m_drawingType;
    m_outlineColor         = obj.m_outlineColor;
    m_drawMedialWallFilled = obj.m_drawMedialWallFilled;
}

/**
 * @return The drawing type.
 */
LabelDrawingTypeEnum::Enum
LabelDrawingProperties::getDrawingType() const
{
    return m_drawingType;
}

/**
 * Set the drawing type to the given value.
 * @param drawingType
 *     New value for drawing type.
 */
void
LabelDrawingProperties::setDrawingType(const LabelDrawingTypeEnum::Enum drawingType)
{
        m_drawingType = drawingType;
}

/**
 * @param displayGroup
 *     Display group.
 * @return The outline color.
 */
CaretColorEnum::Enum
LabelDrawingProperties::getOutlineColor() const
{
    return m_outlineColor;
}

/**
 * Set the outline color to the given value.
 * @param outlineColor
 *     New value for outline color.
 */
void
LabelDrawingProperties::setOutlineColor(const CaretColorEnum::Enum outlineColor)
{
    m_outlineColor = outlineColor;
}

/**
 * @return medial wall is drawn filled
 */
bool
LabelDrawingProperties::isDrawMedialWallFilled() const
{
    return m_drawMedialWallFilled;
}

/**
 * Set medial wall is drawn filled
 * @param drawMedialWallFilled
 *    New value for medial wall is drawn filled
 */
void
LabelDrawingProperties::setDrawMedialWallFilled(const bool drawMedialWallFilled)
{
    m_drawMedialWallFilled = drawMedialWallFilled;
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
LabelDrawingProperties::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "LabelDrawingProperties",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
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
LabelDrawingProperties::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

