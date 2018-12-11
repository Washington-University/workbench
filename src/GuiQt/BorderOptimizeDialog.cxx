
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

#include <limits>

#define __BORDER_OPTIMIZE_DIALOG_DECLARE__
#include "BorderOptimizeDialog.h"
#undef __BORDER_OPTIMIZE_DIALOG_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

#include "Border.h"
#include "BorderFile.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretFileDialog.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "CursorDisplayScoped.h"
#include "EventBrowserTabGet.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MetricFile.h"
#include "OverlaySet.h"
#include "ProgressReportingDialog.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceSelectionViewController.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BorderOptimizeDialog 
 * \brief Border Optimize Dialog.
 * \ingroup GuiQt
 */

static const int STRETCH_NONE = 0;
static const int STRETCH_MAX  = 100;

static const bool DATA_FILES_IN_SCROLL_BARS = false;

/**
 * Constructor.
 *
 * @param parent
 *    Parent of the dialog.
 */
BorderOptimizeDialog::BorderOptimizeDialog(QWidget* parent)
: WuQDialogModal("Border Optimize",
                 parent),
m_surface(NULL),
m_borderEnclosingROI(NULL),
m_borderPairFileSelectionModel(NULL),
m_surfaceSelectionStructure(StructureEnum::INVALID),
m_surfaceSelectionModel(NULL),
m_vertexAreasMetricFileSelectionModel(NULL),
m_browserTabIndex(-1),
m_upsamplingSurfaceSelectionModel(NULL),
m_upsamplingSurfaceStructure(StructureEnum::INVALID)
{
    m_objectNamePrefix + "BorderOptimizeDialog";
    m_optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE);
    m_optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR);
    m_optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
    m_optimizeDataFileTypes.push_back(DataFileTypeEnum::METRIC);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(createBorderSelectionWidget(),
                            STRETCH_NONE);
    
    if (DATA_FILES_IN_SCROLL_BARS) {
        dialogLayout->addWidget(createDataFilesWidget(),
                            STRETCH_MAX);
    }
    else {
        dialogLayout->addWidget(createDataFilesWidget(),
                                STRETCH_MAX);
    }
    dialogLayout->addWidget(createSurfaceSelectionWidget(),
                            STRETCH_NONE);
    dialogLayout->addWidget(createVertexAreasMetricWidget(),
                            STRETCH_NONE);
    dialogLayout->addWidget(createSphericalUpsamplingWidget(),
                            STRETCH_NONE);
    dialogLayout->addWidget(createSavingWidget(), STRETCH_NONE);
    QWidget* optionsWidget = createOptionsWidget();
    if (optionsWidget != NULL) {
        dialogLayout->addWidget(optionsWidget,
                                STRETCH_NONE);
    }
    
    if (DATA_FILES_IN_SCROLL_BARS) {
        dialogLayout->addStretch();
        setCentralWidget(dialogWidget,
                         SCROLL_AREA_NEVER);
    }
    else {
        setCentralWidget(dialogWidget,
                         SCROLL_AREA_AS_NEEDED_VERT_NO_HORIZ);
    }
    
    if (DATA_FILES_IN_SCROLL_BARS) {
        if (m_defaultDataFilesWidgetSize.width() > 300) {
            QSize defaultSize = sizeHint();
            defaultSize.setWidth(m_defaultDataFilesWidgetSize.width() + 100);
            setSizeOfDialogWhenDisplayed(defaultSize);
        }
    }
    
    m_dialogWidget = dialogWidget;
    
//    if ( ! DATA_FILES_IN_SCROLL_BARS) {
//        setSizePolicy(sizePolicy().horizontalPolicy(),
//                      QSizePolicy::Fixed);
//    }
}

/**
 * Destructor.
 */
BorderOptimizeDialog::~BorderOptimizeDialog()
{
    if (m_surfaceSelectionModel != NULL) {
        delete m_surfaceSelectionModel;
    }
    if (m_vertexAreasMetricFileSelectionModel != NULL) {
        delete m_vertexAreasMetricFileSelectionModel;
    }
    if (m_borderPairFileSelectionModel != NULL) {
        delete m_borderPairFileSelectionModel;
    }
    if (m_upsamplingSurfaceSelectionModel != NULL) {
        delete m_upsamplingSurfaceSelectionModel;
    }
}

/**
 * Update the content of the dialog.
 *
 * @param browserTabIndex
 *    Index of browser tab in which borders are drawn.
 * @param surface
 *    Surface on which borders are drawn.
 * @param bordersInsideROI
 *    Map where value is border and key is number of border
 *    points inside the ROI
 * @param borderEnclosingROI
 *    Border that encloses the region of interest
 * @param nodesInsideROI
 *    Nodes inside the region of interest.
 */
void
BorderOptimizeDialog::updateDialog(const int32_t browserTabIndex,
                                   Surface* surface,
                                   std::vector<std::pair<int32_t, Border*> >& bordersInsideROI,
                                   Border* borderEnclosingROI,
                                   std::vector<int32_t>& nodesInsideROI)
{
    CaretAssert(surface);
    CaretAssert(borderEnclosingROI);
    
    m_browserTabIndex    = browserTabIndex;
    m_surface            = surface;
    m_borderEnclosingROI = borderEnclosingROI;
    m_nodesInsideROI     = nodesInsideROI;
    
    m_borderPointsInsideROICount.clear();
    m_bordersInsideROI.clear();
    for (std::vector<std::pair<int32_t, Border*> >::reverse_iterator bi = bordersInsideROI.rbegin();
         bi != bordersInsideROI.rend();
         bi++) {
        m_borderPointsInsideROICount.push_back(bi->first);
        m_bordersInsideROI.push_back(bi->second);
    }
    
    /*
     * Update surface selection.
     * Will need to recreate model if the structure has changed.
     */
    const StructureEnum::Enum structure = m_surface->getStructure();
    if (m_surfaceSelectionModel != NULL) {
        if (structure != m_surfaceSelectionStructure) {
            delete m_surfaceSelectionModel;
            m_surfaceSelectionModel = NULL;
            m_surfaceSelectionStructure = StructureEnum::INVALID;
        }
    }
    if (m_surfaceSelectionModel == NULL) {
        m_surfaceSelectionStructure = structure;
        std::vector<SurfaceTypeEnum::Enum> allSurfaceTypes;
        SurfaceTypeEnum::getAllEnums(allSurfaceTypes);
        m_surfaceSelectionModel = new SurfaceSelectionModel(m_surfaceSelectionStructure,
                                                            allSurfaceTypes);
        
        BrainStructure* bs = GuiManager::get()->getBrain()->getBrainStructure(m_surfaceSelectionStructure,
                                                                              false);
        if (bs != NULL) {
            Surface* surface = bs->getPrimaryAnatomicalSurface();
            if (surface != NULL) {
                m_surfaceSelectionModel->updateModel();
                m_surfaceSelectionModel->setSurface(const_cast<Surface*>(surface));
            }
        }
    }
    else {
        m_surfaceSelectionModel->updateModel();
    }
    
    if (m_surfaceSelectionModel != NULL) {
        m_surfaceSelectionControl->updateControl(m_surfaceSelectionModel);
        m_surfaceSelectionControl->getWidget()->setEnabled(true);
    }
    else {
        m_surfaceSelectionControl->getWidget()->setEnabled(false);
    }
    
    Surface* defaultSphericalSurface = NULL;
    if (m_upsamplingSurfaceSelectionModel != NULL) {
        defaultSphericalSurface = m_upsamplingSurfaceSelectionModel->getSurface();
        if (structure != m_upsamplingSurfaceStructure) {
            delete m_upsamplingSurfaceSelectionModel;
            m_upsamplingSurfaceSelectionModel = NULL;
            m_upsamplingSurfaceStructure = StructureEnum::INVALID;
        }
    }
    if (m_upsamplingSurfaceSelectionModel == NULL) {
        m_upsamplingSurfaceStructure = structure;
        std::vector<SurfaceTypeEnum::Enum> sphericalSurfaceTypes;
        sphericalSurfaceTypes.push_back(SurfaceTypeEnum::SPHERICAL);
        m_upsamplingSurfaceSelectionModel = new SurfaceSelectionModel(m_upsamplingSurfaceStructure,
                                                                      sphericalSurfaceTypes);
        m_upsamplingResolutionSpinBox->setValue(0);
    }
    if (defaultSphericalSurface != NULL) {
        m_upsamplingSurfaceSelectionModel->setSurface(defaultSphericalSurface);
    }
    m_upsamplingSurfaceSelectionModel->updateModel();
    m_upsamplingSurfaceSelectionControl->updateControl(m_upsamplingSurfaceSelectionModel);
    if (m_upsamplingResolutionSpinBox->value() <= 0) {
        Surface* surface = m_upsamplingSurfaceSelectionModel->getSurface();
        if (surface != NULL) {
            m_upsamplingResolutionSpinBox->setValue(surface->getNumberOfNodes() * 4);
        }
    }
    
    /*
     * Update metric selection model
     * Will need to recreate if the structure has changed.
     */
    if (m_vertexAreasMetricFileSelectionModel != NULL) {
        if (structure != m_vertexAreasMetricFileSelectionModel->getStructure()) {
            delete m_vertexAreasMetricFileSelectionModel;
            m_vertexAreasMetricFileSelectionModel = NULL;
        }
    }
    if (m_vertexAreasMetricFileSelectionModel == NULL) {
        std::vector<DataFileTypeEnum::Enum> metricDataTypes;
        metricDataTypes.push_back(DataFileTypeEnum::METRIC);
        m_vertexAreasMetricFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileTypesInStructure(m_surfaceSelectionStructure,
                                                                                                                         metricDataTypes);
        m_vertexAreasMetricFileComboBox->updateComboBox(m_vertexAreasMetricFileSelectionModel);
    }
    
    if (m_vertexAreasMetricFileSelectionModel != NULL) {
        m_vertexAreasMetricFileComboBox->updateComboBox(m_vertexAreasMetricFileSelectionModel);
    }
    
    /*
     * Update border file selection but limit to border files that are the same structure
     * as the surface
     */
    m_borderPairFileSelectionModel->setStructure(structure);
    m_borderPairFileSelectionComboBox->updateComboBox(m_borderPairFileSelectionModel);
    
    QString initialBaseName = "";
    /*
     * Update borders inside ROI selections
     */
    CaretAssert(m_bordersInsideROI.size() == m_borderPointsInsideROICount.size());
    const int32_t NUM_ROWS = 2;
    const int32_t numberOfBorders = static_cast<int32_t>(m_bordersInsideROI.size());
    for (int32_t i = 0; i < numberOfBorders; i++) {
        if (i >= static_cast<int32_t>(m_borderCheckBoxes.size())) {
            QCheckBox* cb = new QCheckBox("");
            m_borderCheckBoxes.push_back(cb);
            
            /*
             * Use two rows and then wrap additional columns on right
             */
            const int32_t row = (i % NUM_ROWS);
            const int32_t col = (i / NUM_ROWS);
            m_bordersInsideROIGridLayout->addWidget(cb, row, col);
        }
        CaretAssertVectorIndex(m_borderCheckBoxes, i);
        CaretAssertVectorIndex(m_bordersInsideROI, i);
        CaretAssertVectorIndex(m_borderPointsInsideROICount, i);
        
        if (m_borderCheckBoxes[i]->text() != m_bordersInsideROI[i]->getName()) {
            m_borderCheckBoxes[i]->blockSignals(true);
            m_borderCheckBoxes[i]->setChecked(true);
            m_borderCheckBoxes[i]->blockSignals(false);
        }
//        m_borderCheckBoxes[i]->setText(m_bordersInsideROI[i]->getName());
        m_borderCheckBoxes[i]->setText(m_bordersInsideROI[i]->getName()
                                       + " ("
                                       + QString::number(m_borderPointsInsideROICount[i])
                                       + ")");
        m_borderCheckBoxes[i]->setVisible(true);
        if (i < 2)
        {
            if (initialBaseName != "") initialBaseName += "_";
            initialBaseName += m_bordersInsideROI[i]->getName();
        }
    }
    m_savingBaseNameLineEdit->setText(initialBaseName);
    
    /*
     * Remove border selections not needed.
     */
    for (int32_t i = numberOfBorders;
         i < static_cast<int32_t>(m_borderCheckBoxes.size());
         i++) {
        m_borderCheckBoxes[i]->setVisible(false);
    }
    
    /*
     * Update the data file selectors.
     */
    for (std::vector<BorderOptimizeDataFileSelector*>::iterator fileIter = m_optimizeDataFileSelectors.begin();
         fileIter != m_optimizeDataFileSelectors.end();
         fileIter++) {
        BorderOptimizeDataFileSelector* selector = *fileIter;
        selector->updateFileData();
    }
    
    m_dialogWidget->adjustSize();
}

/**
 * Called when OK button is clicked.
 */
void
BorderOptimizeDialog::okButtonClicked()
{
    preserveDialogSizeAndPositionWhenReOpened();
    
    AString errorMessage;
    m_selectedBorders.clear();
    
    
    Surface* gradientComputationSurface = m_surfaceSelectionModel->getSurface();
    if (gradientComputationSurface == NULL) {
        errorMessage.appendWithNewLine("Gradient Computation Surface is not valid.");
    }
    
    if (m_borderEnclosingROI != NULL) {
        if (m_borderEnclosingROI->getNumberOfPoints() < 3) {
            errorMessage.appendWithNewLine("Border named "
                                           + m_borderEnclosingROI->getName()
                                           + " enclosing region is invalid must contain at least three points");
        }
    }
    else {
        errorMessage.appendWithNewLine("Border enclosing region is invalid.");
    }

    std::vector<BorderOptimizeExecutor::DataFileInfo> dataFileSelections;
    for (std::vector<BorderOptimizeDataFileSelector*>::iterator fileIter = m_optimizeDataFileSelectors.begin();
         fileIter != m_optimizeDataFileSelectors.end();
         fileIter++) {
        BorderOptimizeDataFileSelector* selector = *fileIter;
        CaretPointer<BorderOptimizeExecutor::DataFileInfo> fileInfo = selector->getSelections();
        if (fileInfo != NULL) {
            dataFileSelections.push_back(*fileInfo);
        }
    }
    
    if (dataFileSelections.empty()) {
        errorMessage.appendWithNewLine("No optimization data files are selected.");
    }
    
    /*
     * "Pair" border file
     */
    const BorderFile* pairBorderFile = m_borderPairFileSelectionModel->getSelectedFileOfType<BorderFile>();
    std::vector<Border*> pairedBorders;
    
    /*
     * Get borders selected by the user.
     */
    AString metricFileMapName;
    CaretAssert(m_bordersInsideROI.size() <= m_borderCheckBoxes.size());
    const int32_t numBorderFileSelections = static_cast<int32_t>(m_bordersInsideROI.size());
    for (int32_t iBorder = 0; iBorder < numBorderFileSelections; iBorder++) {
        if (m_borderCheckBoxes[iBorder]->isVisible()) {
            if (m_borderCheckBoxes[iBorder]->isChecked()) {
                CaretAssertVectorIndex(m_bordersInsideROI, iBorder);
                Border* border = m_bordersInsideROI[iBorder];
                m_selectedBorders.push_back(border);
                metricFileMapName.append(border->getName()
                                         + " ");
                
                if (pairBorderFile != NULL) {
                    if (pairBorderFile->containsBorder(border)) {
                        pairedBorders.push_back(border);
                    }
                }
            }
        }
    }
    
    const int32_t numPairedBorders = static_cast<int32_t>(pairedBorders.size());
    
    if (m_selectedBorders.empty()) {
        errorMessage.appendWithNewLine("No optimization border files are selected.");
    }
    else if (m_borderPairCheckBox->isChecked()) {
        if (pairBorderFile == NULL) {
            errorMessage.appendWithNewLine("Border pair file selection is invalid.");
        }
        else if (numPairedBorders != 2) {
            errorMessage.appendWithNewLine("Two of the selected borders must be in the border pair file.");
            if (pairedBorders.empty()) {
                errorMessage.appendWithNewLine("None of the selected borders are in the border pair file.");
            }
            else {
                errorMessage.appendWithNewLine("There are "
                                               + AString::number(numPairedBorders)
                                               + " selected borders in the border pair file.");
                for (int32_t i = 0; i < numPairedBorders; i++) {
                    CaretAssertVectorIndex(pairedBorders, i);
                    errorMessage.appendWithNewLine("   "
                                                   + pairedBorders[i]->getName());
                }
            }
        }
    }
    
    Surface* upsamplingSphericalSurface = NULL;
    int32_t upsamplingResolution = 0;
    if (m_upsamplingGroupBox->isChecked()) {
        upsamplingSphericalSurface = m_upsamplingSurfaceSelectionModel->getSurface();
        if (upsamplingSphericalSurface == NULL) {
            errorMessage.appendWithNewLine("Upsampling is selected but no spherical surface is available.");
        }
        upsamplingResolution = m_upsamplingResolutionSpinBox->value();
        if (upsamplingResolution <= 0) {
            errorMessage.appendWithNewLine("Upsampling resolution is invalid.");
        }
    }
    
    const QString savingBaseName = m_savingBaseNameLineEdit->text().trimmed();
    const QString savingDirectoryName = m_savingDirectoryLineEdit->text().trimmed();
    if (m_savingGroupBox->isChecked()) {
        if (savingDirectoryName.isEmpty()) {
            errorMessage.appendWithNewLine("Save Results is selected and Saving Directory is empty.");
        }
        if (savingBaseName.isEmpty()) {
            errorMessage.appendWithNewLine("Save Results is selected and Saving Base Filename is empty.");
        }
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    if (m_borderPairCheckBox->isChecked()) {
        CaretAssertMessage((pairedBorders.size() == 2),
                           "Must be exactly two borders in the border pair vector");
    }
    else {
        pairedBorders.clear();
    }
    
    ProgressReportingDialog progressDialog("Border Optimization",
                                           "",
                                           this);
    progressDialog.setMinimum(0);
    progressDialog.setValue(0);
    
    MetricFile* vertexAreasMetricFile = NULL;
    CaretDataFile* vertexAreaFile = m_vertexAreasMetricFileSelectionModel->getSelectedFile();
    if (vertexAreaFile != NULL) {
        vertexAreasMetricFile = dynamic_cast<MetricFile*>(vertexAreaFile);
    }
    
    const float gradientFollowingStrength = m_gradientFollowingStrengthSpinBox->value();
    
    CaretPointer<MetricFile> resultsMetricFile;
    resultsMetricFile.grabNew(new MetricFile());
    BorderOptimizeExecutor::InputData algInput(m_selectedBorders,
                                               pairedBorders,
                                               m_borderEnclosingROI,
                                               m_nodesInsideROI,
                                               gradientComputationSurface,
                                               dataFileSelections,
                                               vertexAreasMetricFile,
                                               gradientFollowingStrength,
                                               upsamplingSphericalSurface,
                                               upsamplingResolution,
                                               resultsMetricFile,
                                               m_savingGroupBox->isChecked(),
                                               savingDirectoryName,
                                               savingBaseName);
    
    /*
     * Run border optimization.
     */
    AString statisticsInformation;
    const bool algSuccessFlag = BorderOptimizeExecutor::run(algInput,
                                                            statisticsInformation,
                                                            errorMessage);
    /*
     * The progress dialog normally closes on its own but
     * may fail to close if the algorithm fails due to 
     * the 'progress value' not reaching 'progress maximum'.
     * This prevent the error dialog from being shown behind
     * the progress dialog.
     */
    progressDialog.close();
    
    if (algSuccessFlag) {
        AString infoMsg;
        if (m_outputGradientMapCheckBox->isChecked()
            && ! resultsMetricFile->isEmpty()) {
            resultsMetricFile->setStructure(gradientComputationSurface->getStructure());
            resultsMetricFile->setMapName(0, metricFileMapName);
            PaletteColorMapping* pcm = resultsMetricFile->getMapPaletteColorMapping(0);
            pcm->setAutoScalePercentageNegativeMaximum(96.0);
            pcm->setAutoScalePercentageNegativeMinimum(4.0);
            pcm->setAutoScalePercentagePositiveMinimum(4.0);
            pcm->setAutoScalePercentagePositiveMaximum(96.0);
            pcm->setSelectedPaletteName("videen_style");
            pcm->setDisplayNegativeDataFlag(false);
            pcm->setDisplayZeroDataFlag(false);
            pcm->setDisplayPositiveDataFlag(true);
            pcm->setInterpolatePaletteFlag(true);
            
            EventDataFileAdd addDataFile(resultsMetricFile.releasePointer());
            EventManager::get()->sendEvent(addDataFile.getPointer());
            infoMsg.appendWithNewLine("Border Optimization Gradient results in file "
                                      + resultsMetricFile->getFileNameNoPath()
                                      + "   Map Name: "
                                      + metricFileMapName);

            if (m_browserTabIndex >= 0) {
                /*
                 * Updating user interface will update content of overlay
                 * so that metric file gets added.
                 */
                EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
                
                EventBrowserTabGet browserTabEvent(m_browserTabIndex);
                EventManager::get()->sendEvent(browserTabEvent.getPointer());
                BrowserTabContent* tabContent = browserTabEvent.getBrowserTab();
                if (tabContent != NULL) {
                    /*
                     * Create a new overlay at the top
                     */
                    OverlaySet* overlaySet = tabContent->getOverlaySet();
                    CaretAssert(overlaySet);
                    overlaySet->insertOverlayAbove(0);
                    
                    /*
                     * Load the gradient metric file into the top-most
                     * (should be the new) overlay.
                     */
                    Overlay* overlay = overlaySet->getPrimaryOverlay();
                    CaretAssert(overlay);
                    overlay->setSelectionData(resultsMetricFile, 0);
                    overlay->setEnabled(true);
                    overlay->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
                    overlay->setOpacity(1.0);
                    
                    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
                }
            }
        }
        
        /*
         * Display the statistics information.
         */
        if ( ! statisticsInformation.isEmpty()) {
            infoMsg.appendWithNewLine("");
            infoMsg.appendWithNewLine(statisticsInformation);
        }
        infoMsg = infoMsg.replaceHtmlSpecialCharactersWithEscapeCharacters();
        EventManager::get()->sendEvent(EventUpdateInformationWindows(infoMsg).getPointer());
        
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
    preserveDialogSizeAndPositionWhenReOpened();
    
    /*
     * Allow dialog to close.
     */
    WuQDialogModal::cancelButtonClicked();
}

/**
 * Make dialog appear in same place and size when repopened.
 */
void
BorderOptimizeDialog::preserveDialogSizeAndPositionWhenReOpened()
{
    setSaveWindowPositionForNextTime("BorderOptimizeDialog");
    setSizeOfDialogWhenDisplayed(size());
}


/**
 * Get borders that were selected by the user.
 *
 * @param selectedBordersOut
 *     Output containing borders selected by the user.
 */
void
BorderOptimizeDialog::getModifiedBorders(std::vector<Border*>& modifiedBordersOut) const
{
    modifiedBordersOut = m_selectedBorders;
}

/**
 * @return The border selection widget.
 */
QWidget*
BorderOptimizeDialog::createBorderSelectionWidget()
{
    m_borderPairCheckBox = new QCheckBox("Border Pair File");
    m_borderPairCheckBox->setChecked(true);
    m_borderPairFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileType(DataFileTypeEnum::BORDER);
    m_borderPairFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QHBoxLayout* borderPairLayout = new QHBoxLayout();
    borderPairLayout->addWidget(m_borderPairCheckBox, 0);
    borderPairLayout->addWidget(m_borderPairFileSelectionComboBox->getWidget(), 100);
    
    QObject::connect(m_borderPairCheckBox, SIGNAL(clicked(bool)),
                     m_borderPairFileSelectionComboBox->getWidget(), SLOT(setEnabled(bool)));
    m_borderPairFileSelectionComboBox->getWidget()->setEnabled(m_borderPairCheckBox->isChecked());
    
    m_bordersInsideROIGridLayout = new QGridLayout();
    m_bordersInsideROIGridLayout->setMargin(2);
    m_bordersInsideROIGridLayout->setHorizontalSpacing(25);
    m_bordersInsideROIGridLayout->setColumnStretch(100, 1000); // force widgets to left
    
    QAction* diableAllAction = new QAction("Disable All",
                                           this);
    QObject::connect(diableAllAction, SIGNAL(triggered(bool)),
                     this, SLOT(bordersDisableAllSelected()));
    QToolButton* disableAllToolButton = new QToolButton();
    disableAllToolButton->setDefaultAction(diableAllAction);
    
    QAction* enableAllAction = new QAction("Enable All",
                                        this);
    QObject::connect(enableAllAction, SIGNAL(triggered(bool)),
                     this, SLOT(bordersEnableAllSelected()));
    QToolButton* enableAllToolButton = new QToolButton();
    enableAllToolButton->setDefaultAction(enableAllAction);
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setMargin(2);
    buttonsLayout->addWidget(enableAllToolButton);
    buttonsLayout->addWidget(disableAllToolButton);
    buttonsLayout->addStretch();
    
    QGroupBox* widget = new QGroupBox("Borders");
    widget->setSizePolicy(widget->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setMargin(2);
    layout->addLayout(borderPairLayout);
    layout->addLayout(m_bordersInsideROIGridLayout, STRETCH_NONE);
    layout->addLayout(buttonsLayout, STRETCH_NONE);
    return widget;
}

/**
 * Disable all borders.
 */
void
BorderOptimizeDialog::bordersDisableAllSelected()
{
    setAllBorderEnabledSelections(false);
}

/**
 * Enable all borders.
 */
void
BorderOptimizeDialog::bordersEnableAllSelected()
{
    setAllBorderEnabledSelections(true);
}

/**
 * Set the enable status of all borders to the given value.
 *
 * @param status
 *    New status.
 */
void
BorderOptimizeDialog::setAllBorderEnabledSelections(const bool status)
{
    for (std::vector<QCheckBox*>::iterator iter = m_borderCheckBoxes.begin();
         iter != m_borderCheckBoxes.end();
         iter++) {
        QCheckBox* cb = *iter;
        cb->setChecked(status);
    }
}

/**
 * @return The data files widget.
 */
QWidget*
BorderOptimizeDialog::createDataFilesWidget()
{
    QWidget* gridWidget = new QWidget();
    if (DATA_FILES_IN_SCROLL_BARS) {
        gridWidget->setSizePolicy(gridWidget->sizePolicy().horizontalPolicy(),
                                  QSizePolicy::Fixed);
    }
    else {
//        gridWidget->setSizePolicy(gridWidget->sizePolicy().horizontalPolicy(),
//                                  QSizePolicy::Fixed);
    }
    m_borderOptimizeDataFileGridLayout = new QGridLayout(gridWidget);
    m_borderOptimizeDataFileGridLayout->setMargin(2);
    
    std::vector<CaretMappableDataFile*> optimizeMapFiles;
    GuiManager::get()->getBrain()->getAllMappableDataFileWithDataFileTypes(m_optimizeDataFileTypes,
                                                                           optimizeMapFiles);
    const int32_t numberOfMapFiles = static_cast<int32_t>(optimizeMapFiles.size());
    
//    const int32_t minimumDataFilesToShow = 10;
//    const int32_t numberOfFilesToShow = std::max(minimumDataFilesToShow,
//                                                 numberOfMapFiles);
    const int32_t numberOfFilesToShow = 3;
    
    for (int32_t i = 0; i < numberOfFilesToShow; i++) {
        CaretMappableDataFile* mapFile = NULL;
        if (i < numberOfMapFiles) {
            CaretAssertVectorIndex(optimizeMapFiles, i);
            mapFile = optimizeMapFiles[i];
        }
        addDataFileRow(mapFile);
    }
    
    QWidget* widget = gridWidget;
    if (DATA_FILES_IN_SCROLL_BARS) {
        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setWidget(gridWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        scrollArea->setSizePolicy(gridWidget->sizePolicy().horizontalPolicy(),
                                  QSizePolicy::Preferred);
        
        widget = scrollArea;
    }
    else {
        widget->setSizePolicy(widget->sizePolicy().horizontalPolicy(),
                              QSizePolicy::Fixed);
    }
    
    QAction* addRowAction = new QAction("Add New Data File Row",
                                        this);
    QObject::connect(addRowAction, SIGNAL(triggered(bool)),
                     this, SLOT(addDataFileRowToolButtonClicked()));
    QToolButton* addRowToolButton = new QToolButton();
    addRowToolButton->setDefaultAction(addRowAction);
    
    QAction* diableAllAction = new QAction("Disable All",
                                           this);
    QObject::connect(diableAllAction, SIGNAL(triggered(bool)),
                     this, SLOT(dataFilesDisableAllSelected()));
    QToolButton* disableAllToolButton = new QToolButton();
    disableAllToolButton->setDefaultAction(diableAllAction);
    
    QAction* enableAllAction = new QAction("Enable All",
                                           this);
    QObject::connect(enableAllAction, SIGNAL(triggered(bool)),
                     this, SLOT(dataFilesEnableAllSelected()));
    QToolButton* enableAllToolButton = new QToolButton();
    enableAllToolButton->setDefaultAction(enableAllAction);
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setMargin(2);
    buttonsLayout->addWidget(addRowToolButton);
    buttonsLayout->addWidget(enableAllToolButton);
    buttonsLayout->addWidget(disableAllToolButton);
    buttonsLayout->addStretch();
    
    QGroupBox* groupBox = new QGroupBox("Data Files");
    if ( ! DATA_FILES_IN_SCROLL_BARS) {
//        groupBox->setSizePolicy(groupBox->sizePolicy().horizontalPolicy(),
//                                QSizePolicy::Minimum);
    }
    
    QVBoxLayout* groupBoxLayout = new QVBoxLayout(groupBox);
    groupBoxLayout->setMargin(2);
    if (DATA_FILES_IN_SCROLL_BARS) {
        groupBox->setMinimumHeight(300);
        groupBoxLayout->addWidget(widget, STRETCH_NONE);
    }
    else {
        groupBoxLayout->addWidget(widget, STRETCH_NONE); // STRETCH_MAX);
    }
    groupBoxLayout->addLayout(buttonsLayout, STRETCH_NONE);
    
    m_defaultDataFilesWidgetSize = gridWidget->sizeHint();
    
    if ( ! DATA_FILES_IN_SCROLL_BARS) {
        groupBox->setSizePolicy(groupBox->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
        //groupBoxLayout->addStretch();
    }
    
    return groupBox;
}

/**
 * Add a data file row
 *
 * @param mapFile
 *     If not NULL, the file selector is set to this file.
 */
void
BorderOptimizeDialog::addDataFileRow(CaretMappableDataFile* mapFile)
{
    const int32_t index = static_cast<int32_t>(m_optimizeDataFileSelectors.size());
    BorderOptimizeDataFileSelector* selector = new BorderOptimizeDataFileSelector(index,
                                                                                  m_optimizeDataFileTypes,
                                                                                  mapFile,
                                                                                  m_borderOptimizeDataFileGridLayout,
                                                                                  this);
    selector->setSelected(true);
    m_optimizeDataFileSelectors.push_back(selector);
}

/**
 * Called when data file's Add Row button is clicked.
 */
void
BorderOptimizeDialog::addDataFileRowToolButtonClicked()
{
    addDataFileRow(NULL);
}

/**
 * Disable all borders.
 */
void
BorderOptimizeDialog::dataFilesDisableAllSelected()
{
    setAllDataFileEnabledSelections(false);
}

/**
 * Enable all borders.
 */
void
BorderOptimizeDialog::dataFilesEnableAllSelected()
{
    setAllDataFileEnabledSelections(true);
}

void BorderOptimizeDialog::saveBrowseButtonClicked()
{
    QString path = CaretFileDialog::getExistingDirectoryDialog();
    if (path != "") m_savingDirectoryLineEdit->setText(path);
}

/**
 * Set the enable status of all borders to the given value.
 *
 * @param status
 *    New status.
 */
void
BorderOptimizeDialog::setAllDataFileEnabledSelections(const bool status)
{
    for (std::vector<BorderOptimizeDataFileSelector*>::iterator iter = m_optimizeDataFileSelectors.begin();
         iter != m_optimizeDataFileSelectors.end();
         iter++) {
        BorderOptimizeDataFileSelector* dfs = *iter;
        dfs->m_selectionCheckBox->setChecked(status);
    }
}


/**
 * @return The metric vertex areas widget.
 */
QWidget*
BorderOptimizeDialog::createVertexAreasMetricWidget()
{
    m_vertexAreasMetricFileComboBox = new CaretDataFileSelectionComboBox(this);
    
    QGroupBox* widget = new QGroupBox("Vertex Areas Metric File");
    widget->setSizePolicy(widget->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setMargin(2);

    layout->addWidget(m_vertexAreasMetricFileComboBox->getWidget());
    
    return widget;
}


/**
 * @return The options widget.
 */
QWidget*
BorderOptimizeDialog::createOptionsWidget()
{
    m_keepRegionBorderCheckBox = new QCheckBox("Keep Boundary Border");
    m_keepRegionBorderCheckBox->setChecked(true);
    
    m_outputGradientMapCheckBox = new QCheckBox("Output Gradient as Map");
    m_outputGradientMapCheckBox->setChecked(true);
    
    QLabel* gradientLabel = new QLabel("Graident Following Strength");
    m_gradientFollowingStrengthSpinBox = new QDoubleSpinBox();
    m_gradientFollowingStrengthSpinBox->setRange(0.0, 1.0e6);
    m_gradientFollowingStrengthSpinBox->setDecimals(2);
    m_gradientFollowingStrengthSpinBox->setSingleStep(1.0);
    m_gradientFollowingStrengthSpinBox->setValue(5);

    QHBoxLayout* gradientLayout = new QHBoxLayout();
    gradientLayout->addWidget(gradientLabel);
    gradientLayout->addWidget(m_gradientFollowingStrengthSpinBox);
    gradientLayout->addStretch();
    
    QGroupBox* widget = new QGroupBox("Options");
    widget->setSizePolicy(widget->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setMargin(2);
    layout->addWidget(m_keepRegionBorderCheckBox);
    layout->addWidget(m_outputGradientMapCheckBox);
    layout->addLayout(gradientLayout);
    
    return widget;
}

/**
 * @return Return keep boundary border selection status.
 */
bool
BorderOptimizeDialog::isKeepBoundaryBorderSelected() const
{
    return m_keepRegionBorderCheckBox->isChecked();
}

/**
 * @return The options widget.
 */
QWidget*
BorderOptimizeDialog::createSurfaceSelectionWidget()
{
    m_surfaceSelectionControl = new SurfaceSelectionViewController(this,
                                                                   (m_objectNamePrefix
                                                                    + ":SurfaceSelectionComboBox"));
    QObject::connect(m_surfaceSelectionControl, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(gradientComputatonSurfaceSelected(Surface*)));
    
    QGroupBox* widget = new QGroupBox("Gradient Computation Surface");
    widget->setSizePolicy(widget->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setMargin(2);
    layout->addWidget(m_surfaceSelectionControl->getWidget());
    
    return widget;
}

/**
 * @return The spherical upsampling widget.
 */
QWidget*
BorderOptimizeDialog::createSphericalUpsamplingWidget()
{
    QLabel* surfaceLabel = new QLabel("Current Sphere");
    m_upsamplingSurfaceSelectionControl = new SurfaceSelectionViewController(this,
                                                                             (m_objectNamePrefix
                                                                              + ":SphericalSurfaceSelectionComboBox"));
    
    QLabel* resolutionLabel = new QLabel("Upsampling Resolution");
    m_upsamplingResolutionSpinBox = new QSpinBox();
    m_upsamplingResolutionSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    m_upsamplingResolutionSpinBox->setSingleStep(1);
    m_upsamplingResolutionSpinBox->setToolTip("Number of vertices to use when making the upsampling sphere");
    
    m_upsamplingGroupBox = new QGroupBox(" Upsampling");
    m_upsamplingGroupBox->setSizePolicy(m_upsamplingGroupBox->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    m_upsamplingGroupBox->setCheckable(true);
    m_upsamplingGroupBox->setChecked(true);
    QGridLayout* layout = new QGridLayout(m_upsamplingGroupBox);
    layout->setMargin(2);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    int row = 0;
    layout->addWidget(surfaceLabel, row, 0);
    layout->addWidget(m_upsamplingSurfaceSelectionControl->getWidget(), row, 1);
    row++;
    layout->addWidget(resolutionLabel, row, 0);
    layout->addWidget(m_upsamplingResolutionSpinBox, row, 1, Qt::AlignLeft);
    
    return m_upsamplingGroupBox;
}

QWidget* BorderOptimizeDialog::createSavingWidget()
{
    m_savingGroupBox = new QGroupBox(" Save results");
    m_savingGroupBox->setSizePolicy(m_savingGroupBox->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    m_savingGroupBox->setCheckable(true);
    m_savingGroupBox->setChecked(true);
    
    QGridLayout* layout = new QGridLayout(m_savingGroupBox);
    layout->setMargin(2);
    layout->setColumnStretch(0, 0);//label
    layout->setColumnStretch(1, 100);//line edit
    layout->setColumnStretch(2, 0);//button or extended line edit
    
    layout->addWidget(new QLabel("Base Filename"), 0, 0);
    m_savingBaseNameLineEdit = new QLineEdit();
    layout->addWidget(m_savingBaseNameLineEdit, 0, 1, 1, -1);//extend to end of row
    layout->addWidget(new QLabel("Path"), 1, 0);
    m_savingDirectoryLineEdit = new QLineEdit();
    layout->addWidget(m_savingDirectoryLineEdit, 1, 1);
    QPushButton* browseButton = new QPushButton("Browse...");
    QObject::connect(browseButton, SIGNAL(clicked()), this, SLOT(saveBrowseButtonClicked()));
    layout->addWidget(browseButton, 1, 2);
    
    return m_savingGroupBox;
}

/**
 * Called when gradient computational surface is selected
 *
 * @param surface
 *     Surface that was selected.
 */
void
BorderOptimizeDialog::gradientComputatonSurfaceSelected(Surface* surface)
{
    if (m_surfaceSelectionModel != NULL) {
        m_surfaceSelectionModel->setSurface(surface);
    }
}

/**
 * Constructor.
 * 
 * @param optimizeDataFileTypes
 *     File types for selection
 * @param defaultFile
 *     The default file (may be NULL)
 * @param gridLayout
 *     Layout for the widgets.
 * @param parent
 *     Parent of the selector.
 */
BorderOptimizeDataFileSelector::BorderOptimizeDataFileSelector(const int32_t itemIndex,
                                                               const std::vector<DataFileTypeEnum::Enum>& optimizeDataFileTypes,
                                                               CaretMappableDataFile* defaultFile,
                                                               QGridLayout* gridLayout,
                                                               QObject* parent)
: QObject(parent)
{
    m_mapFileAndIndexSelectorObject = new CaretMappableDataFileAndMapSelectorObject(optimizeDataFileTypes,
                                                                                    CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                                    this);
    QObject::connect(m_mapFileAndIndexSelectorObject, SIGNAL(selectionWasPerformed()),
                     this, SLOT(mapFileSelectionChanged()));
    
    QWidget* mapFileComboBox;
    QWidget* mapIndexSpinBox;
    QWidget* mapNameComboBox;
    m_mapFileAndIndexSelectorObject->getWidgetsForAddingToLayout(mapFileComboBox,
                                                                 mapIndexSpinBox,
                                                                 mapNameComboBox);
    CaretAssert(mapFileComboBox);
    CaretAssert(mapIndexSpinBox);
    CaretAssert(mapNameComboBox);
    mapIndexSpinBox->setFixedWidth(80);
    
    m_allMapsCheckBox = new QCheckBox("");
    QObject::connect(m_allMapsCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(allMapsCheckBoxToggled(bool)));
    
    m_invertGradientCheckBox = new QCheckBox("");
    
    m_skipGradientCheckBox = new QCheckBox("");
    
    m_selectionCheckBox = new QCheckBox("");
    m_selectionCheckBox->setChecked(true);
    QObject::connect(m_selectionCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(selectionCheckBoxToggled(bool)));
    
    m_exclusionDistanceSpinBox = new QDoubleSpinBox();
    m_exclusionDistanceSpinBox->setRange(0, 1.0e6);
    m_exclusionDistanceSpinBox->setDecimals(2);
    m_exclusionDistanceSpinBox->setSingleStep(1.0);
    m_exclusionDistanceSpinBox->setValue(2);
    
    m_smoothingSpinBox = new QDoubleSpinBox();
    m_smoothingSpinBox->setRange(0.0, 1.0e6);
    m_smoothingSpinBox->setDecimals(2);
    m_smoothingSpinBox->setSingleStep(1.0);
    m_smoothingSpinBox->setValue(1.0);
    
    m_weightSpinBox = new QDoubleSpinBox();
    m_weightSpinBox->setRange(0.0, 1.0);
    m_weightSpinBox->setDecimals(2);
    m_weightSpinBox->setSingleStep(0.01);
    m_weightSpinBox->setValue(0.7);
    
    int32_t columnCount = 0;
    const int32_t COLUMN_SELECT  = columnCount++;
    const int32_t COLUMN_EXCLUDE = columnCount++;
    const int32_t COLUMN_SMOOTH  = columnCount++;
    const int32_t COLUMN_WEIGHT  = columnCount++;
    const int32_t COLUMN_INVERT  = columnCount++;
    const int32_t COLUMN_SKIP    = columnCount++;
    const int32_t COLUMN_ALL_MAP = columnCount++;
    const int32_t COLUMN_MAP_INDEX = columnCount++;
    const int32_t COLUMN_MAP_NAME = columnCount++;
    
    if (itemIndex == 0) {
        const int32_t row = gridLayout->rowCount();
        gridLayout->addWidget(new QLabel("Enable"),
                              row, COLUMN_SELECT,
                              2, 1,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("Exclusion"),
                              row, COLUMN_EXCLUDE,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("Radius (mm)"),
                              row+1, COLUMN_EXCLUDE,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("Smoothing"),
                              row, COLUMN_SMOOTH,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("Sigma (mm)"),
                              row+1, COLUMN_SMOOTH,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("Weight"),
                              row, COLUMN_WEIGHT,
                              2, 1,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("Invert"),
                              row, COLUMN_INVERT,
                              Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Gradient"),
                              row + 1, COLUMN_INVERT,
                              Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Skip"),
                              row, COLUMN_SKIP,
                              Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Gradient"),
                              row + 1, COLUMN_SKIP,
                              Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("All"),
                              row, COLUMN_ALL_MAP,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("Maps"),
                              row + 1, COLUMN_ALL_MAP,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("File/Map"),
                              row, COLUMN_MAP_INDEX,
                              2, 2,
                              Qt::AlignCenter);
        
        gridLayout->setVerticalSpacing(2);
        gridLayout->setColumnStretch(COLUMN_SELECT,     0);
        gridLayout->setColumnStretch(COLUMN_EXCLUDE,    0);
        gridLayout->setColumnStretch(COLUMN_SMOOTH,     0);
        gridLayout->setColumnStretch(COLUMN_WEIGHT,     0);
        gridLayout->setColumnStretch(COLUMN_INVERT,     0);
        gridLayout->setColumnStretch(COLUMN_SKIP,       0);
        gridLayout->setColumnStretch(COLUMN_ALL_MAP,    0);
        gridLayout->setColumnStretch(COLUMN_MAP_INDEX,  0);
        gridLayout->setColumnStretch(COLUMN_MAP_NAME,  100);
    }
    
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_selectionCheckBox,
                          row, COLUMN_SELECT,
                          2, 1,
                          Qt::AlignCenter);
    gridLayout->addWidget(m_exclusionDistanceSpinBox,
                          row, COLUMN_EXCLUDE,
                          2, 1,
                          Qt::AlignCenter);
    gridLayout->addWidget(m_smoothingSpinBox,
                          row, COLUMN_SMOOTH,
                          2, 1,
                          Qt::AlignCenter);
    gridLayout->addWidget(m_weightSpinBox,
                          row, COLUMN_WEIGHT,
                          2, 1,
                          Qt::AlignCenter);
    gridLayout->addWidget(m_invertGradientCheckBox,
                          row, COLUMN_INVERT,
                          2, 1,
                          Qt::AlignCenter);
    gridLayout->addWidget(m_skipGradientCheckBox,
                          row, COLUMN_SKIP,
                          2, 1,
                          Qt::AlignCenter);
    gridLayout->addWidget(m_allMapsCheckBox,
                          row, COLUMN_ALL_MAP,
                          2, 1,
                          Qt::AlignCenter);
    gridLayout->addWidget(mapFileComboBox,
                          row, COLUMN_MAP_INDEX,
                          1, 2);
    row++;
    gridLayout->addWidget(mapIndexSpinBox,
                          row, COLUMN_MAP_INDEX);
    gridLayout->addWidget(mapNameComboBox,
                          row, COLUMN_MAP_NAME);
    
    CaretMappableDataFileAndMapSelectionModel* model = m_mapFileAndIndexSelectorObject->getModel();
    if (defaultFile != NULL) {
        model->setSelectedFile(defaultFile);
    }
    else {
        m_selectionCheckBox->blockSignals(true);
        m_selectionCheckBox->setChecked(false);
        m_selectionCheckBox->blockSignals(false);
    }
    m_mapFileAndIndexSelectorObject->updateFileAndMapSelector(model);
    
    updateFileData();
}


/*
 * Destructor.
 */
BorderOptimizeDataFileSelector::~BorderOptimizeDataFileSelector()
{
    
}

/**
 * Update the file data.
 */
void
BorderOptimizeDataFileSelector::updateFileData()
{
    const bool widgetsEnabled = m_selectionCheckBox->isChecked();
    
    m_allMapsCheckBox->setEnabled(widgetsEnabled);
    m_exclusionDistanceSpinBox->setEnabled(widgetsEnabled);
    m_invertGradientCheckBox->setEnabled(widgetsEnabled);
    m_skipGradientCheckBox->setEnabled(widgetsEnabled);
    m_smoothingSpinBox->setEnabled(widgetsEnabled);
    m_weightSpinBox->setEnabled(widgetsEnabled);
    
    CaretMappableDataFileAndMapSelectionModel* model = m_mapFileAndIndexSelectorObject->getModel();
    m_mapFileAndIndexSelectorObject->updateFileAndMapSelector(model);
    m_mapFileAndIndexSelectorObject->setEnabled(widgetsEnabled);
    
    if (widgetsEnabled) {
        const CaretMappableDataFile* mapFile = model->getSelectedFile();
        const bool denseFileFlag = ((mapFile != NULL)
                                    ? (mapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE)
                                    : false);
        if (denseFileFlag) {
            m_allMapsCheckBox->setChecked(false);
            m_allMapsCheckBox->setEnabled(false);
            m_exclusionDistanceSpinBox->setEnabled(true);
            m_skipGradientCheckBox->setChecked(false);
            m_skipGradientCheckBox->setEnabled(false);
        }
        else {
            m_exclusionDistanceSpinBox->setEnabled(false);
        }
        m_allMapsCheckBox->setEnabled( ! denseFileFlag);
    }
}

/**
 * Set the selection status.
 *
 * @param selectedStatus
 *     New selection status.
 */
void
BorderOptimizeDataFileSelector::setSelected(const bool selectedStatus)
{
    m_selectionCheckBox->setChecked(selectedStatus);
}


/**
 * Gets called when the map file selection changes.
 */
void
BorderOptimizeDataFileSelector::mapFileSelectionChanged()
{
    updateFileData();
}


/**
 * @return Selections made for data file.
 */
CaretPointer<BorderOptimizeExecutor::DataFileInfo>
BorderOptimizeDataFileSelector::getSelections() const
{
    CaretMappableDataFileAndMapSelectionModel* model = m_mapFileAndIndexSelectorObject->getModel();
    
    CaretMappableDataFile* mapFile  = model->getSelectedFile();
    const int32_t          mapIndex = model->getSelectedMapIndex();
    
    CaretPointer<BorderOptimizeExecutor::DataFileInfo> dataOut(NULL);
    
    if (mapFile != NULL) {
        if ((mapIndex >= 0)
            && (mapIndex < mapFile->getNumberOfMaps())) {
            if (m_selectionCheckBox->isChecked()) {
                dataOut.grabNew(new BorderOptimizeExecutor::DataFileInfo(mapFile,
                                                                        mapIndex,
                                                                        m_allMapsCheckBox->isChecked(),
                                                                        m_smoothingSpinBox->value(),
                                                                        m_weightSpinBox->value(),
                                                                        m_invertGradientCheckBox->isChecked(),
                                                                        m_skipGradientCheckBox->isChecked(),
                                                                        m_exclusionDistanceSpinBox->value()));
            }
        }
    }
    
    return dataOut;
}

/*
 * Called when selection checkbox changes.
 * @param checked
 *    New status for selection check box.
 */
void
BorderOptimizeDataFileSelector::selectionCheckBoxToggled(bool /*checked*/)
{
    updateFileData();
}

/*
 * Called when all maps check box is toggled.
 *
 * @param checked
 *    New status for selection check box.
 */
void
BorderOptimizeDataFileSelector::allMapsCheckBoxToggled(bool /*checked*/)
{
    updateFileData();
}

