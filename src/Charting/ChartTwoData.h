#ifndef __CHART_TWO_DATA_H__
#define __CHART_TWO_DATA_H__

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

#include "CaretObjectTracksModification.h"
#include "ChartTwoDataTypeEnum.h"
#include "SceneableInterface.h"


namespace caret {

    class ChartDataSource;
    class SceneClassAssistant;
    
    class ChartTwoData : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        static ChartTwoData* newChartTwoDataForChartTwoDataType(const ChartTwoDataTypeEnum::Enum chartDataType);
        
        virtual ~ChartTwoData();
        
        /**
         * At times a copy of chart data will be needed BUT it must be
         * the proper subclass so copy constructor and assignment operator
         * will function when this abstract, base class is used.  Each
         * subclass will override this method so that the returned class
         * is of the proper type.
         *
         * @return Copy of this instance that is the actual subclass.
         */
        virtual ChartTwoData* clone() const = 0;
        
        ChartTwoDataTypeEnum::Enum getChartTwoDataType() const;
        
        const ChartDataSource* getChartDataSource() const;
        
        ChartDataSource* getChartDataSource();
        
        bool isSelected() const;
        
        void setSelected(const bool selectionStatus);
        
        void clearSelected();
        
        AString getUniqueIdentifier() const;
        
        void setUniqueIdentifier(const AString& uniqueIdentifier);
        
        //void copySelectionStatusForAllTabs(const ChartTwoData* copyFrom);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    protected:
        ChartTwoData(const ChartTwoDataTypeEnum::Enum chartDataType);
        
        ChartTwoData(const ChartTwoData& obj);
        
        ChartTwoData& operator=(const ChartTwoData& obj);
        
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
        void initializeMembersChartTwoData();
        
        void copyHelperChartTwoData(const ChartTwoData& obj);

        SceneClassAssistant* m_sceneAssistant;

        ChartTwoDataTypeEnum::Enum m_chartDataType;
        
        ChartDataSource* m_chartDataSource;
        
        bool m_selectionStatus;
        
        AString m_uniqueIdentifier;
        
        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __CHART_TWO_DATA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_DATA_DECLARE__

} // namespace
#endif  //__CHART_TWO_DATA_H__
