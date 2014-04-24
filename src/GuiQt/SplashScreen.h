#ifndef __SPLASH_SCREEN__H_
#define __SPLASH_SCREEN__H_

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


#include "WuQDialogModal.h"

class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

namespace caret {

    class SplashScreen : public WuQDialogModal {
        Q_OBJECT
        
    public:
        SplashScreen(QWidget* parent = 0);
        
        virtual ~SplashScreen();
        
        AString getSelectedSpecFileName() const;
        
    private slots:
        void websiteLinkActivated(const QString& link);
        
        void specFileTreeWidgetItemClicked(QTreeWidgetItem* item);
        
        void specFileTreeWidgetItemDoubleClicked(QTreeWidgetItem* item);
        
        void chooseSpecFileViaOpenFileDialog();
        
        void twitterActionTriggered();
        
    protected:
        WuQDialogModal::DialogUserButtonResult userButtonPressed(QPushButton* userPushButton);
        
    private:
        SplashScreen(const SplashScreen&);

        SplashScreen& operator=(const SplashScreen&);
        
        QTreeWidgetItem* addRecentSpecFiles();
        
        QTreeWidgetItem* addDirectorySpecFiles();
        
        int32_t loadSpecFileTreeWidget();
        
        QTreeWidget* m_specFileTreeWidget;
        
        AString m_selectedSpecFileName;
        
        QPushButton* m_openOtherSpecFilePushButton;
    };
    
#ifdef __SPLASH_SCREEN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPLASH_SCREEN_DECLARE__

} // namespace
#endif  //__SPLASH_SCREEN__H_
