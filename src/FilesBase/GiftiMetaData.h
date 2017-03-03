#ifndef __GIFTIMETADATA_H__
#define __GIFTIMETADATA_H__

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


#include "CaretObject.h"

#include "GiftiException.h"
#include "TracksModificationInterface.h"

#include <stdint.h>

#include <map>
#include <AString.h>
#include <vector>

class QXmlStreamReader;
class QXmlStreamWriter;

namespace caret {

class XmlWriter;
    
/**
 * Maintains GIFTI metadata using name/value pairs.  A
 * metadata object may be associated with the GIFTI
 * data file and with each GIFTI data array.
 */
class GiftiMetaData : public CaretObject, TracksModificationInterface {

public:
    GiftiMetaData();

public:
    GiftiMetaData(const GiftiMetaData& o);

    GiftiMetaData& operator=(const GiftiMetaData& o);
    
    bool operator==(const GiftiMetaData& rhs) const;
    
    bool operator!=(const GiftiMetaData& rhs) const { return !((*this) == rhs); }

    virtual ~GiftiMetaData();

private:
    void copyHelper(const GiftiMetaData& o);

    void initializeMembersGiftiMetaData();
    
public:
    AString getUniqueID() const;

    //void removeUniqueID();

    void clear(bool keepUUID = true);

    void append(const GiftiMetaData& smd);

    void replace(const GiftiMetaData& smd);

    void set(
                    const AString& name,
                    const AString& value);

    void setInt(
                    const AString& name,
                    const int32_t value);

    void setFloat(
                    const AString& name,
                    const float value);

    void replaceWithMap(const std::map<AString, AString>& map);
    
    std::map<AString, AString> getAsMap() const;
    
    void remove(const AString& name);

    bool exists(const AString& name) const;

    AString get(const AString& name) const;

    int32_t getInt(const AString& name) const;

    float getFloat(const AString& name) const;

    std::vector<AString> getAllMetaDataNames() const;

    void updateFromCaret5Names();

    AString toString() const;
 
    AString toFormattedString(const AString& indentation);

    void writeAsXML(XmlWriter& xmlWriter);

    void writeCiftiXML1(QXmlStreamWriter& xmlWriter) const;
    void writeCiftiXML2(QXmlStreamWriter& xmlWriter) const;//extra names for code style, and in case it changes in a future version
    void writeBorderFileXML3(QXmlStreamWriter& xmlWriter) const;
    void readCiftiXML1(QXmlStreamReader& xml);
    void readCiftiXML2(QXmlStreamReader& xml);
    void readBorderFileXML1(QXmlStreamReader& xml);
    void readBorderFileXML3(QXmlStreamReader& xml);
    
    void setModified();

    void clearModified();

    bool isModified() const;
    
    void resetUniqueIdentifier();

    bool isEmpty() const;
    
private:
    void readEntry(QXmlStreamReader& xml);
    
    std::map<AString,AString> createTreeMap();

    void replaceName(
                    const AString& oldName,
                    const AString& newName);

public:

private:
    /**the metadata storage. */
    mutable std::map<AString,AString> metadata;    
    typedef std::map<AString,AString>::iterator MetaDataIterator;
    typedef std::map<AString,AString>::const_iterator MetaDataConstIterator;
    
    
    /**has the metadata been modified */
    bool modifiedFlag;

};

} // namespace

#endif // __GIFTIMETADATA_H__
