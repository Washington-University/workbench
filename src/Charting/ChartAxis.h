#ifndef __CHART_AXIS_H__
#define __CHART_AXIS_H__

/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
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


#include "CaretObject.h"

#include "ChartAxisLocationEnum.h"
#include "ChartAxisTypeEnum.h"
#include "ChartAxisUnitsEnum.h"
#include "SceneableInterface.h"

namespace caret {
    class ChartModel;
    class SceneClassAssistant;

    class ChartAxis : public CaretObject, public SceneableInterface {
        
    public:
        enum Axis {
            AXIS_BOTTOM,
            AXIS_LEFT,
            AXIS_RIGHT,
            AXIS_TOP
        };
        
        static ChartAxis* newChartAxisForTypeAndLocation(const ChartAxisTypeEnum::Enum axisType,
                                                         const ChartAxisLocationEnum::Enum axisLocation);
        
        virtual ~ChartAxis();
        
        ChartAxis(const ChartAxis&);
        
        ChartAxis& operator=(const ChartAxis&);
        
        /**
         * At times a copy of chart axis will be needed BUT it must be
         * the proper subclass so copy constructor and assignment operator
         * will function when this abstract, base class is used.  Each
         * subclass will override this method so that the returned class
         * is of the proper type.
         *
         * @return Copy of this instance that is the actual subclass.
         */
        virtual ChartAxis* clone() const = 0;

        void setParentChartModel(ChartModel* parentChartModel);
        
        ChartAxisTypeEnum::Enum getAxisType() const;
        
        ChartAxisLocationEnum::Enum getAxisLocation() const;
        
        AString getText() const;
        
        void setText(const AString& text);
        
        ChartAxisUnitsEnum::Enum getAxisUnits() const;

        void setAxisUnits(const ChartAxisUnitsEnum::Enum axisUnits);
        
        AString getAxisUnitsSuffix() const;
        
        int32_t getLabelFontSize() const;
        
        void setLabelFontSize(const float fontSize);
        
        bool isAutoRangeScaleEnabled() const;
        
        void setAutoRangeScaleEnabled(const bool autoRangeScaleEnabled);
        
        bool isVisible() const;
        
        void setVisible(const bool visible);
        
        /**
         * Update for auto range scale.
         */
        virtual void updateForAutoRangeScale() = 0;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    protected:
        ChartAxis(const ChartAxisTypeEnum::Enum axisType,
                  const ChartAxisLocationEnum::Enum axisLocation);
        
        
        ChartModel* getParentChartModel();
        
        const ChartModel* getParentChartModel() const;
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) = 0;
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) = 0;
        
    private:
        void copyHelperChartAxis(const ChartAxis& obj);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void initializeMembersChartAxis();
        
        ChartAxisTypeEnum::Enum m_axisType;
        
        ChartAxisLocationEnum::Enum m_axisLocation;
        
        ChartModel* m_parentChartModel;
        
        AString m_text;
        
        ChartAxisUnitsEnum::Enum m_axisUnits;
        
        int32_t m_labelFontSize;
        
        bool m_visible;
        
        bool m_autoRangeScaleEnabled;
        
        /** helps with scene save/restore */
        SceneClassAssistant* m_sceneAssistant;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_AXIS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_AXIS_DECLARE__

} // namespace
#endif  //__CHART_AXIS_H__
