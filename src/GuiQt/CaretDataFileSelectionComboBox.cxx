
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include <QComboBox>

#define __CARET_DATA_FILE_SELECTION_COMBO_BOX_DECLARE__
#include "CaretDataFileSelectionComboBox.h"
#undef __CARET_DATA_FILE_SELECTION_COMBO_BOX_DECLARE__

#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretDataFileSelectionModel.h"
#include "FilePathNamePrefixCompactor.h"
#include "WuQEventBlockingFilter.h"

using namespace caret;


    
/**
 * \class caret::CaretDataFileSelectionComboBox 
 * \brief Combo box for selection of a CaretDataFile.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent of combo box.
 */
CaretDataFileSelectionComboBox::CaretDataFileSelectionComboBox(QObject* parent)
: WuQWidget(parent)
{
    m_selectionModel = NULL;
    m_comboBox = new QComboBox();
    m_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QObject::connect(m_comboBox, SIGNAL(activated(int)),
                     this, SLOT(slotFileIndexSelected(int)));

    WuQEventBlockingFilter::blockMouseWheelEventInMacComboBox(m_comboBox);
}

/**
 * Destructor.
 */
CaretDataFileSelectionComboBox::~CaretDataFileSelectionComboBox()
{
}

/**
 * @return The widget (combobox) for insertion into a layout.
 */
QWidget*
CaretDataFileSelectionComboBox::getWidget()
{
    return m_comboBox;
}

/**
 * Gets called when the user makes a selection.  Issues the 
 * 'fileSelected()' signal.
 *
 * @param indx
 *   Index of the item that was selected.
 */
void
CaretDataFileSelectionComboBox::slotFileIndexSelected(int indx)
{
    CaretDataFile* caretDataFile = NULL;
    
    if ((indx >= 0)
        && (indx < m_comboBox->count())) {
        void* filePointer = m_comboBox->itemData(indx).value<void*>();
        caretDataFile = (CaretDataFile*)filePointer;
    }
    
    if (m_selectionModel != NULL) {
        m_selectionModel->setSelectedFile(caretDataFile);
    }
    emit fileSelected(caretDataFile);
}

/**
 * Update the content of the combo box.
 *
 * @param
 *    Selection model for the combo box.
 */
void
CaretDataFileSelectionComboBox::updateComboBox(CaretDataFileSelectionModel* selectionModel)
{
    m_comboBox->blockSignals(true);
    m_comboBox->clear();
    
    m_selectionModel = selectionModel;
    
    bool enabledFlag(false);
    
    if (m_selectionModel != NULL) {
        const CaretDataFile* selectedFile = m_selectionModel->getSelectedFile();
        int defaultIndex = 0;
        
        std::vector<CaretDataFile*> caretDataFiles = m_selectionModel->getAvailableFiles();
        
        std::vector<AString> displayNames;
        FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(caretDataFiles,
                                                                                displayNames);
        CaretAssert(caretDataFiles.size() == displayNames.size());
        const int32_t numFiles = static_cast<int32_t>(caretDataFiles.size());
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            CaretAssertVectorIndex(caretDataFiles, iFile);
            CaretDataFile* cdf = caretDataFiles[iFile];
            CaretAssert(cdf);
            
            CaretAssertVectorIndex(displayNames, iFile);
            m_comboBox->addItem(displayNames[iFile],
                                QVariant::fromValue((void*)cdf));
            
            if (cdf == selectedFile) {
                defaultIndex = m_comboBox->count() - 1;
            }
        }
        
        if ((defaultIndex >= 0)
            && (defaultIndex < m_comboBox->count())) {
            m_comboBox->setCurrentIndex(defaultIndex);
        }
        
        enabledFlag = (m_comboBox->count() > 0);
    }
    
    m_comboBox->blockSignals(false);
    
    m_comboBox->setEnabled(enabledFlag);
    
    if ( ! enabledFlag) {
        if ( ! m_noFilesText.isEmpty()) {
            m_comboBox->addItem(m_noFilesText);
        }
    }
}

/**
 * @return Selection model in this combo box.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionComboBox::getSelectionModel()
{
    return m_selectionModel;
}

/**
 * @return Selected file or NULL if invalid
 */
CaretDataFile*
CaretDataFileSelectionComboBox::getSelectedFile() const
{
    if (m_selectionModel != NULL) {
        return m_selectionModel->getSelectedFile();
    }
    return NULL;
}

/**
 * Set the text displayed if no files are available
 * @param noFilesText
 *    The text
 */
void
CaretDataFileSelectionComboBox::setNoFilesText(const AString& noFilesText)
{
    m_noFilesText = noFilesText;
}




