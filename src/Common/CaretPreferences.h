#ifndef __CARET_PREFERENCES__H_
#define __CARET_PREFERENCES__H_

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

#include "CaretObject.h"
#include "LogLevelEnum.h"

class QSettings;
class QStringList;

namespace caret {

    class UserView;
    
    class CaretPreferences : public CaretObject {
        
    public:
        CaretPreferences();
        
        virtual ~CaretPreferences();
        
        void getColorForeground(uint8_t colorForeground[3]) const;

        void getColorForeground(float colorForeground[3]) const;
        
        void setColorForeground(const uint8_t colorForeground[3]);
        
        void getColorBackground(uint8_t colorBackground[3]) const;
        
        void getColorBackground(float colorBackground[3]) const;
        
        void setColorBackground(const uint8_t colorBackground[3]);
        
        void getPreviousSpecFiles(std::vector<AString>& previousSpecFiles) const;
        
        void addToPreviousSpecFiles(const AString& specFileName);
        
        void clearPreviousSpecFiles();
        
        void getPreviousOpenFileDirectories(std::vector<AString>& previousOpenFileDirectories) const;
        
        void getPreviousOpenFileDirectories(QStringList& previousOpenFileDirectories) const;
        
        void addToPreviousOpenFileDirectories(const AString& directoryName);
        
        LogLevelEnum::Enum getLoggingLevel() const;
        
        void setLoggingLevel(const LogLevelEnum::Enum loggingLevel);
        
        std::vector<const UserView*> getAllUserViews();
        
        const UserView* getUserView(const AString& viewName);
        
        void addUserView(const UserView& userView);
        
        void removeUserView(const AString& viewName);
        
        bool isVolumeAxesCrosshairsDisplayed() const;
        
        void setVolumeAxesCrosshairsDisplayed(const bool displayed);
        
        bool isVolumeAxesLabelsDisplayed() const;
        
        void setVolumeAxesLabelsDisplayed(const bool displayed);
        
        void setAnimationStartTime(const double &time);
        
        void getAnimationStartTime(double &time);
        
        //bool isContralateralIdentificationEnabled() const;
        
        //void setContralateralIdentificationEnabled(const bool enabled);
        
        int32_t getToolBoxType() const;
        
        void setToolBoxType(const int32_t toolBoxType);
        
    private:
        CaretPreferences(const CaretPreferences&);

        CaretPreferences& operator=(const CaretPreferences&);
        
    public:
        virtual AString toString() const;
        
    private:
        bool getBoolean(const AString& name,
                        const bool defaultValue = false);
        
        void setBoolean(const AString& name,
                        const bool value);
        
        int getInteger(const AString& name,
                        const int defaultValue = false);
        
        void setInteger(const AString& name,
                        const int value);
        
        void addToPrevious(std::vector<AString>& previousVector,
                           const AString& newName);
        
        void readPreferences();
        
        void removeAllUserViews();
        
        void writeUserViews();
        
        mutable QSettings* qSettings;
        
        uint8_t colorForeground[3];
        
        uint8_t colorBackground[3];
        
        std::vector<AString> previousSpecFiles;
        
        std::vector<AString> previousOpenFileDirectories;
        
        LogLevelEnum::Enum loggingLevel;
        
        std::vector<UserView*> userViews;

        bool displayVolumeAxesCrosshairs;
        
        bool displayVolumeAxesLabels;
        
        double animationStartTime;
        
        int32_t toolBoxType;
        
        //bool contralateralIdentificationEnabled;
        
        static const AString NAME_AXES_CROSSHAIRS;
        static const AString NAME_AXES_LABELS;
        static const AString NAME_COLOR_BACKGROUND;
        static const AString NAME_COLOR_FOREGROUND;
        static const AString NAME_IDENTIFICATION_CONTRALATERAL;
        static const AString NAME_PREVIOUS_SPEC_FILES;
        static const AString NAME_PREVIOUS_OPEN_FILE_DIRECTORIES;
        static const AString NAME_LOGGING_LEVEL;
        static const AString NAME_USER_VIEWS;
        static const AString NAME_ANIMATION_START_TIME;
        
        static const AString NAME_TOOLBOX_TYPE;
    };
    
#ifdef __CARET_PREFERENCES_DECLARE__
    const AString CaretPreferences::NAME_AXES_CROSSHAIRS = "volumeAxesCrosshairs";
    const AString CaretPreferences::NAME_AXES_LABELS     = "volumeAxesLabels";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND     = "colorBackground";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND     = "colorForeground";
    //const AString CaretPreferences::NAME_IDENTIFICATION_CONTRALATERAL     = "identificationContralateral";
    const AString CaretPreferences::NAME_PREVIOUS_SPEC_FILES     = "previousSpecFiles";
    const AString CaretPreferences::NAME_PREVIOUS_OPEN_FILE_DIRECTORIES     = "previousOpenFileDirectories";
    const AString CaretPreferences::NAME_LOGGING_LEVEL     = "loggingLevel";
    const AString CaretPreferences::NAME_USER_VIEWS     = "userViews";
    const AString CaretPreferences::NAME_ANIMATION_START_TIME = "animationStartTime";
    const AString CaretPreferences::NAME_TOOLBOX_TYPE = "toolBoxType";
#endif // __CARET_PREFERENCES_DECLARE__

} // namespace
#endif  //__CARET_PREFERENCES__H_
