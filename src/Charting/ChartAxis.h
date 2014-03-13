#ifndef __CHART_AXIS_H__
#define __CHART_AXIS_H__

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
