
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __CHART_TWO_TITLE_DECLARE__
#include "ChartTwoTitle.h"
#undef __CHART_TWO_TITLE_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoTitle 
 * \brief Title and its attributes for charts.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoTitle::ChartTwoTitle()
: CaretObjectTracksModification()
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    reset();
    
    m_sceneAssistant->add("m_displayed",
                          &m_displayed);
    m_sceneAssistant->add("m_paddingSize",
                          &m_paddingSize);
    m_sceneAssistant->add("m_text",
                          &m_text);
    m_sceneAssistant->add("m_textSize",
                          &m_textSize);
}

/**
 * Destructor.
 */
ChartTwoTitle::~ChartTwoTitle()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoTitle::ChartTwoTitle(const ChartTwoTitle& obj)
: CaretObjectTracksModification(obj),
SceneableInterface(obj)
{
    this->copyHelperChartTwoTitle(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoTitle&
ChartTwoTitle::operator=(const ChartTwoTitle& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperChartTwoTitle(obj);
    }
    return *this;    
}

/**
 * Reset this instance to its default values.
 */
void
ChartTwoTitle::reset()
{
    m_displayed   = false;
    m_paddingSize = 1.0f;
    m_text        = "";
    m_textSize    = 5.0f;
}


/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoTitle::copyHelperChartTwoTitle(const ChartTwoTitle& obj)
{
    m_displayed   = obj.m_displayed;
    m_paddingSize = obj.m_paddingSize;
    m_text        = obj.m_text;
    m_textSize    = obj.m_textSize;
}

/**
 * @return Padding between title and edge of viewport
 */
float
ChartTwoTitle::getPaddingSize() const
{
    return m_paddingSize;
}

/**
 * Set Padding between title and edge of viewport
 *
 * @param paddingSize
 *    New value for Padding between title and edge of viewport
 */
void
ChartTwoTitle::setPaddingSize(const float paddingSize)
{
    if (paddingSize != m_paddingSize) {
        m_paddingSize = paddingSize;
        setModified();
    }
}

/**
 * @return Size of title characters when drawing
 */
float
ChartTwoTitle::getTextSize() const
{
    return m_textSize;
}

/**
 * Set Size of title characters when drawing
 *
 * @param textSize
 *    New value for Size of title characters when drawing
 */
void
ChartTwoTitle::setTextSize(const float textSize)
{
    if (textSize != m_textSize) {
        m_textSize = textSize;
        setModified();
    }
}

/**
 * @return text displayed as title
 */
AString
ChartTwoTitle::getText() const
{
    return m_text;
}

/**
 * Set text displayed as title
 *
 * @param text
 *    New value for text displayed as title
 */
void
ChartTwoTitle::setText(const AString& text)
{
    if (text != m_text) {
        m_text = text;
        setModified();
    }
}

/**
 * @return display status of title
 */
bool
ChartTwoTitle::isDisplayed() const
{
    return m_displayed;
}

/**
 * Set display status of title
 *
 * @param displayed
 *    New value for display status of title
 */
void
ChartTwoTitle::setDisplayed(const bool displayed)
{
    if (displayed != m_displayed) {
        m_displayed = displayed;
        setModified();
    }
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
ChartTwoTitle::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoTitle",
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
ChartTwoTitle::restoreFromScene(const SceneAttributes* sceneAttributes,
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

