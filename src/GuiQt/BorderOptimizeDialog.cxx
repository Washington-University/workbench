
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __BORDER_OPTIMIZE_DIALOG_DECLARE__
#include "BorderOptimizeDialog.h"
#undef __BORDER_OPTIMIZE_DIALOG_DECLARE__

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include "Border.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::BorderOptimizeDialog 
 * \brief Border Optimize Dialog.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent of the dialog.
 * @param surface
 *    Surface on which borders are drawn.
 * @param bordersInsideROI
 *    Borders inside the region of interest.
 * @param nodesInsideROI
 *    Nodes inside the region of interest.
 * @param optimizeDataFiles
 *    Data files for border optimization
 */
BorderOptimizeDialog::BorderOptimizeDialog(QWidget* parent,
                                           const Surface* surface,
                                           const std::vector<Border*>& bordersInsideROI,
                                           const std::vector<int32_t>& nodesInsideROI,
                                           const std::vector<CaretMappableDataFile*>& optimizeDataFiles)
: WuQDialogModal("Border Optimize",
                 parent),
m_surface(surface),
m_bordersInsideROI(bordersInsideROI),
m_nodesInsideROI(nodesInsideROI),
m_optimizeDataFiles(optimizeDataFiles)
{
    QLabel* nodesLabel = new QLabel("Nodes in ROI: "
                                    + AString::number(nodesInsideROI.size()));
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(nodesLabel);
    dialogLayout->addWidget(createBorderSelectionWidget());
    dialogLayout->addWidget(createDataFilesWidget());
    dialogLayout->addWidget(createOptionsWidget());
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
BorderOptimizeDialog::~BorderOptimizeDialog()
{
}

/**
 * Called when OK button is clicked.
 */
void
BorderOptimizeDialog::okButtonClicked()
{
    AString errorMessage;
    
    std::vector<CaretMappableDataFile*> optimizeFiles;
    const int32_t numOptimizeFileSelections = static_cast<int32_t>(m_optimizeDataFileCheckBoxes.size());
    for (int32_t iFile = 0; iFile < numOptimizeFileSelections; iFile++) {
        if (m_optimizeDataFileCheckBoxes[iFile]->isChecked()) {
            optimizeFiles.push_back(m_optimizeDataFiles[iFile]);
        }
    }
    
    if (optimizeFiles.empty()) {
        errorMessage.appendWithNewLine("No optimization data files are selected.");
    }
    
    std::vector<Border*> optimizeBorders;
    const int32_t numBorderFileSelections = static_cast<int32_t>(m_bordersInsideROI.size());
    for (int32_t iBorder = 0; iBorder < numBorderFileSelections; iBorder++) {
        if (m_borderCheckBoxes[iBorder]->isChecked()) {
            optimizeBorders.push_back(m_bordersInsideROI[iBorder]);
        }
    }
    if (optimizeBorders.empty()) {
        errorMessage.appendWithNewLine("No optimization border files are selected.");
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    const float smoothing = m_smoothingLevelSpinBox->value();
    
    const bool invertGradientFlag = m_invertedGradientCheckBox->isChecked();
    
    if (run(optimizeBorders,
            optimizeFiles,
            smoothing,
            invertGradientFlag,
            errorMessage)) {
        
        /*
         * Success, allow dialog to close.
         */
        WuQDialogModal::okButtonClicked();
    }

    WuQMessageBox::errorOk(this, errorMessage);
}

/**
 * @return The border selection widget.
 */
QWidget*
BorderOptimizeDialog::createBorderSelectionWidget()
{
    QGroupBox* widget = new QGroupBox("Borders");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    
    for (std::vector<Border*>::const_iterator iter = m_bordersInsideROI.begin();
         iter != m_bordersInsideROI.end();
         iter++) {
        const Border* border = *iter;
        CaretAssert(border);
        
        QCheckBox* cb = new QCheckBox(border->getName());
        cb->setChecked(true);
        m_borderCheckBoxes.push_back(cb);
        
        layout->addWidget(cb);
    }
    
    CaretAssert(m_borderCheckBoxes.size() == m_bordersInsideROI.size());
    
    return widget;
}

/**
 * @return The data files widget.
 */
QWidget*
BorderOptimizeDialog::createDataFilesWidget()
{
    QGroupBox* widget = new QGroupBox("Data Files");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    
    for (std::vector<CaretMappableDataFile*>::const_iterator iter = m_optimizeDataFiles.begin();
         iter != m_optimizeDataFiles.end();
         iter++) {
        const CaretMappableDataFile* mapFile = *iter;
        CaretAssert(mapFile);
        
        QCheckBox* cb = new QCheckBox(mapFile->getFileNameNoPath());
        cb->setChecked(true);
        m_optimizeDataFileCheckBoxes.push_back(cb);
        
        layout->addWidget(cb);
    }
    
    CaretAssert(m_optimizeDataFileCheckBoxes.size() == m_optimizeDataFiles.size());
    
    return widget;
}

/**
 * @return The options widget.
 */
QWidget*
BorderOptimizeDialog::createOptionsWidget()
{
    m_invertedGradientCheckBox = new QCheckBox("Invert Gradient");
    
    QLabel* smoothingLabel = new QLabel("Smoothing");
    
    m_smoothingLevelSpinBox = new QDoubleSpinBox();
    m_smoothingLevelSpinBox->setRange(0.0, 1.0);
    m_smoothingLevelSpinBox->setDecimals(2);
    m_smoothingLevelSpinBox->setValue(1.0);
    
    QGroupBox* widget = new QGroupBox("Options");
    QGridLayout* layout = new QGridLayout(widget);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 100);
    layout->addWidget(m_invertedGradientCheckBox,
                      0, 0, 1, 2, Qt::AlignLeft);
    layout->addWidget(smoothingLabel,
                      1, 0);
    layout->addWidget(m_smoothingLevelSpinBox,
                      1, 1, Qt::AlignLeft);
    
    return widget;
}

/**
 * Run border optimization.
 *
 * @param borders
 *    The borders.
 * @param dataFiles
 *    The data files.
 * @param smoothingLevel,
 *    Smoothing level [0.0, 1.0].
 * @param invertGradientFlag
 *    Request inverted gradient.
 * @param errorMessageOut
 *    Contains error information.
 * @return True if successful, false if errors.
 */
bool
BorderOptimizeDialog::run(std::vector<Border*>& borders,
                          std::vector<CaretMappableDataFile*>& dataFiles,
                          const float smoothingLevel,
                          const bool invertGradientFlag,
                          AString& errorMessageOut)
{
    /*
     * Returning false implies errors.
     */
    errorMessageOut = "Something went wrong.";
    return false;
}


