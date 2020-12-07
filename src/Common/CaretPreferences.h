#ifndef __CARET_PREFERENCES__H_
#define __CARET_PREFERENCES__H_

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

#include <memory>
#include <set>
#include <utility>

#include "BackgroundAndForegroundColors.h"
#include "BackgroundAndForegroundColorsModeEnum.h"
#include "CaretObject.h"
#include "FileOpenFromOpSysTypeEnum.h"
#include "IdentificationDisplayModeEnum.h"
#include "ImageCaptureMethodEnum.h"
#include "LogLevelEnum.h"
#include "OpenGLDrawingMethodEnum.h"
#include "RecentFilesSystemAccessModeEnum.h"
#include "SpecFileDialogViewFilesTypeEnum.h"
#include "ToolBarWidthModeEnum.h"
#include "VolumeSliceViewAllPlanesLayoutEnum.h"

class QSettings;
class QStringList;

namespace caret {

    class CaretPreferenceDataValue;
    class ModelTransform;
    class RecentFileItemsContainer;
    class TileTabsLayoutBaseConfiguration;
    class WuQMacroGroup;
    
    class CaretPreferences : public CaretObject {
        
    public:
        CaretPreferences();
        
        virtual ~CaretPreferences();

        const BackgroundAndForegroundColors* getBackgroundAndForegroundColors() const;
        
        BackgroundAndForegroundColors getUserBackgroundAndForegroundColors();
        
        void setUserBackgroundAndForegroundColors(const BackgroundAndForegroundColors& colors);
        
        BackgroundAndForegroundColorsModeEnum::Enum getBackgroundAndForegroundColorsMode() const;
        
        void setSceneBackgroundAndForegroundColors(const BackgroundAndForegroundColors& colors);
        
        void setBackgroundAndForegroundColorsMode(const BackgroundAndForegroundColorsModeEnum::Enum colorsMode);
                
        void addToRecentFilesAndOrDirectories(const AString& directoryOrFileName);
        
        bool readRecentSceneAndSpecFiles(RecentFileItemsContainer* container,
                                         AString& errorMessageOut);
        
        bool writeRecentSceneAndSpecFiles(RecentFileItemsContainer* container,
                                          AString& errorMessageOut);
        
        bool readRecentDirectories(RecentFileItemsContainer* container,
                                   AString& errorMessageOut);
        
        bool writeRecentDirectories(RecentFileItemsContainer* container,
                                    AString& errorMessageOut);
        
        void getRecentDirectoriesForOpenFileDialogHistory(const bool favoritesFirstFlag,
                                                          std::vector<AString>& directoriesOut);
        
        int32_t getRecentMaximumNumberOfSceneAndSpecFiles() const;
        
        void setRecentMaximumNumberOfSceneAndSpecFiles(const int32_t maximumNumberOfFiles);
        
        void clearRecentSceneAndSpecFiles(const bool removeFavoritesFlag);
        
        int32_t getRecentMaximumNumberOfDirectories() const;
        
        void setRecentMaximumNumberOfDirectories(const int32_t maximumNumberOfDirectories);
        
        void clearRecentDirectories(const bool removeFavoritesFlag);

        RecentFilesSystemAccessModeEnum::Enum getRecentFilesSystemAccessMode() const;

        void setRecentFilesSystemAccessMode(const RecentFilesSystemAccessModeEnum::Enum filesSystemAccessMode);
        
        void readRecentFilesExclusionPaths(std::set<AString>& exclusionPathsOut);
        
        void writeRecentFilesExclusionPaths(const std::set<AString>& exclusionPaths);
        
        void addToRecentFilesExclusionPaths(const AString& exclusionPath);
        
        void removeFromRecentFilesExclusionPaths(const AString& exclusionPath);
        
        LogLevelEnum::Enum getLoggingLevel() const;
        
        void setLoggingLevel(const LogLevelEnum::Enum loggingLevel);
        
        ImageCaptureMethodEnum::Enum getImageCaptureMethod() const;
        
        void setImageCaptureMethod(const ImageCaptureMethodEnum::Enum imageCaptureMethod);
        
        OpenGLDrawingMethodEnum::Enum getOpenDrawingMethod() const;
        
        void setOpenGLDrawingMethod(const OpenGLDrawingMethodEnum::Enum openGLDrawingMethod);
        
        ToolBarWidthModeEnum::Enum getToolBarWidthMode() const;
        
        void setToolBarWidthMode(const ToolBarWidthModeEnum::Enum widthMode);
        
        FileOpenFromOpSysTypeEnum::Enum getFileOpenFromOpSysType() const;
        
        void setFileOpenFromOpSysType(const FileOpenFromOpSysTypeEnum::Enum openType);
        
        IdentificationDisplayModeEnum::Enum getIdentificationDisplayMode() const;
        
        void setIdentificationDisplayMode(const IdentificationDisplayModeEnum::Enum identificationDisplayMode);
        
        VolumeSliceViewAllPlanesLayoutEnum::Enum getVolumeAllSlicePlanesLayout() const;
        
        void setVolumeAllSlicePlanesLayout(const VolumeSliceViewAllPlanesLayoutEnum::Enum allViewLayout);
        
        float getVolumeCrosshairGap() const;
        
        void setVolumeCrosshairGap(const float gap);
        
        bool isVolumeAxesCrosshairsDisplayed() const;
        
        void setVolumeAxesCrosshairsDisplayed(const bool displayed);
        
        bool isVolumeAxesLabelsDisplayed() const;
        
        void setVolumeAxesLabelsDisplayed(const bool displayed);
        
        bool isVolumeMontageAxesCoordinatesDisplayed() const;
        
        void setVolumeMontageAxesCoordinatesDisplayed(const bool displayed);
        
        int32_t getVolumeMontageGap() const;
        
        void setVolumeMontageGap(const int32_t volumeMontageGap);
        
        int32_t getVolumeMontageCoordinatePrecision() const;
        
        void setVolumeMontageCoordinatePrecision(const int32_t volumeMontageCoordinatePrecision);
        
        void setAnimationStartTime(const double &time);
        
        void getAnimationStartTime(double &time);
        
        bool isSplashScreenEnabled() const;
        
        void setSplashScreenEnabled(const bool enabled);
        
        bool isDevelopMenuEnabled() const;
        
        void setDevelopMenuEnabled(const bool enabled);
        
        bool isShowDataToolTipsEnabled() const;
        
        void setShowDataToolTipsEnabled(const bool enabled);
        
        void readTileTabsUserConfigurations(const bool performSync = true);
        
        std::vector<std::pair<AString, AString>> getTileTabsUserConfigurationsNamesAndUniqueIdentifiers(const bool includeManualConfigurationsFlag) const;
        
        std::unique_ptr<TileTabsLayoutBaseConfiguration> getCopyOfTileTabsUserConfigurationByUniqueIdentifier(const AString& uniqueIdentifier) const;
        
        TileTabsLayoutBaseConfiguration* getTileTabsUserConfigurationByName(const AString& name) const;
        
        void addTileTabsUserConfiguration(const TileTabsLayoutBaseConfiguration* tileTabsConfiguration,
                                          const AString& configurationName);
        
        bool replaceTileTabsUserConfiguration(const AString& replaceUserTileTabsUniqueIdentifier,
                                              const TileTabsLayoutBaseConfiguration* replaceWithConfiguration,
                                              AString& errorMessageOut);
        
        bool renameTileTabsUserConfiguration(const QString& tileTabsUniqueIdentifier,
                                             const AString& name,
                                             AString& errorMessageOut);
        
        bool removeTileTabsUserConfigurationByUniqueIdentifier(const AString& tileTabsUniqueIdentifier,
                                                               AString& errorMessageOut);
        
        void writeTileTabsUserConfigurations();
        
        void readCustomViews(const bool performSync = true);
        
        std::vector<AString> getCustomViewNames() const;
        
        std::vector<std::pair<AString,AString> > getCustomViewNamesAndComments() const;
        
        bool getCustomView(const AString& customViewName,
                           ModelTransform& modelTransformOut) const;
        
        void addOrReplaceCustomView(const ModelTransform& modelTransform);
        
        void removeCustomView(const AString& customViewName);
        
        bool isRemoteFilePasswordSaved();
        
        void setRemoteFilePasswordSaved(const bool saveRemotePasswordToPreferences);

        void getRemoteFileUserNameAndPassword(AString& userNameOut,
                                              AString& passwordOut) const;
        
        void setRemoteFileUserNameAndPassword(const AString& userName,
                                              const AString& password);
        
        AString getBalsaUserName() const;
        
        void setBalsaUserName(const AString& userName);
        
        static void byteRgbToFloatRgb(const uint8_t byteRGB[3],
                                      float floatRGB[3]);

        bool isYokingDefaultedOn() const;
        
        void setYokingDefaultedOn(const bool status);
        
        bool isVolumeIdentificationDefaultedOn() const;
        
        void setVolumeIdentificationDefaultedOn(const bool status);
        
        SpecFileDialogViewFilesTypeEnum::Enum getManageFilesViewFileType() const;
        
        void setManageFilesViewFileType(const SpecFileDialogViewFilesTypeEnum::Enum manageFilesViewFileType);
        
        bool isShowSurfaceIdentificationSymbols() const;
        
        void setShowSurfaceIdentificationSymbols(const bool showSymbols);
        
        bool isShowVolumeIdentificationSymbols() const;
        
        void setShowVolumeIdentificationSymbols(const bool showSymbols);
        
        bool isDynamicConnectivityDefaultedOn() const;
        
        void setDynamicConnectivityDefaultedOn(const bool defaultedOn);
        
        bool isGuiGesturesEnabled() const;
        
        void setGuiGesturesEnabled(const bool status);
        
        WuQMacroGroup* getMacros();
        
        const WuQMacroGroup* getMacros() const;
        
        void readMacros(const bool performSync = true);
        
        void writeMacros();
        
        void invalidateSceneDataValues();
        
        std::vector<CaretPreferenceDataValue*> getPreferenceSceneDataValues();

        bool paletteUserCustomExists(const AString& paletteName);
        
        void paletteUserCustomGetAll(std::vector<AString>& palettesXmlOut);
        
        bool paletteUserCustomGetByName(const AString& paletteName,
                                        AString& paletteXmlOut);
        
        bool paletteUserCustomAdd(const AString& paletteName,
                                  const AString& paletteXML,
                                  AString& errorMessageOut);
        
        bool paletteUserCustomReplace(const AString& paletteName,
                                      const AString& paletteXML,
                                      AString& errorMessageOut);
        
        bool paletteUserCustomRemove(const AString& paletteName,
                                     AString& errorMessageOut);
        
        bool paletteUserCustomRename(const AString& paletteName,
                                     const AString& newPaletteName,
                                     const AString& paletteXML,
                                     AString& errorMessageOut);

    private:
        CaretPreferences(const CaretPreferences&);

        CaretPreferences& operator=(const CaretPreferences&);
        
    public:
        virtual AString toString() const;
        
    private:
        void addToRecentSceneAndSpecFiles(const AString& filename);
        
        void addToRecentDirectories(const AString& directoryOrFileName);
        
        bool getBoolean(const AString& name,
                        const bool defaultValue = false);
        
        void setBoolean(const AString& name,
                        const bool value);
        
        int getInteger(const AString& name,
                        const int defaultValue = false);
        
        void setInteger(const AString& name,
                        const int value);
        
        AString getString(const AString& name,
                          const AString& defaultValue = "");
        
        void setString(const AString& name,
                       const AString& value);
        
        void readUnsignedByteArray(const AString& name,
                                   uint8_t array[],
                                   const int32_t numberOfElements);
        
        void writeUnsignedByteArray(const AString& name,
                                    const uint8_t array[],
                                    const int32_t numberOfElements);
        
        void readPreferences();
        
        void removeAllTileTabsConfigurations();
        
        void removeAllCustomViews();
        
        void writeCustomViews();
        
        AString getPaletteKey(const AString& paletteName) const;
        
        bool readRecentFileItemsContainer(const AString& preferenceName,
                                          RecentFileItemsContainer* container,
                                          AString& errorMessageOut);

        bool writeRecentFileItemsContainer(const AString& preferenceName,
                                           const RecentFileItemsContainer* container,
                                           AString& errorMessageOut);

        bool isInRecentFilesExclusionPaths(const AString& fileOrDirectoryName);
        
        mutable QSettings* qSettings;
        
        BackgroundAndForegroundColors userColors;
        
        BackgroundAndForegroundColors sceneColors;
        
        /** NOTE: colors mode is NOT saved to preferences */
        BackgroundAndForegroundColorsModeEnum::Enum m_colorsMode;
        
        std::vector<AString> previousSpecFiles;
        
        std::vector<AString> previousSceneFiles;
        
        LogLevelEnum::Enum loggingLevel;
        
        ImageCaptureMethodEnum::Enum imageCaptureMethod;
        
        OpenGLDrawingMethodEnum::Enum openGLDrawingMethod;
        
        std::vector<ModelTransform*> customViews;

        std::vector<TileTabsLayoutBaseConfiguration*> tileTabsConfigurations;
        
        bool displayVolumeAxesCrosshairs;
        
        bool displayVolumeAxesLabels;
        
        bool displayVolumeAxesCoordinates;
        
        int32_t volumeMontageGap;
        
        int32_t volumeMontageCoordinatePrecision;
        
        std::unique_ptr<CaretPreferenceDataValue> m_volumeAllSlicePlanesLayout;
        
        std::unique_ptr<CaretPreferenceDataValue> m_volumeCrossHairGapPreference;

        std::unique_ptr<CaretPreferenceDataValue> m_guiGesturesEnabled;
        
        std::unique_ptr<CaretPreferenceDataValue> m_toolBarWidthModePreference;
        
        std::unique_ptr<CaretPreferenceDataValue> m_identificationDisplayModePreference;
        
        std::unique_ptr<CaretPreferenceDataValue> m_fileOpenFromOperatingSystemTypePreference;
        
        std::vector<CaretPreferenceDataValue*> m_preferenceStoredInSceneDataValues;
        
        std::unique_ptr<CaretPreferenceDataValue> m_recentMaximumNumberOfSceneAndSpecFilesPreference;
        
        std::unique_ptr<CaretPreferenceDataValue> m_recentMaximumNumberOfDirectoriesPreferences;

        std::unique_ptr<CaretPreferenceDataValue> m_recentFilesSystemAccessMode;
        
        bool splashScreenEnabled;
        
        bool developMenuEnabled;
        
        double animationStartTime;
        
        bool volumeIdentificationDefaultedOn;
        
        bool showSurfaceIdentificationSymbols;
        
        bool showVolumeIdentificationSymbols;
        
        bool dynamicConnectivityDefaultedOn;
        
        bool yokingDefaultedOn;
        
        bool dataToolTipsEnabled;
        
        AString remoteFileUserName;
        AString remoteFilePassword;
        bool remoteFileLoginSaved;
        
        AString balsaUserName;
        
        SpecFileDialogViewFilesTypeEnum::Enum manageFilesViewFileType;
        
        std::unique_ptr<WuQMacroGroup> m_macros;
        
        static const AString NAME_ANIMATION_START_TIME;
        static const AString NAME_BALSA_USER_NAME;
        static const AString NAME_VOLUME_AXES_CROSSHAIRS;
        static const AString NAME_VOLUME_AXES_LABELS;
        static const AString NAME_VOLUME_AXES_COORDINATE;
        static const AString NAME_VOLUME_MONTAGE_GAP;
        static const AString NAME_VOLUME_MONTAGE_COORDINATE_PRECISION;
        static const AString NAME_COLOR_BACKGROUND;
        static const AString NAME_COLOR_FOREGROUND;
        static const AString NAME_COLOR_BACKGROUND_WINDOW;
        static const AString NAME_COLOR_FOREGROUND_WINDOW;
        static const AString NAME_COLOR_BACKGROUND_ALL;
        static const AString NAME_COLOR_FOREGROUND_ALL;
        static const AString NAME_COLOR_BACKGROUND_CHART;
        static const AString NAME_COLOR_FOREGROUND_CHART;
        static const AString NAME_COLOR_BACKGROUND_MEDIA;
        static const AString NAME_COLOR_FOREGROUND_MEDIA;
        static const AString NAME_COLOR_BACKGROUND_SURFACE;
        static const AString NAME_COLOR_FOREGROUND_SURFACE;
        static const AString NAME_COLOR_BACKGROUND_VOLUME;
        static const AString NAME_COLOR_FOREGROUND_VOLUME;
        static const AString NAME_COLOR_CHART_MATRIX_GRID_LINES;
        static const AString NAME_COLOR_CHART_HISTOGRAM_THRESHOLD;
        static const AString NAME_CUSTOM_VIEWS;
        static const AString NAME_DEVELOP_MENU;
        static const AString NAME_DATA_TOOL_TIPS;
        static const AString NAME_DYNAMIC_CONNECTIVITY_ON;
        static const AString NAME_IMAGE_CAPTURE_METHOD;
        static const AString NAME_LOGGING_LEVEL;
        static const AString NAME_MACROS;
        static const AString NAME_MANAGE_FILES_VIEW_FILE_TYPE;
        static const AString NAME_OPENGL_DRAWING_METHOD;
        static const AString NAME_PALETTE_GROUP_KEY;
        static const AString NAME_PREVIOUS_SCENE_FILES;
        static const AString NAME_PREVIOUS_SPEC_FILES;
        static const AString NAME_PREVIOUS_OPEN_FILE_DIRECTORIES;
        static const AString NAME_SPLASH_SCREEN;
        static const AString NAME_RECENT_DIRECTORIES;
        static const AString NAME_RECENT_EXCLUSION_PATHS;
        static const AString NAME_RECENT_SCENE_AND_SPEC_FILES;
        static const AString NAME_REMOTE_FILE_USER_NAME;
        static const AString NAME_REMOTE_FILE_PASSWORD;
        static const AString NAME_REMOTE_FILE_LOGIN_SAVED;
        static const AString NAME_SHOW_SURFACE_IDENTIFICATION_SYMBOLS;
        static const AString NAME_SHOW_VOLUME_IDENTIFICATION_SYMBOLS;
        static const AString NAME_TILE_TABS_CONFIGURATIONS;
        static const AString NAME_TILE_TABS_CONFIGURATIONS_TWO;
        static const AString NAME_VOLUME_IDENTIFICATION_DEFAULTED_ON;
        static const AString NAME_YOKING_DEFAULT_ON;
        
    };
    
#ifdef __CARET_PREFERENCES_DECLARE__
    const AString CaretPreferences::NAME_ANIMATION_START_TIME = "animationStartTime";
    const AString CaretPreferences::NAME_BALSA_USER_NAME = "balsaUserName";
    const AString CaretPreferences::NAME_VOLUME_AXES_CROSSHAIRS = "volumeAxesCrosshairs";
    const AString CaretPreferences::NAME_VOLUME_AXES_LABELS     = "volumeAxesLabels";
    const AString CaretPreferences::NAME_VOLUME_AXES_COORDINATE     = "volumeAxesCoordinates";
    const AString CaretPreferences::NAME_VOLUME_MONTAGE_GAP     = "volumeMontageGap";
    const AString CaretPreferences::NAME_VOLUME_MONTAGE_COORDINATE_PRECISION     = "volumeMontageCoordinatePrecision";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND     = "colorBackground";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND     = "colorForeground";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND_WINDOW  = "colorBackgroundWindow";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND_WINDOW  = "colorForegroundWindow";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND_ALL     = "colorBackgroundAll";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND_ALL     = "colorForegroundAll";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND_CHART     = "colorBackgroundChart";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND_CHART     = "colorForegroundChart";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND_MEDIA     = "colorBackgroundMedia";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND_MEDIA     = "colorForegroundMedia";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND_SURFACE     = "colorBackgroundSurface";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND_SURFACE     = "colorForegroundSurface";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND_VOLUME     = "colorBackgroundVolume";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND_VOLUME     = "colorForegroundVolume";
    const AString CaretPreferences::NAME_COLOR_CHART_MATRIX_GRID_LINES = "colorChartMatrixGridLines";
    const AString CaretPreferences::NAME_COLOR_CHART_HISTOGRAM_THRESHOLD = "colorChartHistogramThreshold";
    const AString CaretPreferences::NAME_CUSTOM_VIEWS     = "customViews";
    const AString CaretPreferences::NAME_DEVELOP_MENU     = "developMenu";
    const AString CaretPreferences::NAME_DATA_TOOL_TIPS = "dataToolTips";
    const AString CaretPreferences::NAME_DYNAMIC_CONNECTIVITY_ON = "dynamicConnectivityDefaultedOn";
    const AString CaretPreferences::NAME_IMAGE_CAPTURE_METHOD = "imageCaptureMethod";
    const AString CaretPreferences::NAME_LOGGING_LEVEL     = "loggingLevel";
    const AString CaretPreferences::NAME_MACROS = "macros";
    const AString CaretPreferences::NAME_MANAGE_FILES_VIEW_FILE_TYPE     = "manageFilesViewFileType";
    const AString CaretPreferences::NAME_OPENGL_DRAWING_METHOD     = "openGLDrawingMethod";
    const AString CaretPreferences::NAME_PALETTE_GROUP_KEY = "palette";
    const AString CaretPreferences::NAME_PREVIOUS_SCENE_FILES     = "previousSceneFiles";
    const AString CaretPreferences::NAME_PREVIOUS_SPEC_FILES     = "previousSpecFiles";
    const AString CaretPreferences::NAME_PREVIOUS_OPEN_FILE_DIRECTORIES     = "previousOpenFileDirectories";
    const AString CaretPreferences::NAME_SPLASH_SCREEN = "splashScreen";
    const AString CaretPreferences::NAME_RECENT_DIRECTORIES = "recentDirectories";
    const AString CaretPreferences::NAME_RECENT_EXCLUSION_PATHS = "recentExclusionPaths";
    const AString CaretPreferences::NAME_RECENT_SCENE_AND_SPEC_FILES = "recentSceneAndSpecFiles";
    const AString CaretPreferences::NAME_REMOTE_FILE_USER_NAME = "remoteFileUserName";
    const AString CaretPreferences::NAME_REMOTE_FILE_PASSWORD = "remoteFilePassword";
    const AString CaretPreferences::NAME_REMOTE_FILE_LOGIN_SAVED = "removeFileLoginSaved";
    const AString CaretPreferences::NAME_SHOW_SURFACE_IDENTIFICATION_SYMBOLS = "showSurfaceIdentificationSymbols";
    const AString CaretPreferences::NAME_SHOW_VOLUME_IDENTIFICATION_SYMBOLS = "showVolumeIdentificationSymbols";
    const AString CaretPreferences::NAME_TILE_TABS_CONFIGURATIONS = "tileTabsConfigurations";
    const AString CaretPreferences::NAME_TILE_TABS_CONFIGURATIONS_TWO = "tileTabsConfigurationsTwo";
    const AString CaretPreferences::NAME_VOLUME_IDENTIFICATION_DEFAULTED_ON = "volumeIdentificationDefaultedOn";
    const AString CaretPreferences::NAME_YOKING_DEFAULT_ON = "yokingDefaultedOn";
#endif // __CARET_PREFERENCES_DECLARE__

} // namespace
#endif  //__CARET_PREFERENCES__H_
