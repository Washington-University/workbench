#ifndef __GIFTILABELTABLE_H__
#define __GIFTILABELTABLE_H__

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

#include "AString.h"
#include "CaretObject.h"
#include "TracksModificationInterface.h"

#include "CaretPointer.h"
#include "GiftiException.h"

#include <map>
#include <set>
#include <vector>
#include <stdint.h>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

namespace caret {

class CaretHierarchy;
class GiftiLabel;
    
class XmlWriter;
class XmlException;
    
/**
 * Maintains a GIFTI Label Table using key/value pairs.
 */
class GiftiLabelTable : public CaretObject, TracksModificationInterface {

public:
    GiftiLabelTable();

    GiftiLabelTable(const GiftiLabelTable& glt);

    GiftiLabelTable& operator=(const GiftiLabelTable& glt);
    
    bool matches(const GiftiLabelTable& rhs, const bool checkColors = false, const bool checkCoords = false) const;
    
    bool operator==(const GiftiLabelTable& rhs) const { return matches(rhs, true); }
    
    bool operator!=(const GiftiLabelTable& rhs) const { return !((*this) == rhs); }
    
    virtual ~GiftiLabelTable();

private:
    void copyHelper(const GiftiLabelTable& glt);

    void initializeMembersGiftiLabelTable();
    
public:
    void clear();

    std::map<int32_t,int32_t> append(const GiftiLabelTable& glt, const bool errorOnLabelConflict = false);

    int32_t addLabel(
                    const AString& labelName,
                    const float red,
                    const float green,
                    const float blue,
                    const float alpha = 1.0f);

    int32_t addLabel(
                    const AString& labelName,
                    const int32_t red,
                    const int32_t green,
                    const int32_t blue,
                    const int32_t alpha = 255);

    int32_t addLabel(const GiftiLabel* glt, const bool errorOnLabelConflict = false);

    void deleteLabel(const int32_t key);

    void deleteLabel(const GiftiLabel* label);

    void deleteUnusedLabels(const std::set<int32_t>& usedLabelKeys);

    void insertLabel(const GiftiLabel* label);

    int32_t getLabelKeyFromName(const AString& name) const;

    const GiftiLabel* getLabel(const AString& labelName) const;

    GiftiLabel* getLabel(const AString& labelName);
    
    const GiftiLabel* getLabelBestMatching(const AString& name) const;

    const GiftiLabel* getLabel(const int32_t key) const;

    GiftiLabel* getLabel(const int32_t key);
    
    int32_t getUnassignedLabelKey() const;

    int32_t getNumberOfLabels() const;

    AString getLabelName(const int32_t key) const;

    void setLabelName(
                    const int32_t key,
                    const AString& name);

    void setLabel(const int32_t key,
                    const AString& name,
                    const float red,
                    const float green,
                    const float blue,
                    const float alpha);

    void setLabel(const int32_t key,
                  const AString& name,
                  const float red,
                  const float green,
                  const float blue,
                  const float alpha,
                  const float x,
                  const float y, 
                  const float z);
    
    bool isLabelSelected(const int32_t key) const;

    void setLabelSelected(
                    const int32_t key,
                    const bool sel);

    void setSelectionStatusForAllLabels(const bool newStatus);

    float getLabelAlpha(const int32_t key) const;

    void getLabelColor(const int32_t key, float rgbaOut[4]) const;

    void setLabelColor(
                    const int32_t key,
                    const float color[4]);

    std::vector<int32_t> getLabelKeysSortedByName() const;

    void resetLabelCounts();
    
    void removeLabelsWithZeroCounts();

    void createLabelsForKeys(const std::set<int32_t>& newKeys);

    bool hasMedialWallLabel() const;
    
    void writeAsXML(XmlWriter& xmlWriter);

    void writeAsXML(QXmlStreamWriter& xmlWriter) const;

    AString toString() const;

    AString toFormattedString(const AString& indentation) const;

    //void readFromXMLDOM(const Node* rootNode)
    //       ;

    void readFromXmlString(const AString& s);

    void readFromQXmlStreamReader(QXmlStreamReader& xml);

    void setModified();

    void clearModified();

    bool isModified() const;

    //Iterator<int32_t> getKeysIterator() const;

    std::set<int32_t> getKeys() const;

    void getKeys(std::vector<int32_t>& keysOut) const;

    void getKeysAndNames(std::map<int32_t, AString>& keysAndNamesOut) const;
    
//    bool hasLabelsWithInvalidGroupNameHierarchy() const;
    
    int32_t generateUnusedKey() const;
    
    void changeLabelKey(const int32_t currentKey,
                        const int32_t newKey);
    
    void exportToCaret5ColorFile(const AString& filename) const;
    
    void setHierarchy(const CaretHierarchy& hierarchy);
    void clearHierarchy();
    const CaretHierarchy& getHierarchy() const;

private:
    void issueLabelKeyZeroWarning(const AString& name) const;
    
    /** The label table storage.  Use a TreeMap since label keys
 may be sparse.
*/
    typedef std::map<int32_t, GiftiLabel*> LABELS_MAP;
    typedef std::map<int32_t, GiftiLabel*>::iterator LABELS_MAP_ITERATOR;
    typedef std::map<int32_t, GiftiLabel*>::const_iterator LABELS_MAP_CONST_ITERATOR;

    LABELS_MAP labelsMap;

    /**tracks modification status */
    bool modifiedFlag;

    int32_t m_tableModelColumnIndexKey;
    int32_t m_tableModelColumnIndexName;
    int32_t m_tableModelColumnIndexColorSwatch;
    int32_t m_tableModelColumnIndexRed;
    int32_t m_tableModelColumnIndexGreen;
    int32_t m_tableModelColumnIndexBlue;
    int32_t m_tableModelColumnCount;
    
    CaretForwardHelper<CaretHierarchy> m_hierarchy; //helper for implementing forward-declared members with reduced per-class code
};

} // namespace

#endif // __GIFTILABELTABLE_H__
