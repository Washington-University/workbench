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

#include "AString.h"
#include "CaretPointer.h"

class QLineEdit;
class QWizardPage;

namespace caret {

    class BalsaDatabaseCreateZipFilePage;
    class BalsaDatabaseDialogSharedData;
    class BalsaDatabaseLoginPage;
    class BalsaDatabaseManager;
    class BalsaDatabaseUploadPage;
    class ProgressReportingBar;
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
        
        BalsaDatabaseCreateZipFilePage* m_pageCreateZipFile;
        
        BalsaDatabaseUploadPage* m_pageUpload;
        
        // ADD_NEW_MEMBERS_HERE

        friend class BalsaDatabaseCreateZipFilePage;
        friend class BalsaDatabaseLoginPage;
        friend class BalsaDatabaseUploadPage;
    };
    
    /*
     * Data shared by dialog and its pages
     */
    class BalsaDatabaseDialogSharedData {
    public:
        BalsaDatabaseDialogSharedData(const SceneFile* sceneFile);
        
        const SceneFile* m_sceneFile;
        
        CaretPointer<BalsaDatabaseManager> m_balsaDatabaseManager;
        
        AString m_zipFileName;
        
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
     * Create ZIP file page
     */
    class BalsaDatabaseCreateZipFilePage : public QWizardPage {
        Q_OBJECT
        
    public:
        BalsaDatabaseCreateZipFilePage(BalsaDatabaseDialogSharedData* dialogData);
        
        virtual ~BalsaDatabaseCreateZipFilePage();
        
        virtual bool isComplete() const;
        
        virtual void initializePage();
        
        virtual bool validatePage();
        
    private:
        bool runZipSceneFile();
        
        BalsaDatabaseDialogSharedData* m_dialogData;
        
        QLineEdit* m_zipFileNameLineEdit;
        
        QLineEdit* m_extractDirectoryNameLineEdit;
        
        ProgressReportingBar* m_progressReportingBar;
    };
    
    /*
     * Balsa Upload Page
     */
    class BalsaDatabaseUploadPage : public QWizardPage {
        Q_OBJECT
        
    public:
        BalsaDatabaseUploadPage(BalsaDatabaseDialogSharedData* dialogData);
        
        virtual ~BalsaDatabaseUploadPage();
        
        virtual bool isComplete() const;
        
        virtual void initializePage();
        
        virtual bool validatePage();
        
    private:
        bool uploadZipFile();
        
        BalsaDatabaseDialogSharedData* m_dialogData;
        
        QLineEdit* m_zipFileNameLineEdit;
        
        ProgressReportingBar* m_progressReportingBar;
    };

    
#ifdef __BALSA_DATABASE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_DATABASE_DIALOG_DECLARE__

} // namespace
#endif  //__BALSA_DATABASE_DIALOG_H__
