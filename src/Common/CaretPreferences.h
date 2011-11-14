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

namespace caret {

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
        
        void getPreviousOpenFileDirectories(std::vector<AString>& previousOpenFileDirectories) const;
        
        void addToPreviousOpenFileDirectories(const AString& directoryName);
        
        LogLevelEnum::Enum getLoggingLevel() const;
        
        void setLoggingLevel(const LogLevelEnum::Enum loggingLevel);
        
    private:
        CaretPreferences(const CaretPreferences&);

        CaretPreferences& operator=(const CaretPreferences&);
        
    public:
        virtual AString toString() const;
        
    private:
        void addToPrevious(std::vector<AString>& previousVector,
                           const AString& newName);
        
        mutable QSettings* qSettings;
    };
    
#ifdef __CARET_PREFERENCES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_PREFERENCES_DECLARE__

} // namespace
#endif  //__CARET_PREFERENCES__H_
