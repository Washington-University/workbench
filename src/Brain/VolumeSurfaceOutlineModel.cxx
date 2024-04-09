
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__
#include "VolumeSurfaceOutlineModel.h"
#undef __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__

#include "CaretPreferences.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SessionManager.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceTypeEnum.h"
#include "VolumeMappableInterface.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModelCacheValue.h"

using namespace caret;

static const bool debugFlag(false);
    
/**
 * \class VolumeSurfaceOutlineSelection 
 * \brief Controls display of a volume surface outline.
 *
 * Controls display of a volume surface outline.
 */
/**
 * Constructor.
 */
VolumeSurfaceOutlineModel::VolumeSurfaceOutlineModel()
: CaretObject()
{
    std::vector<SurfaceTypeEnum::Enum> validSurfaceTypes;
    validSurfaceTypes.push_back(SurfaceTypeEnum::ANATOMICAL);
    validSurfaceTypes.push_back(SurfaceTypeEnum::RECONSTRUCTION);
    validSurfaceTypes.push_back(SurfaceTypeEnum::INFLATED);
    validSurfaceTypes.push_back(SurfaceTypeEnum::VERY_INFLATED);
    validSurfaceTypes.push_back(SurfaceTypeEnum::UNKNOWN);
    
    m_displayed = false;
    m_thicknessPixelsObsolete = VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PIXELS_OBSOLETE;
    m_surfaceSelectionModel = new SurfaceSelectionModel(validSurfaceTypes);
    m_colorOrTabModel = new VolumeSurfaceOutlineColorOrTabModel();
    m_slicePlaneDepth = 0.0;
    m_drawingMode = VolumeSurfaceOutlineDrawingModeEnum::LINES;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_displayed", &m_displayed);
    m_sceneAssistant->add("m_thickness", &m_thicknessPixelsObsolete); // NOTE: "m_thickness" is OLD name
    m_sceneAssistant->add("m_surfaceSelectionModel", "SurfaceSelectionModel", m_surfaceSelectionModel);
    m_sceneAssistant->add("m_colorOrTabModel", "VolumeSurfaceOutlineColorOrTabModel", m_colorOrTabModel);
    m_sceneAssistant->add("m_slicePlaneDepth", &m_slicePlaneDepth);
    m_sceneAssistant->add<VolumeSurfaceOutlineDrawingModeEnum, VolumeSurfaceOutlineDrawingModeEnum::Enum>("m_drawingMode",
                                                                                                          &m_drawingMode);
    
    /*
     * Percentage viewport height thickness was added in Feb 2, 2017.
     * So, we set the perentage thickness to -1.0f so that when an older
     * scene is restored, the value will be -1.0f.  The code that draws
     * the volume surface outline see this negative value and convert
     * the old pixel thickness to this new percentage thickness.
     * After adding to the scene assistant, default the percentage thickness.
     */
    m_thicknessPercentageViewportHeight = -1.0f;
    m_sceneAssistant->add("m_thicknessPercentageViewportHeight", &m_thicknessPercentageViewportHeight);
    m_thicknessPercentageViewportHeight = VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PERCENTAGE_VIEWPORT_HEIGHT;
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE);
    
    /*
     * Need to clear cache after files added or removed
     */
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_DATA_FILE_ADD);
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_DATA_FILE_DELETE);
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_DATA_FILE_RELOAD);
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_DATA_FILE_RELOAD_ALL);
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineModel::~VolumeSurfaceOutlineModel()
{
    clearOutlineCache();
    
    delete m_surfaceSelectionModel;
    delete m_colorOrTabModel;
    
    delete m_sceneAssistant;
    
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Copy the given volume surface outline model to this model.
 * @param modelToCopy
 *    Model that is copied.
 */
void 
VolumeSurfaceOutlineModel::copyVolumeSurfaceOutlineModel(VolumeSurfaceOutlineModel* modelToCopy)
{
    m_displayed = modelToCopy->m_displayed;
    m_thicknessPixelsObsolete = modelToCopy->m_thicknessPixelsObsolete;
    m_thicknessPercentageViewportHeight = modelToCopy->m_thicknessPercentageViewportHeight;
    m_surfaceSelectionModel->setSurface(modelToCopy->getSurface());
    m_slicePlaneDepth = modelToCopy->getSlicePlaneDepth();
    m_drawingMode = modelToCopy->m_drawingMode;
    VolumeSurfaceOutlineColorOrTabModel* colorTabToCopy = modelToCopy->getColorOrTabModel();
    m_colorOrTabModel->copyVolumeSurfaceOutlineColorOrTabModel(colorTabToCopy);
    
    clearOutlineCache();
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
VolumeSurfaceOutlineModel::receiveEvent(Event* event)
{
    CaretAssert(event);
    
    if (event->getEventType() == EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE) {
        EventSurfaceColoringInvalidate* colorEvent = dynamic_cast<EventSurfaceColoringInvalidate*>(event);
        CaretAssert(colorEvent);
        colorEvent->setEventProcessed();
        
        switch (m_colorOrTabModel->getSelectedItem()->getItemType()) {
            case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
                /*
                 * Color of surface may have changed
                 */
                clearOutlineCache();
                break;
            case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
                clearOutlineCache();
                break;
        }
    }
    else if (   (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_ADD)
             || (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_DELETE)
             || (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_READ)
             || (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_RELOAD)
             || (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_RELOAD_ALL)) {
        /*
         * File that affects outlines may have been added or removed
         */
        clearOutlineCache();
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeSurfaceOutlineModel::toString() const
{
    return "VolumeSurfaceOutlineSelection";
}

/**
 * @return Is this surface outline displayed?
 */
bool 
VolumeSurfaceOutlineModel::isDisplayed() const
{
    return m_displayed;
}

/**
 * Set the display status of the surface outline.
 * @param displayed
 *    New display status.
 */
void 
VolumeSurfaceOutlineModel::setDisplayed(const bool displayed)
{
    m_displayed = displayed;
    
    if ( ! m_displayed) {
        clearOutlineCache();
    }
}

/**
 * @return Thickness for drawing surface outline
 */
float
VolumeSurfaceOutlineModel::getThicknessPercentageViewportHeight() const
{
    return m_thicknessPercentageViewportHeight;
}

/**
 * Set the thickness for drawing surface outline
 */
void
VolumeSurfaceOutlineModel::setThicknessPercentageViewportHeight(const float thickness)
{
    m_thicknessPercentageViewportHeight = thickness;
}

/**
 * @return Thickness for drawing surface (OBSOLETE)
 */
float 
VolumeSurfaceOutlineModel::getThicknessPixelsObsolete() const
{
    return m_thicknessPixelsObsolete;
}

/**
 * Set the thickness for drawing the surface (OBSOLETE)
 * @param thickness
 *    New value for thickness.
 */
void 
VolumeSurfaceOutlineModel::setThicknessPixelsObsolete(const float thickness)
{
    m_thicknessPixelsObsolete = thickness;
}

/**
 * @return The slice plane depth
 */
float
VolumeSurfaceOutlineModel::getSlicePlaneDepth() const
{
    return m_slicePlaneDepth;;
}

/**
 * Set the slice plane depth
 * @param slicePlaneDepth
 *   New slice plane depth value
 */
void
VolumeSurfaceOutlineModel::setSlicePlaneDepth(const float slicePlaneDepth)
{
    m_slicePlaneDepth = slicePlaneDepth;
}

/**
 * @return  The surface selector used to select the surface.
 */
SurfaceSelectionModel* 
VolumeSurfaceOutlineModel::getSurfaceSelectionModel()
{
    return m_surfaceSelectionModel;
}

/**
 * @return  Get the selected surface.
 */
const Surface* 
VolumeSurfaceOutlineModel::getSurface() const
{
    return m_surfaceSelectionModel->getSurface();
}

/**
 * @return  Get the selected surface.
 */
Surface* 
VolumeSurfaceOutlineModel::getSurface()
{
    return m_surfaceSelectionModel->getSurface();
}

/**
 * @return The model for color or tab selection.
 */
VolumeSurfaceOutlineColorOrTabModel* 
VolumeSurfaceOutlineModel::getColorOrTabModel()
{
    return m_colorOrTabModel;
}

/**
 * @return The model for color or tab selection.
 */
const VolumeSurfaceOutlineColorOrTabModel* 
VolumeSurfaceOutlineModel::getColorOrTabModel() const
{
    return m_colorOrTabModel;
}

VolumeSurfaceOutlineDrawingModeEnum::Enum
VolumeSurfaceOutlineModel::getDrawingMode() const
{
    return m_drawingMode;
}

void
VolumeSurfaceOutlineModel::setDrawingMode(const VolumeSurfaceOutlineDrawingModeEnum::Enum drawingMode)
{
    m_drawingMode = drawingMode;
}

/**
 * @return Is a drawing lines mode selected
 */
bool
VolumeSurfaceOutlineModel::isDrawLinesModeSelected() const
{
    bool status(true);
    switch (m_drawingMode) {
        case VolumeSurfaceOutlineDrawingModeEnum::BOTH:
            break;
        case VolumeSurfaceOutlineDrawingModeEnum::LINES:
            break;
        case VolumeSurfaceOutlineDrawingModeEnum::SURFACE:
            status = false;
            break;
    }
    return status;
}

/**
 * @return Is a drawing surface mode selected
 */
bool
VolumeSurfaceOutlineModel::isDrawSurfaceModeSelected() const
{
    bool status(true);
    switch (m_drawingMode) {
        case VolumeSurfaceOutlineDrawingModeEnum::BOTH:
            break;
        case VolumeSurfaceOutlineDrawingModeEnum::LINES:
            status = false;
            break;
        case VolumeSurfaceOutlineDrawingModeEnum::SURFACE:
            break;
    }
    return status;
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
VolumeSurfaceOutlineModel::saveToScene(const SceneAttributes* sceneAttributes,
                                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeSurfaceOutlineModel",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
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
VolumeSurfaceOutlineModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    clearOutlineCache();
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
}

/**
 * Set the outline primitives for the given cache key
 *
 * @param histologySlice
 *    The histology slice
 * @param underlayVolume
 *    The underlay volume
 * @param key
 *     Key into the outline cache identifying axis and slice
 * @param primitivesOut
 *     Input containing the primitives
 */
void
VolumeSurfaceOutlineModel::setOutlineCachePrimitives(const HistologySlice*          histologySlice,
                                                     const VolumeMappableInterface* underlayVolume,
                                                     const VolumeSurfaceOutlineModelCacheKey& key,
                                                     const std::vector<GraphicsPrimitive*>& primitives)
{
    auto iter = m_outlineCache.find(key);
    if (iter != m_outlineCache.end()) {
        iter->second->setGraphicsPrimitive(primitives);
        return;
    }
    
    if (m_outlineCache.empty()) {
        m_outlineCacheInfo.update(this,
                                  histologySlice,
                                  underlayVolume);
    }
    
    if (debugFlag) {
        std::cout << "Adding " << key.toString() << std::endl;
    }
    VolumeSurfaceOutlineModelCacheValue* value = new VolumeSurfaceOutlineModelCacheValue();
    value->setGraphicsPrimitive(primitives);
    m_outlineCache.insert(std::make_pair(key, value));
}


/**
 * Get the outline primitives for the given cache key
 *
 * @param histologySlice
 *    The histology slice
 * @param underlayVolume
 *    The underlay volume
 * @param key
 *     Key into the outline cache identifying axis and slice
 * @param primitivesOut
 *     Output containing the primitives
 * @return
 *     Truie if outline primitives valid, else false.
 */
bool
VolumeSurfaceOutlineModel::getOutlineCachePrimitives(const HistologySlice*          histologySlice,
                                                     const VolumeMappableInterface* underlayVolume,
                                                     const VolumeSurfaceOutlineModelCacheKey& key,
                                                     std::vector<GraphicsPrimitive*>& primitivesOut)
{
    if ( ! m_outlineCacheInfo.isValid(this,
                                      histologySlice,
                                      underlayVolume)) {
        clearOutlineCache();
    }

    /*
     * Don't let the cache become too big.
     * They do occupy buffers in the graphics memory
     * so we don't want to use too much of it.
     */
    const int32_t maximumCacheSize(100);
    if (m_outlineCache.size() > maximumCacheSize) {
        clearOutlineCache();
    }
    
    auto iter = m_outlineCache.find(key);
    if (iter != m_outlineCache.end()) {
        primitivesOut = iter->second->getGraphicsPrimitives();
        if (debugFlag) {
            std::cout << "Found " << iter->first.toString() << std::endl;
        }
        return true;
    }
    
    if (debugFlag) {
        std::cout << m_outlineCache.size() << " items in outline cache" << std::endl;
    }
    return false;
}

/**
 * Clear the outline cache
 */
void
VolumeSurfaceOutlineModel::clearOutlineCache()
{
    if (debugFlag) {
        if ( ! m_outlineCache.empty()) {
            std::cout << "Invalidating non-empty surface outline cache" << std::endl;
        }
    }
    m_outlineCacheInfo.clear();
    
    for (auto iter : m_outlineCache) {
        delete iter.second;
    }
    m_outlineCache.clear();
}

/* ==========================================================================================*/

/**
 * Constructor for outline cache
 */
VolumeSurfaceOutlineModel::OutlineCacheInfo::OutlineCacheInfo()
{
    clear();
}

/**
 * Destructor
 */
VolumeSurfaceOutlineModel::OutlineCacheInfo::~OutlineCacheInfo()
{
    clear();
}

/**
 * Clear the outline cache
 */
void
VolumeSurfaceOutlineModel::OutlineCacheInfo::clear()
{
    m_surface = NULL;
    m_thicknessPercentageViewportHeight = -1.0;
    m_slicePlaneDepth = 0.0;
    m_preferencesVolumeSurfaceOutlineSeparation = -100.0;
    m_colorItem.reset();
}

/**
 * Is the outline cache valid?
 *
 * @param surfaceOutlineModel
 *     The parent surface outline model
 * @param histologySlice
 *    The histology slice
 * @param underlayVolume
 *    The underlay volume
 * @return
 *     True if cache is valid, else false
 */
bool
VolumeSurfaceOutlineModel::OutlineCacheInfo::isValid(VolumeSurfaceOutlineModel* surfaceOutlineModel,
                                                     const HistologySlice*          histologySlice,
                                                     const VolumeMappableInterface* underlayVolume)
{
    bool validFlag(false);
    if (m_surface != NULL) {
        const bool histologyMatchFlag((m_histologySlice == histologySlice)
                                      && (m_histologySlice != NULL));
        const bool volumeMatchFlag((m_underlayVolume    == underlayVolume)
                                   && (m_underlayVolume != NULL));
        if (histologyMatchFlag) {
            CaretAssert( ! volumeMatchFlag);
        }
        else if (volumeMatchFlag) {
            CaretAssert( ! histologyMatchFlag);
        }
        CaretPreferences* prefs(SessionManager::get()->getCaretPreferences());
        CaretAssert(prefs);

        if ((m_surface == surfaceOutlineModel->getSurface())
            && (histologyMatchFlag
                || volumeMatchFlag)
            && (m_thicknessPercentageViewportHeight == surfaceOutlineModel->getThicknessPercentageViewportHeight())
            && (m_slicePlaneDepth == surfaceOutlineModel->getSlicePlaneDepth())
            && (m_preferencesVolumeSurfaceOutlineSeparation == prefs->getVolumeSurfaceOutlineSeparation())) {
            if (m_colorItem != NULL) {
                if (m_colorItem->equals(*(surfaceOutlineModel->getColorOrTabModel()->getSelectedItem()))) {
                    validFlag = true;
                }
            }
        }
    }
    
    return validFlag;
}

/**
 * Update the cache info from the parent surface outline model
 *
 * @param surfaceOutlineModel
 *     The surface outline model
 * @param histologySlice
 *    The histology slice
 * @param underlayVolume
 *    The underlay volume
 */
void
VolumeSurfaceOutlineModel::OutlineCacheInfo::update(VolumeSurfaceOutlineModel* surfaceOutlineModel,
                                                    const HistologySlice*          histologySlice,
                                                    const VolumeMappableInterface* underlayVolume)
{
    const bool histologyMatchFlag((m_histologySlice == histologySlice)
                                  && (m_histologySlice != NULL));
    const bool volumeMatchFlag((m_underlayVolume    == underlayVolume)
                               && (m_underlayVolume != NULL));
    if (histologyMatchFlag) {
        CaretAssert( ! volumeMatchFlag);
    }
    else if (volumeMatchFlag) {
        CaretAssert( ! histologyMatchFlag);
    }
    m_histologySlice = histologySlice;
    m_underlayVolume = underlayVolume;
    m_surface = surfaceOutlineModel->getSurface();
    m_thicknessPercentageViewportHeight = surfaceOutlineModel->getThicknessPercentageViewportHeight();
    m_slicePlaneDepth = surfaceOutlineModel->getSlicePlaneDepth();
    m_colorItem.reset(new VolumeSurfaceOutlineColorOrTabModel::Item(*(surfaceOutlineModel->getColorOrTabModel()->getSelectedItem())));

    CaretPreferences* prefs(SessionManager::get()->getCaretPreferences());
    CaretAssert(prefs);
    m_preferencesVolumeSurfaceOutlineSeparation = prefs->getVolumeSurfaceOutlineSeparation();
}

/**
 * @return Default value for an outline drawn using a surface model depth in millimeters
 */
float
VolumeSurfaceOutlineModel::getDefaultSurfaceDepthMillimeters()
{
    return 0.5;
}

