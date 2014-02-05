#ifndef __CHART_DATA_H__
#define __CHART_DATA_H__

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
#include "ChartDataTypeEnum.h"
#include "SceneableInterface.h"


namespace caret {

    class ChartDataSource;
    class SceneClassAssistant;
    
    class ChartData : public CaretObject, public SceneableInterface {
        
    public:
        static ChartData* newChartDataForChartDataType(const ChartDataTypeEnum::Enum chartDataType);
        
        virtual ~ChartData();
        
        /**
         * At times a copy of chart data will be needed BUT it must be
         * the proper subclass so copy constructor and assignment operator
         * will no function when this abstract, base class is used.  Each
         * subclass will override this method so that the returned class
         * is of the proper type.
         *
         * @return Copy of this instance that is the actual subclass.
         */
        virtual ChartData* clone() const = 0;
        
        ChartDataTypeEnum::Enum getChartDataType() const;
        
        const ChartDataSource* getChartDataSource() const;
        
        ChartDataSource* getChartDataSource();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
    protected:
        ChartData(const ChartDataTypeEnum::Enum chartDataType);
        
        ChartData(const ChartData& obj);
        
        ChartData& operator=(const ChartData& obj);
        
        /**
         * Save subclass data to the scene.  sceneClass
         * will be valid and any scene data should be added to it.
         *
         * @param sceneAttributes
         *    Attributes for the scene.  Scenes may be of different types
         *    (full, generic, etc) and the attributes should be checked when
         *    restoring the scene.
         *
         * @param sceneClass
         *     sceneClass to which data members should be added.
         */
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass) = 0;
        
        /**
         * Restore file data from the scene.  The scene class
         * will be valid and any scene data may be obtained from it.
         *
         * @param sceneAttributes
         *    Attributes for the scene.  Scenes may be of different types
         *    (full, generic, etc) and the attributes should be checked when
         *    restoring the scene.
         *
         * @param sceneClass
         *     sceneClass for the instance of a class that implements
         *     this interface.  Will NEVER be NULL.
         */
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass) = 0;
        
    private:
        void initializeMembersChartData();
        
        void copyHelperChartData(const ChartData& obj);

        SceneClassAssistant* m_sceneAssistant;

        ChartDataTypeEnum::Enum m_chartDataType;
        
        ChartDataSource* m_chartDataSource;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_DATA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_DATA_DECLARE__

} // namespace
#endif  //__CHART_DATA_H__
