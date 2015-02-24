
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

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

#include "Border.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "EventManager.h"
#include "EventUpdateInformationWindows.h"
#include "GuiManager.h"
#include "MetricFile.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceSelectionViewController.h"
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
 */
BorderOptimizeDialog::BorderOptimizeDialog(QWidget* parent)
: WuQDialogModal("Border Optimize",
                 parent),
m_surface(NULL),
m_borderEnclosingROI(NULL),
m_surfaceSelectionStructure(StructureEnum::INVALID),
m_surfaceSelectionModel(NULL),
m_vertexAreasMetricFileSelectionModel(NULL)
{
    m_optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE);
    m_optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR);
    m_optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
    m_optimizeDataFileTypes.push_back(DataFileTypeEnum::METRIC);
    
    const int STRETCH_NONE = 0;
    const int STRETCH_MAX  = 100;
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(createBorderSelectionWidget(),
                            STRETCH_NONE);
    dialogLayout->addWidget(createDataFilesWidget(),
                            STRETCH_MAX);
    dialogLayout->addWidget(createSurfaceSelectionWidget(),
                            STRETCH_NONE);
    dialogLayout->addWidget(createVertexAreasMetricWidget(),
                            STRETCH_NONE);
    QWidget* optionsWidget = createOptionsWidget();
    if (optionsWidget != NULL) {
        dialogLayout->addWidget(optionsWidget,
                                STRETCH_NONE);
    }
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
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
}

/**
 * Update the content of the dialog.
 *
 * @param surface
 *    Surface on which borders are drawn.
 * @param bordersInsideROI
 *    Borders inside the region of interest.
 * @param borderEnclosingROI
 *    Border that encloses the region of interest
 * @param nodesInsideROI
 *    Nodes inside the region of interest.
 */
void
BorderOptimizeDialog::updateDialog(Surface* surface,
                                   std::vector<Border*>& bordersInsideROI,
                                   Border* borderEnclosingROI,
                                   std::vector<int32_t>& nodesInsideROI)
{
    CaretAssert(surface);
    CaretAssert(borderEnclosingROI);
    
    m_surface            = surface;
    m_bordersInsideROI   = bordersInsideROI;
    m_borderEnclosingROI = borderEnclosingROI;
    m_nodesInsideROI     = nodesInsideROI;
    
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
        m_vertexAreasMetricFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileTypesInStructure(GuiManager::get()->getBrain(),
                                                                                                                         m_surfaceSelectionStructure,
                                                                                                                         metricDataTypes);
        m_vertexAreasMetricFileComboBox->updateComboBox(m_vertexAreasMetricFileSelectionModel);
    }
    
    if (m_vertexAreasMetricFileSelectionModel != NULL) {
        m_vertexAreasMetricFileComboBox->updateComboBox(m_vertexAreasMetricFileSelectionModel);
    }
    
    /*
     * Update borders inside ROI selections
     */
    const int32_t numberOfBorders = static_cast<int32_t>(m_bordersInsideROI.size());
    for (int32_t i = 0; i < numberOfBorders; i++) {
        if (i >= static_cast<int32_t>(m_borderCheckBoxes.size())) {
            QCheckBox* cb = new QCheckBox("");
            m_borderCheckBoxes.push_back(cb);
            m_bordersInsideROILayout->addWidget(cb);
        }
        CaretAssertVectorIndex(m_borderCheckBoxes, i);
        CaretAssertVectorIndex(m_bordersInsideROI, i);
        
        if (m_borderCheckBoxes[i]->text() != m_bordersInsideROI[i]->getName()) {
            m_borderCheckBoxes[i]->blockSignals(true);
            m_borderCheckBoxes[i]->setChecked(true);
            m_borderCheckBoxes[i]->blockSignals(false);
        }
        m_borderCheckBoxes[i]->setText(m_bordersInsideROI[i]->getName());
        m_borderCheckBoxes[i]->setVisible(true);
    }
    
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
}

/**
 * Called when OK button is clicked.
 */
void
BorderOptimizeDialog::okButtonClicked()
{
    AString errorMessage;
    
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
    
    std::vector<Border*> selectedBorders;
    getSelectedBorders(selectedBorders);
    
    if (selectedBorders.empty()) {
        errorMessage.appendWithNewLine("No optimization border files are selected.");
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    MetricFile* vertexAreasMetricFile = NULL;
    CaretDataFile* vertexAreaFile = m_vertexAreasMetricFileSelectionModel->getSelectedFile();
    if (vertexAreaFile != NULL) {
        vertexAreasMetricFile = dynamic_cast<MetricFile*>(vertexAreaFile);
    }
    
    const float gradientFollowingStrength = m_gradientFollowingStrengthSpinBox->value();
    
    BorderOptimizeExecutor::InputData algInput(selectedBorders,
                                               m_borderEnclosingROI,
                                               m_nodesInsideROI,
                                               gradientComputationSurface,
                                               dataFileSelections,
                                               vertexAreasMetricFile,
                                               gradientFollowingStrength);
    
    AString statisticsInformation;
    if (BorderOptimizeExecutor::run(algInput,
                                    statisticsInformation,
                                    errorMessage)) {
        /*
         * Display the statistics information.
         */
        EventManager::get()->sendEvent(EventUpdateInformationWindows(statisticsInformation).getPointer());
        
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
    /*
     * Allow dialog to close.
     */
    WuQDialogModal::cancelButtonClicked();
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
    
    CaretAssert(m_bordersInsideROI.size() <= m_borderCheckBoxes.size());
    const int32_t numBorderFileSelections = static_cast<int32_t>(m_bordersInsideROI.size());
    for (int32_t iBorder = 0; iBorder < numBorderFileSelections; iBorder++) {
        if (m_borderCheckBoxes[iBorder]->isVisible()) {
            if (m_borderCheckBoxes[iBorder]->isChecked()) {
                selectedBordersOut.push_back(m_bordersInsideROI[iBorder]);
            }
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
    m_bordersInsideROILayout = new QVBoxLayout(widget);
    
    return widget;
}

/**
 * @return The data files widget.
 */
QWidget*
BorderOptimizeDialog::createDataFilesWidget()
{
    QWidget* widget = new QWidget();
    m_borderOptimizeDataFileGridLayout = new QGridLayout(widget);
    
    std::vector<CaretMappableDataFile*> optimizeMapFiles;
    GuiManager::get()->getBrain()->getAllMappableDataFileWithDataFileTypes(m_optimizeDataFileTypes,
                                                                           optimizeMapFiles);
    const int32_t numMapFiles = static_cast<int32_t>(optimizeMapFiles.size());
    
    const int32_t maxFiles = 10;
    for (int32_t i = 0; i < maxFiles; i++) {
        CaretMappableDataFile* mapFile = NULL;
        if (i < numMapFiles) {
            mapFile = optimizeMapFiles[i];
        }
        addDataFileRow(mapFile);
//        BorderOptimizeDataFileSelector* selector = new BorderOptimizeDataFileSelector(i,
//                                                                                      m_optimizeDataFileTypes,
//                                                                                      mapFile,
//                                                                                      m_borderOptimizeDataFileGridLayout,
//                                                                                      this);
//        m_optimizeDataFileSelectors.push_back(selector);
    }
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    QAction* addRowAction = new QAction("Add New Data File Row",
                                        this);
    QObject::connect(addRowAction, SIGNAL(triggered(bool)),
                     this, SLOT(addDataFileRowToolButtonClicked()));
    QToolButton* addRowToolButton = new QToolButton();
    addRowToolButton->setDefaultAction(addRowAction);
    
    QGroupBox* groupBox = new QGroupBox("Data Files");
    groupBox->setMinimumHeight(300);
    QVBoxLayout* groupBoxLayout = new QVBoxLayout(groupBox);
    groupBoxLayout->addWidget(scrollArea, 100);
    groupBoxLayout->addWidget(addRowToolButton, 0, Qt::AlignLeft);
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
 * @return The metric vertex areas widget.
 */
QWidget*
BorderOptimizeDialog::createVertexAreasMetricWidget()
{
    m_vertexAreasMetricFileComboBox = new CaretDataFileSelectionComboBox(this);
    
    QGroupBox* widget = new QGroupBox("Vertex Areas Metric File");
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(m_vertexAreasMetricFileComboBox->getWidget());
    
    return widget;
}


/**
 * @return The options widget.
 */
QWidget*
BorderOptimizeDialog::createOptionsWidget()
{
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
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gradientLayout);
    
    return widget;
}

/**
 * @return The options widget.
 */
QWidget*
BorderOptimizeDialog::createSurfaceSelectionWidget()
{
    m_surfaceSelectionControl = new SurfaceSelectionViewController(this);
    QObject::connect(m_surfaceSelectionControl, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(gradientComputatonSurfaceSelected(Surface*)));
    
    QGroupBox* widget = new QGroupBox("Gradient Computation Surface");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(m_surfaceSelectionControl->getWidget());
    
    return widget;
}

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
    
    m_selectionCheckBox = new QCheckBox("");
    m_selectionCheckBox->setChecked(true);
    QObject::connect(m_selectionCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(selectionCheckBoxToggled(bool)));
    
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
    const int32_t COLUMN_SMOOTH  = columnCount++;
    const int32_t COLUMN_WEIGHT  = columnCount++;
    const int32_t COLUMN_INVERT  = columnCount++;
    const int32_t COLUMN_ALL_MAP = columnCount++;
    const int32_t COLUMN_MAP_INDEX = columnCount++;
    const int32_t COLUMN_MAP_NAME = columnCount++;
    
    if (itemIndex == 0) {
        const int32_t row = gridLayout->rowCount();
        gridLayout->addWidget(new QLabel("Enable"),
                              row, COLUMN_SELECT,
                              2, 1,
                              Qt::AlignCenter);
        gridLayout->addWidget(new QLabel("Smoothing (mm)"),
                              row, COLUMN_SMOOTH,
                              2, 1,
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
        gridLayout->setColumnStretch(COLUMN_SMOOTH,     0);
        gridLayout->setColumnStretch(COLUMN_WEIGHT,     0);
        gridLayout->setColumnStretch(COLUMN_INVERT,     0);
        gridLayout->setColumnStretch(COLUMN_ALL_MAP,    0);
        gridLayout->setColumnStretch(COLUMN_MAP_INDEX,  0);
        gridLayout->setColumnStretch(COLUMN_MAP_NAME,  100);
    }
    
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_selectionCheckBox,
                          row, COLUMN_SELECT,
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
    
    m_smoothingSpinBox->setEnabled(widgetsEnabled);
    m_weightSpinBox->setEnabled(widgetsEnabled);
    m_invertGradientCheckBox->setEnabled(widgetsEnabled);
    m_allMapsCheckBox->setEnabled(widgetsEnabled);
    
    QWidget* mapFileComboBox;
    QWidget* mapIndexSpinBox;
    QWidget* mapNameComboBox;
    m_mapFileAndIndexSelectorObject->getWidgetsForAddingToLayout(mapFileComboBox,
                                                                 mapIndexSpinBox,
                                                                 mapNameComboBox);
    const bool enableMapSelection = (widgetsEnabled
                                     && ( ! m_allMapsCheckBox->isChecked()));
    mapFileComboBox->setEnabled(widgetsEnabled);
    mapIndexSpinBox->setEnabled(enableMapSelection);
    mapNameComboBox->setEnabled(enableMapSelection);
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
                                                                        m_invertGradientCheckBox->isChecked()));
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

