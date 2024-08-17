#ifndef __CARET_HIERARCHY_H__
#define __CARET_HIERARCHY_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024  Washington University School of Medicine
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

#include <set>
#include <vector>

#include <QStandardItem>
#include <QStandardItemModel>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "AString.h"

namespace caret
{
    
    class CaretHierarchy
    {
    public:
        struct Item;
        
        CaretHierarchy();
        bool addItem(const AString name, const AString parent = "", const AString id = ""); //defaults to making the root element the parent
        void clear();
        bool isEmpty() const { return (m_root.children.size() == 0); }
        std::set<AString> getAllNames() const { auto ret = m_usedNames; ret.erase(""); return ret; } //hide the implicit root element
        const Item& getInvisibleRoot() const { return m_root; } //when in rome...
        
        void readXML(const QString& text);
        void readXML(QXmlStreamReader& xml);
        
        QString writeXMLToString() const;
        void writeXML(QXmlStreamWriter& xml) const;
        
        QStandardItemModel* buildQSIModel() const; //NOTE: allocates a new, unowned object
        
        struct Item
        {
            bool add(const Item toAdd, const AString parent); //search for parent and add to its children - reverse depth first for parsing to be somewhat efficient
            
            AString name;
            AString id;
            std::vector<Item> children;
            
            Item(const AString nameIn, const AString idIn) : name(nameIn), id(idIn) {}
            Item() {}

            void XMLWriteHelper(QXmlStreamWriter& xml) const;
            void QSIModelHelper(QStandardItem* qsiOut) const;
        };
    private:
        Item m_root;
        std::set<AString> m_usedNames; //efficiently prevent duplicate names
    };
    
}

#endif
