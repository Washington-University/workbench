#ifndef __TILE_TABS_BASE_CONFIGURATION_H__
#define __TILE_TABS_BASE_CONFIGURATION_H__

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
#include "TileTabsConfigurationLayoutTypeEnum.h"
#include "TileTabsGridModeEnum.h"
#include "TileTabsGridRowColumnElement.h"

class QXmlStreamReader;
class QXmlStreamWriter;

namespace caret {

    class TileTabsBaseConfiguration : public CaretObject {
        
    protected:
        TileTabsBaseConfiguration(const TileTabsConfigurationLayoutTypeEnum::Enum layoutType);
        
    public:
        virtual ~TileTabsBaseConfiguration();
        
        TileTabsBaseConfiguration(const TileTabsBaseConfiguration& obj);

        TileTabsBaseConfiguration& operator=(const TileTabsBaseConfiguration& obj);
        
        void copy(const TileTabsBaseConfiguration& rhs);
        
        virtual TileTabsBaseConfiguration* newCopyWithNewUniqueIdentifier() const = 0;
        
        TileTabsConfigurationLayoutTypeEnum::Enum getLayoutType() const;

        AString getName() const;
        
        void setName(const AString& name);
        
        AString getUniqueIdentifier() const;
        
        
        AString encodeInXML() const;
        
        static TileTabsBaseConfiguration* decodeFromXML(const AString& xmlString,
                                                        AString& errorMessageOut);
        
        AString toString() const override;
        
        static bool lessThanComparisonByName(const TileTabsBaseConfiguration* ttc1,
                                             const TileTabsBaseConfiguration* ttc2);
        
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
        virtual void decodeFromXML(QXmlStreamReader& xml,
                                   const QString& rootElementText) = 0;
        
        /**
         * Encode the configuration in XML.
         *
         * @param xmlTextOut
         *     Contains XML representation of configuration.
         */
        virtual void encodeInXML(AString& xmlTextOut) const = 0;

        void setUniqueIdentifierProtected(const AString& uniqueID);
        
        void copyHelperTileTabsBaseConfiguration(const TileTabsBaseConfiguration& obj);
        
    private:

        void initializeTileTabsBaseConfiguration();
        
        // ADD_NEW_MEMBERS_HERE
        
        const TileTabsConfigurationLayoutTypeEnum::Enum m_layoutType;

        AString m_name;
        
        /** Unique identifier does not get copied */
        AString m_uniqueIdentifier;
        
        
    };
    
#ifdef __TILE_TABS_BASE_CONFIGURATION_DECLARE__

#endif // __TILE_TABS_BASE_CONFIGURATION_DECLARE__

} // namespace
#endif  //__TILE_TABS_BASE_CONFIGURATION_H__
