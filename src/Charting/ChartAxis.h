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

#include "ChartAxisUnitsEnum.h"

namespace caret {

    class ChartAxis : public CaretObject {
        
    public:
        enum Axis {
            AXIS_BOTTOM,
            AXIS_LEFT,
            AXIS_RIGHT,
            AXIS_TOP
        };
        
        ChartAxis(const Axis axis);
        
        virtual ~ChartAxis();
        
        Axis getAxis() const;
        
        AString getText() const;
        
        void setText(const AString& text);
        
        ChartAxisUnitsEnum::Enum getAxisUnits() const;

        void setAxisUnits(const ChartAxisUnitsEnum::Enum axisUnits);
        
        float getMinimumValue() const;
        
        void setMinimumValue(const float minimumValue);
        
        float getMaximumValue() const;
        
        void setMaximumValue(const float maximumValue);
        
        bool isMinimumMaximumValueValid();
        
        int32_t getLabelFontSize() const;
        
        void setLabelFontSize(const float fontSize);
        
        bool isAutoRangeScale() const;
        
        void setAutoRangeScale(const bool autoRangeScale);
        
        bool isVisible() const;
        
        void setVisible(const bool visible);
        
    private:
        ChartAxis(const ChartAxis&);

        ChartAxis& operator=(const ChartAxis&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        const Axis m_axis;
        
        AString m_text;
        
        ChartAxisUnitsEnum::Enum m_axisUnits;
        
        float m_maximumValue;
        
        float m_minimumValue;
        
        int32_t m_labelFontSize;
        
        bool m_visible;
        
        bool m_autoRangeScale;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_AXIS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_AXIS_DECLARE__

} // namespace
#endif  //__CHART_AXIS_H__
