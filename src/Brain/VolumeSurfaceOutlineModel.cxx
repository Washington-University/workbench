
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

#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
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
    
    m_displayed = false;
    m_thicknessPixelsObsolete = VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PIXELS_OBSOLETE;
    m_surfaceSelectionModel = new SurfaceSelectionModel(validSurfaceTypes);
    m_colorOrTabModel = new VolumeSurfaceOutlineColorOrTabModel();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_displayed", &m_displayed);
    m_sceneAssistant->add("m_thickness", &m_thicknessPixelsObsolete); // NOTE: "m_thickness" is OLD name
    m_sceneAssistant->add("m_surfaceSelectionModel", "SurfaceSelectionModel", m_surfaceSelectionModel);
    m_sceneAssistant->add("m_colorOrTabModel", "VolumeSurfaceOutlineColorOrTabModel", m_colorOrTabModel);
    
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
 * @param key
 *     Key into the outline cache identifying axis and slice
 * @param primitivesOut
 *     Input containing the primitives
 */
void
VolumeSurfaceOutlineModel::setOutlineCachePrimitives(const VolumeMappableInterface* underlayVolume,
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
VolumeSurfaceOutlineModel::getOutlineCachePrimitives(const VolumeMappableInterface* underlayVolume,
                                                     const VolumeSurfaceOutlineModelCacheKey& key,
                                                     std::vector<GraphicsPrimitive*>& primitivesOut)
{
    if ( ! m_outlineCacheInfo.isValid(this,
                                      underlayVolume)) {
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
    
    return false;
}

/**
 * Clear the outline cache
 */
void
VolumeSurfaceOutlineModel::clearOutlineCache()
{
    if (debugFlag) {
        std::cout << "Invalidating surface outline cache" << std::endl;
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
    m_colorItem.reset();
}

/**
 * Is the outline cache valid?
 *
 * @param surfaceOutlineModel
 *     The parent surface outline model
 * @param underlayVolume
 *    The underlay volume
 * @return
 *     True if cache is valid, else false
 */
bool
VolumeSurfaceOutlineModel::OutlineCacheInfo::isValid(VolumeSurfaceOutlineModel* surfaceOutlineModel,
                                                     const VolumeMappableInterface* underlayVolume)
{
    bool validFlag(false);
    if (m_surface != NULL) {
        if ((m_surface == surfaceOutlineModel->getSurface())
            && (m_underlayVolume == underlayVolume)
            && (m_thicknessPercentageViewportHeight == surfaceOutlineModel->getThicknessPercentageViewportHeight())) {
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
 * @param underlayVolume
 *    The underlay volume
 */
void
VolumeSurfaceOutlineModel::OutlineCacheInfo::update(VolumeSurfaceOutlineModel* surfaceOutlineModel,
                                                    const VolumeMappableInterface* underlayVolume)
{
    m_underlayVolume = underlayVolume;
    m_surface = surfaceOutlineModel->getSurface();
    m_thicknessPercentageViewportHeight = surfaceOutlineModel->getThicknessPercentageViewportHeight();
    m_colorItem.reset(new VolumeSurfaceOutlineColorOrTabModel::Item(*(surfaceOutlineModel->getColorOrTabModel()->getSelectedItem())));
}
