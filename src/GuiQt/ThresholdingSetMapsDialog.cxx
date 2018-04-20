
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

#define __THRESHOLDING_SET_MAPS_DIALOG_DECLARE__
#include "ThresholdingSetMapsDialog.h"
#undef __THRESHOLDING_SET_MAPS_DIALOG_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "WuQMessageBox.h"

using namespace caret;

#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>

/**
 * \class caret::ThresholdingSetMapsDialog 
 * \brief Dialog for setting threshold
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param dataFile
 *     Data file that is being thresholded
 * @param thresholdFile
 *     File that is used for thresholding
 * @param thresholdFileMapIndex
 *     Map index from thresholding file.
 * @param parent
 *     Parent widget for this dialog.
 */
ThresholdingSetMapsDialog::ThresholdingSetMapsDialog(CaretMappableDataFile* dataFile,
                                                     CaretMappableDataFile* thresholdFile,
                                                     const int32_t thresholdFileMapIndex,
                                                     QWidget* parent)
: WuQDialogModal("Set Thresholding Maps",
                 parent),
m_dataFile(dataFile),
m_thresholdFile(thresholdFile),
m_thresholdFileMapIndex(thresholdFileMapIndex)
{
    CaretAssert(m_dataFile);
    CaretAssert(m_thresholdFile);
    CaretAssert(m_thresholdFileMapIndex >= 0);
    CaretAssert(m_thresholdFileMapIndex < m_thresholdFile->getNumberOfMaps());
    
    const AString numName("("
                          + AString::number(m_thresholdFileMapIndex + 1)
                          + "): "
                          + m_thresholdFile->getMapName(m_thresholdFileMapIndex));
    m_setSameIndexRadioButton = new QRadioButton("Threshold Each Map With Map "
                                                 + numName);
    const int32_t numMaps = std::min(m_dataFile->getNumberOfMaps(),
                                     m_thresholdFile->getNumberOfMaps());
    m_setOneToEnRadioButton   = new QRadioButton("Threshold Each Map With Sequential Indices "
                                                 "1 to "
                                                 + AString::number(numMaps));
    m_setOneToEnRadioButton->setEnabled(m_thresholdFile->getNumberOfMaps() > 1);
    
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(m_setSameIndexRadioButton);
    buttonGroup->addButton(m_setOneToEnRadioButton);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(m_setSameIndexRadioButton);
    layout->addWidget(m_setOneToEnRadioButton);
    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    setCentralWidget(widget, WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
ThresholdingSetMapsDialog::~ThresholdingSetMapsDialog()
{
}


/**
 * Called when user presses the OK or Cancel button
 *
 * @param resultCode
 *     Result code from dialog.
 */
void
ThresholdingSetMapsDialog::done(int resultCode)
{
    if (resultCode == Accepted) {
        if (m_setSameIndexRadioButton->isChecked()) {
            const int32_t numMaps = m_dataFile->getNumberOfMaps();
            for (int32_t i = 0; i < numMaps; i++) {
                m_dataFile->getMapThresholdFileSelectionModel(i)->setSelectedFile(m_thresholdFile);
                m_dataFile->getMapThresholdFileSelectionModel(i)->setSelectedMapIndex(m_thresholdFileMapIndex);
            }
        }
        else if (m_setOneToEnRadioButton->isChecked()) {
            const int32_t numMaps = std::min(m_dataFile->getNumberOfMaps(),
                                             m_thresholdFile->getNumberOfMaps());
            for (int32_t i = 0; i < numMaps; i++) {
                m_dataFile->getMapThresholdFileSelectionModel(i)->setSelectedFile(m_thresholdFile);
                m_dataFile->getMapThresholdFileSelectionModel(i)->setSelectedMapIndex(i);
            }
        }
        else {
            WuQMessageBox::errorOk(this, "Select a Threshold Option");
            return;
        }
    }
    else if (resultCode == Rejected) {
        /* nothing */
    }
    else {
        CaretAssertMessage(0, "Should never get here");
    }
    
    WuQDialogModal::done(resultCode);
}
