
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

#define __CARET_PREFERENCES_DECLARE__
#include "CaretPreferences.h"
#undef __CARET_PREFERENCES_DECLARE__

#include <algorithm>
#include <set>

#include <QSettings>
#include <QStringList>

#include "ApplicationInformation.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferenceDataValue.h"
#include "DataFileTypeEnum.h"
#include "FileInformation.h"
#include "ModelTransform.h"
#include "RecentFileItem.h"
#include "RecentFileItemsContainer.h"
#include "RecentFileItemsFilter.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "TileTabsLayoutManualConfiguration.h"
#include "WuQMacroGroup.h"

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
    m_macros.reset(new WuQMacroGroup("Preferences"));
    
    this->qSettings = new QSettings("brainvis.wustl.edu",
                                    "Caret7");
    this->readPreferences();
    
    m_volumeCrossHairGapPreference.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                      "volumeAxesCrosshairGap",
                                                                      CaretPreferenceDataValue::DataType::FLOAT,
                                                                      CaretPreferenceDataValue::SavedInScene::SAVE_YES,
                                                                      0.0));
    m_preferenceStoredInSceneDataValues.push_back(m_volumeCrossHairGapPreference.get());

    const QString defAllSliceLayout = VolumeSliceViewAllPlanesLayoutEnum::toName(VolumeSliceViewAllPlanesLayoutEnum::ROW_LAYOUT);
    m_volumeAllSlicePlanesLayout.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                    "volumeAllSlicePlanesLayout",
                                                                    CaretPreferenceDataValue::DataType::STRING,
                                                                    CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                    defAllSliceLayout));
    m_preferenceStoredInSceneDataValues.push_back(m_volumeAllSlicePlanesLayout.get());
    
    m_guiGesturesEnabled.reset(new CaretPreferenceDataValue(this->qSettings,
                                                            "guiGesturesEnabled",
                                                            CaretPreferenceDataValue::DataType::BOOLEAN,
                                                            CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                            false));
 
    m_graphicsFramePerSecondEnabled.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                       "graphicsFramePerSecondEnabled",
                                                                       CaretPreferenceDataValue::DataType::BOOLEAN,
                                                                       CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                       false));
    
    m_cropSceneImagesEnabled.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                "sceneImagesEnabled",
                                                                CaretPreferenceDataValue::DataType::BOOLEAN,
                                                                CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                true));
    
    m_toolBarWidthModePreference.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                    "toolBarWidthMode",
                                                                    CaretPreferenceDataValue::DataType::STRING,
                                                                    CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                    ToolBarWidthModeEnum::toName(ToolBarWidthModeEnum::STANDARD)));
    
    m_fileOpenFromOperatingSystemTypePreference.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                                   "openFileFromOperatingSystemType",
                                                                                   CaretPreferenceDataValue::DataType::STRING,
                                                                                   CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                                   FileOpenFromOpSysTypeEnum::toName(FileOpenFromOpSysTypeEnum::ASK_USER)));
    
    m_identificationDisplayModePreference.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                             "identificationDisplayMode",
                                                                             CaretPreferenceDataValue::DataType::STRING,
                                                                             CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                             IdentificationDisplayModeEnum::toName(IdentificationDisplayModeEnum::OVERLAY_TOOLBOX)));
    m_preferenceStoredInSceneDataValues.push_back(m_identificationDisplayModePreference.get());
    
    
    const int32_t maximumFilesDirectories(25);
    m_recentMaximumNumberOfSceneAndSpecFilesPreference.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                              "recentMaximumNumberOfFiles",
                                                                              CaretPreferenceDataValue::DataType::INTEGER,
                                                                              CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                              maximumFilesDirectories));

    m_recentMaximumNumberOfDirectoriesPreferences.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                               "recentMaximumNumberOfDirectories",
                                                                               CaretPreferenceDataValue::DataType::INTEGER,
                                                                               CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                               maximumFilesDirectories));
    
    m_recentFilesSystemAccessMode.reset(new CaretPreferenceDataValue(this->qSettings,
                                                                     "recentFilesSystemAccessMode",
                                                                     CaretPreferenceDataValue::DataType::STRING,
                                                                     CaretPreferenceDataValue::SavedInScene::SAVE_NO,
                                                                     RecentFilesSystemAccessModeEnum::toName(RecentFilesSystemAccessModeEnum::ON)));
    
    m_colorsMode = BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES;
}

/**
 * Destructor.
 */
CaretPreferences::~CaretPreferences()
{
    /**
     * Note DO NOT delete items in this vector as they are pointers to items
     * in unique_ptr's
     */
    m_preferenceStoredInSceneDataValues.clear();
    
    this->removeAllCustomViews();
    
    this->removeAllTileTabsConfigurations();
    
    delete this->qSettings;
}

/**
 * Some preferences are temporarily overriden with a value from a scene
 * and these 'scene overrides' are invalidated by this method
 */
void
CaretPreferences::invalidateSceneDataValues()
{
    for (auto pdv : m_preferenceStoredInSceneDataValues) {
        pdv->setSceneValueValid(false);
    }
}

/**
 * @return The scene data value for items that are saved to
 * and restored from scenes.   Primarily for use by SessionManager.
 */
std::vector<CaretPreferenceDataValue*>
CaretPreferences::getPreferenceSceneDataValues()
{
    return m_preferenceStoredInSceneDataValues;
}

/**
 * Get the boolean value for the given preference name.
 * @param name
 *    Name of the preference.
 * @param defaultValue
 *    Value returned in the preference with the given name was not found.
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
 * @param defaultValue
 *    Value returned in the preference with the given name was not found.
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
 * Get the string value for the given preference name.
 * @param name
 *    Name of the preference.
 * @param defaultValue
 *    Value returned in the preference with the given name was not found.
 * @return
 *    String value of preference or defaultValue if the
 *    named preference is not found.
 */
AString
CaretPreferences::getString(const AString& name,
                            const AString& defaultValue)
{
    AString s = this->qSettings->value(name,
                                       defaultValue).toString();
    return s;
}

/**
 * Set the given preference name with the string value.
 * @param
 *    Name of the preference.
 * @param
 *    New value for preference.
 */
void
CaretPreferences::setString(const AString& name,
                            const AString& value)
{
    this->qSettings->setValue(name,
                              value);
}

/**
 * Remove all custom views.
 */
void
CaretPreferences::removeAllCustomViews()
{
    for (std::vector<ModelTransform*>::iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        delete *iter;
    }
    this->customViews.clear();
}

/**
 * @return Names of custom views sorted by name.  May want to precede this
 * method with a call to 'readCustomViews(true)' so that the custom views
 * are the latest from the settings.
 */
std::vector<AString>
CaretPreferences::getCustomViewNames() const
{
    std::vector<AString> names;
    
    for (std::vector<ModelTransform*>::const_iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        const ModelTransform* mt = *iter;
        names.push_back(mt->getName());
    }
    
    std::sort(names.begin(),
              names.end());
    
    return names;
}

/**
 * @return Names and comments of custom views sorted by name.  May want to precede this
 * method with a call to 'readCustomViews(true)' so that the custom views
 * are the latest from the settings.
 */
std::vector<std::pair<AString,AString> >
CaretPreferences::getCustomViewNamesAndComments() const
{
    std::vector<AString> customViewNames = getCustomViewNames();
    
    std::vector<std::pair<AString,AString> > namesAndComments;
    
    for (std::vector<AString>::const_iterator iter = customViewNames.begin();
         iter != customViewNames.end();
         iter++) {
        const AString name = *iter;
        ModelTransform modelTransform;
        if (getCustomView(name, modelTransform)) {
            const AString comment = modelTransform.getComment();
            namesAndComments.push_back(std::make_pair(name,
                                                      comment));
        }
    }
    
    return namesAndComments;
}


/**
 * Get a custom view with the given name.
 *
 * @param customViewName
 *     Name of requested custom view.
 * @param modelTransformOut
 *     Custom view will be loaded into this model transform.
 * @return true if a custom view with the name exists.  If no
 *     custom view exists with the name, false is returned and
 *     the model transform will be the identity transform.
 */
bool
CaretPreferences::getCustomView(const AString& customViewName,
                   ModelTransform& modelTransformOut) const
{
    for (std::vector<ModelTransform*>::const_iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        const ModelTransform* mt = *iter;
        
        if (customViewName == mt->getName()) {
            modelTransformOut = *mt;
            return true;
        }
    }
    
    modelTransformOut.setToIdentity();
    
    return false;
}

/**
 * Add or update a custom view.  If a custom view exists with the name
 * in the given model transform it is replaced.
 *
 * @param modelTransform
 *     Custom view's model transform.
 */
void
CaretPreferences::addOrReplaceCustomView(const ModelTransform& modelTransform)
{
    bool addNewCustomView = true;
    
    for (std::vector<ModelTransform*>::iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        ModelTransform* mt = *iter;
        if (mt->getName() == modelTransform.getName()) {
            *mt = modelTransform;
            addNewCustomView = false;
            break;
        }
    }
    
    if (addNewCustomView) {
        this->customViews.push_back(new ModelTransform(modelTransform));
    }
    this->writeCustomViews();
}

/**
 * Remove the custom view with the given name.
 *
 * @param customViewName
 *     Name of custom view.
 */
void
CaretPreferences::removeCustomView(const AString& customViewName)
{
    for (std::vector<ModelTransform*>::iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        ModelTransform* mt = *iter;
        if (mt->getName() == customViewName) {
            this->customViews.erase(iter);
            delete mt;
            break;
        }
    }
    
    this->writeCustomViews();
}

/**
 * Write the custom views.
 */
void 
CaretPreferences::writeCustomViews()
{
    /*
     * Remove "userViews" that were replaced by customView
     */
    this->qSettings->remove("userViews");
    
    this->qSettings->beginWriteArray(NAME_CUSTOM_VIEWS);
    const int32_t numViews = static_cast<int32_t>(this->customViews.size());
    for (int32_t i = 0; i < numViews; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->customViews[i]->getAsString());
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Read macros from preferences
 *
 * @param performSync
 *     If true, synchronize preferences before reading macros
 */
void
CaretPreferences::readMacros(const bool performSync)
{
    if (performSync) {
        this->qSettings->sync();
    }
 
    const QString macrosXmlString = this->getString(NAME_MACROS);
    if (macrosXmlString.isEmpty()) {
        m_macros->clear();
    }
    else {
        QString errorMessage;
        QString warningMessage;
        if ( ! m_macros->readXmlFromStringOld(macrosXmlString,
                                           errorMessage,
                                           warningMessage)) {
            CaretLogSevere("Reading macros from preferences: "
                            + errorMessage);
        }
        else if ( ! warningMessage.isEmpty()) {
            CaretLogWarning(warningMessage);
        }
    }
}

/**
 * Write macros to preferences
 */
void
CaretPreferences::writeMacros()
{
    if ( ! m_macros->isModified()) {
        return;
    }
    
    QString macrosXmlString;
    
    if (m_macros->getNumberOfMacros() > 0) {
        QString errorMessage;
        if ( ! m_macros->writeXmlToString(macrosXmlString,
                                          errorMessage)) {
            CaretLogSevere("Writing macros to preferences: "
                            + errorMessage);
        }
    }
    
    this->setString(NAME_MACROS,
                    macrosXmlString);
    this->qSettings->sync();
}


/**
 * Remove all of the tile tabs configurations.
 */
void
CaretPreferences::removeAllTileTabsConfigurations()
{
    for (auto ttc : this->tileTabsConfigurations) {
        delete ttc;
    }
    this->tileTabsConfigurations.clear();
}

/**
 * Write the tile tabs configurations.
 */
void
CaretPreferences::writeTileTabsUserConfigurations()
{
    /*
     * NOTE: The GRID and MANUAL configurations are written separately.
     * Older versions of wb_view will correctly read the GRID configurations
     * and ignore the MANUAL configurations (not log error messages).
     * Newer versions of wb_view will read both.
     */
    
    /*
     * Write only GRID configurations to "NAME_TILE_TABS_CONFIGURATIONS",
     * This allows previous versions of the software to read the configurations
     */
    this->qSettings->beginWriteArray(NAME_TILE_TABS_CONFIGURATIONS);
    const int32_t numViews = static_cast<int32_t>(this->tileTabsConfigurations.size());
    int32_t gridConfigCounter(0);
    for (int32_t i = 0; i < numViews; i++) {
        CaretAssertVectorIndex(this->tileTabsConfigurations, i);
        const TileTabsLayoutBaseConfiguration* config = this->tileTabsConfigurations[i];
        switch (config->getLayoutType()) {
            case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                break;
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                this->qSettings->setArrayIndex(gridConfigCounter);
                this->qSettings->setValue(AString::number(gridConfigCounter),
                                          config->encodeInXML());
                gridConfigCounter++;
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                break;
        }
    }
    this->qSettings->endArray();

    /*
     * Write only MANUAL configurations to "NAME_TILE_TABS_CONFIGURATIONS_TWO",
     * Older versions of the software will not try to read these configurations
     */
    this->qSettings->beginWriteArray(NAME_TILE_TABS_CONFIGURATIONS_TWO);
    int32_t manualConfigCounter(0);
    for (int32_t i = 0; i < numViews; i++) {
        CaretAssertVectorIndex(this->tileTabsConfigurations, i);
        const TileTabsLayoutBaseConfiguration* config = this->tileTabsConfigurations[i];
        switch (config->getLayoutType()) {
            case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                this->qSettings->setArrayIndex(manualConfigCounter);
                this->qSettings->setValue(AString::number(manualConfigCounter),
                                          config->encodeInXML());
                manualConfigCounter++;
                break;
        }
    }
    this->qSettings->endArray();
    
    this->qSettings->sync();
}

/**
 * Read the tile tabs configuration.  Since user's may want to use them
 * in multiple instance of workbench that are running, this method allows
 * the tile tab configurations to be read without affecting other preferences.
 *
 * @param performSync
 *    Sync with preferences since preferences may have been changed
 *    by a concurrently running workbench.
 */
void
CaretPreferences::readTileTabsUserConfigurations(const bool performSync)
{
    if (performSync) {
        this->qSettings->sync();
    }
    
    this->removeAllTileTabsConfigurations();
    
    /*
     * NOTE: The GRID and MANUAL configurations are written separately.
     * Older versions of wb_view will correctly read the GRID configurations
     * and ignore the MANUAL configurations (not log error messages).
     * Newer versions of wb_view will read both.
     */
    std::vector<AString> configurationStrings;
    
    /*
     * Read GRID Configurations (Array Name: NAME_TILE_TABS_CONFIGURATIONS)
     */
    const int numGridConfigs = this->qSettings->beginReadArray(NAME_TILE_TABS_CONFIGURATIONS);
    for (int32_t i = 0; i < numGridConfigs; i++) {
        this->qSettings->setArrayIndex(i);
        const AString str = this->qSettings->value(AString::number(i)).toString();
        configurationStrings.push_back(str);
    }
    this->qSettings->endArray();
    
    /*
     * Read MANUAL Configurations (Array Name: NAME_TILE_TABS_CONFIGURATIONS_TWO)
     */
    const int numManualConfigs = this->qSettings->beginReadArray(NAME_TILE_TABS_CONFIGURATIONS_TWO);
    for (int32_t i = 0; i < numManualConfigs; i++) {
        this->qSettings->setArrayIndex(i);
        const AString str = this->qSettings->value(AString::number(i)).toString();
        configurationStrings.push_back(str);
    }
    this->qSettings->endArray();

    /*
     * Read from the configuration strings to create layouts
     */
    const int numConfigurations = static_cast<int32_t>(configurationStrings.size());
    for (int i = 0; i < numConfigurations; i++) {
        CaretAssertVectorIndex(configurationStrings, i);
        const AString configString = configurationStrings[i];
        AString errorMessage;
        TileTabsLayoutBaseConfiguration* ttc = TileTabsLayoutBaseConfiguration::decodeFromXML(configString,
                                                                                              errorMessage);
        if (ttc != NULL) {
            this->tileTabsConfigurations.push_back(ttc);
        }
        else {
            CaretLogWarning(errorMessage);
        }
        
        const bool testFlag(false);
        if (testFlag) {
            AString errorMessage;
            TileTabsLayoutBaseConfiguration* config = TileTabsLayoutBaseConfiguration::decodeFromXML(configString,
                                                                                         errorMessage);
            if (config != NULL) {
                std::cout << "Read config: " << config->toString() << std::endl;
                
                TileTabsLayoutGridConfiguration* gridConfig = dynamic_cast<TileTabsLayoutGridConfiguration*>(config);
                if (gridConfig != NULL) {
                    const int32_t rowCount = gridConfig->getNumberOfRows();
                    const int32_t colCount = gridConfig->getNumberOfColumns();
                    const int32_t numTabs = rowCount * colCount;
                    std::vector<int32_t> tabIndices;
                    for (int32_t i = 0; i < numTabs; i++) {
                        tabIndices.push_back(i);
                    }
                    TileTabsLayoutManualConfiguration* manualLayout
                    = TileTabsLayoutManualConfiguration::newInstanceFromGridLayout(gridConfig,
                                                                                   TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID,
                                                                                   tabIndices);
                    if (manualLayout != NULL) {
                        std::cout << "MANUAL VERSION OF GRID LAYOUT: " << std::endl;
                        std::cout << manualLayout->toString() << std::endl;
                        
                        AString xmlText = manualLayout->encodeInXML();
                        std::cout << "XML: " << std::endl;
                        std::cout << xmlText << std::endl << std::endl;
                        
                        delete manualLayout;
                        
                        TileTabsLayoutBaseConfiguration* configMan =
                            TileTabsLayoutBaseConfiguration::decodeFromXML(xmlText,
                                                                     errorMessage);
                        if (configMan != NULL) {
                            std::cout << "After reading manual configuration and writing it: " << std::endl;
                            std::cout << configMan->toString() << std::endl << std::endl;
                        }
                        else {
                            std::cout << "ERROR decoding from manual layout: "
                            << errorMessage << std::endl;
                        }
                    }
                }
            }
            else {
                std::cout << "Error reading config: " << errorMessage << std::endl;
                std::cout << "FROM: " << configString << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

/**
 * @return A vector containing pairs of Name and Unique Identifier for each
 * Tile Tabs configuration.  The items are sorted by name with 'first' being
 * the name, and 'second' being the unique identifier.
 *
 * @param includeManualConfigurationsFlag
 *     If true, include manual configurations; if false exclude manual configurations
 */
std::vector<std::pair<AString, AString>>
CaretPreferences::getTileTabsUserConfigurationsNamesAndUniqueIdentifiers(const bool includeManualConfigurationsFlag) const
{
    std::vector<std::pair<AString, AString>> nameIDs;
    
    for (const auto ttc : this->tileTabsConfigurations) {
        if (ttc->getLayoutType() == TileTabsLayoutConfigurationTypeEnum::MANUAL) {
            if ( ! includeManualConfigurationsFlag) {
                continue;
            }
        }
        
        QString typeString;
        switch (ttc->getLayoutType()) {
            case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                typeString = " (AG)";
                break;
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                typeString = (" (G,"
                              + AString::number(ttc->getNumberOfTabs())
                              + ")");
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                typeString = (" (M,"
                              + AString::number(ttc->getNumberOfTabs())
                              + ")");
                break;
        }
        nameIDs.push_back(std::make_pair(ttc->getName() + typeString,
                                         ttc->getUniqueIdentifier()));
    }
    
    std::sort(nameIDs.begin(),
              nameIDs.end(),
              [](const std::pair<AString, AString>& a, const std::pair<AString, AString>& b) { return a.first < b.first; });
    
    return nameIDs;
}

/**
 * @return A copy of the tile tabs configuration with the given unique identifier.  A copy is returned
 * since preferenes may get updated which causes reloading of the user configurations and if the caller
 * had a pointer to a user configuration, that pointer would be invalid (point to destroyed configuration)
 * when preferences are updated (synced).  Pointer will be NULL if there is no user configuration with
 * the given unique identifier.
 *
 * @param uniqueIdentifier
 *     Unique identifier of the requested tile tabs configuration.
 */
std::unique_ptr<TileTabsLayoutBaseConfiguration>
CaretPreferences::getCopyOfTileTabsUserConfigurationByUniqueIdentifier(const AString& uniqueIdentifier) const
{
    std::unique_ptr<TileTabsLayoutBaseConfiguration> pointer;
    
    for (std::vector<TileTabsLayoutBaseConfiguration*>::const_iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        const TileTabsLayoutBaseConfiguration* ttc = *iter;
        if (ttc->getUniqueIdentifier() == uniqueIdentifier) {
            pointer.reset(ttc->newCopyWithNewUniqueIdentifier());
        }
    }

    return pointer;
}

/**
 * Get the tile tabs configuration with the given name.
 *
 * @param name
 *     Name of the requested tile tabs configuration.
 * @return
 *     Pointer to tile tabs configuration with the given name
 *     or NULL is it does not exist.
 */
TileTabsLayoutBaseConfiguration*
CaretPreferences::getTileTabsUserConfigurationByName(const AString& name) const
{
    for (std::vector<TileTabsLayoutBaseConfiguration*>::const_iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        TileTabsLayoutBaseConfiguration* ttc = *iter;
        
        if (name == ttc->getName()) {
            return ttc;
        }
    }
    
    return NULL;
}

/**
 * Add a new tile tabs user configuration.
 * 
 * @param tileTabsConfiguration
 *    New tile tabs configuration that is copied and added.
 * @param configurationName
 *    New name for copied configuration (ignored if empty)
 */
void
CaretPreferences::addTileTabsUserConfiguration(const TileTabsLayoutBaseConfiguration* tileTabsConfiguration,
                                               const AString& configurationName)
{
    TileTabsLayoutBaseConfiguration* configCopy = tileTabsConfiguration->newCopyWithNewUniqueIdentifier();
    if ( ! configurationName.isEmpty()) {
        configCopy->setName(configurationName);
    }
    this->tileTabsConfigurations.push_back(configCopy);
    this->writeTileTabsUserConfigurations();
}

/**
 * Replace a tile tabs configuration with another tile tabs configuration
 *
 * @param replaceUserTileTabsUniqueIdentifier
 *     Unique identifier of configuration that is to be replaced
 *     This user configuration will be replaced and this pointer will be INVALID after
 *     this method is called.
 * @param replaceWithConfiguration
 *     This configuration is copied and replaces the other configuration.
 * @param errorMessageOut
 *      Contains error information if fails to replace configuration
 * @return
 *      True if successful, else false
 */
bool
CaretPreferences::replaceTileTabsUserConfiguration(const AString& replaceUserTileTabsUniqueIdentifier,
                                                   const TileTabsLayoutBaseConfiguration* replaceWithConfiguration,
                                                   AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    CaretAssert(replaceWithConfiguration);
    
    int32_t replaceIndex = -1;
    const int32_t numConfigs = static_cast<int32_t>(this->tileTabsConfigurations.size());
    for (int32_t i = 0; i < numConfigs; i++) {
        CaretAssertVectorIndex(this->tileTabsConfigurations, i);
        if (this->tileTabsConfigurations[i]->getUniqueIdentifier() == replaceUserTileTabsUniqueIdentifier) {
            replaceIndex = i;
            break;
        }
    }
    
    if (replaceIndex < 0) {
        errorMessageOut = "Failed to find configuration for replacement.";
        return false;
    }
    
    /*
     * Delete configuration since it may be a different subclass than other configuration
     */
    CaretAssertVectorIndex(this->tileTabsConfigurations, replaceIndex);
    const AString name = this->tileTabsConfigurations[replaceIndex]->getName();
    const AString uuid = this->tileTabsConfigurations[replaceIndex]->getUniqueIdentifier();
    delete this->tileTabsConfigurations[replaceIndex];
    
    TileTabsLayoutBaseConfiguration* newConfig = replaceWithConfiguration->newCopyWithUniqueIdentifier(uuid);
    CaretAssert(newConfig);
    newConfig->setName(name);
    this->tileTabsConfigurations[replaceIndex] = newConfig;
    
    this->writeTileTabsUserConfigurations();

    return true;
}

/**
 * Rename the tile tabs configuration with the given name.
 *
 * @param tileTabsUniqueIdentifier
 *     Unique identifier of configuration that will be renamed.
 * @param name
 *     New name for configuration
 * @param errorMessageOut
 *     Will contain error information if command fails
 * @return
 *     True if configuration was removed, else false.
 */
bool
CaretPreferences::renameTileTabsUserConfiguration(const QString& tileTabsUniqueIdentifier,
                                                  const AString& name,
                                                  AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (tileTabsUniqueIdentifier.isEmpty()) {
        errorMessageOut = "Tile Tabs Unique Identifier is empty.";
        return false;
    }
    
    for (std::vector<TileTabsLayoutBaseConfiguration*>::iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        TileTabsLayoutBaseConfiguration* ttc = *iter;
        if (ttc->getUniqueIdentifier() == tileTabsUniqueIdentifier) {
            ttc->setName(name);
            this->writeTileTabsUserConfigurations();
            return true;
        }
    }
    
    errorMessageOut = ("Did not find a Tile Tabs Configuration with UniqueID="
                       + tileTabsUniqueIdentifier);
    return false;
}

/**
 * Remove the tile tabs configuration with the given unique identifier.
 *
 * @param tileTabsUniqueIdentifier
 *     Unique identifier of configuration that will be removed.
 * @param errorMessageOut
 *     Will contain error information if command fails
 * @return
 *     True if configuration was removed, else false.
 */
bool
CaretPreferences::removeTileTabsUserConfigurationByUniqueIdentifier(const AString& tileTabsUniqueIdentifier,
                                                                    AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (tileTabsUniqueIdentifier.isEmpty()) {
        errorMessageOut = "Tile Tabs Unique Identifier is empty.";
        return false;
    }
    
    for (std::vector<TileTabsLayoutBaseConfiguration*>::iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        TileTabsLayoutBaseConfiguration* ttc = *iter;
        if (ttc->getUniqueIdentifier() == tileTabsUniqueIdentifier) {
            this->tileTabsConfigurations.erase(iter);
            delete ttc;
            this->writeTileTabsUserConfigurations();
            return true;
        }
    }
    
    errorMessageOut = ("Did not find a Tile Tabs Configuration with UniqueID="
                       + tileTabsUniqueIdentifier);
    return false;
}


/**
 * @return Pointer to the background and foreground colors for
 * use when drawing graphics.  The colors returned may be
 * either the user's preferences or the from the currently
 * loaded scene.
 */
const BackgroundAndForegroundColors*
CaretPreferences::getBackgroundAndForegroundColors() const
{
    switch (m_colorsMode) {
        case BackgroundAndForegroundColorsModeEnum::SCENE:
            return &this->sceneColors;
            break;
        case BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES:
            return &this->userColors;
            break;
    }
    
    CaretAssert(0);
    return &this->userColors;
}

/**
 * @return The USER'S preferred background and foreground colors.
 *    This method is used only by the PreferencesDialog to
 *    update the user's preferred colors.
 */
BackgroundAndForegroundColors
CaretPreferences::getUserBackgroundAndForegroundColors()
{
    return this->userColors;
}

/**
 * Set the USER'S preferred background and foreground colors.
 *    This method is used only by the PreferencesDialog to
 *    update the user's preferred colors.
 */
void
CaretPreferences::setUserBackgroundAndForegroundColors(const BackgroundAndForegroundColors& colors)
{
    if (this->userColors == colors) {
        return;
    }
    
    /*
     * "in memory" colors
     */
    this->userColors = colors;
    
    /*
     * Update preferences file with colors
     */
    writeUnsignedByteArray(NAME_COLOR_BACKGROUND_WINDOW,
                           this->userColors.m_colorBackgroundWindow,
                           3);
    
    writeUnsignedByteArray(NAME_COLOR_FOREGROUND_ALL,
                           this->userColors.m_colorForegroundAll,
                           3);
    writeUnsignedByteArray(NAME_COLOR_BACKGROUND_ALL,
                           this->userColors.m_colorBackgroundAll,
                           3);
    
    writeUnsignedByteArray(NAME_COLOR_FOREGROUND_CHART,
                           this->userColors.m_colorForegroundChart,
                           3);
    writeUnsignedByteArray(NAME_COLOR_BACKGROUND_CHART,
                           this->userColors.m_colorBackgroundChart,
                           3);
    
    writeUnsignedByteArray(NAME_COLOR_FOREGROUND_MEDIA,
                           this->userColors.m_colorForegroundMedia,
                           3);
    writeUnsignedByteArray(NAME_COLOR_BACKGROUND_MEDIA,
                           this->userColors.m_colorBackgroundMedia,
                           3);
    
    writeUnsignedByteArray(NAME_COLOR_FOREGROUND_SURFACE,
                           this->userColors.m_colorForegroundSurface,
                           3);
    writeUnsignedByteArray(NAME_COLOR_BACKGROUND_SURFACE,
                           this->userColors.m_colorBackgroundSurface,
                           3);
    
    writeUnsignedByteArray(NAME_COLOR_FOREGROUND_VOLUME,
                           this->userColors.m_colorForegroundVolume,
                           3);
    writeUnsignedByteArray(NAME_COLOR_BACKGROUND_VOLUME,
                           this->userColors.m_colorBackgroundVolume,
                           3);
    
    writeUnsignedByteArray(NAME_COLOR_CHART_MATRIX_GRID_LINES,
                           this->userColors.m_colorChartMatrixGridLines,
                           3);
    
    writeUnsignedByteArray(NAME_COLOR_CHART_HISTOGRAM_THRESHOLD,
                           this->userColors.m_colorChartHistogramThreshold,
                           3);
}

/**
 * Set the SCENE background and foreground colors.
 *    This method is called when scenes are restored.
 */
void
CaretPreferences::setSceneBackgroundAndForegroundColors(const BackgroundAndForegroundColors& colors)
{
    this->sceneColors = colors;
}

/**
 * @return Mode for background and foreground colors.
 */
BackgroundAndForegroundColorsModeEnum::Enum
CaretPreferences::getBackgroundAndForegroundColorsMode() const
{
    return m_colorsMode;
}

/**
 * Set the mode for background and foreground colors.
 * NOTE: This is a transient value and NOT saved to preferences.
 *
 * @param colorsMode
 *      New colors mode.
 */
void
CaretPreferences::setBackgroundAndForegroundColorsMode(const BackgroundAndForegroundColorsModeEnum::Enum colorsMode)
{
    m_colorsMode = colorsMode;
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
    if (this->loggingLevel == loggingLevel) {
        return;
    }
    
    this->loggingLevel = loggingLevel;
    
    const AString name = LogLevelEnum::toName(this->loggingLevel);
    this->qSettings->setValue(NAME_LOGGING_LEVEL, name);
    this->qSettings->sync();
    CaretLogger::getLogger()->setLevel(loggingLevel);
}

/**
 * @return The OpenGL Drawing method.
 */
OpenGLDrawingMethodEnum::Enum
CaretPreferences::getOpenDrawingMethod() const
{
    OpenGLDrawingMethodEnum::Enum drawMethod = this->openGLDrawingMethod;

    /*
     * Disable vertex buffers for now
     */
    drawMethod = OpenGLDrawingMethodEnum::DRAW_WITH_VERTEX_BUFFERS_OFF;
    
    return drawMethod;
}

/**
 * Set the OpenGL Drawing method.
 *
 * @param openGLDrawingMethod
 *    New value for OpenGL Drawing method.
 */
void
CaretPreferences::setOpenGLDrawingMethod(const OpenGLDrawingMethodEnum::Enum openGLDrawingMethod)
{
    if (this->openGLDrawingMethod == openGLDrawingMethod) {
        return;
    }
    
    this->openGLDrawingMethod = openGLDrawingMethod;
    this->setString(NAME_OPENGL_DRAWING_METHOD,
                    OpenGLDrawingMethodEnum::toName(this->openGLDrawingMethod));
}

/**
 * @return The view file type for manage files dialog.
 */
SpecFileDialogViewFilesTypeEnum::Enum
CaretPreferences::getManageFilesViewFileType() const
{
    return this->manageFilesViewFileType;
}

/**
 * Set the view file type for manage files dialog.
 *
 * @param manageFilesViewFileType
 *     New view file type.
 */
void
CaretPreferences::setManageFilesViewFileType(const SpecFileDialogViewFilesTypeEnum::Enum manageFilesViewFileType)
{
    if (this->manageFilesViewFileType == manageFilesViewFileType) {
        return;
    }
    
    this->manageFilesViewFileType = manageFilesViewFileType;
    this->setString(NAME_MANAGE_FILES_VIEW_FILE_TYPE,
                    SpecFileDialogViewFilesTypeEnum::toName(this->manageFilesViewFileType));
}

/**
 * @return Show surface identification symbols?
 */
bool
CaretPreferences::isShowSurfaceIdentificationSymbols() const
{
    return this->showSurfaceIdentificationSymbols;
}

/**
 * Set show surface identification symbols.
 *  
 * @param showSymbols
 *     New status.
 */
void
CaretPreferences::setShowSurfaceIdentificationSymbols(const bool showSymbols)
{
    if (this->showSurfaceIdentificationSymbols == showSymbols) {
        return;
    }
    
    this->showSurfaceIdentificationSymbols = showSymbols;
    this->setBoolean(NAME_SHOW_SURFACE_IDENTIFICATION_SYMBOLS,
                     this->showSurfaceIdentificationSymbols);
}

/**
 * @return Show volume identification symbols?
 */
bool
CaretPreferences::isShowVolumeIdentificationSymbols() const
{
    return this->showVolumeIdentificationSymbols;
}

/**
 * Set show volume identification symbols.
 *
 * @param showSymbols
 *     New status.
 */
void
CaretPreferences::setShowVolumeIdentificationSymbols(const bool showSymbols)
{
    if (this->showVolumeIdentificationSymbols == showSymbols) {
        return;
    }
    
    this->showVolumeIdentificationSymbols = showSymbols;
    this->setBoolean(NAME_SHOW_VOLUME_IDENTIFICATION_SYMBOLS,
                     this->showVolumeIdentificationSymbols);
}

/**
 * @return Is dynamic connectivity defaulted on?
 */
bool
CaretPreferences::isDynamicConnectivityDefaultedOn() const
{
    return this->dynamicConnectivityDefaultedOn;
}

/**
 * Set dynamic connectivity defaulted on.
 *
 * @param defaultedOn
 *     New status.
 */
void
CaretPreferences::setDynamicConnectivityDefaultedOn(const bool defaultedOn)
{
    if (this->dynamicConnectivityDefaultedOn == defaultedOn) {
        return;
    }
    
    this->dynamicConnectivityDefaultedOn = defaultedOn;
    this->setBoolean(NAME_DYNAMIC_CONNECTIVITY_ON,
                     defaultedOn);
}


/**
 * @return The image capture method.
 */
ImageCaptureMethodEnum::Enum
CaretPreferences::getImageCaptureMethod() const
{
    return this->imageCaptureMethod;
}

/**
 * Set the image capture method.
 *
 * @param imageCaptureMethod
 *     New value for image capture method.
 */
void
CaretPreferences::setImageCaptureMethod(const ImageCaptureMethodEnum::Enum imageCaptureMethod)
{
    if (this->imageCaptureMethod == imageCaptureMethod) {
        return;
    }
    
    this->imageCaptureMethod = imageCaptureMethod;
    this->setString(NAME_IMAGE_CAPTURE_METHOD,
                    ImageCaptureMethodEnum::toName(this->imageCaptureMethod));
}


/**
 * @return Save remote login to preferences.
 */
bool
CaretPreferences::isRemoteFilePasswordSaved()
{
    return this->remoteFileLoginSaved;
}

/**
 * Set saving of remote login and password to preferences.
 *
 * @param saveRemoteLoginToPreferences
 *    New status.
 */
void
CaretPreferences::setRemoteFilePasswordSaved(const bool saveRemotePasswordToPreferences)
{
    if (this->remoteFileLoginSaved == saveRemotePasswordToPreferences) {
        return;
    }
    
    this->remoteFileLoginSaved = saveRemotePasswordToPreferences;
    this->setBoolean(NAME_REMOTE_FILE_LOGIN_SAVED,
                     this->remoteFileLoginSaved);
    this->qSettings->sync();
}


/**
 * Get the remote file username and password
 *
 * @param userNameOut
 *    Contains username upon exit
 * @param passwordOut
 *    Contains password upon exit.
 */
void
CaretPreferences::getRemoteFileUserNameAndPassword(AString& userNameOut,
                                                   AString& passwordOut) const
{
    userNameOut = this->remoteFileUserName;
    passwordOut = this->remoteFilePassword;
}

/**
 * Set the remote file username and password
 *
 * @param userName
 *    New value for username.
 * @param passwordOut
 *    New value for password.
 */
void
CaretPreferences::setRemoteFileUserNameAndPassword(const AString& userName,
                                                   const AString& password)
{
    if ((this->remoteFileUserName == userName)
        && (this->remoteFilePassword == password)) {
        return;
    }
    
    this->remoteFileUserName = userName;
    this->remoteFilePassword = password;
    
    this->setString(NAME_REMOTE_FILE_USER_NAME,
                    userName);
    this->setString(NAME_REMOTE_FILE_PASSWORD,
                    password);
    this->qSettings->sync();
}

/**
 * @return The BALSA username
 */
AString
CaretPreferences::getBalsaUserName() const
{
    return this->balsaUserName;
}

/**
 * Set the BALSA username
 *
 * @param userName
 *     New value for BALSA username.
 */
void
CaretPreferences::setBalsaUserName(const AString& userName)
{
    if (this->balsaUserName == userName) {
        return;
    }
    
    this->balsaUserName = userName;
    this->setString(NAME_BALSA_USER_NAME,
                    userName);
}

/**
 * @return The toolbar's width mode
 */
ToolBarWidthModeEnum::Enum
CaretPreferences::getToolBarWidthMode() const
{
    QString stringValue(m_toolBarWidthModePreference->getValue().toString());
    bool validFlag(false);
    const ToolBarWidthModeEnum::Enum enumValue =
    ToolBarWidthModeEnum::fromName(stringValue, &validFlag);
    return enumValue;
}

/**
 * Set the toolbar's width mode
 * @param widthMode
 * The new width mode
 */
void
CaretPreferences::setToolBarWidthMode(const ToolBarWidthModeEnum::Enum widthMode)
{
    const QString stringValue = ToolBarWidthModeEnum::toName(widthMode);
    m_toolBarWidthModePreference->setValue(stringValue);
}

/**
 * @return File open from operating system type
 */
FileOpenFromOpSysTypeEnum::Enum
CaretPreferences::getFileOpenFromOpSysType() const
{
    QString stringValue(m_fileOpenFromOperatingSystemTypePreference->getValue().toString());
    bool validFlag(false);
    const FileOpenFromOpSysTypeEnum::Enum enumValue = FileOpenFromOpSysTypeEnum::fromName(stringValue,
                                                                                          &validFlag);
    return enumValue;
}

/**
 * Set the open file from operating system type
 * @param openType
 * The new open type
 */
void
CaretPreferences::setFileOpenFromOpSysType(const FileOpenFromOpSysTypeEnum::Enum openType)
{
    const QString stringValue = FileOpenFromOpSysTypeEnum::toName(openType);
    m_fileOpenFromOperatingSystemTypePreference->setValue(stringValue);
}

/**
 * @return The identification display mode
 */
IdentificationDisplayModeEnum::Enum
CaretPreferences::getIdentificationDisplayMode() const
{
    QString stringValue(m_identificationDisplayModePreference->getValue().toString());
    bool validFlag(false);
    const IdentificationDisplayModeEnum::Enum enumValue =
       IdentificationDisplayModeEnum::fromName(stringValue, &validFlag);
    return enumValue;
}

/**
 * Set the identification display mode
 *  @param identificationDisplayMode
 *  New identification display mode
 */
void
CaretPreferences::setIdentificationDisplayMode(const IdentificationDisplayModeEnum::Enum identificationDisplayMode)
{
    const QString stringValue = IdentificationDisplayModeEnum::toName(identificationDisplayMode);
    m_identificationDisplayModePreference->setValue(stringValue);
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
    if (this->displayVolumeAxesCrosshairs == displayed) {
        return;
    }

    this->displayVolumeAxesCrosshairs = displayed;
    this->setBoolean(CaretPreferences::NAME_VOLUME_AXES_CROSSHAIRS,
                     this->displayVolumeAxesCrosshairs);
    this->qSettings->sync();
}

/**
 * @return The volume all slice planes layout
 */
VolumeSliceViewAllPlanesLayoutEnum::Enum
CaretPreferences::getVolumeAllSlicePlanesLayout() const
{
    QString stringValue(m_volumeAllSlicePlanesLayout->getValue().toString());
    bool validFlag(false);
    VolumeSliceViewAllPlanesLayoutEnum::Enum enumValue =
       VolumeSliceViewAllPlanesLayoutEnum::fromName(stringValue, &validFlag);
    return enumValue;
}

/**
 * Set volume all slice planes layout
 *
 * @param allViewLayout
 *     The all slice planes layout
 */
void
CaretPreferences::setVolumeAllSlicePlanesLayout(const VolumeSliceViewAllPlanesLayoutEnum::Enum allViewLayout)
{
    const QString stringValue = VolumeSliceViewAllPlanesLayoutEnum::toName(allViewLayout);
    m_volumeAllSlicePlanesLayout->setValue(stringValue);
}

/**
 * @retrurn Gestures enabled in GUI
 */
bool
CaretPreferences::isGuiGesturesEnabled() const
{
    return m_guiGesturesEnabled->getValue().toBool();
}

/**
 * Set Gestures enabled in GUI
 *
 *  @param status
 *   New enabled status
 */
void
CaretPreferences::setGuiGesturesEnabled(const bool status)
{
    m_guiGesturesEnabled->setValue(status);
}

/**
 * @retrurn Graphics frames per second enabled
 */
bool
CaretPreferences::isGraphicsFramesPerSecondEnabled() const
{
    return m_graphicsFramePerSecondEnabled->getValue().toBool();
}

/**
 * Set graphics frames per second enabled
 *
 *  @param status
 *   New enabled status
 */
void
CaretPreferences::setGraphicsFramesPerSecondEnabled(const bool status)
{
    m_graphicsFramePerSecondEnabled->setValue(status);
}

/**
 * @retrurn Crop scene images enabled
 */
bool
CaretPreferences::isCropSceneImagesEnabled() const
{
    return m_cropSceneImagesEnabled->getValue().toBool();
}

/**
 * Set crop scene images enabled
 *
 *  @param status
 *   New enabled status
 */
void
CaretPreferences::setCropSceneImagesEnabled(const bool status)
{
    m_cropSceneImagesEnabled->setValue(status);
}

/**
 * @return The crosshair gap
 */
float
CaretPreferences::getVolumeCrosshairGap() const
{
    return m_volumeCrossHairGapPreference->getValue().toFloat();
}

/**
 * Set the volume crosshair gap
 *
 * @param gap
 *     New value for crosshair gap.
 */
void
CaretPreferences::setVolumeCrosshairGap(const float gap)
{
    return m_volumeCrossHairGapPreference->setValue(gap);
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
    if (this->displayVolumeAxesLabels == displayed) {
        return;
    }
    
    this->displayVolumeAxesLabels = displayed;
    this->setBoolean(CaretPreferences::NAME_VOLUME_AXES_LABELS, 
                     this->displayVolumeAxesLabels);
    this->qSettings->sync();
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
    if (this->displayVolumeAxesCoordinates == displayed) {
        return;
    }
    
    this->displayVolumeAxesCoordinates = displayed;
    this->setBoolean(CaretPreferences::NAME_VOLUME_AXES_COORDINATE,
                     this->displayVolumeAxesCoordinates);
    this->qSettings->sync();
}

/**
 * @return The volume montage gap.
 */
int32_t
CaretPreferences::getVolumeMontageGap() const
{
    return this->volumeMontageGap;
}

/**
 * Set the volume montage gap.
 *
 * @param volumeMontageGap
 *     New value for montage gap.
 */
void
CaretPreferences::setVolumeMontageGap(const int32_t volumeMontageGap)
{
    if (this->volumeMontageGap == volumeMontageGap) {
        return;
    }
    
    this->volumeMontageGap = volumeMontageGap;
    this->setInteger(CaretPreferences::NAME_VOLUME_MONTAGE_GAP,
                     this->volumeMontageGap);
    this->qSettings->sync();
}

/**
 * @return The volume montage coordinate precision
 */
int32_t
CaretPreferences::getVolumeMontageCoordinatePrecision() const
{
    return this->volumeMontageCoordinatePrecision;
}

/**
 * Set the volume montage coordinate precision
 *
 * @param volumeMontageCoordinatePrecision
 *     New value for montage coordinate precision
 */
void
CaretPreferences::setVolumeMontageCoordinatePrecision(const int32_t volumeMontageCoordinatePrecision)
{
    if (this->volumeMontageCoordinatePrecision == volumeMontageCoordinatePrecision) {
        return;
    }
    
    this->volumeMontageCoordinatePrecision = volumeMontageCoordinatePrecision;
    this->setInteger(CaretPreferences::NAME_VOLUME_MONTAGE_COORDINATE_PRECISION,
                     this->volumeMontageCoordinatePrecision);
    this->qSettings->sync();
}

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
    if (this->splashScreenEnabled == enabled) {
        return;
    }
    
    this->splashScreenEnabled = enabled;
    this->setBoolean(CaretPreferences::NAME_SPLASH_SCREEN,
                     this->splashScreenEnabled);
    this->qSettings->sync();
}

/**
 * @return Is the Develop Menu enabled?
 */
bool
CaretPreferences::isDevelopMenuEnabled() const
{
    return this->developMenuEnabled;
}

/**
 * Set the Develop Menu enabled.
 * @param enabled
 *    New status.
 */
void
CaretPreferences::setDevelopMenuEnabled(const bool enabled)
{
    if (this->developMenuEnabled == enabled) {
        return;
    }
    
    this->developMenuEnabled = enabled;
    this->setBoolean(CaretPreferences::NAME_DEVELOP_MENU,
                     this->developMenuEnabled);
    this->qSettings->sync();
}

/**
 * @return Is Show Data ToolTips enabled?
 */
bool
CaretPreferences::isShowDataToolTipsEnabled() const
{
    return this->dataToolTipsEnabled;
}

/**
 * Set Show Data ToolTips enabled.
 * @param enabled
 *    New status.
 */
void
CaretPreferences::setShowDataToolTipsEnabled(const bool enabled)
{
    if (this->dataToolTipsEnabled == enabled) {
        return;
    }
    
    this->dataToolTipsEnabled = enabled;
    this->setBoolean(CaretPreferences::NAME_DATA_TOOL_TIPS,
                     this->dataToolTipsEnabled);
    this->qSettings->sync();
}


/**
 * @param Is yoking defaulted on ?
 */
bool CaretPreferences::isYokingDefaultedOn() const
{
    return this->yokingDefaultedOn;
}

/**
 * Set yoking defaulted on
 *
 * @param status
 *    New status for yoking on.
 */
void CaretPreferences::setYokingDefaultedOn(const bool status)
{
    if (this->yokingDefaultedOn == status) {
        return;
    }
    
    this->yokingDefaultedOn = status;
    this->setBoolean(CaretPreferences::NAME_YOKING_DEFAULT_ON,
                     this->yokingDefaultedOn);
    this->qSettings->sync();
}

/**
 * @param Is volume identification defaulted on ?
 */
bool CaretPreferences::isVolumeIdentificationDefaultedOn() const
{
    return this->volumeIdentificationDefaultedOn;
}

/**
 * Set volume identification defaulted on
 *
 * @param status
 *    New status for yoking on.
 */
void CaretPreferences::setVolumeIdentificationDefaultedOn(const bool status)
{
    if (this->volumeIdentificationDefaultedOn == status) {
        return;
    }
    
    this->volumeIdentificationDefaultedOn = status;
    this->setBoolean(CaretPreferences::NAME_VOLUME_IDENTIFICATION_DEFAULTED_ON,
                     this->volumeIdentificationDefaultedOn);
    this->qSettings->sync();
}

/**
 * @return Pointer to the macros.
 */
WuQMacroGroup*
CaretPreferences::getMacros()
{
    return m_macros.get();
}

/**
 * @return Const pointer to the macros.
 */
const WuQMacroGroup*
CaretPreferences::getMacros() const
{
    return m_macros.get();
}

/**
 * Read an unsigned byte array to the preferences.
 *
 * @param name
 *     Name for preferences
 * @param array
 *     The array that is read.
 * @param numberOfElements
 *     Number of elements in the array.
 */
void
CaretPreferences::readUnsignedByteArray(const AString& name,
                                        uint8_t array[],
                                        const int32_t numberOfElements)
{
    const int numAvailable = this->qSettings->beginReadArray(name);
    const int numToRead = std::min(numAvailable,
                                   numberOfElements);
    for (int i = 0; i < numToRead; i++) {
        this->qSettings->setArrayIndex(i);
        array[i] = static_cast<uint8_t>(this->qSettings->value(AString::number(i)).toInt());
    }
    this->qSettings->endArray();
}

/**
 * Write an unsigned byte array to the preferences.
 *
 * @param name
 *     Name for preferences
 * @param array
 *     The array that is written.
 * @param numberOfElements
 *     Number of elements in the array.
 */
void
CaretPreferences::writeUnsignedByteArray(const AString& name,
                                         const uint8_t array[],
                                         const int32_t numberOfElements)
{
    this->qSettings->beginWriteArray(name);
    for (int i = 0; i < numberOfElements; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  array[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Initialize/Read the preferences
 */
void 
CaretPreferences::readPreferences()
{
    userColors.reset();
    
    uint8_t colorRGB[3] = { 0, 0, 0 };
    
    userColors.getColorForegroundWindow(colorRGB);
    readUnsignedByteArray(NAME_COLOR_FOREGROUND_WINDOW,
                          colorRGB,
                          3);
    userColors.setColorForegroundWindow(colorRGB);
    
    userColors.getColorBackgroundWindow(colorRGB);
    readUnsignedByteArray(NAME_COLOR_BACKGROUND_WINDOW,
                          colorRGB,
                          3);
    userColors.setColorBackgroundWindow(colorRGB);
    
    userColors.getColorForegroundAllView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_FOREGROUND_ALL,
                          colorRGB,
                          3);
    userColors.setColorForegroundAllView(colorRGB);
    
    userColors.getColorBackgroundAllView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_BACKGROUND_ALL,
                          colorRGB,
                          3);
    userColors.setColorBackgroundAllView(colorRGB);
    
    userColors.getColorForegroundChartView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_FOREGROUND_CHART,
                          colorRGB,
                          3);
    userColors.setColorForegroundChartView(colorRGB);
    
    userColors.getColorBackgroundChartView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_BACKGROUND_CHART,
                          colorRGB,
                          3);
    userColors.setColorBackgroundChartView(colorRGB);
    
    userColors.getColorForegroundSurfaceView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_FOREGROUND_SURFACE,
                          colorRGB,
                          3);
    userColors.setColorForegroundSurfaceView(colorRGB);
    
    userColors.getColorBackgroundSurfaceView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_BACKGROUND_SURFACE,
                          colorRGB,
                          3);
    userColors.setColorBackgroundSurfaceView(colorRGB);
    
    userColors.getColorForegroundMediaView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_FOREGROUND_MEDIA,
                          colorRGB,
                          3);
    userColors.setColorForegroundMediaView(colorRGB);
    
    userColors.getColorBackgroundMediaView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_BACKGROUND_MEDIA,
                          colorRGB,
                          3);
    userColors.setColorBackgroundMediaView(colorRGB);

    userColors.getColorForegroundVolumeView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_FOREGROUND_VOLUME,
                          colorRGB,
                          3);
    userColors.setColorForegroundVolumeView(colorRGB);
    
    userColors.getColorBackgroundVolumeView(colorRGB);
    readUnsignedByteArray(NAME_COLOR_BACKGROUND_VOLUME,
                          colorRGB,
                          3);
    userColors.setColorBackgroundVolumeView(colorRGB);

    userColors.getColorChartMatrixGridLines(colorRGB);
    readUnsignedByteArray(NAME_COLOR_CHART_MATRIX_GRID_LINES,
                          colorRGB,
                          3);
    userColors.setColorChartMatrixGridLines(colorRGB);
    
    userColors.getColorChartHistogramThreshold(colorRGB);
    readUnsignedByteArray(NAME_COLOR_CHART_HISTOGRAM_THRESHOLD,
                          colorRGB,
                          3);
    userColors.setColorChartHistogramThreshold(colorRGB);
    
    /*
     * Old storage for previous spec and scene no longer used
     * but might want to use them to initialize the new
     * scene and spce files in RecentFileItem's.
     */
    const bool readObsoleteFlag(false);
    if (readObsoleteFlag) {
        this->previousSpecFiles.clear();
        const int numPrevSpec = this->qSettings->beginReadArray(NAME_PREVIOUS_SPEC_FILES);
        for (int i = 0; i < numPrevSpec; i++) {
            this->qSettings->setArrayIndex(i);
            previousSpecFiles.push_back(this->qSettings->value(AString::number(i)).toString());
        }
        this->qSettings->endArray();
        
        this->previousSceneFiles.clear();
        const int numPrevScene = this->qSettings->beginReadArray(NAME_PREVIOUS_SCENE_FILES);
        for (int i = 0; i < numPrevScene; i++) {
            this->qSettings->setArrayIndex(i);
            previousSceneFiles.push_back(this->qSettings->value(AString::number(i)).toString());
        }
        this->qSettings->endArray();
    }
    
    this->readCustomViews(false);
    
    this->readTileTabsUserConfigurations(false);
    
    this->readMacros(false);

    AString levelName = this->qSettings->value(NAME_LOGGING_LEVEL,
                                          LogLevelEnum::toName(LogLevelEnum::INFO)).toString();
    bool valid = false;
    LogLevelEnum::Enum logLevel = LogLevelEnum::fromName(levelName, &valid);
    if (valid == false) {
        logLevel = LogLevelEnum::INFO;
    }
    /* Do not call setLoggingLevel() as it will cause preferences to sync */
    this->loggingLevel = logLevel;
    CaretLogger::getLogger()->setLevel(this->loggingLevel);
    
    ImageCaptureMethodEnum::Enum defaultCaptureType = ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_RENDER_PIXMAP;
    AString imageCaptureMethodName = this->qSettings->value(NAME_IMAGE_CAPTURE_METHOD,
                                                        ImageCaptureMethodEnum::toName(defaultCaptureType)).toString();
    bool validImageCaptureMethodName = false;
    this->imageCaptureMethod = ImageCaptureMethodEnum::fromName(imageCaptureMethodName,
                                                            &validImageCaptureMethodName);
    if ( ! validImageCaptureMethodName) {
        this->imageCaptureMethod = defaultCaptureType;
    }
    
    AString openGLDrawingMethodName = this->qSettings->value(NAME_OPENGL_DRAWING_METHOD,
                                                             OpenGLDrawingMethodEnum::toName(OpenGLDrawingMethodEnum::DRAW_WITH_VERTEX_BUFFERS_OFF)).toString();
    bool validDrawingMethod = false;
    this->openGLDrawingMethod = OpenGLDrawingMethodEnum::fromName(openGLDrawingMethodName,
                                                                  &validDrawingMethod);
    if ( ! validDrawingMethod) {
        this->openGLDrawingMethod = OpenGLDrawingMethodEnum::DRAW_WITH_VERTEX_BUFFERS_OFF;
    }
    
    AString viewFileTypesName = this->qSettings->value(NAME_MANAGE_FILES_VIEW_FILE_TYPE,
                                                       SpecFileDialogViewFilesTypeEnum::toName(SpecFileDialogViewFilesTypeEnum::VIEW_FILES_ALL)).toString();
    bool viewFilesTypeValid = false;
    this->manageFilesViewFileType = SpecFileDialogViewFilesTypeEnum::fromName(viewFileTypesName,
                                                                              &viewFilesTypeValid);
    if ( ! viewFilesTypeValid) {
        this->manageFilesViewFileType = SpecFileDialogViewFilesTypeEnum::VIEW_FILES_ALL;
    }
    
    this->displayVolumeAxesLabels = this->getBoolean(CaretPreferences::NAME_VOLUME_AXES_LABELS,
                                                     true);
    this->displayVolumeAxesCrosshairs = this->getBoolean(CaretPreferences::NAME_VOLUME_AXES_CROSSHAIRS,
                                                         true);    
    this->displayVolumeAxesCoordinates = this->getBoolean(CaretPreferences::NAME_VOLUME_AXES_COORDINATE,
                                                          true);
    
    this->volumeMontageGap = this->getInteger(CaretPreferences::NAME_VOLUME_MONTAGE_GAP,
                                              3);
    
    this->volumeMontageCoordinatePrecision = this->getInteger(CaretPreferences::NAME_VOLUME_MONTAGE_COORDINATE_PRECISION,
                                                              0);
    
    this->animationStartTime = 0.0;//this->qSettings->value(CaretPreferences::NAME_ANIMATION_START_TIME).toDouble();

    
    this->splashScreenEnabled = this->getBoolean(CaretPreferences::NAME_SPLASH_SCREEN,
                                                 true);
    
    this->developMenuEnabled = this->getBoolean(CaretPreferences::NAME_DEVELOP_MENU,
                                                false);
    
    this->dataToolTipsEnabled = this->getBoolean(CaretPreferences::NAME_DATA_TOOL_TIPS,
                                                 true);

    this->yokingDefaultedOn = this->getBoolean(CaretPreferences::NAME_YOKING_DEFAULT_ON,
                                               true);
    
    this->volumeIdentificationDefaultedOn = this->getBoolean(CaretPreferences::NAME_VOLUME_IDENTIFICATION_DEFAULTED_ON,
                                                             true);
    
    this->dynamicConnectivityDefaultedOn = this->getBoolean(CaretPreferences::NAME_DYNAMIC_CONNECTIVITY_ON,
                                                            true);
    
    this->remoteFileUserName = this->getString(NAME_REMOTE_FILE_USER_NAME);
    this->remoteFilePassword = this->getString(NAME_REMOTE_FILE_PASSWORD);
    this->remoteFileLoginSaved = this->getBoolean(NAME_REMOTE_FILE_LOGIN_SAVED,
                                                  false);
    
    this->balsaUserName = this->getString(NAME_BALSA_USER_NAME);
    
    this->showSurfaceIdentificationSymbols = this->getBoolean(NAME_SHOW_SURFACE_IDENTIFICATION_SYMBOLS,
                                                              true);
    this->showVolumeIdentificationSymbols = this->getBoolean(NAME_SHOW_VOLUME_IDENTIFICATION_SYMBOLS,
                                                             true);
}

/**
 * Read the custom views.  Since user's may want to use them
 * in multiple instance of workbench that are running, this method allows
 * the custom views to be read without affecting other preferences.
 *
 * @param performSync
 *    Sync with preferences since preferences may have been changed
 *    by a concurrently running workbench.
 */
void
CaretPreferences::readCustomViews(const bool performSync)
{
    if (performSync) {
        this->qSettings->sync();
    }
    
    this->removeAllCustomViews();
    
    /*
     * Previously had "userViews" prior to CustomViews
     */
    const int numUserViews = this->qSettings->beginReadArray("userViews");
    for (int i = 0; i < numUserViews; i++) {
        this->qSettings->setArrayIndex(i);
        const AString viewString = this->qSettings->value(AString::number(i)).toString();
        ModelTransform uv;
        if (uv.setFromString(viewString)) {
            this->customViews.push_back(new ModelTransform(uv));
        }
    }
    this->qSettings->endArray();

    /*
     * Read Custom Views
     */
    const int numCustomViews = this->qSettings->beginReadArray(NAME_CUSTOM_VIEWS);
    for (int i = 0; i < numCustomViews; i++) {
        this->qSettings->setArrayIndex(i);
        const AString viewString = this->qSettings->value(AString::number(i)).toString();
        ModelTransform uv;
        if (uv.setFromString(viewString)) {
            this->customViews.push_back(new ModelTransform(uv));
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

/**
 * Convert RGB bytes to Floats.
 *
 * @param bytesRGB
 *     Byte RGB color [0, 255] INPUT
 * @param floatRGB
 *     Float RGB color [0.0, 1.0]  OUTPUT
 */
void
CaretPreferences::byteRgbToFloatRgb(const uint8_t byteRGB[3],
                                    float floatRGB[3])
{
    for (int32_t i = 0; i < 3; i++) {
        floatRGB[i] = static_cast<float>(byteRGB[i]) / 255.0;
    }
}

/**
 * Read the recent scene and spec files from preferences and add them to the given container.
 *
 *  @param container
 *   Recent scene and spec files are added to this container
 *  @param errorMessageOut
 *   Container error information if false returned
 *  @return True if successful, false if error.
 */
bool
CaretPreferences::readRecentSceneAndSpecFiles(RecentFileItemsContainer* container,
                                              AString& errorMessageOut)
{
    const bool successFlag = readRecentFileItemsContainer(NAME_RECENT_SCENE_AND_SPEC_FILES,
                                                          container,
                                                          errorMessageOut);
    if (successFlag) {
        container->removeItemsExceedingMaximumNumber(getRecentMaximumNumberOfSceneAndSpecFiles());
    }
    
    return successFlag;
}

/**
 * Write the recent scene and spec files to preferences from the given container.
 *
 *  @param container
 *   Recent scene and spec files written to preferences
 *  @param errorMessageOut
 *   Container error information if false returned
 *  @return True if successful, false if error.
 */
bool
CaretPreferences::writeRecentSceneAndSpecFiles(RecentFileItemsContainer* container,
                                               AString& errorMessageOut)
{
    container->removeItemsExceedingMaximumNumber(getRecentMaximumNumberOfSceneAndSpecFiles());
    return writeRecentFileItemsContainer(NAME_RECENT_SCENE_AND_SPEC_FILES,
                                         container,
                                         errorMessageOut);
}

/**
 * If the file is a scene or spec file (detected by extension) add it to recent scene/spec files.
 * Always add the directory containing the file to the recent directories.
 *
 * @param filename
 *  Name of file.
 */
void
CaretPreferences::addToRecentFilesAndOrDirectories(const AString& directoryOrFileName)
{
    /*
     * Only update recents directories if GUI application (wb_view)
     */
    if (ApplicationInformation().getApplicationType() != ApplicationTypeEnum::APPLICATION_TYPE_GRAPHICAL_USER_INTERFACE) {
        return;
    }
    
    if ((getRecentMaximumNumberOfSceneAndSpecFiles() <= 0)
        && (getRecentMaximumNumberOfDirectories() <= 0)) {
        return;
    }
    
    /*
     * User could have a directory ending with a scene or spec file extension.
     * While highly unlikely, make sure it is a file before adding to
     * recent scene/spec files.
     */
    FileInformation fileInfo(directoryOrFileName);
    if (fileInfo.isFile()) {
        bool validFlag;
        const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(directoryOrFileName,
                                                                                        &validFlag);
        if (validFlag) {
            switch (dataFileType) {
                case DataFileTypeEnum::SCENE:
                case DataFileTypeEnum::SPECIFICATION:
                    addToRecentSceneAndSpecFiles(directoryOrFileName);
                    break;
                default:
                    break;
            }
        }
    }
    
    /*
     * Always update the recent directories
     * unless a remote file (http, ftp, etc)
     */
    if ( ! fileInfo.isRemoteFile()) {
        addToRecentDirectories(directoryOrFileName);
    }
}

/**
 * Add the given filename to the recent scene and spec files
 * @param filename
 *  Name of file.
 */
void
CaretPreferences::addToRecentSceneAndSpecFiles(const AString& filename)
{
    /*
     * Only update recents files if GUI application (wb_view)
     */
    if (ApplicationInformation().getApplicationType() != ApplicationTypeEnum::APPLICATION_TYPE_GRAPHICAL_USER_INTERFACE) {
        return;
    }
    
    if (getRecentMaximumNumberOfSceneAndSpecFiles() <= 0) {
        return;
    }
    
    if (filename.isEmpty()) {
        return;
    }
    
    if (isInRecentFilesExclusionPaths(filename)) {
        return;
    }
    
    const AString errorPrefix("Error updating recent scene spec files ");
    
    RecentFileItemTypeEnum::Enum itemType = RecentFileItemTypeEnum::SCENE_FILE;
    bool validFlag(false);
    const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(filename,
                                                                                    &validFlag);
    if (validFlag) {
        switch (dataFileType) {
            case DataFileTypeEnum::SCENE:
                itemType = RecentFileItemTypeEnum::SCENE_FILE;
                break;
            case DataFileTypeEnum::SPECIFICATION:
                itemType = RecentFileItemTypeEnum::SPEC_FILE;
                break;
            default:
                CaretLogSevere(errorPrefix
                               + "file extension is neither scene nor spec "
                               + filename);
                return;
                break;
        }
    }
    else {
        CaretLogSevere(errorPrefix
                       + "invalid file extension on "
                       + filename);
        return;
    }
    
    std::unique_ptr<RecentFileItemsContainer> container(RecentFileItemsContainer::newInstance());
    CaretAssert(container);
    
    AString errorMessage;
    bool successFlag(readRecentSceneAndSpecFiles(container.get(),
                                                 errorMessage));
    if ( ! successFlag) {
        CaretLogSevere(errorPrefix
                       + " during reading: "
                       + errorMessage);
        return;
    }
    
    RecentFileItem* newItem = new RecentFileItem(itemType,
                                                 filename);
    newItem->setLastAccessByWorkbenchDateTimeToCurrentDateTime();
    container->addItem(newItem);
    
    successFlag = writeRecentSceneAndSpecFiles(container.get(),
                                        errorMessage);
    if ( ! successFlag) {
        CaretLogSevere(errorPrefix
                       + " during writing: "
                       + errorMessage);
        return;
    }
}

/**
 * Read the recent file items container using the  given preference name.
 *
 * @param preferenceName
 *   Name of container in preferences
 * @param container
 *   The recent file items container
 * @param errorMessageOut
 *   Eerror information if false returned
 * @return True if successful, false if error.
 */
bool
CaretPreferences::readRecentFileItemsContainer(const AString& preferenceName,
                                               RecentFileItemsContainer* container,
                                               AString& errorMessageOut)
{
    CaretAssert(container);
    bool resultFlag(false);
    errorMessageOut.clear();
    
    const AString xmlText = getString(preferenceName);
    if ( ! xmlText.isEmpty()) {
        resultFlag = container->readFromXML(xmlText,
                                            errorMessageOut);
    }
    else {
        /* Empty string (no previous value in preferences) is OK */
        resultFlag = true;
    }
    
    return resultFlag;
}

/**
 * Write the recent file items container using the  given preference name.
 *
 * @param preferenceName
 *   Name of container in preferences
 * @param container
 *   The recent file items container
 * @param errorMessageOut
 *   Eerror information if false returned
 * @return True if successful, false if error.
 */
bool
CaretPreferences::writeRecentFileItemsContainer(const AString& preferenceName,
                                                const RecentFileItemsContainer* container,
                                                AString& errorMessageOut)
{
    CaretAssert(container);
    bool resultFlag(false);
    errorMessageOut.clear();
    
    AString xmlText;
    resultFlag = container->writeToXML(xmlText,
                                       errorMessageOut);
    if (resultFlag) {
        setString(preferenceName,
                  xmlText);
    }
    
    return resultFlag;
}

/**
 * Read the recent directories from preferences and add them to the given container.
 *
 *  @param container
 *   Recent directories are added to this container
 *  @param errorMessageOut
 *   Container error information if false returned
 *  @return True if successful, false if error.
 */
bool
CaretPreferences::readRecentDirectories(RecentFileItemsContainer* container,
                                        AString& errorMessageOut)
{
    const bool successFlag = readRecentFileItemsContainer(NAME_RECENT_DIRECTORIES,
                                                          container,
                                                          errorMessageOut);
    if (successFlag) {
        container->removeItemsExceedingMaximumNumber(getRecentMaximumNumberOfDirectories());
    }
    
    return successFlag;
}

/**
 * Write the recent directories to preferences from the given container.
 *
 *  @param container
 *   Recent directories written to preferences
 *  @param errorMessageOut
 *   Container error information if false returned
 *  @return True if successful, false if error.
 */
bool
CaretPreferences::writeRecentDirectories(RecentFileItemsContainer* container,
                                         AString& errorMessageOut)
{
    container->removeItemsExceedingMaximumNumber(getRecentMaximumNumberOfDirectories());
    
    return writeRecentFileItemsContainer(NAME_RECENT_DIRECTORIES,
                                         container,
                                         errorMessageOut);
}

/**
 * Add a directory (or parent directory of file) to the recent directories.  If the given directory/file is a file, the files parent
 * directory is added to the recent directories.  If the parameter is neither a valid file nor valid directory,
 * no action is taken.
 *
 * @param directoryOrFileName
 *  Name of directory or file.
 */
void
CaretPreferences::addToRecentDirectories(const AString& directoryOrFileName)
{
    /*
     * Only update recents directories if GUI application (wb_view)
     */
    if (ApplicationInformation().getApplicationType() != ApplicationTypeEnum::APPLICATION_TYPE_GRAPHICAL_USER_INTERFACE) {
        return;
    }

    if (getRecentMaximumNumberOfDirectories() <= 0) {
        return;
    }
    
    if (isInRecentFilesExclusionPaths(directoryOrFileName)) {
        return;
    }
    
    AString directoryName(directoryOrFileName);
    
    if (directoryName.isEmpty()) {
        return;
    }
    
    const AString errorPrefix("Error updating recent directories ");
    
    FileInformation fileInfo(directoryName);
    if ( ! fileInfo.isDirectory()) {
        if (fileInfo.isFile()) {
            directoryName = fileInfo.getAbsolutePath();
            
            if (directoryName.isEmpty()) {
                CaretLogSevere(errorPrefix
                               + " unable to determine path for file: "
                               + directoryName);
                return;
            }
        }
        else {
            CaretLogSevere(errorPrefix
                           + " not a valid directory nor file: "
                           + directoryName);
            return;
        }
    }
        
    std::unique_ptr<RecentFileItemsContainer> container(RecentFileItemsContainer::newInstance());
    CaretAssert(container);
    
    AString errorMessage;
    bool successFlag(readRecentDirectories(container.get(),
                                           errorMessage));
    if ( ! successFlag) {
        CaretLogSevere(errorPrefix
                       + " during reading: "
                       + errorMessage);
        return;
    }
    
    RecentFileItem* newItem = new RecentFileItem(RecentFileItemTypeEnum::DIRECTORY,
                                                 directoryName);
    newItem->setLastAccessByWorkbenchDateTimeToCurrentDateTime();
    container->addItem(newItem);
    
    successFlag = writeRecentDirectories(container.get(),
                                         errorMessage);
    if ( ! successFlag) {
        CaretLogSevere(errorPrefix
                       + " during writing: "
                       + errorMessage);
        return;
    }
}

/**
 * Get the recent directories for use in the Open Data File Dialog's "history".
 * @param favoritesFirstFlag
 * If true, any directories that are in user's favorites are listed first.
 * @param directoriesOut
 * Output containing the recent directories
 */
void
CaretPreferences::getRecentDirectoriesForOpenFileDialogHistory(const bool favoritesFirstFlag,
                                                               std::vector<AString>& directoriesOut)
{
    directoriesOut.clear();
    
    std::unique_ptr<RecentFileItemsContainer> dirsContainer(RecentFileItemsContainer::newInstanceRecentDirectories(this,
                                                                                                                   RecentFileItemsContainer::WriteIfModifiedType::WRITE_NO));
    RecentFileItemsFilter filter;
    filter.setListDirectories(true);
    std::vector<RecentFileItem*> items = dirsContainer->getItems(filter);
    RecentFileItemsContainer::sort(RecentFileItemSortingKeyEnum::DATE_NEWEST,
                                   items);
    
    std::vector<AString> notFavs;
    for (auto rfi : items) {
        if (favoritesFirstFlag
            && rfi->isFavorite()) {
            directoriesOut.push_back(rfi->getPathAndFileName());
        }
        else {
            notFavs.push_back(rfi->getPathAndFileName());
        }
    }
    directoriesOut.insert(directoriesOut.end(),
                          notFavs.begin(),
                          notFavs.end());
}

/**
 * @return Maximum number of recent files for open recent files dialog
 */
int32_t
CaretPreferences::getRecentMaximumNumberOfSceneAndSpecFiles() const
{
    return m_recentMaximumNumberOfSceneAndSpecFilesPreference->getValue().toInt();
}

/**
 * Set maximum number of recent files for open recent files dialog
 * @param maximumNumberOfFiles
 *    New maximum number of files
 */
void
CaretPreferences::setRecentMaximumNumberOfSceneAndSpecFiles(const int32_t maximumNumberOfFiles)
{
    m_recentMaximumNumberOfSceneAndSpecFilesPreference->setValue(maximumNumberOfFiles);
}

/**
 * Clear the recent files
 * @param removeFavoritesFlag
 *    If true remove any favorites, else keep them.
 */
void
CaretPreferences::clearRecentSceneAndSpecFiles(const bool removeFavoritesFlag)
{
    const AString errorPrefix("Error updating recent directories ");
    
    std::unique_ptr<RecentFileItemsContainer> container(RecentFileItemsContainer::newInstance());
    AString errorMessage;
    bool successFlag(readRecentSceneAndSpecFiles(container.get(),
                                                 errorMessage));
    if ( ! successFlag) {
        CaretLogSevere(errorPrefix
                       + " during reading: "
                       + errorMessage);
        return;
    }
    
    if (removeFavoritesFlag) {
        container->removeAllItemsIncludingFavorites();
    }
    else {
        container->removeAllItemsExcludingFavorites();
    }
    
    successFlag = writeRecentSceneAndSpecFiles(container.get(),
                                               errorMessage);
    if ( ! successFlag) {
        CaretLogSevere(errorPrefix
                       + " during writing: "
                       + errorMessage);
        return;
    }
}

/**
 * @return Maximum number of recent directories for open recent files dialog
 */
int32_t
CaretPreferences::getRecentMaximumNumberOfDirectories() const
{
    return m_recentMaximumNumberOfDirectoriesPreferences->getValue().toInt();
}

/**
 * Set maximum number of recent directories for open recent files dialog
 * @param maximumNumberOfDirectories
 *    New maximum number of directories
 */
void CaretPreferences::setRecentMaximumNumberOfDirectories(const int32_t maximumNumberOfDirectories)
{
    m_recentMaximumNumberOfDirectoriesPreferences->setValue(maximumNumberOfDirectories);
}

/**
 * Clear the recent directories
 * @param removeFavoritesFlag
 *    If true remove any favorites, else keep them.
 */
void
CaretPreferences::clearRecentDirectories(const bool removeFavoritesFlag)
{
    const AString errorPrefix("Error updating recent directories ");
    
    std::unique_ptr<RecentFileItemsContainer> container(RecentFileItemsContainer::newInstance());
    AString errorMessage;
    bool successFlag(readRecentDirectories(container.get(),
                                           errorMessage));
    if ( ! successFlag) {
        CaretLogSevere(errorPrefix
                       + " during reading: "
                       + errorMessage);
        return;
    }
    
    if (removeFavoritesFlag) {
        container->removeAllItemsIncludingFavorites();
    }
    else {
        container->removeAllItemsExcludingFavorites();
    }
    
    successFlag = writeRecentDirectories(container.get(),
                                         errorMessage);
    if ( ! successFlag) {
        CaretLogSevere(errorPrefix
                       + " during writing: "
                       + errorMessage);
        return;
    }
}

/**
 * @return Mode for accessing the file system for recent files/directories file info (last modified)
 */
RecentFilesSystemAccessModeEnum::Enum
CaretPreferences::getRecentFilesSystemAccessMode() const
{
    bool validFlag(false);
    RecentFilesSystemAccessModeEnum::Enum mode = RecentFilesSystemAccessModeEnum::fromName(m_recentFilesSystemAccessMode->getValue().toString(),
                                                                                           &validFlag);
    return mode;
}

/**
 * Set mode for accessing the file system for recent files/directories file info (last modified)
 * @param filesSystemAccessMode
 *    New mode
 */
void
CaretPreferences::setRecentFilesSystemAccessMode(const RecentFilesSystemAccessModeEnum::Enum filesSystemAccessMode)
{
    m_recentFilesSystemAccessMode->setValue(RecentFilesSystemAccessModeEnum::toName(filesSystemAccessMode));
}

/**
 * Get the recent files exclusion paths
 * @param exclusionPathsOut
 *    Output containing the exclusion paths
 */
void
CaretPreferences::readRecentFilesExclusionPaths(std::set<AString>& exclusionPathsOut)
{
    exclusionPathsOut.clear();
    
    std::unique_ptr<RecentFileItemsContainer> container(RecentFileItemsContainer::newInstance());
    AString errorMessage;
    const bool resultFlag = readRecentFileItemsContainer(NAME_RECENT_EXCLUSION_PATHS,
                                                         container.get(),
                                                         errorMessage);
    if (resultFlag) {
        RecentFileItemsFilter filter;
        filter.setListDirectories(true);
        std::vector<RecentFileItem*> items = container->getItems(filter);
        for (auto p : items) {
            exclusionPathsOut.insert(p->getPathAndFileName());
        }
    }
    else {
        CaretLogWarning("Reading recent files exclusion paths: "
                        + errorMessage);
    }
}

/**
 * Set the recent files exclusion paths
 * @param exclusionPathsOut
 *    Exclusion paths saved into preferences
 */
void
CaretPreferences::writeRecentFilesExclusionPaths(const std::set<AString>& exclusionPaths)
{
    std::unique_ptr<RecentFileItemsContainer> container(RecentFileItemsContainer::newInstance());
    for (auto p : exclusionPaths) {
        RecentFileItem* item = new RecentFileItem(RecentFileItemTypeEnum::DIRECTORY,
                                                  p);
        container->addItem(item);
    }
    AString errorMessage;
    const bool resultFlag = writeRecentFileItemsContainer(NAME_RECENT_EXCLUSION_PATHS,
                                                         container.get(),
                                                         errorMessage);
    if ( ! resultFlag) {
        CaretLogWarning("Writing recent files exclusion paths: "
                        + errorMessage);
    }
}

/**
 * Add a path to the recent files exclusion paths
 * @param exclusionPath
 *    Path to add
 */
void
CaretPreferences::addToRecentFilesExclusionPaths(const AString& exclusionPath)
{
    std::set<AString> paths;
    readRecentFilesExclusionPaths(paths);
    paths.insert(exclusionPath);
    writeRecentFilesExclusionPaths(paths);
}

/**
 * Remove a path to the recent files exclusion paths
 * @param exclusionPath
 *    Path to remove
 */
void
CaretPreferences::removeFromRecentFilesExclusionPaths(const AString& exclusionPath)
{
    std::set<AString> paths;
    readRecentFilesExclusionPaths(paths);
    paths.erase(exclusionPath);
    writeRecentFilesExclusionPaths(paths);
}

/**
 * @return True if the given file/directory is in a recent files exclusion path
 * @param fileOrDirectoryName
 *    Name of file or directory
 */
bool
CaretPreferences::isInRecentFilesExclusionPaths(const AString& fileOrDirectoryName)
{
    if (fileOrDirectoryName.isEmpty()) {
        return false;
    }
    
    std::set<AString> exclusionPaths;
    readRecentFilesExclusionPaths(exclusionPaths);
    if (exclusionPaths.empty()) {
        return false;
    }
    
    const AString directoryName = FileInformation(fileOrDirectoryName).getAbsolutePath();

    for (auto ep : exclusionPaths) {
        if (directoryName.startsWith(ep)) {
            return true;
        }
    }
    
    return false;
}

/**
 * Get the key in QSettings for a palette with the given name
 * @param paletteName
 *    Name of palette
 * @return
 *    Key in the form value-of-NAME_PALETTE_GROUP_KEY/value-of-paletteName
 *          eg: palette/PSYCH
 */
AString
CaretPreferences::getPaletteKey(const AString& paletteName) const
{
    const QString name(paletteName.trimmed());
    if (name.isEmpty()) {
        CaretLogSevere("Empty palette name for generation of preferences key");
        return "";
    }
    
    return (NAME_PALETTE_GROUP_KEY
            + "/"
            + name);
}


/**
 * Test to verify a palette with the given name exists
 * @param paletteName
 *    Name of palette
 * @return True if palette exists, else false.
 */
bool
CaretPreferences::paletteUserCustomExists(const AString& paletteName)
{
    return this->qSettings->contains(getPaletteKey(paletteName));
}


/**
 * Get the XML representation of all user custom palettes
 * @param palettesXmlOut
 *     XML text for all palettes
 */
void
CaretPreferences::paletteUserCustomGetAll(std::vector<AString>& palettesXmlOut)
{
    palettesXmlOut.clear();
    
    /*
     * Get keys in palette group.  These keys will be just
     * the name of the palette since the child keys are requested
     * while the palette group is active
     */
    this->qSettings->beginGroup(NAME_PALETTE_GROUP_KEY);
    QStringList paletteKeyNames = this->qSettings->childKeys();
    this->qSettings->endGroup();
    
    QStringListIterator iter(paletteKeyNames);
    while (iter.hasNext()) {
        AString paletteXML;
        if (paletteUserCustomGetByName(iter.next(),
                                       paletteXML)) {
            palettesXmlOut.push_back(paletteXML);
        }
    }
}

/**
 * Get a user custom palette XML representation by name of palette
 * @param paletteName
 *    Name of palette
 * @param paletteXmlOut
 *    XML representation of palette
 * @return True if palette exists and output xml is valid, else false.
 */
bool
CaretPreferences::paletteUserCustomGetByName(const AString& paletteName,
                                             AString& paletteXmlOut)
{
    paletteXmlOut.clear();
    
    const QString keyName = getPaletteKey(paletteName);
    if ( ! keyName.isEmpty()) {
        QString paletteXML = this->qSettings->value(keyName, "").toString();
        if (paletteXML.isEmpty()) {
            CaretLogSevere("Palette XML is empty in Preferences for "
                           + keyName);
        }
        else {
            paletteXmlOut = paletteXML;
            return true;
        }
    }
    
    return false;
}

/**
 * Add a user custom palette XML representation by name of palette.
 * Palette names must be unique and attempting to add a palette whose name
 * matches the name of an existing palette will fail.
 * @param paletteName
 *    Name of palette
 * @param paletteXml
 *    XML representation of palette
 * @param errorMessageOut
 *    Output with error message if adding palette fails.
 * @return True if palette was added, else false (palette with name exists)
 */
bool
CaretPreferences::paletteUserCustomAdd(const AString& paletteName,
                                       const AString& paletteXML,
                                       AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (paletteName.trimmed().isEmpty()) {
        errorMessageOut = ("Attempting to add palette with empty name.");
        return false;
    }
    
    if (paletteUserCustomExists(paletteName)) {
        errorMessageOut = ("Unable to add palette named \""
                           + paletteName
                           + "\".  Palette with that names exists and palette "
                           "names must be unique.");
        return false;
    }
    
    if (paletteXML.trimmed().isEmpty()) {
        errorMessageOut = ("Unable to add palette with name \""
                           + paletteName
                           + "\".  Palette XML is empty.");
        return false;
    }
    
    this->qSettings->setValue(getPaletteKey(paletteName),
                              paletteXML);
    return true;
}

/**
 * Replace a user custom palette.
 * A palette with the given name must exist or an error will occur.
 * @param paletteName
 *    Name of palette
 * @param paletteXml
 *    XML representation of palette
 * @param errorMessageOut
 *    Output with error message if replacing palette fails.
 * @return True if palette was replaced, else false (palette with name does not exist)
 */
bool
CaretPreferences::paletteUserCustomReplace(const AString& paletteName,
                                           const AString& paletteXML,
                                           AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (paletteName.trimmed().isEmpty()) {
        errorMessageOut = ("Attempting to replace palette with empty name.");
        return false;
    }
    
    if ( ! paletteUserCustomExists(paletteName)) {
        errorMessageOut = ("Unable to replace palette named \""
                           + paletteName
                           + "\".  Palette with that names does not exist.");
        return false;
    }
    
    if (paletteXML.trimmed().isEmpty()) {
        errorMessageOut = ("Unable to replace palette with name \""
                           + paletteName
                           + "\".  Palette XML is empty.");
        return false;
    }
    
    this->qSettings->setValue(getPaletteKey(paletteName),
                              paletteXML);
    return true;
}

/**
 * Remove a user custom palette.
 * A palette with the given name must exist or an error will occur.
 * @param paletteName
 *    Name of palette
 * @param errorMessageOut
 *    Output with error message if removing palette fails.
 * @return True if palette was removed, else false (palette with name does not exist)
 */
bool
CaretPreferences::paletteUserCustomRemove(const AString& paletteName,
                                          AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (paletteName.trimmed().isEmpty()) {
        errorMessageOut = ("Attempting to remove palette with empty name.");
        return false;
    }
    
    if ( ! paletteUserCustomExists(paletteName)) {
        errorMessageOut = ("Unable to remove palette named \""
                           + paletteName
                           + "\".  Palette with that names does not exist.");
        return false;
    }
    
    this->qSettings->remove(getPaletteKey(paletteName));
    
    return true;
}

/**
 * Rename a user custom palette.
 * A palette with the given name must exist or an error will occur.
 * @param paletteName
 *    Name of palette
 * @param newPaletteName
 *    New name of palette
 * @param paletteXML
 *    XML for palette with new name
 * @param errorMessageOut
 *    Output with error message if renaming palette fails.
 * @return True if palette was renamed, else false (palette with name does not exist)
 */
bool
CaretPreferences::paletteUserCustomRename(const AString& paletteName,
                                          const AString& newPaletteName,
                                          const AString& paletteXML,
                                          AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    errorMessageOut.clear();
    
    if (paletteName.trimmed().isEmpty()) {
        errorMessageOut = ("Attempting to replace palette but old name is empty");
        return false;
    }
    if (newPaletteName.trimmed().isEmpty()) {
        errorMessageOut = ("Attempting to replace palette but new name is empty.");
        return false;
    }

    if (paletteXML.trimmed().isEmpty()) {
        errorMessageOut = ("Unable to rename palette from \""
                           + paletteName
                           + "\" to \""
                           + newPaletteName
                           + ".  Palette XML is empty.");
        return false;
    }
    
    if ( ! paletteUserCustomExists(newPaletteName)) {
        errorMessageOut = ("Unable to rename palette from \""
                           + paletteName
                           + "\" to \""
                           + newPaletteName
                           + ".  Palette with new name exists and palette names "
                           "must be unique.");
        return false;
    }
    
    this->qSettings->remove(getPaletteKey(paletteXML));
    
    this->qSettings->setValue(getPaletteKey(newPaletteName),
                              paletteXML);
    
    return true;
}

