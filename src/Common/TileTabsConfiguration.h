#ifndef __TILE_TABS_CONFIGURATION_H__
#define __TILE_TABS_CONFIGURATION_H__

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

#include "CaretException.h"
#include "CaretObject.h"
#include "TileTabsConfigurationModeEnum.h"

class QXmlStreamReader;

namespace caret {

    class TileTabsConfiguration : public CaretObject {
        
    public:
        TileTabsConfiguration();
        
        virtual ~TileTabsConfiguration();
        
        TileTabsConfiguration(const TileTabsConfiguration& obj);

        TileTabsConfiguration& operator=(const TileTabsConfiguration& obj);
        
        void copy(const TileTabsConfiguration& rhs);
        
        TileTabsConfiguration* newCopyWithNewUniqueIdentifier() const;
                
        bool getRowHeightsAndColumnWidthsForWindowSize(const int32_t windowWidth,
                                                       const int32_t windowHeight,
                                                       const int32_t numberOfModelsToDraw,
                                                       const TileTabsConfigurationModeEnum::Enum configurationMode,
                                                       std::vector<int32_t>& rowHeightsOut,
                                                       std::vector<int32_t>& columnWidthsOut);
        
        AString getName() const;
        
        void setName(const AString& name);
        
        AString getUniqueIdentifier() const;
        
        int32_t getNumberOfRows() const;
        
        void setNumberOfRows(const int32_t numberOfRows);
        
        int32_t getNumberOfColumns() const;

        void setNumberOfColumns(const int32_t numberOfColumns);
        
        float getColumnStretchFactor(const int32_t columnIndex) const;

        void setColumnStretchFactor(const int32_t columnIndex,
                                    const float stretchFactor);
        
        float getRowStretchFactor(const int32_t rowIndex) const;
        
        void setRowStretchFactor(const int32_t rowIndex,
                                 const float stretchFactor);
        
        AString encodeInXML() const;
        
        bool decodeFromXML(const AString& xmlString,
                           AString& errorMessageOut);
        
        void updateAutomaticConfigurationRowsAndColumns(const int32_t numberOfTabs);
        
        AString toString() const override;
        
        static bool lessThanComparisonByName(const TileTabsConfiguration* ttc1,
                                             const TileTabsConfiguration* ttc2);
        
        /**
         * @return Maximum number of rows in a tile tabs configuration
         */
        static inline int32_t getMaximumNumberOfRows() { return 20; }
        
        /**
         * @return Maximum number of columns in a tile tabs configuration
         */
        static inline int32_t getMaximumNumberOfColumns() { return 20; }
        
        static void getRowsAndColumnsForNumberOfTabs(const int32_t numberOfTabs,
                                                     int32_t& numberOfRowsOut,
                                                     int32_t& numberOfColumnsOut);
        // ADD_NEW_METHODS_HERE
        
        
    private:
        void setRowStretchFactors(const std::vector<float>& stretchFactors,
                                  const int32_t numberOfRows);
        
        void setColumnStretchFactors(const std::vector<float>& stretchFactors,
                                     const int32_t numberOfColumns);
        
        void copyHelperTileTabsConfiguration(const TileTabsConfiguration& obj);

        bool decodeFromXMLWithStreamReader(const AString& xmlString,
                                           AString& errorMessageOut);
        
        void decodeFromXMLWithStreamReaderVersionOne(QXmlStreamReader& xml);
        
        void decodeFromXMLWithStreamReaderVersionTwo(QXmlStreamReader& xml);
        
        AString encodeInXMLWithStreamWriterVersionOne() const;
        
        AString encodeInXMLWithStreamWriterVersionTwo() const;
        
        void initialize();
        
        // ADD_NEW_MEMBERS_HERE
        
        AString m_name;
        
        /** Unique identifier does not get copied */
        AString m_uniqueIdentifier;
        
        int32_t m_numberOfRows;
        
        int32_t m_numberOfColumns;
        
        std::vector<float> m_rowStretchFactors;
        
        std::vector<float> m_columnStretchFactors;

        static const AString s_rootTagName;
        static const AString s_v2_versionAttributeName;
        static const AString s_versionTagName;
        static const AString s_nameTagName;
        static const AString s_uniqueIdentifierTagName;
        static const AString s_versionNumberAttributeName;
        static const AString s_columnStretchFactorsTagName;
        static const AString s_columnStretchFactorsSelectedCountAttributeName;
        static const AString s_columnStretchFactorsTotalCountAttributeName;
        static const AString s_rowStretchFactorsTagName;
        static const AString s_rowStretchFactorsSelectedCountAttributeName;
        static const AString s_rowStretchFactorsTotalCountAttributeName;
        
    };
    
#ifdef __TILE_TABS_CONFIGURATION_DECLARE__
    const AString TileTabsConfiguration::s_rootTagName = "TileTabsConfiguration";
    const AString TileTabsConfiguration::s_v2_versionAttributeName = "Version";
    const AString TileTabsConfiguration::s_versionTagName = "Version";
    const AString TileTabsConfiguration::s_versionNumberAttributeName = "Number";
    const AString TileTabsConfiguration::s_nameTagName = "Name";
    const AString TileTabsConfiguration::s_uniqueIdentifierTagName = "UniqueIdentifier";
    const AString TileTabsConfiguration::s_columnStretchFactorsTagName = "ColumnStretchFactors";
    const AString TileTabsConfiguration::s_columnStretchFactorsSelectedCountAttributeName = "SelectedRowCount";
    const AString TileTabsConfiguration::s_columnStretchFactorsTotalCountAttributeName = "TotalRowCount";
    const AString TileTabsConfiguration::s_rowStretchFactorsTagName = "RowStretchFactors";
    const AString TileTabsConfiguration::s_rowStretchFactorsSelectedCountAttributeName = "SelectedColumnCount";
    const AString TileTabsConfiguration::s_rowStretchFactorsTotalCountAttributeName = "TotalColumnCount";
#endif // __TILE_TABS_CONFIGURATION_DECLARE__

} // namespace
#endif  //__TILE_TABS_CONFIGURATION_H__
