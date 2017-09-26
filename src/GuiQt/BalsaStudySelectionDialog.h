#ifndef __BALSA_STUDY_SELECTION_DIALOG_H__
#define __BALSA_STUDY_SELECTION_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include "BalsaStudyInformation.h"
#include "WuQDialogModal.h"

class QPushButton;
class QTableWidget;


namespace caret {
    class BalsaDatabaseManager;
    
    class BalsaStudySelectionDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        BalsaStudySelectionDialog(const std::vector<BalsaStudyInformation>& studyInformation,
                                  const AString selectedStudyName,
                                  BalsaDatabaseManager* balsaDatabaseManager,
                                  const AString& databaseURL,
                                  const AString& balsaUsername,
                                  const AString& balsaPassword,
                                  QWidget* parent);
        
        virtual ~BalsaStudySelectionDialog();

        virtual void okButtonClicked() override;

        BalsaStudyInformation getSelectedStudyInformation() const;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void newStudyButtonClicked();
        
        void editSelectedStudyButtonClicked();
        
    private:
        const int32_t COLUMN_TITLE = 0;
        
        const int32_t COLUMN_ID = 1;
        
        const int32_t COLUMN_COUNT = 2;
        
        BalsaStudySelectionDialog(const BalsaStudySelectionDialog&);

        BalsaStudySelectionDialog& operator=(const BalsaStudySelectionDialog&);
        
        void loadStudyTitleTableWidget(const AString selectedStudyName);
        
        std::vector<BalsaStudyInformation> m_studyInformation;
        
        BalsaDatabaseManager* m_balsaDatabaseManager;
        
        const AString& m_databaseURL;
        
        const AString& m_balsaUsername;
        
        const AString& m_balsaPassword;
        
        QTableWidget* m_studyTableWidget;
        
        QPushButton* m_editSelectedStudyPushButton;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BALSA_STUDY_SELECTION_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_STUDY_SELECTION_DIALOG_DECLARE__

} // namespace
#endif  //__BALSA_STUDY_SELECTION_DIALOG_H__
