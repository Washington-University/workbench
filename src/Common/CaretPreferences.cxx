
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __CARET_PREFERENCES_DECLARE__
#include "CaretPreferences.h"
#undef __CARET_PREFERENCES_DECLARE__

#include <algorithm>

#include <QSettings>
#include <QStringList>

using namespace caret;


    
/**
 * \class CaretPreferences 
 * \brief Preferences for use in Caret.
 *
 * Maintains preferences for use in Caret.  The
 * preferences are read only one time, when an 
 * instance is created.  If a preference is changed,
 * it is written.
 */
/**
 * Constructor.
 */
CaretPreferences::CaretPreferences()
: CaretObject()
{
    this->qSettings = new QSettings("brainvis.wustl.edu",
                                    "Caret7");
    this->readPreferences();
}

/**
 * Destructor.
 */
CaretPreferences::~CaretPreferences()
{
    delete this->qSettings;
}

/**
 * Get the foreground color as integer rgb components ranging in value
 * from 0 to 255.
 *
 * @param colorForeground
 *   The output into which rgb color components are loaded.
 */
void 
CaretPreferences::getColorForeground(uint8_t colorForeground[3]) const
{
    colorForeground[0] = this->colorForeground[0];
    colorForeground[1] = this->colorForeground[1];
    colorForeground[2] = this->colorForeground[2];
}

/**
 * Get the foreground color as float rgb components ranging in value
 * from 0 to 1
 *
 * @param colorForeground
 *   The output into which rgb color components are loaded.
 */
void 
CaretPreferences::getColorForeground(float colorForeground[3]) const
{
    uint8_t byteForeground[3];
    this->getColorForeground(byteForeground);
    
    colorForeground[0] = std::min((byteForeground[0] / 255.0), 255.0);
    colorForeground[1] = std::min((byteForeground[1] / 255.0), 255.0);
    colorForeground[2] = std::min((byteForeground[2] / 255.0), 255.0);
}

/**
 * Set the foreground color as integer rgb components ranging in value
 * from 0 to 255.
 *
 * @param colorForeground
 *   New values for the foreground rgb color components.
 */
void 
CaretPreferences::setColorForeground(const uint8_t colorForeground[3])
{
    this->colorForeground[0] = colorForeground[0];
    this->colorForeground[1] = colorForeground[1];
    this->colorForeground[2] = colorForeground[2];
    
    this->qSettings->beginWriteArray("colorForeground");
    for (int i = 0; i < 3; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  colorForeground[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Get the background color as integer rgb components ranging in value
 * from 0 to 255.
 *
 * @param colorBackground
 *   The output into which rgb color components are loaded.
 */
void 
CaretPreferences::getColorBackground(uint8_t colorBackground[3]) const
{
    colorBackground[0] = this->colorBackground[0];
    colorBackground[1] = this->colorBackground[1];
    colorBackground[2] = this->colorBackground[2];
}

/**
 * Get the background color as float rgb components ranging in value
 * from 0 to 1
 *
 * @param colorBackground
 *   The output into which rgb color components are loaded.
 */
void 
CaretPreferences::getColorBackground(float colorBackground[3]) const
{
    uint8_t byteBackground[3];
    this->getColorBackground(byteBackground);
    
    colorBackground[0] = std::min((byteBackground[0] / 255.0), 255.0);
    colorBackground[1] = std::min((byteBackground[1] / 255.0), 255.0);
    colorBackground[2] = std::min((byteBackground[2] / 255.0), 255.0);
}

/**
 * Set the background color as integer rgb components ranging in value
 * from 0 to 255.
 *
 * @param colorBackground
 *   New values for the background rgb color components.
 */
void 
CaretPreferences::setColorBackground(const uint8_t colorBackground[3])
{
    this->qSettings->beginWriteArray("colorBackground");
    for (int i = 0; i < 3; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  colorBackground[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Get the previous spec files.
 *
 * @param previousSpecFiles
 *    Will contain previous spec files.
 */
void 
CaretPreferences::getPreviousSpecFiles(std::vector<AString>& previousSpecFiles) const
{
    previousSpecFiles = this->previousSpecFiles;
}

/**
 * Add to the previous spec files.
 * 
 * @param specFileName
 *    Spec file added to the previous spec files.
 */
void 
CaretPreferences::addToPreviousSpecFiles(const AString& specFileName)
{
    if (specFileName.isEmpty() == false) {
        this->addToPrevious(this->previousSpecFiles,
                            specFileName);
    }
    
    const int32_t num = static_cast<int32_t>(this->previousSpecFiles.size());
    this->qSettings->beginWriteArray("previousSpecFiles");
    for (int i = 0; i < num; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->previousSpecFiles[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();

    QSettings::Status status = this->qSettings->status();
}

void 
CaretPreferences::clearPreviousSpecFiles()
{
    this->previousSpecFiles.clear();
    this->addToPreviousSpecFiles("");
}

/**
 * Get the directories that were used in the Open File Dialog.
 *
 * @param previousOpenFileDirectories
 *    Will contain previous directories.
 */
void 
CaretPreferences::getPreviousOpenFileDirectories(std::vector<AString>& previousOpenFileDirectories) const
{
    previousOpenFileDirectories = this->previousOpenFileDirectories;
}

/**
 * Get the directories that were used in the Open File Dialog.
 *
 * @param previousOpenFileDirectories
 *    Will contain previous directories.
 */
void 
CaretPreferences::getPreviousOpenFileDirectories(QStringList& previousOpenFileDirectoriesList) const
{
    previousOpenFileDirectoriesList.clear();
    const int32_t numDirectories = static_cast<int32_t>(this->previousOpenFileDirectories.size());
    for (int32_t i = 0; i < numDirectories; i++) {
        previousOpenFileDirectoriesList.append(this->previousOpenFileDirectories[i]);
    }
}

/**
 * Add to the previous directories that were used in the Open File Dialog.
 * 
 * @param directoryName
 *    Directory added to the previous directories from Open File Dialog.
 */
void 
CaretPreferences::addToPreviousOpenFileDirectories(const AString& directoryName)
{
    this->addToPrevious(this->previousOpenFileDirectories,
                        directoryName);
    
    const int32_t num = static_cast<int32_t>(this->previousOpenFileDirectories.size());    
    this->qSettings->beginWriteArray("previousOpenFileDirectories");
    for (int i = 0; i < num; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->previousOpenFileDirectories[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Add to a list of previous, removing any matching entries
 * and limiting the size of the list.
 *
 * @param previousDeque
 *   Deque containing the previous elements.
 * @param newName
 *   Name that is added at the front.
 */
void 
CaretPreferences::addToPrevious(std::vector<AString>& previousVector,
                                const AString& newName)
{
    /*
     * Note: remove moves duplicate elements to after 'pos' but
     * does not change the size of the container so use erase
     * to remove the duplicate elements from the container.
     */
    std::vector<AString>::iterator pos = std::remove(previousVector.begin(),
                                          previousVector.end(),
                                          newName);
    previousVector.erase(pos, 
                        previousVector.end());

    const uint64_t MAX_ELEMENTS = 10;
    if (previousVector.size() > MAX_ELEMENTS) {
        previousVector.erase(previousVector.begin() + MAX_ELEMENTS,
                            previousVector.end());
    }
    
    previousVector.insert(previousVector.begin(),
                          newName);
}

/**
 * @return  The logging level.
 */
LogLevelEnum::Enum 
CaretPreferences::getLoggingLevel() const
{
    return this->loggingLevel;
}

/**
 * Set the logging level.
 * @param loggingLevel
 *    New value for logging level.
 */
void 
CaretPreferences::setLoggingLevel(const LogLevelEnum::Enum loggingLevel)
{
    this->loggingLevel = loggingLevel;
    
    const AString name = LogLevelEnum::toName(this->loggingLevel);
    this->qSettings->setValue("loggingLevel", name);
    this->qSettings->sync();
}

/**
 * Initialize/Read the preferences
 */
void 
CaretPreferences::readPreferences()
{
    this->colorForeground[0] = 255;
    this->colorForeground[1] = 255;
    this->colorForeground[2] = 255;
    const int numFG = this->qSettings->beginReadArray("colorForeground");
    for (int i = 0; i < numFG; i++) {
        this->qSettings->setArrayIndex(i);
        colorForeground[i] = static_cast<uint8_t>(this->qSettings->value(AString::number(i)).toInt());
    }
    this->qSettings->endArray();
    
    this->colorBackground[0] = 0;
    this->colorBackground[1] = 0;
    this->colorBackground[2] = 0;
    const int numBG = this->qSettings->beginReadArray("colorBackground");
    for (int i = 0; i < numBG; i++) {
        this->qSettings->setArrayIndex(i);
        colorBackground[i] = static_cast<uint8_t>(this->qSettings->value(AString::number(i)).toInt());
    }
    this->qSettings->endArray();
    
    this->previousSpecFiles.clear();    
    const int numPrevSpec = this->qSettings->beginReadArray("previousSpecFiles");
    for (int i = 0; i < numPrevSpec; i++) {
        this->qSettings->setArrayIndex(i);
        previousSpecFiles.push_back(this->qSettings->value(AString::number(i)).toString());
    }
    this->qSettings->endArray();
    
    this->previousOpenFileDirectories.clear();
    const int numPrevDir = this->qSettings->beginReadArray("previousOpenFileDirectories");
    for (int i = 0; i < numPrevDir; i++) {
        this->qSettings->setArrayIndex(i);
        previousOpenFileDirectories.push_back(this->qSettings->value(AString::number(i)).toString());
    }
    this->qSettings->endArray();
    
    
    LogLevelEnum::Enum defaultValue = LogLevelEnum::INFO;
    AString levelName = this->qSettings->value("loggingLevel", 
                                          LogLevelEnum::toName(defaultValue)).toString();
    bool valid = false;
    this->loggingLevel = LogLevelEnum::fromName(levelName, &valid);
    if (valid == false) {
        this->loggingLevel = LogLevelEnum::INFO;
    }
    
    QSettings::Status status = this->qSettings->status();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CaretPreferences::toString() const
{
    return "CaretPreferences";
}
