
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __OVERLAY_SET_ARRAY_DECLARE__
#include "OverlaySetArray.h"
#undef __OVERLAY_SET_ARRAY_DECLARE__

#include "BrainConstants.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabGet.h"
#include "EventOverlayYokingGroupGet.h"
#include "EventManager.h"
#include "OverlaySet.h"

using namespace caret;


    
/**
 * \class caret::OverlaySetArray 
 * \brief Maintains an array of overlay sets for use with a model
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param brainStructure
 *    Brain structure.
 */
OverlaySetArray::OverlaySetArray(BrainStructure* brainStructure)
: CaretObject()
{
    initialize(brainStructure, NULL, NULL, NULL);
}

/**
 * Constructor.
 *
 * @param modelVolume
 *    Volume model.
 */
OverlaySetArray::OverlaySetArray(ModelVolume* modelVolume)
: CaretObject()
{
    initialize(NULL, NULL, modelVolume, NULL);
}

/**
 * Constructor.
 *
 * @param modelWholeBrain
 *    Whole brain model.
 */
OverlaySetArray::OverlaySetArray(ModelWholeBrain* modelWholeBrain)
: CaretObject()
{
    initialize(NULL, NULL, NULL, modelWholeBrain);
}

/**
 * Constructor.
 *
 * @param modelSurfaceMontage
 *    Surface montage model.
 *
 */
OverlaySetArray::OverlaySetArray(ModelSurfaceMontage* modelSurfaceMontage)
: CaretObject()
{
    initialize(NULL, modelSurfaceMontage, NULL, NULL);
}

/**
 * Destructor.
 */
OverlaySetArray::~OverlaySetArray()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (std::vector<OverlaySet*>::iterator iter = m_overlaySets.begin();
         iter != m_overlaySets.end();
         iter++) {
        delete *iter;
    }
    m_overlaySets.clear();
}

/**
 * Initialize this overlay set array.
 *
 * @param brainStructure
 *    Brain structure.
 * @param modelSurfaceMontage
 *    Surface montage model.
 * @param modelVolume
 *    Volume model.
 * @param modelWholeBrain
 *    Whole brain model.
 */
void
OverlaySetArray::initialize(BrainStructure* brainStructure,
                            ModelSurfaceMontage* modelSurfaceMontage,
                            ModelVolume* modelVolume,
                            ModelWholeBrain* modelWholeBrain)
{
    m_brainStructure = NULL;
    m_modelSurfaceMontage = NULL;
    m_modelVolume = NULL;
    m_modelWholeBrain = NULL;
    
    if (brainStructure != NULL) {
        m_brainStructure = brainStructure;
    }
    else if (modelSurfaceMontage != NULL) {
        m_modelSurfaceMontage = modelSurfaceMontage;
    }
    else if (modelVolume != NULL) {
        m_modelVolume = modelVolume;
    }
    else if (modelWholeBrain != NULL) {
        m_modelWholeBrain = modelWholeBrain;
    }
    else {
        CaretAssert(0);
    }
    
    m_overlaySets.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (m_brainStructure != NULL) {
            m_overlaySets[i] = new OverlaySet(m_brainStructure);
        }
        else if (m_modelSurfaceMontage != NULL) {
            m_overlaySets[i] = new OverlaySet(m_modelSurfaceMontage);
        }
        else if (m_modelVolume != NULL) {
            m_overlaySets[i] = new OverlaySet(m_modelVolume);
        }
        else if (m_modelWholeBrain != NULL) {
            m_overlaySets[i] = new OverlaySet(m_modelWholeBrain);
        }
        else {
            CaretAssert(0);
        }
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_OVERLAY_GET_YOKED);
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OverlaySetArray::toString() const
{
    return "OverlaySetArray";
}

/**
 * @return The number of overlay sets.
 */
int32_t
OverlaySetArray::getNumberOfOverlaySets()
{
    return m_overlaySets.size();
}

/**
 * Get the overlay set at the given index.
 *
 * @param indx
 *    Index of overlay set.
 * @return
 *    Overlay set at given index.
 */
OverlaySet*
OverlaySetArray::getOverlaySet(const int32_t indx)
{
    CaretAssertVectorIndex(m_overlaySets, indx);
    
    return m_overlaySets[indx];
}

/**
 * Initialize the overlay selections.
 */
void
OverlaySetArray::initializeOverlaySelections()
{
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        CaretAssertVectorIndex(m_overlaySets, iTab);
        m_overlaySets[iTab]->initializeOverlays();
    }
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
OverlaySetArray::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_DELETE) {
        EventBrowserTabDelete* deleteTabEvent = dynamic_cast<EventBrowserTabDelete*>(event);
        CaretAssert(deleteTabEvent);

        /*
         * Since tab is being deleted, reset any overlay yoking for the tab.
         */
        const int32_t tabIndex = deleteTabEvent->getBrowserTabIndex();
        if ((tabIndex > 0)
            && (tabIndex < getNumberOfOverlaySets())) {
            m_overlaySets[tabIndex]->resetOverlayYokingToOff();
        }
        
        deleteTabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_OVERLAY_GET_YOKED) {
        EventOverlayYokingGroupGet* yokeGroupEvent = dynamic_cast<EventOverlayYokingGroupGet*>(event);
        CaretAssert(yokeGroupEvent);
        const OverlayYokingGroupEnum::Enum requestedYokingGroup = yokeGroupEvent->getYokingGroup();
        
        if (requestedYokingGroup != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
            AString modelName;
            if (m_brainStructure != NULL) {
                modelName = "Structure " + StructureEnum::toGuiName(m_brainStructure->getStructure());
            }
            else if (m_modelSurfaceMontage != NULL) {
                modelName = "Surface Montage View";
            }
            else if (m_modelVolume != NULL) {
                modelName = "Volume View";
            }
            else if (m_modelWholeBrain != NULL) {
                modelName = "All View";
            }
            
            /*
             * Find all overlays with the requested yoking
             */
            for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
                CaretAssertVectorIndex(m_overlaySets, iTab);
                OverlaySet* overlaySet = m_overlaySets[iTab];
                const int32_t overlayCount = overlaySet->getNumberOfDisplayedOverlays();
                for (int32_t j = 0; j < overlayCount; j++) {
                    Overlay* overlay = overlaySet->getOverlay(j);
                    if (overlay->getYokingGroup() == requestedYokingGroup) {
                        yokeGroupEvent->addYokedOverlay(modelName,
                                                        iTab,
                                                        overlay);
                    }
                }
            }
        }
        
        yokeGroupEvent->setEventProcessed();
    }
}

