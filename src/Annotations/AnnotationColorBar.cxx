
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

#define __ANNOTATION_COLOR_BAR_DECLARE__
#include "AnnotationColorBar.h"
#undef __ANNOTATION_COLOR_BAR_DECLARE__

#include "CaretAssert.h"
#include "SceneClassAssistant.h"
using namespace caret;


    
/**
 * \class caret::AnnotationColorBar 
 * \brief Annotation used for drawing a color bar.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationColorBar::AnnotationColorBar(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationTwoDimensionalShape(AnnotationTypeEnum::COLOR_BAR,
                                attributeDefaultType)
{
    reset();
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_sceneAssistant->add<AnnotationTextFontNameEnum, AnnotationTextFontNameEnum::Enum>("m_fontName",
                                                                                        &m_fontName);
    m_sceneAssistant->add("m_fontPercentViewportHeight",
                          &m_fontPercentViewportHeight);
    m_sceneAssistant->add<AnnotationColorBarPositionModeEnum, AnnotationColorBarPositionModeEnum::Enum>("m_positionMode",
                                                                                                        &m_positionMode);
    m_sceneAssistant->add("m_displayedFlag",
                          &m_displayedFlag);
}

/**
 * Destructor.
 */
AnnotationColorBar::~AnnotationColorBar()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationColorBar::AnnotationColorBar(const AnnotationColorBar& obj)
: AnnotationTwoDimensionalShape(obj)
{
    this->copyHelperAnnotationColorBar(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationColorBar&
AnnotationColorBar::operator=(const AnnotationColorBar& obj)
{
    if (this != &obj) {
        AnnotationTwoDimensionalShape::operator=(obj);
        this->copyHelperAnnotationColorBar(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 *
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationColorBar::copyHelperAnnotationColorBar(const AnnotationColorBar& obj)
{
    m_fontName                  = obj.m_fontName;
    m_fontPercentViewportHeight = obj.m_fontPercentViewportHeight;
    m_positionMode              = obj.m_positionMode;
    m_displayedFlag             = obj.m_displayedFlag;
}

/**
 * Reset the annotation colorbar.
 *
 * DO NOT make this method virtual is it is called from constructor.
 */
void
AnnotationColorBar::reset()
{
    m_fontPercentViewportHeight = 0.05;
    m_fontName      = AnnotationTextFontNameEnum::getDefaultFontName();
    m_positionMode  = AnnotationColorBarPositionModeEnum::AUTO;
    m_displayedFlag = false;
}

/**
 * @return The font.
 */
AnnotationTextFontNameEnum::Enum
AnnotationColorBar::getFont() const
{
    return m_fontName;
}

/**
 * Set the font.
 *
 * @param font
 *     New value for font.
 */
void
AnnotationColorBar::setFont(const AnnotationTextFontNameEnum::Enum font)
{
    if (font != m_fontName) {
        m_fontName = font;
        setModified();
    }
}

/**
 * @return THe percent viewport height for the font.
 */
float
AnnotationColorBar::getFontPercentViewportSize() const
{
    return m_fontPercentViewportHeight;
}

/**
 * Set the percent viewport size for the font.
 *
 * @param fontPercentViewportHeight
 *     New value for percent viewport height.
 */
void
AnnotationColorBar::setFontPercentViewportSize(const float fontPercentViewportHeight)
{
    if (fontPercentViewportHeight != m_fontPercentViewportHeight) {
        m_fontPercentViewportHeight = fontPercentViewportHeight;
        setModified();
    }
}

/**
 * @return The position mode for the colorbar annotation.
 */
AnnotationColorBarPositionModeEnum::Enum
AnnotationColorBar::getPositionMode() const
{
    return m_positionMode;
}

/**
 * Set the position mode for the colorbar.
 *
 * @param positionMode
 *     New position mode for the colorbar.
 */
void
AnnotationColorBar::setPositionMode(const AnnotationColorBarPositionModeEnum::Enum positionMode)
{
    if (positionMode != m_positionMode) {
        m_positionMode = positionMode;
        setModified();
    }
}

/**
 * @return Display status of colorbar.
 */
bool
AnnotationColorBar::isDisplayed() const
{
    return m_displayedFlag;
}

/**
 * Set the color bar annotation displayed.
 *
 * @param displayed
 *     New status for display of colorbar.
 */
void
AnnotationColorBar::setDisplayed(const bool displayed)
{
    if (displayed != m_displayedFlag) {
        m_displayedFlag = displayed;
        setModified();
    }
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
AnnotationColorBar::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::saveSubClassDataToScene(sceneAttributes,
                                                           sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
AnnotationColorBar::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
}
