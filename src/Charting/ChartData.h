#ifndef __CHART_DATA_H__
#define __CHART_DATA_H__

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

#include "BrainConstants.h"
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
         * will function when this abstract, base class is used.  Each
         * subclass will override this method so that the returned class
         * is of the proper type.
         *
         * @return Copy of this instance that is the actual subclass.
         */
        virtual ChartData* clone() const = 0;
        
        ChartDataTypeEnum::Enum getChartDataType() const;
        
        const ChartDataSource* getChartDataSource() const;
        
        ChartDataSource* getChartDataSource();
        
        bool isSelected(const int32_t tabIndex) const;
        
        void setSelected(const int32_t tabIndex,
                         const bool selectionStatus);
        
        void clearSelected();
        
        AString getUniqueIdentifier() const;
        
        void setUniqueIdentifier(const AString& uniqueIdentifier);
        
        void copySelectionStatusForAllTabs(const ChartData* copyFrom);
        
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
        
        bool m_selectionStatus[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        AString m_uniqueIdentifier;
        
        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __CHART_DATA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_DATA_DECLARE__

} // namespace
#endif  //__CHART_DATA_H__
