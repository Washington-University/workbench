
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __ANNOTATION_TEXT_SUBSTITUTION_FILE_DECLARE__
#include "AnnotationTextSubstitutionFile.h"
#undef __ANNOTATION_TEXT_SUBSTITUTION_FILE_DECLARE__

#include <QBuffer>
#include <QFile>

#include "DataFileException.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventMapYokingValidation.h"
#include "EventAnnotationTextSubstitutionGet.h"
#include "EventAnnotationTextSubstitutionInvalidate.h"
#include "GiftiMetaData.h"
#include "qxtcsvmodel.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTextSubstitutionFile 
 * \brief Substitutes text within text annotations
 * \ingroup Files
 */

/**
 * Constructor.
 */
AnnotationTextSubstitutionFile::AnnotationTextSubstitutionFile()
: CaretDataFile(DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION)
{
    initializeAnnotationTextSubstitutionFile();
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
}

/**
 * Destructor.
 */
AnnotationTextSubstitutionFile::~AnnotationTextSubstitutionFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationTextSubstitutionFile::AnnotationTextSubstitutionFile(const AnnotationTextSubstitutionFile& obj)
: CaretDataFile(obj),
EventListenerInterface()
{
    initializeAnnotationTextSubstitutionFile();
    this->copyHelperAnnotationTextSubstitutionFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationTextSubstitutionFile&
AnnotationTextSubstitutionFile::operator=(const AnnotationTextSubstitutionFile& obj)
{
    if (this != &obj) {
        CaretDataFile::operator=(obj);
        this->copyHelperAnnotationTextSubstitutionFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationTextSubstitutionFile::copyHelperAnnotationTextSubstitutionFile(const AnnotationTextSubstitutionFile& obj)
{
    *m_metadata = *obj.m_metadata;
    m_dataValues = obj.m_dataValues;
    m_numberOfSubstitutions = obj.m_numberOfSubstitutions;
    m_numberOfRows = obj.m_numberOfRows;
    m_substitutionNameToIndexMap = obj.m_substitutionNameToIndexMap;
    m_mapYokingGroup = obj.m_mapYokingGroup;
    setSelectedRowIndexPrivate(obj.m_selectedRowIndex);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationTextSubstitutionFile::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP) {
        EventMapYokingSelectMap* mapEvent = dynamic_cast<EventMapYokingSelectMap*>(event);
        CaretAssert(mapEvent);
        
        if (mapEvent->getMapYokingGroup() == m_mapYokingGroup) {
            setSelectedMapIndex(mapEvent->getMapIndex());
        }

        event->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_VALIDATION) {
        EventMapYokingValidation* yokeEvent = dynamic_cast<EventMapYokingValidation*>(event);
        CaretAssert(yokeEvent);
        if (yokeEvent->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            yokeEvent->addAnnotationTextSubstitutionFile(this, m_mapYokingGroup);
        }
    }
}

/**
 * Initialize an instance of an annotation substitution file.
 */
void
AnnotationTextSubstitutionFile::initializeAnnotationTextSubstitutionFile()
{
    m_metadata.reset(new GiftiMetaData());
    clearPrivate();
    
    m_sceneAssistant.reset(new SceneClassAssistant());
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
AnnotationTextSubstitutionFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
AnnotationTextSubstitutionFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* nothing */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
AnnotationTextSubstitutionFile::getFileMetaData()
{
    return m_metadata.get();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
AnnotationTextSubstitutionFile::getFileMetaData() const
{
    return m_metadata.get();
}

/**
 * @return True if this file is empty, else false.
 */
bool
AnnotationTextSubstitutionFile::isEmpty() const
{
    return m_dataValues.empty();
}

/**
 * Clear the content of this file.
 * This method is virtual so do not call from constructor/destructor.
 */
void
AnnotationTextSubstitutionFile::clear()
{
    CaretDataFile::clear();
    
    clearPrivate();
    
}

/**
 * Clear the content of this file.
 */
void
AnnotationTextSubstitutionFile::clearPrivate()
{
    m_metadata->clear();
    m_dataValues.clear();
    m_substitutionNameToIndexMap.clear();
    m_numberOfSubstitutions = 0;
    m_numberOfRows = 0;
    m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    m_selectedRowIndex = -1;  /* invalid so text substitutions get invalidated */
}

/**
 * @return true if file is modified, else false.
 */
bool
AnnotationTextSubstitutionFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    
    if (m_metadata->isModified()) {
        return true;
    }
    
    return false;
}

/**
 * @return Number of substitutions in the file.
 */
int32_t
AnnotationTextSubstitutionFile::getNumberOfSubstitutions() const
{
    return m_numberOfSubstitutions;
}

/**
 * @return Number of maps for each substitutions in the file.
 */
int32_t
AnnotationTextSubstitutionFile::getNumberOfMaps() const
{
    return m_numberOfRows;
}

/**
 * For the substitution at the given index, get the value at the given index.
 *
 * @param textSubstitutionIndex
 *    Index of the text substitution
 * @param mapIndex
 *    Index of the map in the text substitution
 */
AString
AnnotationTextSubstitutionFile::getTextSubstitution(const int32_t textSubstitutionIndex,
                                                    const int32_t mapIndex) const
{
    const int32_t rowIndex(mapIndex);
    CaretAssert((textSubstitutionIndex >=0)
                && (textSubstitutionIndex < m_numberOfSubstitutions));
    CaretAssert((rowIndex >= 0)
                && (rowIndex < m_numberOfRows));

    AString textValueOut;
    if ((rowIndex >= 0)
        && (rowIndex < m_numberOfRows)) {
        const int32_t substitutionOffset = (textSubstitutionIndex * m_numberOfRows);
        const int32_t dataIndex = substitutionOffset + rowIndex;
        
        CaretAssertVectorIndex(m_dataValues, dataIndex);
        textValueOut = m_dataValues[dataIndex];
    }
    
    return textValueOut;
}

/**
 * For the substitution at the given name, get the value at the given index.
 *
 * @param textSubstitutionName
 *    Name of the text substitution
 * @param mapIndex
 *    Index of the map in the text substitution
 */
AString
AnnotationTextSubstitutionFile::getTextSubstitution(const AString& textSubstitutionName,
                                                    const int32_t mapIndex) const
{
    /*
     * Convert name to a substitution index
     */
    const int32_t textSubstitutionIndex = getColumnIndexForSubstitutionName(textSubstitutionName);

    AString text;
    if (textSubstitutionIndex >= 0) {
        text = getTextSubstitution(textSubstitutionIndex,
                                   mapIndex);
    }
    
    return text;
}

/**
 * Get text substitution values for the given event.
 *
 * @param substituteEvent
 *     The text substitution event.
 */
void
AnnotationTextSubstitutionFile::getSubstitutionValues(EventAnnotationTextSubstitutionGet* substituteEvent) const
{
    const int32_t numSubs(substituteEvent->getNumberOfSubstitutionIDs());
    for (int32_t i = 0; i < numSubs; i++) {
        const AnnotationTextSubstitution& subs(substituteEvent->getSubstitutionID(i));
        const AString name(subs.getColumnID());
        const AString value(getTextSubstitution(name,
                                                getSelectedMapIndex()));
        substituteEvent->setSubstitutionTextValue(i,
                                                  value);
    }
}


/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
AnnotationTextSubstitutionFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                    SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * @return True if this file type supports writing, else false.
 *
 * By default, this method returns true.  Files that do not support
 * writing should override this method and return false.
 */
bool
AnnotationTextSubstitutionFile::supportsWriting() const
{
    return false;
}


/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
AnnotationTextSubstitutionFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                         const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Try to restore values for older scenes
     */
    m_oldSceneValuesValidFlag = false;
    m_selectedRowIndex = -1;
    m_mapYokingGroup   = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    const SceneObject* valueIndexObject(sceneClass->getObjectWithName("m_selectedValueIndex"));
    const SceneObject* mapYokingObject(sceneClass->getObjectWithName("m_mapYokingGroup"));
    if ((valueIndexObject != NULL)
        && (mapYokingObject != NULL)) {
        m_selectedRowIndex = sceneClass->getIntegerValue("m_selectedValueIndex");
        m_mapYokingGroup   = sceneClass->getEnumeratedTypeValue<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_mapYokingGroup",
                                                                                                              MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
        m_oldSceneValuesValidFlag = true;
    }
}

/**
 * @return Are old scene values valid?
 */
bool
AnnotationTextSubstitutionFile::isOldSceneValuesValid() const
{
    return m_oldSceneValuesValidFlag;
}

/**
 * Clean up the CSV model
 * @param csvModel
 *    The CSV model
 * @param filename
 *    Name of the file
 */
void
AnnotationTextSubstitutionFile::cleanCsvModel(QxtCsvModel* csvModel,
                                              const AString& filename)
{
    /*
     * QxtCsvModel interprets blank lines as rows.
     * So, find blank rows at end and reduce number of rows
     */
    int32_t lastValidRowIndex = -1;
    for (int32_t iRow = 0; iRow < m_numberOfRows; iRow++) {
        bool validRowFlag = false;
        for (int32_t iCol = 0; iCol < m_numberOfSubstitutions; iCol++) {
            const QModelIndex modelIndex = csvModel->index(iRow,
                                                          iCol);
            if ( ! modelIndex.isValid()) {
                throw DataFileException("Getting QModelIndex failed for row="
                                        + AString::number(iRow)
                                        + " and column="
                                        + AString::number(iCol)
                                        + " for file "
                                        + filename);
            }
            
            const QVariant dataVariant = csvModel->data(modelIndex,
                                                       Qt::DisplayRole);
            AString cell = dataVariant.toString();
            if ( ! cell.isEmpty()) {
                validRowFlag = true;
                
                /*
                 * Remove double quotes from ends of a cell
                 */
                if (cell.startsWith('\"')
                    && cell.endsWith('\"')) {
                    cell = cell.mid(1, cell.length() - 2);
                    csvModel->setData(modelIndex,
                                      cell,
                                      Qt::DisplayRole);
                }
            }
        }
        
        if (validRowFlag) {
            lastValidRowIndex = iRow;
        }
    }
    
    if (lastValidRowIndex >= 0) {
        int32_t numberOfValidRows = lastValidRowIndex + 1;
        m_numberOfRows = numberOfValidRows;
    }
}


/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void
AnnotationTextSubstitutionFile::readFile(const AString& filename)
{
    clear();
    
    try {
        checkFileReadability(filename);
        
        QFile file(filename);
        if ( ! file.open(QFile::ReadOnly)) {
            throw DataFileException("Unable to open "
                                    + filename
                                    + " for reading.");
        }
        
        /*
         * Excel writes CSV files using '\r' between lines.
         * QxtCsvModel uses QIODevice::readLine(), and Qt's documentation 
         * indicates that QIODevice::readLine() uses only '\n' as a line separator.
         * See http://doc.qt.io/Qt-5/qiodevice.html#readLine
         */
        AString fileContent = file.readAll();
        fileContent = fileContent.trimmed();
        fileContent = fileContent.replace('\r', '\n');
        fileContent = fileContent.replace("\n\n", "\n");
        
        QByteArray byteArray(fileContent.toLatin1());
        QBuffer buffer(&byteArray);
        const bool hasHeaderFlag(false);
        const QChar separator(',');
        
        QxtCsvModel csvModel;
        csvModel.setSource(&buffer,
                           hasHeaderFlag,
                           separator);
        
        m_numberOfSubstitutions = csvModel.columnCount();
        m_numberOfRows          = csvModel.rowCount();
        
        if ((m_numberOfSubstitutions > 0)
            && (m_numberOfRows > 0)) {
            cleanCsvModel(&csvModel,
                          filename);
        }
        
        const int32_t numberOfValues = (m_numberOfSubstitutions
                                        * m_numberOfRows);
        if (numberOfValues <= 0) {
            throw DataFileException(filename
                                    + " is empty or reading failed.");
        }
        
        /*
         * Read data from the CSV Model.
         * Data is stored in Column Major Order so
         * All values for first column, then
         * all values for second column, etc.
         */
        m_dataValues.reserve(numberOfValues);
        for (int32_t iColumn = 0; iColumn < m_numberOfSubstitutions; iColumn++) {
            for (int32_t iRow = 0; iRow < m_numberOfRows; iRow++) {
                const QModelIndex modelIndex = csvModel.index(iRow,
                                                              iColumn);
                if ( ! modelIndex.isValid()) {
                    throw DataFileException("Getting QModelIndex failed for row="
                                            + AString::number(iRow)
                                            + " and column="
                                            + AString::number(iColumn)
                                            + " for file "
                                            + filename);
                }
                
                const QVariant dataVariant = csvModel.data(modelIndex,
                                                           Qt::DisplayRole);
                m_dataValues.push_back(dataVariant.toString());
            }
        }
        
        CaretAssert(numberOfValues == static_cast<int32_t>(m_dataValues.size()));
    }
    catch (const DataFileException& dfe) {
        clear();
        throw dfe;
    }
    
    /*
     * At this time, the file does not support 'names' for the substitutions
     *
     * Column names in Excel are A, B, ..., Z, AA, AB, ..., AZ, BA, BB, ...
     */
    for (int32_t iColumn = 0; iColumn < m_numberOfSubstitutions; iColumn++) {
        const AString columnName = columnIndexToDefaultSubstitutionName(iColumn);
        m_substitutionNameToIndexMap.insert(std::make_pair(columnName, iColumn));
    }
    
    setFileName(filename);
    
    clearModified();
}

/**
 * Convert a column index into a default subsitution name (AA, AB, ..., BA, ...) that
 * is the same as the column names in Excel.
 *
 * @param columnIndex
 *     Index of the column.
 * @return 
 *     The default name for the column.
 */
AString
AnnotationTextSubstitutionFile::columnIndexToDefaultSubstitutionName(const int32_t columnIndex) const
{
    CaretAssert(columnIndex >= 0);
    
    AString columnName;
    
    const int32_t zeroCharacterUnicodeValue(48);
    const int32_t nineCharacterUnicodeValue(57);
    const int32_t aCharacterUnicodeValue(97);
    const int32_t pCharacterUnicodeValue(112);
    
    /*
     * Use QString's number() method to generate a Base 26 number
     * from the column index.  The characters int he Base 26 number
     * are 0 to 9, and 'a' to 'p'.
     */
    const QString baseTwentySixChars(QString::number(columnIndex, 26));
    const int32_t numChars = baseTwentySixChars.length();
    for (int32_t iChar = 0; iChar < numChars; iChar++) {
        QChar unicodeChar = baseTwentySixChars.at(iChar).toLower();
        int32_t unicodeValue = unicodeChar.unicode();
        int32_t offsetFromCapitalA = -1;
        if ((unicodeValue >= zeroCharacterUnicodeValue)
            && (unicodeValue <= nineCharacterUnicodeValue)) {
            offsetFromCapitalA = unicodeValue - zeroCharacterUnicodeValue;
        }
        else if ((unicodeValue >= aCharacterUnicodeValue)
                 && (unicodeValue <= pCharacterUnicodeValue)) {
            offsetFromCapitalA = unicodeValue - aCharacterUnicodeValue + 10;
        }
        else {
            CaretAssertMessage(0, ("Unicode character=" + QString(unicodeChar)
                                   + " unicode value=" + AString::number(unicodeValue)
                                   + " is not a digit nor uppercase character"));
        }
        if ((numChars > 1)
            && (iChar == 0)) {
            --offsetFromCapitalA;
        }
        
        CaretAssert((offsetFromCapitalA >= 0)
                    && (offsetFromCapitalA < 26));
        columnName.append(QChar(static_cast<int>('A') + offsetFromCapitalA));
    }
    
    return columnName;
}

/**
 * Get the column containing substitutions for the given substitution name.
 * The name should be a valid name.
 *
 * @param substitutionName
 *     Name of the substitution
 * @return
 *     Column for the name or -1 if name not found.
 */
int32_t
AnnotationTextSubstitutionFile::getColumnIndexForSubstitutionName(const AString substitutionName) const
{
    int32_t columnIndex = -1;
    
    const auto iter = m_substitutionNameToIndexMap.find(substitutionName);
    if (iter != m_substitutionNameToIndexMap.end()) {
        columnIndex = iter->second;
    }
    else {
        CaretLogWarning("Requesting column for invalid substitution name \""
                        + substitutionName
                        + "\"");
    }
    
    return columnIndex;
}

/**
 * @return The selected map yoking group for this file.
 */
MapYokingGroupEnum::Enum
AnnotationTextSubstitutionFile::getMapYokingGroup() const
{
    return m_mapYokingGroup;
}

/**
 * Set the map yoking group for this file.
 *
 * @param mapYokingGroup
 *     New map yoking group selection.
 */
void
AnnotationTextSubstitutionFile::setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup)
{
    m_mapYokingGroup = mapYokingGroup;
}

/**
 * @return The selected map index.
 */
int32_t
AnnotationTextSubstitutionFile::getSelectedMapIndex() const
{
    /* validates index */
    setSelectedRowIndexPrivate(m_selectedRowIndex);
    
    return m_selectedRowIndex;
}

/**
 * Set the selected map index.
 *
 * @param mapIndex
 *     New value for index.
 */
void
AnnotationTextSubstitutionFile::setSelectedMapIndex(const int32_t mapIndex)
{
    setSelectedRowIndexPrivate(mapIndex);
}

/**
 * Set the selected row index.  Private method that invalidates text substitutions
 * and ensures selected row index is valid.
 *
 * @param rowIndex
 *     New value for index.
 */
void
AnnotationTextSubstitutionFile::setSelectedRowIndexPrivate(const int32_t rowIndex) const
{
    const int32_t previousRowIndex = m_selectedRowIndex;
    
    m_selectedRowIndex = rowIndex;
    
    if (m_selectedRowIndex < 0) {
        m_selectedRowIndex = 0;
    }
    else if (m_selectedRowIndex >= m_numberOfRows) {
        m_selectedRowIndex = m_numberOfRows - 1;
    }

    if (previousRowIndex != m_selectedRowIndex) {
        EventManager::get()->sendEvent(EventAnnotationTextSubstitutionInvalidate().getPointer());
    }
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void
AnnotationTextSubstitutionFile::writeFile(const AString& /*filename*/)
{
    throw DataFileException("Writing of Annotation Text Substitution Files is not supported.");
}
