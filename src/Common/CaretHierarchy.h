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

#include <map>
#include <set>
#include <utility>
#include <vector>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "AString.h"

class QStandardItem;
class QStandardItemModel;

namespace caret
{
    
    class CaretHierarchy
    {
    public:
        class OrderedKVStore;
        struct Item;
        
        CaretHierarchy();
        bool addItem(const Item& toAdd, const AString parent = "", OrderedKVStore** extraInfoOut = NULL); //defaults to making the root element the parent
        void clear();
        bool isEmpty() const { return (m_root.children.size() == 0); }
        std::set<AString> getAllNames() const { auto ret = m_usedNames; ret.erase(""); return ret; } //hide the implicit root element
        const Item& getInvisibleRoot() const { return m_root; } //when in rome...
        
        void readXML(const QString& text);
        void readXML(QXmlStreamReader& xml);
        void readJsonFile(const AString& filename);
        
        QString writeXMLToString() const;
        void writeXML(QXmlStreamWriter& xml) const;
        
        QStandardItemModel* buildQSIModel() const; //NOTE: allocates a new, unowned object
        
        class OrderedKVStore
        {
            std::map<AString, size_t> m_nameLookup;
            std::vector<std::pair<AString, AString> > m_store;
        public:
            const std::vector<std::pair<AString, AString> >& getAllData() const { return m_store; }
            AString get(const AString& name) const
            {
                auto iter = m_nameLookup.find(name);
                if (iter == m_nameLookup.end()) return "";
                return m_store[iter->second].second;
            }
            void set(const AString& name, const AString& value)
            {
                auto iter = m_nameLookup.find(name);
                if (iter == m_nameLookup.end())
                {
                    m_nameLookup[name] = m_store.size();
                    m_store.push_back(std::make_pair(name, value));
                } else {
                    m_store[iter->second].second = value;
                }
            }
            void erase(const AString& name)
            {
                auto iter = m_nameLookup.find(name);
                if (iter == m_nameLookup.end()) return;
                for (size_t i = iter->second + 1; i < m_store.size(); ++i)
                {
                    m_nameLookup[m_store[i].first] -= 1;
                }
                m_store.erase(m_store.begin() + iter->second);
                m_nameLookup.erase(iter);//now iter is invalid, but we are done
            }
            void clear() { m_nameLookup.clear(); m_store.clear(); }
            
            void readXML(QXmlStreamReader& xml);
        };
        
        struct Item
        {
            bool add(const Item& toAdd, const AString parent, OrderedKVStore** extraInfoOut); //search for parent and add to its children - reverse depth first for parsing to be somewhat efficient
            
            AString name;
            //AString id; //NOTE: defunct
            OrderedKVStore extraInfo;
            std::vector<Item> children;
            
            Item(const AString nameIn) : name(nameIn) {}
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
