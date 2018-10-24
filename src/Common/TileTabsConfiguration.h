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
#include "TileTabsRowColumnElement.h"

class QXmlStreamReader;
class QXmlStreamWriter;

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
        
        TileTabsRowColumnElement* getColumn(const int32_t columnIndex);
        
        const TileTabsRowColumnElement* getColumn(const int32_t columnIndex) const;
        
        TileTabsRowColumnElement* getRow(const int32_t rowIndex);
        
        const TileTabsRowColumnElement* getRow(const int32_t rowIndex) const;
        
        AString encodeInXML() const;
        
        AString encodeVersionInXML(const int32_t versionNumber) const;
        
        bool decodeFromXML(const AString& xmlString,
                           AString& errorMessageOut);
        
        void updateAutomaticConfigurationRowsAndColumns(const int32_t numberOfTabs);
        
        AString toString() const override;
        
        static bool lessThanComparisonByName(const TileTabsConfiguration* ttc1,
                                             const TileTabsConfiguration* ttc2);
        
        static void getRowsAndColumnsForNumberOfTabs(const int32_t numberOfTabs,
                                                     int32_t& numberOfRowsOut,
                                                     int32_t& numberOfColumnsOut);
        // ADD_NEW_METHODS_HERE
        
        
    private:
        void copyHelperTileTabsConfiguration(const TileTabsConfiguration& obj);

        bool decodeFromXMLWithStreamReader(const AString& xmlString,
                                           AString& errorMessageOut);
        
        void decodeFromXMLWithStreamReaderVersionOne(QXmlStreamReader& xml);
        
        void decodeFromXMLWithStreamReaderVersionTwo(QXmlStreamReader& xml);
        
        AString encodeInXMLWithStreamWriterVersionOne() const;
        
        AString encodeInXMLWithStreamWriterVersionTwo() const;
        
        void encodeRowColumnElement(QXmlStreamWriter& writer,
                                    const AString tagName,
                                    const std::vector<TileTabsRowColumnElement>& elements) const;
        
        bool decodeRowColumnElement(QXmlStreamReader& reader,
                                    TileTabsRowColumnElement& element,
                                    AString& errorMessageOut);
        
        void initialize();
        
        // ADD_NEW_MEMBERS_HERE
        
        AString m_name;
        
        /** Unique identifier does not get copied */
        AString m_uniqueIdentifier;
        
        std::vector<TileTabsRowColumnElement> m_columns;
        
        std::vector<TileTabsRowColumnElement> m_rows;
        
        static const AString s_nameTagName;
        static const AString s_uniqueIdentifierTagName;

        static const AString s_v1_rootTagName;
        static const AString s_v1_versionTagName;
        static const AString s_v1_versionNumberAttributeName;
        static const AString s_v1_columnStretchFactorsTagName;
        static const AString s_v1_columnStretchFactorsSelectedCountAttributeName;
        static const AString s_v1_columnStretchFactorsTotalCountAttributeName;
        static const AString s_v1_rowStretchFactorsTagName;
        static const AString s_v1_rowStretchFactorsSelectedCountAttributeName;
        static const AString s_v1_rowStretchFactorsTotalCountAttributeName;
        
        static const AString s_v2_rootTagName;
        static const AString s_v2_versionAttributeName;
        static const AString s_v2_columnsTagName;
        static const AString s_v2_contentTypeAttributeName;
        static const AString s_v2_elementTagName;
        static const AString s_v2_percentStretchAttributeName;
        static const AString s_v2_rowsTagName;
        static const AString s_v2_stretchTypeAttributeName;
        static const AString s_v2_weightStretchAttributeName;
    };
    
#ifdef __TILE_TABS_CONFIGURATION_DECLARE__
    const AString TileTabsConfiguration::s_nameTagName = "Name";
    const AString TileTabsConfiguration::s_uniqueIdentifierTagName = "UniqueIdentifier";

    const AString TileTabsConfiguration::s_v1_rootTagName = "TileTabsConfiguration";
    const AString TileTabsConfiguration::s_v1_versionTagName = "Version";
    const AString TileTabsConfiguration::s_v1_versionNumberAttributeName = "Number";
    const AString TileTabsConfiguration::s_v1_columnStretchFactorsTagName = "ColumnStretchFactors";
    const AString TileTabsConfiguration::s_v1_columnStretchFactorsSelectedCountAttributeName = "SelectedRowCount";
    const AString TileTabsConfiguration::s_v1_columnStretchFactorsTotalCountAttributeName = "TotalRowCount";
    const AString TileTabsConfiguration::s_v1_rowStretchFactorsTagName = "RowStretchFactors";
    const AString TileTabsConfiguration::s_v1_rowStretchFactorsSelectedCountAttributeName = "SelectedColumnCount";
    const AString TileTabsConfiguration::s_v1_rowStretchFactorsTotalCountAttributeName = "TotalColumnCount";
    
    const AString TileTabsConfiguration::s_v2_rootTagName = "TileTabsConfigurationTwo";
    const AString TileTabsConfiguration::s_v2_versionAttributeName = "Version";
    const AString TileTabsConfiguration::s_v2_columnsTagName = "Columns";
    const AString TileTabsConfiguration::s_v2_contentTypeAttributeName = "ContentType";
    const AString TileTabsConfiguration::s_v2_elementTagName = "Element";
    const AString TileTabsConfiguration::s_v2_percentStretchAttributeName = "PercentStretch";
    const AString TileTabsConfiguration::s_v2_rowsTagName = "Rows";
    const AString TileTabsConfiguration::s_v2_stretchTypeAttributeName = "StretchType";
    const AString TileTabsConfiguration::s_v2_weightStretchAttributeName = "WeightStretch";
#endif // __TILE_TABS_CONFIGURATION_DECLARE__

} // namespace
#endif  //__TILE_TABS_CONFIGURATION_H__
