
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __SAMPLES_DRAWING_SETTINGS_DECLARE__
#include "SamplesDrawingSettings.h"
#undef __SAMPLES_DRAWING_SETTINGS_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "BrowserTabContent.h"
#include "DrawingViewportContent.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabIndexGetWindowIndex.h"
#include "EventDrawingViewportContentGet.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::SamplesDrawingSettings 
 * \brief Contains settings for drawing samples
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param parentBrowserTabContent
 *    Browser tab content that 'owns' this instance
 *    Note: We just need the index of the browser tab but the index may change from the original value
 *    in the BrowserTabContent's constructor to the tab index when restoring the scene.  Thus, we can
 *    just call BrowserTabContent::getTabNumber() and always get the correct tab index.
 */
SamplesDrawingSettings::SamplesDrawingSettings(BrowserTabContent* parentBrowserTabContent)
: CaretObject(),
m_parentBrowserTabContent(parentBrowserTabContent)
{
    CaretAssert(m_parentBrowserTabContent);
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add<SamplesDrawingModeEnum, SamplesDrawingModeEnum::Enum>("m_drawingMode",
                                                                                &m_drawingMode);
    m_sceneAssistant->add("m_lowSliceIndex",
                          &m_lowSliceIndex);
    m_sceneAssistant->add("m_highSliceIndex",
                          &m_highSliceIndex);
    m_sceneAssistant->add("m_lowerSliceOffset",
                          &m_lowerSliceOffset);
    m_sceneAssistant->add("m_upperSliceOffset",
                          &m_upperSliceOffset);
}

/**
 * Destructor.
 */
SamplesDrawingSettings::~SamplesDrawingSettings()
{
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SamplesDrawingSettings&
SamplesDrawingSettings::operator=(const SamplesDrawingSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperSamplesDrawingSettings(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SamplesDrawingSettings::copyHelperSamplesDrawingSettings(const SamplesDrawingSettings& obj)
{
    m_drawingMode      = obj.m_drawingMode;
    m_lowSliceIndex    = obj.m_lowSliceIndex;
    m_highSliceIndex   = obj.m_highSliceIndex;
    m_lowerSliceOffset = obj.m_lowerSliceOffset;
    m_upperSliceOffset = obj.m_upperSliceOffset;
}

/**
 * @return The samples drawing mode
 */
SamplesDrawingModeEnum::Enum
SamplesDrawingSettings::getDrawingMode() const
{
    return m_drawingMode;
}

/**
 * Set the samples drawing mode
 * @param samplesDrawingMode
 *    New mode
 */
void
SamplesDrawingSettings::setDrawingMode(const SamplesDrawingModeEnum::Enum drawingMode)
{
    m_drawingMode = drawingMode;
}

/**
 * @return A pair containing the minimum and maximum slice indices for the tab
 * in this
 */
std::pair<int32_t, int32_t>
SamplesDrawingSettings::getSliceRange() const
{
    std::pair<int32_t, int32_t> minMaxSliceIndices;
    minMaxSliceIndices.first  = 0;
    minMaxSliceIndices.second = 0;

    EventBrowserTabGet tabEvent(m_parentBrowserTabContent->getTabNumber());
    EventManager::get()->sendEvent(tabEvent.getPointer());
    const BrowserTabContent* tabContent(tabEvent.getBrowserTab());
    if (tabContent != NULL) {
        int32_t numSlices(0);
        switch (tabContent->getVolumeSliceDrawingType()) {
            case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
                numSlices = (tabContent->getVolumeMontageNumberOfRows()
                             * tabContent->getVolumeMontageNumberOfColumns());
                break;
            case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
                numSlices = 1;
                break;
        }
        
        /*
         * Range 1..N
         */
        if (numSlices > 0) {
            minMaxSliceIndices.first = 1;
        }
        minMaxSliceIndices.second = numSlices;
    }

    return minMaxSliceIndices;
}

/**
 * @return True if slice montage is enabled for tab using these settings
 * and the slice at the given row and column is in range for sample drawing
 * when in CUSTOM mode.
 * @param sliceRow
 *    Row of the slice
 * @param sliceColumn
 *    Column of the slice
 */
bool
SamplesDrawingSettings::isSliceInLowerUpperOffsetRange(const int32_t sliceRow,
                                                       const int32_t sliceColumn) const
{
    bool inRangeFlag(false);
    
    EventBrowserTabGet tabEvent(m_parentBrowserTabContent->getTabNumber());
    EventManager::get()->sendEvent(tabEvent.getPointer());
    const BrowserTabContent* tabContent(tabEvent.getBrowserTab());
    
    if (tabContent != NULL) {
        switch (m_drawingMode) {
            case SamplesDrawingModeEnum::ALL_SLICES:
                inRangeFlag = true;
                break;
            case SamplesDrawingModeEnum::EXCLUDE:
                switch (tabContent->getVolumeSliceDrawingType()) {
                    case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
                    {
                        const int32_t numSlices(tabContent->getVolumeMontageNumberOfRows()
                                                * tabContent->getVolumeMontageNumberOfColumns());
                        
                        const int32_t sliceOffset((sliceRow * tabContent->getVolumeMontageNumberOfColumns())
                                                  + sliceColumn);
                        if ((sliceOffset >= m_upperSliceOffset)
                            && (sliceOffset < (numSlices - m_lowerSliceOffset))) {
                            inRangeFlag = true;
                        }
                    }
                        break;
                    case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
                        inRangeFlag = true;
                        break;
                }
                break;
        }
    }
    
    return inRangeFlag;
}


/**
 * @return The samples drawing low slice index
 */
int32_t
SamplesDrawingSettings::getLowSliceIndex() const
{
    const auto minMax(getSliceRange());
    const int32_t minValue(minMax.first);
    const int32_t maxValue(minMax.second);
    
    if (m_lowSliceIndex > maxValue) {
        m_lowSliceIndex = maxValue;
    }
    else if (m_lowSliceIndex < minValue) {
        m_lowSliceIndex = minValue;
    }
    
    switch (m_drawingMode) {
        case SamplesDrawingModeEnum::ALL_SLICES:
            m_lowSliceIndex = minValue;
            break;
        case SamplesDrawingModeEnum::EXCLUDE:
            break;
    }
    
    return m_lowSliceIndex;
}

/**
 * @return The samples drawing high slice index
 */
int32_t
SamplesDrawingSettings::getHighSliceIndex() const
{
    const auto minMax(getSliceRange());
    const int32_t minValue(minMax.first);
    const int32_t maxValue(minMax.second);
    
    if (m_highSliceIndex > maxValue) {
        m_highSliceIndex = maxValue;
    }
    else if (m_highSliceIndex < minValue) {
        m_highSliceIndex = minValue;
    }
    
    switch (m_drawingMode) {
        case SamplesDrawingModeEnum::ALL_SLICES:
            m_highSliceIndex = maxValue;
            break;
        case SamplesDrawingModeEnum::EXCLUDE:
            break;
    }
    
    return m_highSliceIndex;
}

/**
 * Set the samples low slice index
 * @param lowSliceIndex
 *    The sliced index
 */
void
SamplesDrawingSettings::setLowSliceIndex(const int32_t lowSliceIndex)
{
    m_lowSliceIndex = lowSliceIndex;
    if (m_lowSliceIndex >= m_highSliceIndex) {
        m_highSliceIndex = m_lowSliceIndex;// + 1;
    }
}

/**
 * Set the samples high slice index
 * @param highSliceIndex
 *    The sliced index
 */
void
SamplesDrawingSettings::setHighSliceIndex(const int32_t highSliceIndex)
{
    m_highSliceIndex = highSliceIndex;
    if (m_highSliceIndex <= m_lowSliceIndex) {
        m_lowSliceIndex = m_highSliceIndex;// - 1;
    }
}

/**
 * @return The upper slice offset
 */
int32_t
SamplesDrawingSettings::getUpperSliceOffset() const
{
    return m_upperSliceOffset;
}

/**
 * Set the upper slice offset
 * @param sliceOffset
 *    New offset
 */
void
SamplesDrawingSettings::setUpperSliceOffset(const int32_t sliceOffset)
{
    m_upperSliceOffset = sliceOffset;
}

/**
 * @return The lower slice offset
 */
int32_t
SamplesDrawingSettings::getLowerSliceOffset() const
{
    return m_lowerSliceOffset;
}

/**
 * Set the lower slice offset
 * @param sliceOffset
 *    New offset
 */
void
SamplesDrawingSettings::setLowerSliceOffset(const int32_t sliceOffset)
{
    m_lowerSliceOffset = sliceOffset;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SamplesDrawingSettings::toString() const
{
    return "SamplesDrawingSettings";
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
SamplesDrawingSettings::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SamplesDrawingSettings",
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
SamplesDrawingSettings::restoreFromScene(const SceneAttributes* sceneAttributes,
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

