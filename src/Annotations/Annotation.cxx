
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

#include "AnnotationBox.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
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
    //std::cout << "Deleting annotation of type: " << qPrintable(AnnotationTypeEnum::toGuiName(m_type)) << std::endl;
    
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Annotation::Annotation(const Annotation& obj)
: CaretObjectTracksModification(obj),
SceneableInterface(obj),
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
    m_foregroundLineWidth = obj.m_foregroundLineWidth;
    m_colorForeground     = obj.m_colorForeground;
    m_colorBackground     = obj.m_colorBackground;
    m_customColorBackground[0]  = obj.m_customColorBackground[0];
    m_customColorBackground[1]  = obj.m_customColorBackground[1];
    m_customColorBackground[2]  = obj.m_customColorBackground[2];
    m_customColorBackground[3]  = obj.m_customColorBackground[3];
    m_customColorForeground[0]  = obj.m_customColorForeground[0];
    m_customColorForeground[1]  = obj.m_customColorForeground[1];
    m_customColorForeground[2]  = obj.m_customColorForeground[2];
    m_customColorForeground[3]  = obj.m_customColorForeground[3];

    /*
     * Selected status is NOT copied.
     */
    m_selectedFlag = false;
}

/**
 * @return An identical copy (clone) of "this" annotation.
 *    The type of the annotation is used to ensure the 
 *    correct type of annotation is created and returned.
 */
Annotation*
Annotation::clone() const
{
    Annotation* myClone = NULL;
    
    switch (getType()) {
        case AnnotationTypeEnum::BOX:
        {
            const AnnotationBox* box = dynamic_cast<const AnnotationBox*>(this);
            CaretAssert(box);
            myClone = new AnnotationBox(*box);
        }
            break;
        case AnnotationTypeEnum::IMAGE:
        {
            const AnnotationImage* image = dynamic_cast<const AnnotationImage*>(this);
            CaretAssert(image);
            myClone = new AnnotationImage(*image);
        }
            break;
        case AnnotationTypeEnum::LINE:
        {
            const AnnotationLine* line = dynamic_cast<const AnnotationLine*>(this);
            CaretAssert(line);
            myClone = new AnnotationLine(*line);
        }
            break;
        case AnnotationTypeEnum::OVAL:
        {
            const AnnotationOval* oval = dynamic_cast<const AnnotationOval*>(this);
            CaretAssert(oval);
            myClone = new AnnotationOval(*oval);
        }
            break;
        case AnnotationTypeEnum::TEXT:
        {
            const AnnotationText* text = dynamic_cast<const AnnotationText*>(this);
            CaretAssert(text);
            myClone = new AnnotationText(*text);
        }
            break;
    }
    
    CaretAssert(myClone);
    
    return myClone;
}

/**
 * Replace "this" annotation with content of the given annotation.
 * The annotation must by the same type and class.
 *
 * @param annotation
 *     Annotation whose content is copied to "this" annotation.
 */
void
Annotation::replaceWithCopyOfAnnotation(const Annotation* annotation)
{
    CaretAssert(annotation);
    
    const AnnotationTypeEnum::Enum myType = getType();
    
    if (myType != annotation->getType()) {
        CaretLogSevere("Attempting to replace  "
                       + AnnotationTypeEnum::toGuiName(myType)
                       + " with annotation of different type: "
                       + AnnotationTypeEnum::toGuiName(annotation->getType()));
        return;
    }
    
    *this = *annotation;
}


/**
 * Factory method for creating an annotation of the given type.
 *
 * @param annotationType
 *     Type of annotation that will be created.
 * @return
 *     New annotation of the given type.
 */
Annotation*
Annotation::newAnnotationOfType(const AnnotationTypeEnum::Enum annotationType)
{
    Annotation* annotation = NULL;
    
    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
            annotation = new AnnotationBox();
            break;
        case AnnotationTypeEnum::IMAGE:
            annotation = new AnnotationImage();
            break;
        case AnnotationTypeEnum::LINE:
            annotation = new AnnotationLine();
            break;
        case AnnotationTypeEnum::OVAL:
            annotation = new AnnotationOval();
            break;
        case AnnotationTypeEnum::TEXT:
            annotation = new AnnotationText();
            break;
    }
    
    return annotation;
}

/**
 * Initialize members of this class.
 */
void
Annotation::initializeAnnotationMembers()
{
    m_selectedFlag = false;
    
    m_coordinateSpace = AnnotationCoordinateSpaceEnum::TAB;
    
    m_tabIndex    = -1;
    m_windowIndex = -1;
    
    m_foregroundLineWidth = 3.0;
    
    m_colorBackground = CaretColorEnum::NONE;
    m_colorForeground = CaretColorEnum::WHITE;
    
    m_customColorBackground[0]  = 0.0;
    m_customColorBackground[1]  = 0.0;
    m_customColorBackground[2]  = 0.0;
    m_customColorBackground[3]  = 1.0;
    
    m_customColorForeground[0]  = 1.0;
    m_customColorForeground[1]  = 1.0;
    m_customColorForeground[2]  = 1.0;
    m_customColorForeground[3]  = 1.0;
    
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
    m_sceneAssistant->add("m_foregroundLineWidth",
                          &m_foregroundLineWidth);
    m_sceneAssistant->addArray("m_customColorBackground",
                               m_customColorBackground, 4, 0.0);
    m_sceneAssistant->addArray("m_customColorForeground",
                               m_customColorForeground, 4, 1.0);
}


/**
 * @return The annotation drawing type.
 */
AnnotationTypeEnum::Enum
Annotation::getType() const
{
    return m_type;
}

QString
Annotation::getShortDescriptiveString() const
{
    QString s = AnnotationTypeEnum::toGuiName(m_type);
    
    if (m_type == AnnotationTypeEnum::TEXT) {
        const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(this);
        s += (" \"" + textAnn->getText() + "\"");
    }
    
    return s;
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
 * @return The foreground line width.
 */
float
Annotation::getForegroundLineWidth() const
{
    return m_foregroundLineWidth;
}

/**
 * Set the foreground line width.
 *
 * @param lineWidth
 *    New value for foreground line width.
 */
void
Annotation::setForegroundLineWidth(const float lineWidth)
{
    if (lineWidth != m_foregroundLineWidth) {
        m_foregroundLineWidth = lineWidth;
        setModified();
    }
}

/**
 * @return Is foreground line width supported?
 * Most annotations support a foreground line width.
 * Annotations that do not support a foreground line width
 * must override this method and return a value of false.
 */
bool
Annotation::isForegroundLineWidthSupported() const
{
    return true;
}

/**
 * @return Is background color supported?
 * Most annotations support a background color.
 * Annotations that do not support a background color
 * must override this method and return a value of false.
 */
bool
Annotation::isBackgroundColorSupported() const
{
    return true;
}


/**
 * @return The foreground color.
 */
CaretColorEnum::Enum
Annotation::getForegroundColor() const
{
    return m_colorForeground;
}

/**
 * Set the foreground color.
 *
 * @param color
 *     New value for foreground color.
 */
void
Annotation::setForegroundColor(const CaretColorEnum::Enum color)
{
    if (m_colorForeground != color) {
        m_colorForeground = color;
        setModified();
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0.0 to 1.0.
 */
void
Annotation::getForegroundColorRGBA(float rgbaOut[4]) const
{
    switch (m_colorForeground) {
        case CaretColorEnum::NONE:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 0.0;
            break;
        case CaretColorEnum::CUSTOM:
            getCustomForegroundColor(rgbaOut);
            break;
        case CaretColorEnum::AQUA:
        case CaretColorEnum::BLACK:
        case CaretColorEnum::BLUE:
        case CaretColorEnum::FUCHSIA:
        case CaretColorEnum::GRAY:
        case CaretColorEnum::GREEN:
        case CaretColorEnum::LIME:
        case CaretColorEnum::MAROON:
        case CaretColorEnum::NAVY:
        case CaretColorEnum::OLIVE:
        case CaretColorEnum::PURPLE:
        case CaretColorEnum::RED:
        case CaretColorEnum::SILVER:
        case CaretColorEnum::TEAL:
        case CaretColorEnum::WHITE:
        case CaretColorEnum::YELLOW:
            CaretColorEnum::toRGBFloat(m_colorForeground,
                                       rgbaOut);
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0 to 255.
 */
void
Annotation::getForegroundColorRGBA(uint8_t rgbaOut[4]) const
{
    float rgbaFloat[4] = { 0.0, 0.0, 0.0, 0.0 };
    getForegroundColorRGBA(rgbaFloat);
    
    rgbaOut[0] = static_cast<int8_t>(rgbaFloat[0] * 255.0);
    rgbaOut[1] = static_cast<int8_t>(rgbaFloat[1] * 255.0);
    rgbaOut[2] = static_cast<int8_t>(rgbaFloat[2] * 255.0);
    rgbaOut[3] = static_cast<int8_t>(rgbaFloat[3] * 255.0);
}

/**
 * @return The background color.
 */
CaretColorEnum::Enum
Annotation::getBackgroundColor() const
{
    return m_colorBackground;
}

/**
 * Set the background color.
 *
 * @param color
 *     New value for background color.
 */
void
Annotation::setBackgroundColor(const CaretColorEnum::Enum color)
{
    if (m_colorBackground != color) {
        m_colorBackground = color;
        setModified();
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0.0 to 1.0.
 */
void
Annotation::getBackgroundColorRGBA(float rgbaOut[4]) const
{
    switch (m_colorBackground) {
        case CaretColorEnum::NONE:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 0.0;
            break;
        case CaretColorEnum::CUSTOM:
            getCustomBackgroundColor(rgbaOut);
            break;
        case CaretColorEnum::AQUA:
        case CaretColorEnum::BLACK:
        case CaretColorEnum::BLUE:
        case CaretColorEnum::FUCHSIA:
        case CaretColorEnum::GRAY:
        case CaretColorEnum::GREEN:
        case CaretColorEnum::LIME:
        case CaretColorEnum::MAROON:
        case CaretColorEnum::NAVY:
        case CaretColorEnum::OLIVE:
        case CaretColorEnum::PURPLE:
        case CaretColorEnum::RED:
        case CaretColorEnum::SILVER:
        case CaretColorEnum::TEAL:
        case CaretColorEnum::WHITE:
        case CaretColorEnum::YELLOW:
            CaretColorEnum::toRGBFloat(m_colorBackground,
                                       rgbaOut);
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0 to 255.
 */
void
Annotation::getBackgroundColorRGBA(uint8_t rgbaOut[4]) const
{
    float rgbaFloat[4] = { 0.0, 0.0, 0.0, 0.0 };
    getBackgroundColorRGBA(rgbaFloat);
    
    rgbaOut[0] = static_cast<int8_t>(rgbaFloat[0] * 255.0);
    rgbaOut[1] = static_cast<int8_t>(rgbaFloat[1] * 255.0);
    rgbaOut[2] = static_cast<int8_t>(rgbaFloat[2] * 255.0);
    rgbaOut[3] = static_cast<int8_t>(rgbaFloat[3] * 255.0);
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getCustomForegroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_customColorForeground[0];
    rgbaOut[1] = m_customColorForeground[1];
    rgbaOut[2] = m_customColorForeground[2];
    rgbaOut[3] = m_customColorForeground[3];
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getCustomForegroundColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_customColorForeground[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_customColorForeground[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_customColorForeground[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_customColorForeground[3] * 255.0);
}

/**
 * Set the foreground color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setCustomForegroundColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_customColorForeground[i]) {
            m_customColorForeground[i] = rgba[i];
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
Annotation::setCustomForegroundColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_customColorForeground[i]) {
            m_customColorForeground[i] = component;
            setModified();
        }
    }
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getCustomBackgroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_customColorBackground[0];
    rgbaOut[1] = m_customColorBackground[1];
    rgbaOut[2] = m_customColorBackground[2];
    rgbaOut[3] = m_customColorBackground[3];
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getCustomBackgroundColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_customColorBackground[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_customColorBackground[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_customColorBackground[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_customColorBackground[3] * 255.0);
}

/**
 * Set the background color with floats.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setCustomBackgroundColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_customColorBackground[i]) {
            m_customColorBackground[i] = rgba[i];
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
Annotation::setCustomBackgroundColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_customColorBackground[i]) {
            m_customColorBackground[i] = component;
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
 * This method is private - AnnotationManager handles selection and allowing
 * public access to this method could cause improper selection status.
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
 * @return True if the annotation can be resized or moved by
 * the GUI.  This status is dependent upon the annotation's
 * coordinate space.
 */
bool
Annotation::isMovableOrResizableFromGUI() const
{
    bool resizableSpaceFlag = false;
    switch (getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::MODEL:
            break;
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            resizableSpaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            resizableSpaceFlag = true;
            break;
    }

    return resizableSpaceFlag;
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

