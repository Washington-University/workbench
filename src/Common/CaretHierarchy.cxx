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

#include "CaretHierarchy.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretPointer.h"

using namespace caret;
using namespace std;

CaretHierarchy::CaretHierarchy()
{
    m_usedNames.insert(""); //don't allow adding the empty string, which we implicitly use for the root element
}

void CaretHierarchy::clear()
{
    m_root = Item();
    m_usedNames.clear();
    m_usedNames.insert(""); //include the implicit root element
}

//backwards recursive depth-first, for some efficiency when reading from file
bool CaretHierarchy::Item::add(const CaretHierarchy::Item toAdd, const AString parent)
{
    if (name == parent)
    {
        children.push_back(toAdd);
        return true;
    }
    for (auto iter = children.rbegin(); iter != children.rend(); ++iter)
    {
        if (iter->add(toAdd, parent)) return true;
    }
    return false;
}

bool CaretHierarchy::addItem(const AString name, const AString parent, const AString id)
{
    if (m_usedNames.find(name) != m_usedNames.end()) return false;
    if (m_usedNames.find(parent) == m_usedNames.end()) return false; //we can predict when it would fail, so return early
    if (m_root.add(Item(name, id), parent))
    {
        m_usedNames.insert(name);
        return true;
    }
    return false;
}

void CaretHierarchy::Item::XMLWriteHelper(QXmlStreamWriter& xml) const
{
    if (name == "")
    {//don't write the implicit root element, it exists for convenience rather than structure
        for (auto iter : children)
        {
            iter.XMLWriteHelper(xml);
        }
    } else {
        xml.writeStartElement("Item"); //TODO: better name?
        xml.writeAttribute("Name", name);
        xml.writeAttribute("Identifier", id);
        for (auto iter : children)
        {
            iter.XMLWriteHelper(xml);
        }
        xml.writeEndElement();
    }
}

void CaretHierarchy::writeXML(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("CaretHierarchy");
    xml.writeAttribute("Version", "1");
    m_root.XMLWriteHelper(xml);
    xml.writeEndElement();
}

QString CaretHierarchy::writeXMLToString() const
{
    QString ret;
    QXmlStreamWriter xml(&ret);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    writeXML(xml);
    xml.writeEndDocument();
    return ret;
}

void CaretHierarchy::readXML(QXmlStreamReader& xml)
{
    clear(); //leaves us with just the root element and "" used
    vector<QString> parents; //can use add() for sanity checking rather than recursive parsing, just need to track the XML parent name
    parents.push_back(""); //trick for handling the root case without special code
    try
    {
        bool haveRoot = false;
        bool rootEnded = false;
        for (; !xml.atEnd(); xml.readNext())
        {
            if (xml.isStartElement())
            {
                auto name = xml.name();
                if (name == QLatin1String("CaretHierarchy"))
                {
                    if (haveRoot) throw CaretException("found root 'CaretHierarchy' element more than once");
                    haveRoot = true;
                    QXmlStreamAttributes attributes = xml.attributes();
                    if (!attributes.hasAttribute("Version")) throw CaretException("no Version attribute in hierarch XML");
                    if (attributes.value("Version").toString() != "1") throw CaretException("unknown hierarchy version '" + attributes.value("Version").toString() + "'");
                } else if (name == QLatin1String("Item")) {
                    if (!haveRoot) throw CaretException("hierarchy XML is missing root element");
                    if (rootEnded) throw CaretException("found Item tag after closing root tag in hierarchy XML");
                    QXmlStreamAttributes attributes = xml.attributes();
                    AString name = attributes.value("Name").toString();
                    AString id = attributes.value("Identifier").toString();
                    if (!addItem(name, parents.back(), id)) throw CaretException("failed to add item '" + name + "' to hierarchy, check for a duplicate, empty, or missing Name attribute");
                    parents.push_back(name);
                } else {
                    throw CaretException("unexpected element '" + name.toString() + "' in hierarchy XML");
                }
            } else if (xml.isEndElement()) {
                auto name = xml.name();
                if (name == QLatin1String("Item"))
                {
                    parents.pop_back();
                } else {
                    CaretAssert(name == QLatin1String("CaretHierarchy"));
                    rootEnded = true;
                } //start element case will already have sanity checked the tag names
            }
        }
    } catch (CaretException& e) {
        throw CaretException("Hierarchy XML error: " + e.whatString()); //so we can throw on error instead of doing a bunch of dancing with xml.raiseError and xml.hasError
    }
    if(xml.hasError())
    {
        throw CaretException("Hierarchy XML error: " + xml.errorString());
    }
}

void CaretHierarchy::readXML(const QString& text)
{
    QXmlStreamReader xml(text);
    readXML(xml);
}


void CaretHierarchy::Item::QSIModelHelper(QStandardItem* qsiOut) const
{
    for (int index = 0; index < int(children.size()); ++index)
    {
        const Item& child = children[index];
        auto node = new QStandardItem(child.name);
        qsiOut->setChild(index, node);
        node->setData(QVariant(id));
        child.QSIModelHelper(node);
    }
}

QStandardItemModel* CaretHierarchy::buildQSIModel() const
{
    CaretPointerNonsync<QStandardItemModel> ret(new QStandardItemModel()); //not mutex protected, but we only need it for throw guarantees, not multithreaded replacement
    m_root.QSIModelHelper(ret->invisibleRootItem()); //QT also has a "root above tree" idea for convenience, rather than an array of top-level items
    return ret.releasePointer();
}
