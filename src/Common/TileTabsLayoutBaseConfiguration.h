#ifndef __TILE_TABS_LAYOUT_BASE_CONFIGURATION_H__
#define __TILE_TABS_LAYOUT_BASE_CONFIGURATION_H__

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
#include "TileTabsLayoutConfigurationTypeEnum.h"
#include "TileTabsGridRowColumnElement.h"

class QXmlStreamReader;
class QXmlStreamWriter;

namespace caret {
    
    class TileTabsLayoutGridConfiguration;
    class TileTabsLayoutManualConfiguration;

    class TileTabsLayoutBaseConfiguration : public CaretObject {
        
    protected:
        TileTabsLayoutBaseConfiguration(const TileTabsLayoutConfigurationTypeEnum::Enum layoutType);
        
    public:
        virtual ~TileTabsLayoutBaseConfiguration();
        
        TileTabsLayoutBaseConfiguration(const TileTabsLayoutBaseConfiguration& obj);

        TileTabsLayoutBaseConfiguration& operator=(const TileTabsLayoutBaseConfiguration& obj);
        
        /**
         * Copy the given configuration to "this" configuration.  If given configuration
         * does not cast to "this class type" log a warning and do not copy.
         * Name property is not copied.
         *
         * @param rhs
         *      Configuration to copy.
         */
        virtual void copy(const TileTabsLayoutBaseConfiguration& rhs) = 0; 
        
        virtual TileTabsLayoutBaseConfiguration* newCopyWithNewUniqueIdentifier() const = 0;
        
        TileTabsLayoutBaseConfiguration* newCopyWithUniqueIdentifier(const AString& uniqueIdentifier) const;
        
        TileTabsLayoutConfigurationTypeEnum::Enum getLayoutType() const;

        AString getName() const;
        
        void setName(const AString& name);
        
        AString getUniqueIdentifier() const;
        
        
        AString encodeInXML() const;
        
        static TileTabsLayoutBaseConfiguration* decodeFromXML(const AString& xmlString,
                                                        AString& errorMessageOut);
        
        AString toString() const override;
        
        virtual TileTabsLayoutGridConfiguration* castToGridConfiguration();

        virtual const TileTabsLayoutGridConfiguration* castToGridConfiguration() const;
        
        virtual TileTabsLayoutManualConfiguration* castToManualConfiguration();
        
        virtual const TileTabsLayoutManualConfiguration* castToManualConfiguration() const;
        
        static bool lessThanComparisonByName(const TileTabsLayoutBaseConfiguration* ttc1,
                                             const TileTabsLayoutBaseConfiguration* ttc2);
        
        // ADD_NEW_METHODS_HERE
        
        
    protected:
        /**
         * Decode the configuration using the given XML stream reader and root element
         * If there is an error, xml.raiseError() should be used to specify the error
         * and caller of this method can test for the error using xml.isError().
         *
         * @param xml
         *     The XML stream reader.
         * @param rootElement
         *     The root element.
         */
        virtual void decodeFromXMLString(QXmlStreamReader& xml,
                                         const AString& rootElementText) = 0;
        
        /**
         * Encode the configuration in XML.
         *
         * @param xmlTextOut
         *     Contains XML representation of configuration.
         */
        virtual void encodeInXMLString(AString& xmlTextOut) const = 0;

        void setUniqueIdentifierProtected(const AString& uniqueID);
        
        void copyHelperTileTabsLayoutBaseConfiguration(const TileTabsLayoutBaseConfiguration& obj);
        
    private:

        void initializeTileTabsLayoutBaseConfiguration();
        
        // ADD_NEW_MEMBERS_HERE
        
        const TileTabsLayoutConfigurationTypeEnum::Enum m_layoutType;

        AString m_name;
        
        /** Unique identifier does not get copied */
        AString m_uniqueIdentifier;
        
        
    };
    
#ifdef __TILE_TABS_LAYOUT_BASE_CONFIGURATION_H__

#endif // __TILE_TABS_LAYOUT_BASE_CONFIGURATION_DECLARE__

} // namespace
#endif  //__TILE_TABS_LAYOUT_BASE_CONFIGURATION_DECLARE__
