#ifndef __WB_MACRO_HELPER_H__
#define __WB_MACRO_HELPER_H__

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


#include <memory>

#include "EventListenerInterface.h"
#include "MovieRecorderModeEnum.h"
#include "WuQMacroHelperInterface.h"

class QWidget;

namespace caret {

    class WbMacroHelper : public WuQMacroHelperInterface, public EventListenerInterface {
        Q_OBJECT
        
    public:
        WbMacroHelper(QObject* parent);
        
        virtual ~WbMacroHelper();
        
        WbMacroHelper(const WbMacroHelper&) = delete;

        WbMacroHelper& operator=(const WbMacroHelper&) = delete;
        
        void receiveEvent(Event* event) override;
        
        virtual std::vector<WuQMacroGroup*> getMacroGroups() override;
        
        virtual void macroWasModified(WuQMacro* macro) override;
        
        virtual void macroGroupWasModified(WuQMacroGroup* macroGroup) override;

        virtual std::vector<QString> getMainWindowIdentifiers() override;
        
        virtual QMainWindow* getMainWindowWithIdentifier(const QString& identifier) override;

        virtual void macroExecutionStarting(const WuQMacro* macro,
                                            QWidget* window,
                                            const WuQMacroExecutorOptions* executorOptions) override;
        
        virtual void macroExecutionEnding(const WuQMacro* macro,
                                          QWidget* window,
                                          const WuQMacroExecutorOptions* executorOptions) override;
        
        virtual void macroCommandHasCompleted(QWidget* window,
                                              const WuQMacroCommand* command,
                                              bool& allowDelayFlagOut) override;
        
        virtual void macroCommandAboutToStart(QWidget* window,
                                              const WuQMacroCommand* command,
                                              bool& allowDelayFlagOut) override;
        
        // ADD_NEW_METHODS_HERE
        
    private:
        void
        recordImagesForDelay(QWidget* window,
                             const WuQMacroCommand* command,
                             bool& allowDelayFlagOut) const;

        MovieRecorderModeEnum::Enum m_savedRecordingMode = MovieRecorderModeEnum::MANUAL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WB_MACRO_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WB_MACRO_HELPER_DECLARE__

} // namespace
#endif  //__WB_MACRO_HELPER_H__
