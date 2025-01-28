#ifndef __SAMPLES_DRAWING_SETTINGS_H__
#define __SAMPLES_DRAWING_SETTINGS_H__

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



#include <memory>

#include "AnnotationPolyhedronTypeEnum.h"
#include "CaretObject.h"
#include "SamplesDrawingModeEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class BrowserTabContent;
    class SceneClassAssistant;

    class SamplesDrawingSettings : public CaretObject, public SceneableInterface {
        
    public:
        SamplesDrawingSettings(BrowserTabContent* parentBrowserTabContent);
        
        virtual ~SamplesDrawingSettings();
        
        /* implmentation requires management of the parent browser tab content */
        SamplesDrawingSettings(const SamplesDrawingSettings& obj) = delete;

        SamplesDrawingSettings& operator=(const SamplesDrawingSettings& obj);
        
        SamplesDrawingModeEnum::Enum getDrawingMode() const;
        
        void setDrawingMode(const SamplesDrawingModeEnum::Enum samplesDrawingMode);
        
        AnnotationPolyhedronTypeEnum::Enum getPolyhedronDrawingType() const;
        
        void setPolyhedronDrawingType(const AnnotationPolyhedronTypeEnum::Enum polyhedronDrawingType);
        
        std::pair<int32_t, int32_t> getSliceRange() const;
        
        int32_t getLowSliceIndex() const;
        
        int32_t getHighSliceIndex() const;
        
        void setLowSliceIndex(const int32_t lowSliceIndex);
        
        void setHighSliceIndex(const int32_t highSliceIndex);

        int32_t getUpperSliceOffset() const;
        
        void setUpperSliceOffset(const int32_t sliceOffset);
        
        int32_t getLowerSliceOffset() const;
        
        void setLowerSliceOffset(const int32_t sliceOffset);
        
        bool isSliceInLowerUpperOffsetRange(const int32_t sliceRow,
                                            const int32_t sliceColumn) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperSamplesDrawingSettings(const SamplesDrawingSettings& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        const BrowserTabContent* m_parentBrowserTabContent;
        
        SamplesDrawingModeEnum::Enum m_drawingMode = SamplesDrawingModeEnum::ALL_SLICES;
        
        AnnotationPolyhedronTypeEnum::Enum m_polyhedronDrawingType = AnnotationPolyhedronTypeEnum::INVALID;
        
        mutable int32_t m_lowSliceIndex = 0;
        
        mutable int32_t m_highSliceIndex = 0;
        
        int32_t m_upperSliceOffset = 0;
        
        int32_t m_lowerSliceOffset = 0;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SAMPLES_DRAWING_SETTINGS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SAMPLES_DRAWING_SETTINGS_DECLARE__

} // namespace
#endif  //__SAMPLES_DRAWING_SETTINGS_H__
