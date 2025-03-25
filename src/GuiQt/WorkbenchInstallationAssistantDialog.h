#ifndef __WORKBENCH_INSTALLATION_ASSISTANT_DIALOG_H__
#define __WORKBENCH_INSTALLATION_ASSISTANT_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include <QDialog>

class QTextEdit;

namespace caret {

    class WorkbenchInstallationAssistantDialog : public QDialog {
        
        Q_OBJECT

    public:
        WorkbenchInstallationAssistantDialog(QWidget* parent = 0);
        
        virtual ~WorkbenchInstallationAssistantDialog();
        
        WorkbenchInstallationAssistantDialog(const WorkbenchInstallationAssistantDialog&) = delete;

        WorkbenchInstallationAssistantDialog& operator=(const WorkbenchInstallationAssistantDialog&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        void loadPathInformation();
        
        QTextEdit* m_pathTextEdit;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_INSTALLATION_ASSISTANT_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WORKBENCH_INSTALLATION_ASSISTANT_DIALOG_DECLARE__

} // namespace
#endif  //__WORKBENCH_INSTALLATION_ASSISTANT_DIALOG_H__
