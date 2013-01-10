
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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "UserView.h"

using namespace caret;


    
/**
 * \class caret::CaretPreferences 
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
    this->removeAllUserViews();
    
    delete this->qSettings;
}

/**
 * Get the boolean value for the given preference name.
 * @param name
 *    Name of the preference.
 * @return
 *    Boolean value of preference or defaultValue if the
 *    named preference is not found.
 */
bool CaretPreferences::getBoolean(const AString& name,
                                  const bool defaultValue)
{
    bool b = this->qSettings->value(name, defaultValue).toBool();
    return b;
}

/**
 * Set the given preference name with the boolean value.
 * @param
 *    Name of the preference.
 * @param
 *    New value for preference.
 */
void CaretPreferences::setBoolean(const AString& name,
                                  const bool value)
{
    this->qSettings->setValue(name, value);
}

/**
 * Get the boolean value for the given preference name.
 * @param name
 *    Name of the preference.
 * @return
 *    Integer value of preference or defaultValue if the
 *    named preference is not found.
 */
int CaretPreferences::getInteger(const AString& name,
                                  const int defaultValue)
{
    int b = this->qSettings->value(name, defaultValue).toInt();
    return b;
}

/**
 * Set the given preference name with the integer value.
 * @param
 *    Name of the preference.
 * @param
 *    New value for preference.
 */
void CaretPreferences::setInteger(const AString& name,
                                  const int value)
{
    this->qSettings->setValue(name, value);
}

/**
 * Remove all user views.
 */
void 
CaretPreferences::removeAllUserViews()
{
    for (std::vector<UserView*>::iterator iter = this->userViews.begin();
         iter != this->userViews.end();
         iter++) {
        delete *iter;
    }
    this->userViews.clear();
}

/**
 * Get all of the user views.
 * @return
 *    All of the user views.
 */
std::vector<const UserView*> 
CaretPreferences::getAllUserViews()
{
    std::vector<const UserView*> viewsOut;
    viewsOut.insert(viewsOut.end(),
                    this->userViews.begin(),
                    this->userViews.end());
    return viewsOut;
}

/**
 * Get the user view with the specified name.
 * @param viewName
 *    Name of view.
 * @return
 *    Pointer to view or NULL if not found.
 */
const UserView* 
CaretPreferences::getUserView(const AString& viewName)
{
    for (std::vector<UserView*>::iterator iter = this->userViews.begin();
         iter != this->userViews.end();
         iter++) {
        UserView* uv = *iter;
        if (uv->getName() == viewName) {
            return uv;
        }
    }
    
    return NULL;
}

/**
 * Add a user view.  If a view with the same name exists
 * it is replaced.
 * @param
 *    New user view.
 */
void 
CaretPreferences::addUserView(const UserView& userView)
{
    for (std::vector<UserView*>::iterator iter = this->userViews.begin();
         iter != this->userViews.end();
         iter++) {
        UserView* uv = *iter;
        if (uv->getName() == userView.getName()) {
            *uv = userView;
            return;
        }
    }
    
    this->userViews.push_back(new UserView(userView));
    
    this->writeUserViews();
}

/**
 * Remove the user view with the specified name.
 */
void 
CaretPreferences::removeUserView(const AString& viewName)
{
    for (std::vector<UserView*>::iterator iter = this->userViews.begin();
         iter != this->userViews.end();
         iter++) {
        UserView* uv = *iter;
        if (uv->getName() == viewName) {
            this->userViews.erase(iter);
            delete uv;
            break;
        }
    }
    
    this->writeUserViews();
}

/**
 * Write the user views.
 */
void 
CaretPreferences::writeUserViews()
{
    this->qSettings->beginWriteArray(NAME_USER_VIEWS);
    const int32_t numViews = static_cast<int32_t>(this->userViews.size());
    for (int32_t i = 0; i < numViews; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->userViews[i]->getAsString());
    }
    this->qSettings->endArray();
    this->qSettings->sync();
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
    
    this->qSettings->beginWriteArray(NAME_COLOR_FOREGROUND);
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
    this->colorBackground[0] = colorBackground[0];
    this->colorBackground[1] = colorBackground[1];
    this->colorBackground[2] = colorBackground[2];
    this->qSettings->beginWriteArray(NAME_COLOR_BACKGROUND);
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
    this->qSettings->beginWriteArray(NAME_PREVIOUS_SPEC_FILES);
    for (int i = 0; i < num; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->previousSpecFiles[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
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
    this->qSettings->beginWriteArray(NAME_PREVIOUS_OPEN_FILE_DIRECTORIES);
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
 * Will also update the level in the Caret Logger.
 * @param loggingLevel
 *    New value for logging level.
 */
void 
CaretPreferences::setLoggingLevel(const LogLevelEnum::Enum loggingLevel)
{
    this->loggingLevel = loggingLevel;
    
    const AString name = LogLevelEnum::toName(this->loggingLevel);
    this->qSettings->setValue(NAME_LOGGING_LEVEL, name);
    this->qSettings->sync();
    CaretLogger::getLogger()->setLevel(loggingLevel);
}

/**
 * @return  Are axes crosshairs displayed?
 */
bool 
CaretPreferences::isVolumeAxesCrosshairsDisplayed() const
{
    return this->displayVolumeAxesCrosshairs;
}

/**
 * Set axes crosshairs displayed
 * @param displayed
 *   New status.
 */
void 
CaretPreferences::setVolumeAxesCrosshairsDisplayed(const bool displayed)
{
    this->displayVolumeAxesCrosshairs = displayed;
    this->setBoolean(CaretPreferences::NAME_AXES_CROSSHAIRS, 
                     this->displayVolumeAxesCrosshairs);
}

/**
 * @return  Are axes labels displayed?
 */
bool 
CaretPreferences::isVolumeAxesLabelsDisplayed() const
{
    return this->displayVolumeAxesLabels;
}

/**
 * Set axes labels displayed
 * @param displayed
 *   New status.
 */
void 
CaretPreferences::setVolumeAxesLabelsDisplayed(const bool displayed)
{
    this->displayVolumeAxesLabels = displayed;
    this->setBoolean(CaretPreferences::NAME_AXES_LABELS, 
                     this->displayVolumeAxesLabels);
}


/**
 * @return  Are montage axes coordinates displayed?
 */
bool
CaretPreferences::isVolumeMontageAxesCoordinatesDisplayed() const
{
    return this->displayVolumeAxesCoordinates;
}

/**
 * Set montage axes coordinates displayed
 * @param displayed
 *   New status.
 */
void
CaretPreferences::setVolumeMontageAxesCoordinatesDisplayed(const bool displayed)
{
    this->displayVolumeAxesCoordinates = displayed;
    this->setBoolean(CaretPreferences::NAME_AXES_COORDINATE,
                     this->displayVolumeAxesCoordinates);
}

/**
 * @return The toolbox type.
 */
int32_t 
CaretPreferences::getToolBoxType() const
{
    return this->toolBoxType;
}

/**
 * Set the toolbox type.
 * @param toolBoxType
 *    New toolbox type.
 */
void 
CaretPreferences::setToolBoxType(const int32_t toolBoxType)
{
    this->toolBoxType = toolBoxType;
    this->setInteger(CaretPreferences::NAME_TOOLBOX_TYPE, 
                     this->toolBoxType);
}


/**
 * @return Is contralateral identification enabled?
 *
bool 
CaretPreferences::isContralateralIdentificationEnabled() const
{
    return this->contralateralIdentificationEnabled;
}
*/

/**
 * Set contralateral identification enabled.
 * @param enabled
 *    New status.
 *
void 
CaretPreferences::setContralateralIdentificationEnabled(const bool enabled)
{
    this->contralateralIdentificationEnabled = enabled;
    this->setBoolean(CaretPreferences::NAME_IDENTIFICATION_CONTRALATERAL, 
                     this->contralateralIdentificationEnabled);
}
*/

/**
 * @return Is the splash screen enabled?
 */
bool 
CaretPreferences::isSplashScreenEnabled() const
{
    return this->splashScreenEnabled;
}

/**
 * Set the splash screen enabled.
 * @param enabled
 *    New status.
 */
void 
CaretPreferences::setSplashScreenEnabled(const bool enabled)
{
    this->splashScreenEnabled = enabled;
    this->setBoolean(CaretPreferences::NAME_SPLASH_SCREEN, 
                     this->splashScreenEnabled);
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
    const int numFG = this->qSettings->beginReadArray(NAME_COLOR_FOREGROUND);
    for (int i = 0; i < numFG; i++) {
        this->qSettings->setArrayIndex(i);
        colorForeground[i] = static_cast<uint8_t>(this->qSettings->value(AString::number(i)).toInt());
    }
    this->qSettings->endArray();
    
    this->colorBackground[0] = 0;
    this->colorBackground[1] = 0;
    this->colorBackground[2] = 0;
    const int numBG = this->qSettings->beginReadArray(NAME_COLOR_BACKGROUND);
    for (int i = 0; i < numBG; i++) {
        this->qSettings->setArrayIndex(i);
        colorBackground[i] = static_cast<uint8_t>(this->qSettings->value(AString::number(i)).toInt());
    }
    this->qSettings->endArray();
    
    this->previousSpecFiles.clear();    
    const int numPrevSpec = this->qSettings->beginReadArray(NAME_PREVIOUS_SPEC_FILES);
    for (int i = 0; i < numPrevSpec; i++) {
        this->qSettings->setArrayIndex(i);
        previousSpecFiles.push_back(this->qSettings->value(AString::number(i)).toString());
    }
    this->qSettings->endArray();
    
    this->previousOpenFileDirectories.clear();
    const int numPrevDir = this->qSettings->beginReadArray(NAME_PREVIOUS_OPEN_FILE_DIRECTORIES);
    for (int i = 0; i < numPrevDir; i++) {
        this->qSettings->setArrayIndex(i);
        previousOpenFileDirectories.push_back(this->qSettings->value(AString::number(i)).toString());
    }
    this->qSettings->endArray();
    
    this->readUserViews(false);

    AString levelName = this->qSettings->value(NAME_LOGGING_LEVEL,
                                          LogLevelEnum::toName(LogLevelEnum::INFO)).toString();
    bool valid = false;
    LogLevelEnum::Enum logLevel = LogLevelEnum::fromName(levelName, &valid);
    if (valid == false) {
        logLevel = LogLevelEnum::INFO;
    }
    this->setLoggingLevel(logLevel);
    
    this->displayVolumeAxesLabels = this->getBoolean(CaretPreferences::NAME_AXES_LABELS,
                                                     true);
    this->displayVolumeAxesCrosshairs = this->getBoolean(CaretPreferences::NAME_AXES_CROSSHAIRS,
                                                         true);    
    this->displayVolumeAxesCoordinates = this->getBoolean(CaretPreferences::NAME_AXES_COORDINATE,
                                                          true);
    
    this->animationStartTime = 0.0;//this->qSettings->value(CaretPreferences::NAME_ANIMATION_START_TIME).toDouble();

    this->toolBoxType = this->getInteger(CaretPreferences::NAME_TOOLBOX_TYPE,
                                         0);
    
    this->splashScreenEnabled = this->getBoolean(CaretPreferences::NAME_SPLASH_SCREEN,
                                                 true);
    
//    this->contralateralIdentificationEnabled = this->getBoolean(CaretPreferences::NAME_IDENTIFICATION_CONTRALATERAL,
//                                                                   false);
    
}

/**
 * Read the user views.  Since user's may created views and want to use them
 * in multiple instance of workbench that are running, this method allows 
 * the user view's to be read without affecting other preferences.
 */
void
CaretPreferences::readUserViews(const bool performSync)
{
    this->qSettings->sync();
    
    this->removeAllUserViews();
    const int numUserViews = this->qSettings->beginReadArray(NAME_USER_VIEWS);
    for (int i = 0; i < numUserViews; i++) {
        this->qSettings->setArrayIndex(i);
        const AString viewString = this->qSettings->value(AString::number(i)).toString();
        UserView uv;
        if (uv.setFromString(viewString)) {
            this->userViews.push_back(new UserView(uv));
        }
    }
    this->qSettings->endArray();    
}


void CaretPreferences::getAnimationStartTime(double& time)
{  
   time = animationStartTime;
   
}

void CaretPreferences::setAnimationStartTime(const double& time)
{
   animationStartTime = time;
   //this->qSettings->setValue(CaretPreferences::NAME_ANIMATION_START_TIME, time);
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
