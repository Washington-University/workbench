#ifndef __ABOUT_WORKBENCH_DIALOG__H_
#define __ABOUT_WORKBENCH_DIALOG__H_

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

namespace caret {

    class BrainOpenGLWidget;
    
    class AboutWorkbenchDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        AboutWorkbenchDialog(BrainOpenGLWidget* openGLParentWidget);
        
        virtual ~AboutWorkbenchDialog();
        
    protected:
        virtual DialogUserButtonResult userButtonPressed(QPushButton* userPushButton);
        
    private slots:
        void websiteLinkActivated(const QString& link);
        
    private:
        AboutWorkbenchDialog(const AboutWorkbenchDialog&);

        AboutWorkbenchDialog& operator=(const AboutWorkbenchDialog&);
        
        void displaySystemInformation();
        
        void displayMoreInformation();
        
        void displayOpenGLInformation();
        
        QPushButton* m_morePushButton;
        
        QPushButton* m_openGLPushButton;
        
        QPushButton* m_systemPushButton;
        
        BrainOpenGLWidget* m_openGLParentWidget;
    };
    
#ifdef __ABOUT_WORKBENCH_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ABOUT_WORKBENCH_DIALOG_DECLARE__

} // namespace
#endif  //__ABOUT_WORKBENCH_DIALOG__H_
