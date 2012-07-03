#ifndef __MODEL_YOKING_H__
#define __MODEL_YOKING_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "EventListenerInterface.h"
#include "Model.h"
#include "ModelVolumeInterface.h"

#include "VolumeSliceCoordinateSelection.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSliceViewModeEnum.h"

namespace caret {

    class Brain;
    
    /// Model information for yoking (contains transforms and volume slice info for volume yoking)
    class ModelYokingGroup: public Model, public ModelVolumeInterface, public EventListenerInterface {
        
    public:       
        enum YokingType {
            YOKING_TYPE_SURFACE,
            YOKING_TYPE_VOLUME
        };
        
        ModelYokingGroup(Brain* brain,
                         const YokingType yokingType,
                         const AString& yokingName);
        
        virtual ~ModelYokingGroup();
        
        AString getYokingName() const;
        
        void initializeOverlays();
        
        void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        VolumeSliceViewPlaneEnum::Enum getSliceViewPlane(const int32_t windowTabNumber) const;
        
        void setSliceViewPlane(const int32_t windowTabNumber,
                              VolumeSliceViewPlaneEnum::Enum sliceAxisMode);
        
        VolumeSliceViewModeEnum::Enum getSliceViewMode(const int32_t windowTabNumber) const;
        
        void setSliceViewMode(const int32_t windowTabNumber,
                              VolumeSliceViewModeEnum::Enum sliceViewMode);
                
        int32_t getMontageNumberOfColumns(const int32_t windowTabNumber) const;
        
        void setMontageNumberOfColumns(const int32_t windowTabNumber,
                                    const int32_t montageNumberOfColumns);
        
        int32_t getMontageNumberOfRows(const int32_t windowTabNumber) const;
        
        void setMontageNumberOfRows(const int32_t windowTabNumber,
                                    const int32_t montageNumberOfRows);
        
        int32_t getMontageSliceSpacing(const int32_t windowTabNumber) const;
        
        void setMontageSliceSpacing(const int32_t windowTabNumber,
                                    const int32_t montageSliceSpacing);
        
        VolumeSliceCoordinateSelection* getSelectedVolumeSlices(const int32_t windowTabNumber);
        
        const VolumeSliceCoordinateSelection* getSelectedVolumeSlices(const int32_t windowTabNumber) const;
        
        virtual void setSlicesToOrigin(const int32_t windowTabNumber);
        
        virtual void copyTransformationsAndViews(const Model& controller,
                                                 const int32_t windowTabNumberSource,
                                                 const int32_t windowTabNumberTarget);
        
        YokingType getYokingType() const;
        
        bool isSurfaceYoking() const;
        
        bool isVolumeYoking() const;
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass);
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
        ModelYokingGroup(const ModelYokingGroup&);
        
        ModelYokingGroup& operator=(const ModelYokingGroup&);
        
        void initializeMembersModelYoking();
        
        /** Axis of slice being viewed */
        VolumeSliceViewPlaneEnum::Enum m_sliceViewPlane;
        
        /** Type of slice viewing */
        VolumeSliceViewModeEnum::Enum m_sliceViewMode;
        
        /** Number of montage rows */
        int32_t m_montageNumberOfRows;
        
        /** Number of montage columns */
        int32_t m_montageNumberOfColumns;
        
        /** Montage slice spacing */
        int32_t m_montageSliceSpacing;
        
        /** Selected volume slices */
        mutable VolumeSliceCoordinateSelection m_volumeSlicesSelected;
        
        /** Name of yoking */
        AString m_yokingName;
        
        /** Type of yoking */
        YokingType m_yokingType;
    };

} // namespace

#endif // __MODEL_YOKING_H__
