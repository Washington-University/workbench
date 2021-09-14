
#ifndef __WU_Q_DATA_ENTRY_DIALOG_H__
#define __WU_Q_DATA_ENTRY_DIALOG_H__

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

#include <QStringList>
#include <QVariant>

#include "CaretColorEnum.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QRadioButton;
class QSpinBox;
class QTextEdit;

namespace caret {
    class BrainStructure;
    class CaretColorEnumComboBox;
    class CaretDataFileSelectionComboBox;
    class StructureEnumComboBox;
    class SurfaceSelectionViewController;
    
    /// class for a modal data entry dialog
    class WuQDataEntryDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        // constructor
        WuQDataEntryDialog(const QString& title,
                           QWidget* parent,
                           const bool addScrollBarsFlag = false,
                           Qt::WindowFlags f = Qt::WindowFlags());
        
        WuQDataEntryDialog(const QString& title,
                           QWidget* parent,
                           const WuQDialog::ScrollAreaStatus scrollBarStatus,
                           Qt::WindowFlags f = Qt::WindowFlags());
        
        void hideCancelButton();
        
        // destructor
        ~WuQDataEntryDialog();
        
        // add widget to next available row in the dialog
        QWidget* addWidget(const QString& labelText,
                           QWidget* widget);
        
        QWidget* addWidget(QWidget* widget);
        
        // add widgets to the next available row in the dialog
        void addWidgetsToNextRow(QWidget* leftColumnWidget,
                                 QWidget* rightColumnWidget);
        
        // add a check box
        QCheckBox* addCheckBox(const QString& text,
                               const bool defaultValue = false);
        
        // add a combo box
        QComboBox* addComboBox(const QString& labelText,
                               const QStringList& comboBoxItems,
                               const QList<QVariant>* comboBoxItemsUserData = NULL);
        
        // File selection combo box
        CaretDataFileSelectionComboBox* addFileSelectionComboBox(const QString& labelText,
                                                                 const std::vector<DataFileTypeEnum::Enum> dataFileTypes);
        
        // add line edit
        QLineEdit* addLineEditWidget(const QString& labelText,
                                     const QString& defaultText = "");
        
        // add list box
        QListWidget* addListWidget(const QString& labelText,
                                   const QStringList& listBoxItems);
        
        // add a radio button (all radio buttons will be mutually exclusive)
        QRadioButton* addRadioButton(const QString& text,
                                     const bool defaultValue = false);
        
        // get radio button selected (-1 if none, value is sequence added)
        int getRadioButtonSelected() const;
        
        // add spin box
        QSpinBox* addSpinBox(const QString& labelText,
                             const int defaultValue,
                             const int minimumValue = -10000,
                             const int maximumValue =  10000,
                             const int singleStep = 1);
        
        // add double spin box
        QDoubleSpinBox* addDoubleSpinBox(const QString& labelText,
                                         const float defaultValue,
                                         const float minimumValue = -10000000.0,
                                         const float maximumValue =  10000000.0,
                                         const float singleStep = 1.0,
                                         const int numberOfDecimals = 3);
        
        // add a text edit
        QTextEdit* addTextEdit(const QString& labelText,
                               const QString& defaultText,
                               const bool readOnlyFlag);
        
        // add a structure selection control
        StructureEnumComboBox* addStructureEnumComboBox(const QString& labelText,
                                                                const StructureEnum::Enum defaultStructure = StructureEnum::INVALID);
        
        // add a caret color selection control
        CaretColorEnumComboBox* addCaretColorEnumComboBox(const QString& labelText,
                                                        const CaretColorEnum::Enum defaultColor = CaretColorEnum::BLACK);
        
        
        SurfaceSelectionViewController* addSurfaceSelectionViewController(const QString& labelText,
                                                            BrainStructure* brainStructure);
        
        // set text at top of dialog 
        void setTextAtTop(const QString& s,
                          const bool wrapTheText);
        
        void setDataValid(const bool isValid,
                          const QString& dataInvalidErrorMessage);
        
        QWidget* getDataWidgetWithName(const QString& name);
        
    signals:
        /** 
         * This signal is emitted when the user presses the OK button.
         * The user of the dialog can then examine the widgets that were
         * added to the dialog for validity and then pass the validity 
         * status back to this dialog via setDataValid(). 
         */
        void validateData(WuQDataEntryDialog* dataEntryDialog);
        
    protected:
        virtual void okButtonClicked();
        
//        // override to verify data after OK button pressed if subclassing this dialog
//        virtual bool dataEnteredIsValid();
        
    private:
        void constructDialog(const WuQDialog::ScrollAreaStatus scrollBarStatus);
        
        /// widgets in dialog
        QVector<QWidget*> widgets;
        
        /// layout for widgets in dialog
        QGridLayout* widgetGridLayout;
        
        /// label for text at dialog top
        QLabel* textAtTopLabel;
        
        /// button group for radio buttons
        QButtonGroup* radioButtonGroup;
        
        /** Indicates validity of data */
        bool m_isDataValid;
        
        /** Error message displayed when data is invalid */
        QString m_dataInvalidErrorMessage;
    };
} // namespace

#endif // __WU_Q_DATA_ENTRY_DIALOG_H__
