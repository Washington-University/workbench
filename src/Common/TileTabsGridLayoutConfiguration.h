#ifndef __TILE_TABS_GRID_LAYOUT_CONFIGURATION_H__
#define __TILE_TABS_GRID_LAYOUT_CONFIGURATION_H__

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
#include "TileTabsBaseConfiguration.h"
#include "TileTabsGridModeEnum.h"
#include "TileTabsGridRowColumnElement.h"

class QXmlStreamReader;
class QXmlStreamWriter;

namespace caret {

    class TileTabsGridLayoutConfiguration : public TileTabsBaseConfiguration {
        
    public:
        TileTabsGridLayoutConfiguration();
        
        virtual ~TileTabsGridLayoutConfiguration();
        
        TileTabsGridLayoutConfiguration(const TileTabsGridLayoutConfiguration& obj);

        TileTabsGridLayoutConfiguration& operator=(const TileTabsGridLayoutConfiguration& obj);
        
        void copy(const TileTabsGridLayoutConfiguration& rhs);
        
        virtual TileTabsGridLayoutConfiguration* newCopyWithNewUniqueIdentifier() const override;
                
        bool getRowHeightsAndColumnWidthsForWindowSize(const int32_t windowWidth,
                                                       const int32_t windowHeight,
                                                       const int32_t numberOfModelsToDraw,
                                                       const TileTabsGridModeEnum::Enum configurationMode,
                                                       std::vector<int32_t>& rowHeightsOut,
                                                       std::vector<int32_t>& columnWidthsOut);
        
        int32_t getNumberOfRows() const;
        
        void setNumberOfRows(const int32_t numberOfRows);
        
        int32_t getNumberOfColumns() const;

        void setNumberOfColumns(const int32_t numberOfColumns);
        
        TileTabsGridRowColumnElement* getColumn(const int32_t columnIndex);
        
        const TileTabsGridRowColumnElement* getColumn(const int32_t columnIndex) const;
        
        TileTabsGridRowColumnElement* getRow(const int32_t rowIndex);
        
        const TileTabsGridRowColumnElement* getRow(const int32_t rowIndex) const;
        
        void updateAutomaticConfigurationRowsAndColumns(const int32_t numberOfTabs);
        
        bool isCenteringCorrectionEnabled() const;
        
        void setCenteringCorrectionEnabled(const bool status);
        
        virtual AString toString() const override;
        
        static void getRowsAndColumnsForNumberOfTabs(const int32_t numberOfTabs,
                                                     int32_t& numberOfRowsOut,
                                                     int32_t& numberOfColumnsOut);
        // ADD_NEW_METHODS_HERE
        
    protected:
    public:
        virtual void decodeFromXML(QXmlStreamReader& xml,
                                   const QString& rootElement) override;
        
        virtual void encodeInXML(AString& xmlTextOut) const override;
        
    private:
        void copyHelperTileTabsGridLayoutConfiguration(const TileTabsGridLayoutConfiguration& obj);

        void decodeFromXMLWithStreamReaderVersionOne(QXmlStreamReader& xml);
        
        void decodeFromXMLWithStreamReaderVersionTwo(QXmlStreamReader& xml);
        
        AString encodeVersionInXML(const int32_t versionNumber) const;
        
        AString encodeInXMLWithStreamWriterVersionOne() const;
        
        AString encodeInXMLWithStreamWriterVersionTwo() const;
        
        void encodeRowColumnElement(QXmlStreamWriter& writer,
                                    const AString tagName,
                                    const std::vector<TileTabsGridRowColumnElement>& elements) const;
        
        bool decodeRowColumnElement(QXmlStreamReader& reader,
                                    TileTabsGridRowColumnElement& element,
                                    AString& errorMessageOut);
        
        void initialize();
        
        // ADD_NEW_MEMBERS_HERE
        
        std::vector<TileTabsGridRowColumnElement> m_columns;
        
        std::vector<TileTabsGridRowColumnElement> m_rows;
        
        bool m_centeringCorrectionEnabled = false;
        
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
        static const AString s_v2_centeringCorrectionName;
        
        
        friend class TileTabsBaseConfiguration;
    };
    
#ifdef __TILE_TABS_GRID_LAYOUT_CONFIGURATION_DECLARE__
    const AString TileTabsGridLayoutConfiguration::s_nameTagName = "Name";
    const AString TileTabsGridLayoutConfiguration::s_uniqueIdentifierTagName = "UniqueIdentifier";

    const AString TileTabsGridLayoutConfiguration::s_v1_rootTagName = "TileTabsGridLayoutConfiguration";
    const AString TileTabsGridLayoutConfiguration::s_v1_versionTagName = "Version";
    const AString TileTabsGridLayoutConfiguration::s_v1_versionNumberAttributeName = "Number";
    const AString TileTabsGridLayoutConfiguration::s_v1_columnStretchFactorsTagName = "ColumnStretchFactors";
    const AString TileTabsGridLayoutConfiguration::s_v1_columnStretchFactorsSelectedCountAttributeName = "SelectedRowCount";
    const AString TileTabsGridLayoutConfiguration::s_v1_columnStretchFactorsTotalCountAttributeName = "TotalRowCount";
    const AString TileTabsGridLayoutConfiguration::s_v1_rowStretchFactorsTagName = "RowStretchFactors";
    const AString TileTabsGridLayoutConfiguration::s_v1_rowStretchFactorsSelectedCountAttributeName = "SelectedColumnCount";
    const AString TileTabsGridLayoutConfiguration::s_v1_rowStretchFactorsTotalCountAttributeName = "TotalColumnCount";
    
    const AString TileTabsGridLayoutConfiguration::s_v2_rootTagName = "TileTabsGridLayoutConfigurationTwo";
    const AString TileTabsGridLayoutConfiguration::s_v2_versionAttributeName = "Version";
    const AString TileTabsGridLayoutConfiguration::s_v2_columnsTagName = "Columns";
    const AString TileTabsGridLayoutConfiguration::s_v2_contentTypeAttributeName = "ContentType";
    const AString TileTabsGridLayoutConfiguration::s_v2_elementTagName = "Element";
    const AString TileTabsGridLayoutConfiguration::s_v2_percentStretchAttributeName = "PercentStretch";
    const AString TileTabsGridLayoutConfiguration::s_v2_rowsTagName = "Rows";
    const AString TileTabsGridLayoutConfiguration::s_v2_stretchTypeAttributeName = "StretchType";
    const AString TileTabsGridLayoutConfiguration::s_v2_weightStretchAttributeName = "WeightStretch";
    const AString TileTabsGridLayoutConfiguration::s_v2_centeringCorrectionName = "CenteringCorrection";
#endif // __TILE_TABS_GRID_LAYOUT_CONFIGURATION_DECLARE__

} // namespace
#endif  //__TILE_TABS_GRID_LAYOUT_CONFIGURATION_H__
