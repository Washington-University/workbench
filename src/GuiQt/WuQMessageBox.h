
#ifndef __WU_QMESSAGE_BOX_H__
#define __WU_QMESSAGE_BOX_H__

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

#include <map>

#include <QMessageBox>

class QCheckBox;

namespace caret {
    class CaretMappableDataFile;
    
    class WuQMessageBox : public QMessageBox {
        
        Q_OBJECT
        
    public:
        enum YesNoCancelResult {
            RESULT_YES,
            RESULT_NO,
            RESULT_CANCEL
        };
        
        enum class YesToAllYesNoResult {
            YES_TO_ALL,
            YES,
            NO
        };
        
        enum class DefaultButtonOkCancel {
            OK,
            CANCEL
        };
        
        static void errorOk(QWidget* parent,
                                  const QString& text);
        
        static void informationOk(QWidget* parent,
                                  const QString& text);
        
        static int32_t informationTwoButtons(QWidget* parent,
                                          const QString& text,
                                          const QString& buttonOneText,
                                          const QString& buttonTwoText);
        
        static void warningOk(QWidget* parent,
                              const QString& text);
        
        static void warningOkWithDoNotShowAgain(QWidget* parent,
                                                const QString& uniqueIdentifier,
                                                const QString& text);
        
        static bool warningOkCancel(QWidget* parent,
                                    const QString& text);
        
        static bool warningYesNoWithDoNotShowAgain(QWidget* parent,
                                                   const QString& uniqueIdentifier,
                                                   const QString& text);
        
        static bool warningYesNo(QWidget* parent,
                                 const QString& text);
        
        static bool warningOkCancel(QWidget* parent,
                                    const QString& text,
                                    const QString& informativeText,
                                    const DefaultButtonOkCancel defaultButton = DefaultButtonOkCancel::OK);
        
        static bool warningYesNoWithDoNotShowAgain(QWidget* parent,
                                                      const QString& uniqueIdentifier,
                                                      const QString& text,
                                                      const QString& informativeText);
        
        static bool warningYesNo(QWidget* parent,
                                    const QString& text,
                                    const QString& informativeText);
        
        static YesNoCancelResult warningYesNoCancel(QWidget* parent,
                                                    const QString& text,
                                                    const QString& informativeText);
        
        static bool warningCloseCancel(QWidget* parent,
                                       const QString& text,
                                       const QString& informativeText);
        
        static QMessageBox::StandardButton saveDiscardCancel(QWidget* parent,
                                                             const QString& text,
                                                             const QString& informativeText);
        
        static YesToAllYesNoResult warningYesToAllYesNo(QWidget* parent,
                                                                const QString& yesToAllButtonText,
                                                                const QString& yesButtonText,
                                                                const QString& noButtonText,
                                                                const QString& text,
                                                                const QString& informativeText);
        static bool warningAcceptReject(QWidget* parent,
                                        const QString& text,
                                        const QString& acceptButtonText,
                                        const QString& rejectButtonText);
        
        
        static bool warningLargeFileSizeOkCancel(QWidget* parent,
                                                 const CaretMappableDataFile* caretMappableDataFile);
        
    private:
        WuQMessageBox(QWidget* parent = 0);
        ~WuQMessageBox();
        
        WuQMessageBox(const WuQMessageBox&);
        WuQMessageBox& operator=(const WuQMessageBox&);
        
        static QCheckBox* addDoNotShowAgainCheckBox(QMessageBox& messageBox);
        
        static void updateButtonText(QMessageBox& messageBox,
                                     const QMessageBox::StandardButton standardButton,
                                     const QString& text);
        
        /*
         * Maintains "do not show again selections.
         * Key is unique identifier supplied by caller to a dialog.
         * Value is the button that was clicked.
         */
        static std::map<QString, QMessageBox::StandardButton> s_doNotShowAgainButtonSelection;
        
    };

#ifdef __WU_QMESSAGE_DEFINE__
    std::map<QString, QMessageBox::StandardButton> WuQMessageBox::s_doNotShowAgainButtonSelection;
#endif // __WU_QMESSAGE_DEFINE__

}


#endif // __WU_QMESSAGE_DIALOG_H__
