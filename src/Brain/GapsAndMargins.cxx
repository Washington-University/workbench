
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

#define __GAPS_AND_MARGINS_DECLARE__
#include "GapsAndMargins.h"
#undef __GAPS_AND_MARGINS_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * \class caret::GapsAndMargins 
 * \brief Gaps for surface montage and volume montage.  Margins for tabs.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
GapsAndMargins::GapsAndMargins()
: CaretObject()
{
    reset();
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->addArray("m_tabMarginsBottom",
                               m_tabMarginsBottom,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               0.0);
    
    m_sceneAssistant->addArray("m_tabMarginsLeft",
                               m_tabMarginsLeft,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               0.0);
    
    m_sceneAssistant->addArray("m_tabMarginsRight",
                               m_tabMarginsRight,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               0.0);
    
    m_sceneAssistant->addArray("m_tabMarginsTop",
                               m_tabMarginsTop,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               0.0);
    
    m_sceneAssistant->add("m_tabMarginLeftAllSelected",
                          &m_tabMarginLeftAllSelected);
    m_sceneAssistant->add("m_tabMarginRightAllSelected",
                          &m_tabMarginRightAllSelected);
    m_sceneAssistant->add("m_tabMarginBottomAllSelected",
                          &m_tabMarginBottomAllSelected);
    m_sceneAssistant->add("m_tabMarginTopAllSelected",
                          &m_tabMarginTopAllSelected);
    
    m_sceneAssistant->addArray("m_surfaceMontageGaps",
                               m_surfaceMontageGaps,
                               2,
                               0.0);
    
    m_sceneAssistant->addArray("m_volumeMontageGaps",
                               m_volumeMontageGaps,
                               2,
                               0.0);
    
    m_sceneAssistant->add("m_surfaceMontageScaleProportionatelySelected",
                          &m_surfaceMontageScaleProportionatelySelected);
    m_sceneAssistant->add("m_volumeMontageScaleProportionatelySelected",
                          &m_volumeMontageScaleProportionatelySelected);
}

/**
 * Destructor.
 */
GapsAndMargins::~GapsAndMargins()
{
    delete m_sceneAssistant;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GapsAndMargins::toString() const
{
    return "GapsAndMargins";
}

void
GapsAndMargins::reset()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_tabMarginsBottom[i] = 0.0;
        m_tabMarginsLeft[i]   = 0.0;
        m_tabMarginsRight[i]  = 0.0;
        m_tabMarginsTop[i]    = 0.0;
    }

    m_tabMarginLeftAllSelected   = false;
    m_tabMarginRightAllSelected  = false;
    m_tabMarginBottomAllSelected = false;
    m_tabMarginTopAllSelected    = false;
    
    for (int32_t i = 0; i < 2; i++) {
        m_surfaceMontageGaps[i] = 0.0;
        m_volumeMontageGaps[i]  = 0.0;
    }
    
    m_surfaceMontageScaleProportionatelySelected = false;
    m_volumeMontageScaleProportionatelySelected  = false;
}

/**
 * Get the LEFT margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Left margin for the tab.
 */
float
GapsAndMargins::getTabMarginLeft(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginsLeft, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginsLeft[tabIndex];
}

/**
 * Get the RIGHT margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Right margin for the tab.
 */
float
GapsAndMargins::getTabMarginRight(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginsRight, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginsRight[tabIndex];
}

/**
 * Get the BOTTOM margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Bottom margin for the tab.
 */
float
GapsAndMargins::getTabMarginBottom(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginsBottom, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginsBottom[tabIndex];
}

/**
 * Get the TOP margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Top margin for the tab.
 */
float
GapsAndMargins::getTabMarginTop(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginsTop, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginsTop[tabIndex];
}

/**
 * Set the LEFT margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param margin
 *     Left margin for the tab.
 */
void
GapsAndMargins::setTabMarginLeft(const int32_t tabIndex,
                                 const float margin)
{
    CaretAssertArrayIndex(m_tabMarginsLeft, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginsLeft[tabIndex] = margin;
    
    if (tabIndex == 0) {
        copyTabOneLeftMarginToAllLeftMargins();
    }
}

/**
 * Set the RIGHT margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param margin
 *     Right margin for the tab.
 */
void
GapsAndMargins::setTabMarginRight(const int32_t tabIndex,
                                  const float margin)
{
    CaretAssertArrayIndex(m_tabMarginsRight, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginsRight[tabIndex] = margin;
    
    if (tabIndex == 0) {
        copyTabOneRightMarginToAllRightMargins();
    }
}

/**
 * Set the BOTTOM margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param margin
 *     Bottom margin for the tab.
 */
void
GapsAndMargins::setTabMarginBottom(const int32_t tabIndex,
                                  const float margin)
{
    CaretAssertArrayIndex(m_tabMarginsBottom, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginsBottom[tabIndex] = margin;
    
    if (tabIndex == 0) {
        copyTabOneBottomMarginToAllBottomMargins();
    }
}

/**
 * Set the Top margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param margin
 *     Top margin for the tab.
 */
void
GapsAndMargins::setTabMarginTop(const int32_t tabIndex,
                                  const float margin)
{
    CaretAssertArrayIndex(m_tabMarginsTop, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginsTop[tabIndex] = margin;
    
    if (tabIndex == 0) {
        copyTabOneTopMarginToAllTopMargins();
    }
}

/**
 * @return The surface montage horizontal gap.
 */
float
GapsAndMargins::getSurfaceMontageHorizontalGap() const
{
    return m_surfaceMontageGaps[0];
}

/**
 * @return The surface montage vertical gap.
 */
float
GapsAndMargins::getSurfaceMontageVerticalGap() const
{
    return m_surfaceMontageGaps[1];
}

/**
 * @return The volume montage horizontal gap.
 */
float
GapsAndMargins::getVolumeMontageHorizontalGap() const
{
    return m_volumeMontageGaps[0];
}

/**
 * @return The volume montage vertical gap.
 */
float
GapsAndMargins::getVolumeMontageVerticalGap() const
{
    return m_volumeMontageGaps[1];
}

/**
 * Set the surface montage horizontal gap.
 *
 * @param gap
 *    New value for horizontal gap.
 */
void
GapsAndMargins::setSurfaceMontageHorizontalGap(const float gap)
{
    m_surfaceMontageGaps[0] = gap;
}

/**
 * Set the surface montage vertical gap.
 *
 * @param gap
 *    New value for vertical gap.
 */
void
GapsAndMargins::setSurfaceMontageVerticalGap(const float gap)
{
    m_surfaceMontageGaps[1] = gap;
}

/**
 * Set the volume montage horizontal gap.
 *
 * @param gap
 *    New value for horizontal gap.
 */
void
GapsAndMargins::setVolumeMontageHorizontalGap(const float gap)
{
    m_volumeMontageGaps[0] = gap;
}

/**
 * Set the volume montage vertical gap.
 *
 * @param gap
 *    New value for vertical gap.
 */
void
GapsAndMargins::setVolumeMontageVerticalGap(const float gap)
{
    m_volumeMontageGaps[1] = gap;
}

/**
 * @return Tab margin left all selected (applies the tab 1 margin to all tabs)
 */
bool
GapsAndMargins::isTabMarginLeftAllSelected() const
{
    return m_tabMarginLeftAllSelected;
}

/**
 * @return Tab margin right all selected (applies the tab 1 margin to all tabs)
 */
bool
GapsAndMargins::isTabMarginRightAllSelected() const
{
    return m_tabMarginRightAllSelected;
}

/**
 * @return Tab margin bottom all selected (applies the tab 1 margin to all tabs)
 */
bool
GapsAndMargins::isTabMarginBottomAllSelected() const
{
    return m_tabMarginBottomAllSelected;
}

/**
 * @return Tab margin top all selected (applies the tab 1 margin to all tabs)
 */
bool
GapsAndMargins::isTabMarginTopAllSelected() const
{
    return m_tabMarginTopAllSelected;
}

/**
 * Set tab margin left all selected and sets the left margin
 * for every tab with the tab zero's current value.
 *
 * @param status
 *     New status for tab margin left all.
 */
void
GapsAndMargins::setTabMarginLeftAllSelected(const bool status)
{
    m_tabMarginLeftAllSelected = status;

    copyTabOneLeftMarginToAllLeftMargins();
}

/**
 * Set tab margin right all selected and sets the right margin
 * for every tab with the tab zero's current value.
 *
 * @param status
 *     New status for tab margin right all.
 */
void
GapsAndMargins::setTabMarginRightAllSelected(const bool status)
{
    m_tabMarginRightAllSelected = status;
    
    copyTabOneRightMarginToAllRightMargins();
}

/**
 * Set tab margin bottom all selected and sets the bottom margin
 * for every tab with the tab zero's current value.
 *
 * @param status
 *     New status for tab margin bottom all.
 */
void
GapsAndMargins::setTabMarginBottomAllSelected(const bool status)
{
    m_tabMarginBottomAllSelected = status;
    
    copyTabOneBottomMarginToAllBottomMargins();
}

/**
 * Set tab margin top all selected and sets the top margin
 * for every tab with the tab zero's current value.
 *
 * @param status
 *     New status for tab margin top all.
 */
void
GapsAndMargins::setTabMarginTopAllSelected(const bool status)
{
    m_tabMarginTopAllSelected = status;
    
    copyTabOneTopMarginToAllTopMargins();
}

/**
 * Copy tab one left margin to all left margins.
 */
void
GapsAndMargins::copyTabOneLeftMarginToAllLeftMargins()
{
    if (m_tabMarginLeftAllSelected) {
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_tabMarginsLeft[i] = m_tabMarginsLeft[0];
        }
    }
}

/**
 * Copy tab one right margin to all right margins.
 */
void
GapsAndMargins::copyTabOneRightMarginToAllRightMargins()
{
    if (m_tabMarginRightAllSelected) {
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_tabMarginsRight[i] = m_tabMarginsRight[0];
        }
    }
}

/**
 * Copy tab one bottom margin to all bottom margins.
 */
void
GapsAndMargins::copyTabOneBottomMarginToAllBottomMargins()
{
    if (m_tabMarginBottomAllSelected) {
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_tabMarginsBottom[i] = m_tabMarginsBottom[0];
        }
    }
}

/**
 * Copy tab one top margin to all top margins.
 */
void
GapsAndMargins::copyTabOneTopMarginToAllTopMargins()
{
    if (m_tabMarginTopAllSelected) {
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_tabMarginsTop[i] = m_tabMarginsTop[0];
        }
    }
}

/**
 * @return Is surface montage scale proportionately selected ?
 */
bool
GapsAndMargins::isSurfaceMontageScaleProportionatelySelected() const
{
    return m_surfaceMontageScaleProportionatelySelected;
}

/**
 * @return Is volume montage scale proportionately selected ?
 */
bool
GapsAndMargins::isVolumeMontageScaleProportionatelySelected() const
{
    return m_volumeMontageScaleProportionatelySelected;
}

/**
 * Set surface montage scale proportionately selected.
 *
 * @param selected
 *    New selection status.
 */
void
GapsAndMargins::setSurfaceMontageScaleProportionatelySelected(const bool selected)
{
    m_surfaceMontageScaleProportionatelySelected = selected;
}

/**
 * Set volume montage scale proportionately selected.
 *
 * @param selected
 *    New selection status.
 */
void
GapsAndMargins::setVolumeMontageScaleProportionatelySelected(const bool selected)
{
    m_volumeMontageScaleProportionatelySelected = selected;
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
GapsAndMargins::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "GapsAndMargins",
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
GapsAndMargins::restoreFromScene(const SceneAttributes* sceneAttributes,
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

