#ifndef __GIFTIMETADATA_H__
#define __GIFTIMETADATA_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "CaretObject.h"

#include "GiftiException.h"
#include "TracksModificationInterface.h"

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

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

    virtual ~GiftiMetaData();

private:
    void copyHelper(const GiftiMetaData& o);

    void initializeMembersGiftiMetaData();
    
public:
    std::string getUniqueID() const;

    void removeUniqueID();

    void clear();

    void append(const GiftiMetaData& smd);

    void replace(const GiftiMetaData& smd);

    void set(
                    const std::string& name,
                    const std::string& value);

    void setInt(
                    const std::string& name,
                    const int32_t value);

    void setFloat(
                    const std::string& name,
                    const float value);

    void remove(const std::string& name);

    bool exists(const std::string& name);

    std::string get(const std::string& name) const;

    int32_t getInt(const std::string& name) const;

    float getFloat(const std::string& name) const;

    std::vector<std::string> getAllMetaDataNames() const;

    void updateFromCaret5Names();

    std::string toString() const;
 
    std::string toFormattedString(const std::string& indentation);

    void writeAsXML(XmlWriter& xmlWriter)
            throw (GiftiException);

    void updateUserNameDate();

    void setModified();

    void clearModified();

    bool isModified() const;

private:
    std::map<std::string,std::string> createTreeMap();

    void replaceName(
                    const std::string& oldName,
                    const std::string& newName);

public:

private:
    /**the metadata storage. */
    std::map<std::string,std::string> metadata;

    typedef std::map<std::string,std::string>::iterator MetaDataIterator;
    typedef std::map<std::string,std::string>::const_iterator MetaDataConstIterator;
    
    
    /**has the metadata been modified */
    bool modifiedFlag;

};

} // namespace

#endif // __GIFTIMETADATA_H__
