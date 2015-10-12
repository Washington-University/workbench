
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
    
    m_sceneAssistant->addArray("m_tabMarginScaleProportionatelySelected",
                               m_tabMarginScaleProportionatelySelected,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               false);
    
    m_sceneAssistant->add("m_tabMarginLeftApplyTabOneToAllSelected",
                          &m_tabMarginLeftApplyTabOneToAllSelected);
    m_sceneAssistant->add("m_tabMarginRightApplyTabOneToAllSelected",
                          &m_tabMarginRightApplyTabOneToAllSelected);
    m_sceneAssistant->add("m_tabMarginBottomApplyTabOneToAllSelected",
                          &m_tabMarginBottomApplyTabOneToAllSelected);
    m_sceneAssistant->add("m_tabMarginTopApplyTabOneToAllSelected",
                          &m_tabMarginTopApplyTabOneToAllSelected);
    
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
        m_tabMarginScaleProportionatelySelected[i] = false;
    }

    m_tabMarginLeftApplyTabOneToAllSelected   = false;
    m_tabMarginRightApplyTabOneToAllSelected  = false;
    m_tabMarginBottomApplyTabOneToAllSelected = false;
    m_tabMarginTopApplyTabOneToAllSelected    = false;
    
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
GapsAndMargins::getMarginLeftForTab(const int32_t tabIndex) const
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
GapsAndMargins::getMarginRightForTab(const int32_t tabIndex) const
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
GapsAndMargins::getMarginBottomForTab(const int32_t tabIndex) const
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
GapsAndMargins::getMarginTopForTab(const int32_t tabIndex) const
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
GapsAndMargins::setMarginLeftForTab(const int32_t tabIndex,
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
GapsAndMargins::setMarginRightForTab(const int32_t tabIndex,
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
GapsAndMargins::setMarginBottomForTab(const int32_t tabIndex,
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
GapsAndMargins::setMarginTopForTab(const int32_t tabIndex,
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
GapsAndMargins::isTabMarginLeftApplyTabOneToAllSelected() const
{
    return m_tabMarginLeftApplyTabOneToAllSelected;
}

/**
 * @return Tab margin right all selected (applies the tab 1 margin to all tabs)
 */
bool
GapsAndMargins::isTabMarginRightApplyTabOneToAllSelected() const
{
    return m_tabMarginRightApplyTabOneToAllSelected;
}

/**
 * @return Tab margin bottom all selected (applies the tab 1 margin to all tabs)
 */
bool
GapsAndMargins::isTabMarginBottomApplyTabOneToAllSelected() const
{
    return m_tabMarginBottomApplyTabOneToAllSelected;
}

/**
 * @return Tab margin top all selected (applies the tab 1 margin to all tabs)
 */
bool
GapsAndMargins::isTabMarginTopApplyTabOneToAllSelected() const
{
    return m_tabMarginTopApplyTabOneToAllSelected;
}

/**
 * Set tab margin left all selected and sets the left margin
 * for every tab with the tab zero's current value.
 *
 * @param selected
 *     New status for tab margin left all.
 */
void
GapsAndMargins::setTabMarginLeftApplyTabOneToAllSelected(const bool selected)
{
    m_tabMarginLeftApplyTabOneToAllSelected = selected;

    copyTabOneLeftMarginToAllLeftMargins();
}

/**
 * Set tab margin right all selected and sets the right margin
 * for every tab with the tab zero's current value.
 *
 * @param selected
 *     New status for tab margin right all.
 */
void
GapsAndMargins::setTabMarginRightApplyTabOneToAllSelected(const bool selected)
{
    m_tabMarginRightApplyTabOneToAllSelected = selected;
    
    copyTabOneRightMarginToAllRightMargins();
}

/**
 * Set tab margin bottom all selected and sets the bottom margin
 * for every tab with the tab zero's current value.
 *
 * @param selected
 *     New status for tab margin bottom all.
 */
void
GapsAndMargins::setTabMarginBottomApplyTabOneToAllSelected(const bool selected)
{
    m_tabMarginBottomApplyTabOneToAllSelected = selected;
    
    copyTabOneBottomMarginToAllBottomMargins();
}

/**
 * Set tab margin top all selected and sets the top margin
 * for every tab with the tab zero's current value.
 *
 * @param selected
 *     New status for tab margin top all.
 */
void
GapsAndMargins::setTabMarginTopApplyTabOneToAllSelected(const bool selected)
{
    m_tabMarginTopApplyTabOneToAllSelected = selected;
    
    copyTabOneTopMarginToAllTopMargins();
}

/**
 * Copy tab one left margin to all left margins.
 */
void
GapsAndMargins::copyTabOneLeftMarginToAllLeftMargins()
{
    if (m_tabMarginLeftApplyTabOneToAllSelected) {
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
    if (m_tabMarginRightApplyTabOneToAllSelected) {
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
    if (m_tabMarginBottomApplyTabOneToAllSelected) {
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
    if (m_tabMarginTopApplyTabOneToAllSelected) {
        for (int32_t i = 1; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
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
 * Is tab margin scale proportionately selected?
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     The selected status.
 */
bool
GapsAndMargins::isTabMarginScaleProportionatelyForTabSelected(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginScaleProportionatelySelected, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginScaleProportionatelySelected[tabIndex];
}

/**
 * Set tab margin scale proportionately selection status.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param selected
 *     New selection status.
 */
void
GapsAndMargins::setTabMarginScaleProportionatelyForTabSelected(const int32_t tabIndex,
                                                         const bool selected)
{
    CaretAssertArrayIndex(m_tabMarginScaleProportionatelySelected, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginScaleProportionatelySelected[tabIndex] = selected;
}

/**
 * Set the selected status for all tabs scale proportionately property.
 *
 * @param selected
 *     New selected status.
 */
void
GapsAndMargins::setScaleProportionatelyForAll(const bool selected)
{
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        setTabMarginScaleProportionatelyForTabSelected(iTab, selected);
    }
}

/**
 * Is the margin value gui control enabled for the given tab?
 *
 * @param tabIndex
 *     Index of the tab.
 * @param applyTabOneToAllSelected
 *     Is apply tab one to all tabs selected for the margin?
 * @param topMarginFlag
 *     True if this is the top margin.
 * @return
 *     True if the the margin for the given tab is enabled, else false.
 */
bool
GapsAndMargins::isTabMarginGuiControlEnabled(const int32_t tabIndex,
                                   const bool applyTabOneToAllSelected,
                                   const bool topMarginFlag) const
{
    bool marginEnabled = true;

    /*
     * If apply tab one to all is selected, only first tab (index zero) is enabled.
     */
    if (applyTabOneToAllSelected) {
        if (tabIndex > 0) {
            marginEnabled = false;
        }
        
    }
    
    if (marginEnabled) {
        /*
         * If scale proportionately is enabled for tab, only top margin is enabled.
         */
        CaretAssertArrayIndex(m_tabMarginScaleProportionatelySelected, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
        if (m_tabMarginScaleProportionatelySelected[tabIndex]) {
            if ( ! topMarginFlag) {
                marginEnabled = false;
            }
        }
    }
    
    return marginEnabled;
}


/**
 * @return Is margin left gui control enabled for the given tab?
 *
 * @param tabIndex
 *     Index of the tab.
 */
bool
GapsAndMargins::isMarginLeftForTabGuiControlEnabled(const int32_t tabIndex) const
{
    return isTabMarginGuiControlEnabled(tabIndex,
                              m_tabMarginLeftApplyTabOneToAllSelected,
                              false);
}

/**
 * @return Is margin right gui control enabled for the given tab?
 *
 * @param tabIndex
 *     Index of the tab.
 */
bool
GapsAndMargins::isMarginRightForTabGuiControlEnabled(const int32_t tabIndex) const
{
    return isTabMarginGuiControlEnabled(tabIndex,
                              m_tabMarginRightApplyTabOneToAllSelected,
                              false);
}

/**
 * @return Is margin top gui control enabled for the given tab?
 *
 * @param tabIndex
 *     Index of the tab.
 */
bool
GapsAndMargins::isMarginTopForTabGuiControlEnabled(const int32_t tabIndex) const
{
    return isTabMarginGuiControlEnabled(tabIndex,
                              m_tabMarginTopApplyTabOneToAllSelected,
                              true);
}

/**
 * @return Is margin bottom gui control enabled for the given tab?
 *
 * @param tabIndex
 *     Index of the tab.
 */
bool
GapsAndMargins::isMarginBottomForTabGuiControlEnabled(const int32_t tabIndex) const
{
    return isTabMarginGuiControlEnabled(tabIndex,
                              m_tabMarginBottomApplyTabOneToAllSelected,
                              false);
}

/**
 * @return Is scale proportionately enabled for the given tab?
 *
 * @param tabIndex
 *     Index of the tab.
 */
bool
GapsAndMargins::isTabMarginScaleProportionatelyForTabEnabled(const int32_t /*tabIndex*/) const
{
    return true;
}

/**
 * Get the margins for drawing a tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param viewportWidth
 *     Width of viewport.
 * @param viewportHeight
 *     Height of viewport.
 * @param MarginOut
 *     Margin for
 * @param MarginOut
 *     Margin for
 * @param MarginOut
 *     Margin for
 * @param MarginOut
 *     Margin for
 */
void
GapsAndMargins::getMarginsInPixelsForDrawing(const int32_t tabIndex,
                                             const int32_t viewportWidth,
                                             const int32_t viewportHeight,
                                             int32_t& leftMarginOut,
                                             int32_t& rightMarginOut,
                                             int32_t& bottomMarginOut,
                                             int32_t& topMarginOut) const
{
    leftMarginOut   = 0;
    rightMarginOut  = 0;
    bottomMarginOut = 0;
    topMarginOut    = 0;

    if (isTabMarginScaleProportionatelyForTabSelected(tabIndex)) {
        const float margin = static_cast<int32_t>(viewportHeight * (getMarginTopForTab(tabIndex) / 100.0));
        topMarginOut    = margin;
        bottomMarginOut = margin;
        leftMarginOut   = margin;
        rightMarginOut  = margin;
    }
    else {
        topMarginOut    = static_cast<int32_t>(viewportHeight * (getMarginTopForTab(tabIndex)    / 100.0));
        bottomMarginOut = static_cast<int32_t>(viewportHeight * (getMarginBottomForTab(tabIndex) / 100.0));
        leftMarginOut   = static_cast<int32_t>(viewportWidth  * (getMarginLeftForTab(tabIndex)   / 100.0));
        rightMarginOut  = static_cast<int32_t>(viewportWidth  * (getMarginRightForTab(tabIndex)  / 100.0));
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

