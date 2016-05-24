#ifndef __BALSA_DATABASE_DIALOG_H__
#define __BALSA_DATABASE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


#include <QWizard>

#include "CaretPointer.h"

class QLineEdit;
class QWizardPage;

namespace caret {

    class BalsaDatabaseDialogSharedData;
    class BalsaDatabaseLoginPage;
    class BalsaDatabaseManager;
    class BalsaDatabaseTestingPage;
    class SceneFile;
    
    /*
     * BALSA Wizard Dialog
     */
    class BalsaDatabaseDialog : public QWizard {
        
        Q_OBJECT

    public:
        BalsaDatabaseDialog(const SceneFile* sceneFile,
                            QWidget* parent);
        
        virtual ~BalsaDatabaseDialog();
        

        // ADD_NEW_METHODS_HERE

    private:
        BalsaDatabaseDialog(const BalsaDatabaseDialog&);

        BalsaDatabaseDialog& operator=(const BalsaDatabaseDialog&);
        
        CaretPointer<BalsaDatabaseDialogSharedData> m_dialogData;
        
        BalsaDatabaseLoginPage* m_pageLogin;
        
        BalsaDatabaseTestingPage* m_pageCreateZipFile;
        
        // ADD_NEW_MEMBERS_HERE

        friend class BalsaDatabaseLoginPage;
        friend class BalsaDatabaseTestingPage;
    };
    
    /*
     * Data shared by dialog and its pages
     */
    class BalsaDatabaseDialogSharedData {
    public:
        BalsaDatabaseDialogSharedData(const SceneFile* sceneFile);
        
        const SceneFile* m_sceneFile;
        
        CaretPointer<BalsaDatabaseManager> m_balsaDatabaseManager;
        
    private:
        BalsaDatabaseDialogSharedData(const BalsaDatabaseDialogSharedData&);
        
        BalsaDatabaseDialogSharedData& operator=(const BalsaDatabaseDialogSharedData&);
    };
    
    /*
     * BALSA login page
     */
    class BalsaDatabaseLoginPage : public QWizardPage {
        Q_OBJECT
        
    public:
        BalsaDatabaseLoginPage(BalsaDatabaseDialogSharedData* dialogData);
        
        virtual ~BalsaDatabaseLoginPage();
        
        virtual bool isComplete() const;
        
        virtual bool validatePage();
        
    private slots:
        void labelHtmlLinkClicked(const QString&);
        
    private:
        BalsaDatabaseDialogSharedData* m_dialogData;
        
        QLineEdit* m_usernameLineEdit;
        
        QLineEdit* m_passwordLineEdit;
        
    };
    
    /*
     * Balsa Testing (create zip) page
     */
    class BalsaDatabaseTestingPage : public QWizardPage {
        Q_OBJECT
        
    public:
        BalsaDatabaseTestingPage(BalsaDatabaseDialogSharedData* dialogData);
        
        virtual ~BalsaDatabaseTestingPage();
        
        virtual bool isComplete() const;
        
    private slots:
        void runZipSceneFile();
        
    private:
        BalsaDatabaseDialogSharedData* m_dialogData;
        
        QLineEdit* m_testingZipFileNameLineEdit;
        
        QLineEdit* m_testingExtractDirectoryNameLineEdit;
        
    };
    
#ifdef __BALSA_DATABASE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_DATABASE_DIALOG_DECLARE__

} // namespace
#endif  //__BALSA_DATABASE_DIALOG_H__
