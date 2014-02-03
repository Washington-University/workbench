#ifndef __CHART_DATA_CARTESIAN_H__
#define __CHART_DATA_CARTESIAN_H__

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

#include "CaretColorEnum.h"
#include "ChartAxisUnitsEnum.h"
#include "ChartData.h"



namespace caret {

    class ChartPoint;
    
    class ChartDataCartesian : public ChartData {
        
    public:
        ChartDataCartesian(const ChartDataTypeEnum::Enum chartDataType,
                                const ChartAxisUnitsEnum::Enum dataAxisUnitsX,
                                const ChartAxisUnitsEnum::Enum dataAxisUnitsY);
        
        virtual ~ChartDataCartesian();
        
        virtual ChartData* clone();
        
        void addPoint(const float x,
                      const float y);
        
        int32_t getNumberOfPoints() const;
        
        const ChartPoint* getPointAtIndex(const int32_t pointIndex) const;
        
        void getBounds(float& xMinimumOut,
                       float& xMaximumOut,
                       float& yMinimumOut,
                       float& yMaximumOut) const;
        
        ChartAxisUnitsEnum::Enum getDataAxisUnitsX();
        
        ChartAxisUnitsEnum::Enum getDataAxisUnitsY();
        
        CaretColorEnum::Enum getColor();
        
        float getTimeStartInSecondsAxisX() const;
        
        void setTimeStartInSecondsAxisX(const float timeStart);
        
        float getTimeStepInSecondsAxisX() const;
        
        void setTimeStepInSecondsAxisX(const float timeStep);
        
        AString getDescription() const;
        
        void setDescription(const AString& description);
        
        AString getEncodedDataSource() const;
        
        void setEncodedDataSource(const AString& encodedDataSource);

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);
        
        
    private:
        ChartDataCartesian(const ChartDataCartesian& obj);
        
        ChartDataCartesian& operator=(const ChartDataCartesian& obj);
        
        void copyHelperChartDataCartesian(const ChartDataCartesian& obj);

        void initializeMembersChartDataCartesian();
        
        void removeAllPoints();
        
        std::vector<ChartPoint*> m_points;
        
        mutable float m_bounds[6];
        
        mutable bool m_boundsValid;
        
        ChartAxisUnitsEnum::Enum m_dataAxisUnitsX;
        
        ChartAxisUnitsEnum::Enum m_dataAxisUnitsY;
        
        CaretColorEnum::Enum m_color;
        
        float m_timeStartInSecondsAxisX;
        
        float m_timeStepInSecondsAxisX;
        
        AString m_description;
        
        AString m_encodedDataSource;
        
        static int32_t caretColorIndex;
        
        SceneClassAssistant* m_sceneAssistant;
        

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_DATA_CARTESIAN_DECLARE__
    int32_t ChartDataCartesian::caretColorIndex = 0;
#endif // __CHART_DATA_CARTESIAN_DECLARE__

} // namespace
#endif  //__CHART_DATA_CARTESIAN_H__
