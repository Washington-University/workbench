
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

#define __ANNOTATION_DECLARE__
#include "Annotation.h"
#undef __ANNOTATION_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::Annotation 
 * \brief Abstract class for annotations.
 * \ingroup Annotations
 */

/**
 * Constructor for an annotation.
 * 
 * @param drawingType
 *    Type of annotation for drawing.
 * @param shapeDimension
 *    Dimension of the annotation's shape.
 */
Annotation::Annotation(const AnnotationTypeEnum::Enum type)
: CaretObjectTracksModification(),
m_type(type)
{
    initializeAnnotationMembers();
}

/**
 * Destructor.
 */
Annotation::~Annotation()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Annotation::Annotation(const Annotation& obj)
: CaretObjectTracksModification(obj),
m_type(obj.m_type)
{
    initializeAnnotationMembers();
    this->copyHelperAnnotation(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
Annotation&
Annotation::operator=(const Annotation& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperAnnotation(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Annotation::copyHelperAnnotation(const Annotation& obj)
{
    m_coordinateSpace     = obj.m_coordinateSpace;
    m_tabIndex            = obj.m_tabIndex;
    m_windowIndex         = obj.m_windowIndex;
    m_colorBackground[0]  = obj.m_colorBackground[0];
    m_colorBackground[1]  = obj.m_colorBackground[1];
    m_colorBackground[2]  = obj.m_colorBackground[2];
    m_colorBackground[3]  = obj.m_colorBackground[3];
    m_colorForeground[0]  = obj.m_colorForeground[0];
    m_colorForeground[1]  = obj.m_colorForeground[1];
    m_colorForeground[2]  = obj.m_colorForeground[2];
    m_colorForeground[3]  = obj.m_colorForeground[3];
 
    /*
     * Selected status is NOT copied.
     */
    m_selectedFlag = false;
}

/**
 * Initialize members of this class.
 */
void
Annotation::initializeAnnotationMembers()
{
    m_selectedFlag = false;
    
    m_coordinateSpace = AnnotationCoordinateSpaceEnum::TAB;
    
    m_tabIndex = -1;
    m_windowIndex = -1;
    
    m_colorBackground[0]  = 0.0;
    m_colorBackground[1]  = 0.0;
    m_colorBackground[2]  = 0.0;
    m_colorBackground[3]  = 0.0;
    
    m_colorForeground[0]  = 1.0;
    m_colorForeground[1]  = 1.0;
    m_colorForeground[2]  = 1.0;
    m_colorForeground[3]  = 1.0;
    
    /*
     * Note: The 'const' members are not saved to the scene as they 
     * are set by constructor.
     *
     * The 'selected' status is not saved to the scene.
     */
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add<AnnotationCoordinateSpaceEnum>("m_coordinateSpace",
                                                         &m_coordinateSpace);
    m_sceneAssistant->add("m_tabIndex",
                          &m_tabIndex);
    m_sceneAssistant->add("m_windowIndex",
                          &m_windowIndex);
    m_sceneAssistant->addArray("m_colorBackground",
                               m_colorBackground, 4, 0.0);
    m_sceneAssistant->addArray("m_colorForeground",
                               m_colorForeground, 4, 1.0);
}


/**
 * @return The annotation drawing type.
 */
AnnotationTypeEnum::Enum
Annotation::getType() const
{
    return m_type;
}

/**
 * @return The coordinate space.
 */
AnnotationCoordinateSpaceEnum::Enum
Annotation::getCoordinateSpace() const
{
    return m_coordinateSpace;
}

/**
 * Set the coordinate space.
 *
 * @param coordinateSpace
 *     New value for coordinate space.
 */
void
Annotation::setCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace)
{
    if (m_coordinateSpace != coordinateSpace) {
        m_coordinateSpace = coordinateSpace;
        setModified();
    }
}

/**
 * @return The tab index.  Valid only for tab coordinate space annotations.
 */
int32_t
Annotation::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Set tab index.  Valid only for tab coordinate space annotations.
 *
 * @param tabIndex
 */
void
Annotation::setTabIndex(const int32_t tabIndex)
{
    if (tabIndex != m_tabIndex) {
        m_tabIndex = tabIndex;
        setModified();
    }
}

/**
 * @return The window index.  Valid only for window coordinate space annotations.
 */
int32_t
Annotation::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * Set window index.  Valid only for window coordinate space annotations.
 *
 * @param tabIndex
 */
void
Annotation::setWindowIndex(const int32_t windowIndex)
{
    if (windowIndex != m_windowIndex) {
        m_windowIndex = windowIndex;
        setModified();
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
Annotation::toString() const
{
    const AString msg("Annotation type="
                      + AnnotationTypeEnum::toName(m_type));
    return msg;
}

/**
 * @return
 *    Foreground color RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
const float*
Annotation::getForegroundColor() const
{
    return m_colorForeground;
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getForegroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_colorForeground[0];
    rgbaOut[1] = m_colorForeground[1];
    rgbaOut[2] = m_colorForeground[2];
    rgbaOut[3] = m_colorForeground[3];
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getForegroundColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_colorForeground[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_colorForeground[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_colorForeground[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_colorForeground[3] * 255.0);
}

/**
 * Set the foreground color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setForegroundColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_colorForeground[i]) {
            m_colorForeground[i] = rgba[i];
            setModified();
        }
    }
}

/**
 * Set the foreground color with unsigned bytes.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::setForegroundColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_colorForeground[i]) {
            m_colorForeground[i] = component;
            setModified();
        }
    }
}

/**
 * @return
 *    Background color RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 *    The background color is applied only when its alpha component is greater than zero.
 */
const float*
Annotation::getBackgroundColor() const
{
    return m_colorBackground;
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getBackgroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_colorBackground[0];
    rgbaOut[1] = m_colorBackground[1];
    rgbaOut[2] = m_colorBackground[2];
    rgbaOut[3] = m_colorBackground[3];
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getBackgroundColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_colorBackground[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_colorBackground[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_colorBackground[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_colorBackground[3] * 255.0);
}

/**
 * Set the background color with floats.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setBackgroundColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_colorBackground[i]) {
            m_colorBackground[i] = rgba[i];
            setModified();
        }
    }
}

/**
 * Set the background color with bytes.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::setBackgroundColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_colorBackground[i]) {
            m_colorBackground[i] = component;
            setModified();
        }
    }
}

/**
 * @return The annotation's selected status.
 *
 * Note: (1) The selection status is never saved to a scene
 * or file.  (2) Changing the selection status DOES NOT
 * alter the annotation's modified status.
 */
bool
Annotation::isSelected() const
{
    return m_selectedFlag;
}

/**
 * Set the annotation's selected status.
 *
 * Note: (1) The selection status is never saved to a scene
 * or file.  (2) Changing the selection status DOES NOT
 * alter the annotation's modified status.
 */
void
Annotation::setSelected(const bool selectedStatus) const
{
    m_selectedFlag = selectedStatus;
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
Annotation::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "Annotation",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    saveSubClassDataToScene(sceneAttributes,
                            sceneClass);
    
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
Annotation::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    restoreSubClassDataFromScene(sceneAttributes,
                                 sceneClass);
    
}

