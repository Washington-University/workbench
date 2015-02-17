
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
#include "EventManager.h"
#include "EventUpdateInformationWindows.h"
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
 * @param borderEnclosingROI
 *    Border that encloses the region of interest
 * @param nodesInsideROI
 *    Nodes inside the region of interest.
 * @param optimizeDataFiles
 *    Data files for border optimization
 */
BorderOptimizeDialog::BorderOptimizeDialog(QWidget* parent,
                                           const Surface* surface,
                                           const std::vector<Border*>& bordersInsideROI,
                                           Border* borderEnclosingROI,
                                           const std::vector<int32_t>& nodesInsideROI,
                                           const std::vector<CaretMappableDataFile*>& optimizeDataFiles)
: WuQDialogModal("Border Optimize",
                 parent),
m_surface(surface),
m_bordersInsideROI(bordersInsideROI),
m_borderEnclosingROI(borderEnclosingROI),
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
    
    writeUserSelectionsToGUI(s_savedUserSelections);
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
    
    UserSelections userSelections;
    readUserSelectionsFromGUI(userSelections);
    
    if (m_borderEnclosingROI == NULL) {
        errorMessage.appendWithNewLine("Border enclosing region is invalid.");
    }
    if (m_borderEnclosingROI->getNumberOfPoints() < 3) {
        errorMessage.appendWithNewLine("Border named "
                                       + m_borderEnclosingROI->getName()
                                       + " enclosing region is invalid must contain at least three points");
    }
    if (userSelections.m_optimizeDataFiles.empty()) {
        errorMessage.appendWithNewLine("No optimization data files are selected.");
    }
    
    if (userSelections.m_borders.empty()) {
        errorMessage.appendWithNewLine("No optimization border files are selected.");
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    AString statisticsInformation;
    if (run(userSelections,
            statisticsInformation,
            errorMessage)) {

        /*
         * Display the statistics information.
         */
        EventManager::get()->sendEvent(EventUpdateInformationWindows(statisticsInformation).getPointer());
        
        /*
         * Save user's selections for next time.
         */
        readUserSelectionsFromGUI(s_savedUserSelections);
        
        /*
         * Success, allow dialog to close.
         */
        WuQDialogModal::okButtonClicked();
        
        return;
    }

    WuQMessageBox::errorOk(this, errorMessage);
}

/**
 * Called when cancel button clicked.
 */
void
BorderOptimizeDialog::cancelButtonClicked()
{
    readUserSelectionsFromGUI(s_savedUserSelections);
    
    /*
     * Allow dialog to close.
     */
    WuQDialogModal::cancelButtonClicked();
}

/**
 * Read the user selections from the GUI components.
 *
 * @param userSelectionsOut
 *     Output containing user selections.
 */
void
BorderOptimizeDialog::readUserSelectionsFromGUI(UserSelections& userSelectionsOut)
{
    userSelectionsOut.m_optimizeDataFiles.clear();
    const int32_t numOptimizeFileSelections = static_cast<int32_t>(m_optimizeDataFileCheckBoxes.size());
    for (int32_t iFile = 0; iFile < numOptimizeFileSelections; iFile++) {
        if (m_optimizeDataFileCheckBoxes[iFile]->isChecked()) {
            userSelectionsOut.m_optimizeDataFiles.push_back(m_optimizeDataFiles[iFile]);
        }
    }
    getSelectedBorders(userSelectionsOut.m_borders);
    userSelectionsOut.m_borderEnclosingROI = m_borderEnclosingROI;
    userSelectionsOut.m_nodesInsideROI = m_nodesInsideROI;
    userSelectionsOut.m_invertedGradientFlag = m_invertedGradientCheckBox->isChecked();
    userSelectionsOut.m_smoothingLevel = m_smoothingLevelSpinBox->value();
    userSelectionsOut.m_valid = true;
}

/**
 * Write the user selections to the GUI.
 *
 * @param userSelections
 *     The user's selections.
 */
void
BorderOptimizeDialog::writeUserSelectionsToGUI(const UserSelections& userSelections)
{
    if (userSelections.m_valid) {
        m_smoothingLevelSpinBox->setValue(userSelections.m_smoothingLevel);
        m_invertedGradientCheckBox->setChecked(userSelections.m_invertedGradientFlag);
        
        for (uint32_t i = 0; i < m_bordersInsideROI.size(); i++) {
            m_borderCheckBoxes[i]->setChecked(false);
            if (std::find(userSelections.m_borders.begin(),
                          userSelections.m_borders.end(),
                          m_bordersInsideROI[i]) != userSelections.m_borders.end()) {
                CaretAssertVectorIndex(m_borderCheckBoxes, i);
                m_borderCheckBoxes[i]->setChecked(true);
            }
        }
        
        for (uint32_t i = 0; i < m_optimizeDataFiles.size(); i++) {
            m_optimizeDataFileCheckBoxes[i]->setChecked(false);
            if (std::find(userSelections.m_optimizeDataFiles.begin(),
                          userSelections.m_optimizeDataFiles.end(),
                          m_optimizeDataFiles[i]) != userSelections.m_optimizeDataFiles.end()) {
                CaretAssertVectorIndex(m_optimizeDataFileCheckBoxes, i);
                m_optimizeDataFileCheckBoxes[i]->setChecked(true);
            }
        }
    }
}

/**
 * Get borders that were selected by the user.
 *
 * @param selectedBordersOut
 *     Output containing borders selected by the user.
 */
void
BorderOptimizeDialog::getSelectedBorders(std::vector<Border*>& selectedBordersOut) const
{
    selectedBordersOut.clear();
    
    const int32_t numBorderFileSelections = static_cast<int32_t>(m_bordersInsideROI.size());
    for (int32_t iBorder = 0; iBorder < numBorderFileSelections; iBorder++) {
        if (m_borderCheckBoxes[iBorder]->isChecked()) {
            selectedBordersOut.push_back(m_bordersInsideROI[iBorder]);
        }
    }
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
    m_smoothingLevelSpinBox->setSingleStep(0.01);
    
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
 * @param userSelections
 *    Contains the user's selections.
 * @param statisticsInformationOut
 *    Contains statistics information upon exit.
 * @param errorMessageOut
 *    Contains error information upon exit.
 * @return 
 &    True if successful, false if errors.
 */
bool
BorderOptimizeDialog::run(const UserSelections& userSelections,
                          AString& statisticsInformationOut,
                          AString& errorMessageOut)
{
    statisticsInformationOut.clear();
    errorMessageOut.clear();

    /*
     * Inputs for algorithm
     *
     * Pointers to borders selected by user for optimization. 
     *     std:vector<Border*> userSelections.m_borders;
     * Pointer to border enclosing ROI drawn by user
     *     Border* userSelectionsOut.m_borderEnclosingROI;
     * Ponters to data files used for border optimization.
     *     std::vector<CaretMappableDataFile*> userSelections.m_optimizeDataFiles;
     * Contains nodes found inside border drawn by the user.
     *     std::vector<int32_t> userSelections.m_nodesInsideROI;
     * Smoothing level ranging [0.0, 1.0]
     *     float userSelections.m_smoothingLevel;
     * Flag for inverted gradient.
     *     bool  userSelections.m_invertedGradientFlag;
     */
     
     
    /*
     * Modifying a border:
     * 
     * (1) Make a copy of the border
     *
     *     Border* borderCopy = new Border(*border)
     *
     * (2) Modify the 'copied' border
     *
     * (3) When modification is complete, calling this method
     *     will first make an 'undo' copy of 'border' that is stored
     *     inside of border and then replace the points in 'border'
     *     with those from 'borderCopy'.   This will allow the
     *     user to press the Border ToolBar's 'Undo Finish' button
     *     if the changes are not acceptable.
     * 
     *     border->replacePointsWithUndoSaving(borderCopy)
     */
    
    /*
     * Returning false implies errors.
     */
    errorMessageOut = "Implementation is missing.";
    return false;
}


