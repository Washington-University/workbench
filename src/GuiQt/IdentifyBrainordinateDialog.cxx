
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

#define __IDENTIFY_BRAINORDINATE_DIALOG_DECLARE__
#include "IdentifyBrainordinateDialog.h"
#undef __IDENTIFY_BRAINORDINATE_DIALOG_DECLARE__

using namespace caret;

#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>

#include "Brain.h"
#include "BrainordinateRegionOfInterest.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CaretMappableDataFile.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiFiberTrajectoryManager.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "CiftiParcelSelectionComboBox.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabelTableSelectionComboBox.h"
#include "GuiManager.h"
#include "IdentifiedItemNode.h"
#include "IdentificationManager.h"
#include "SelectionItemCiftiConnectivityMatrixRowColumn.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "StructureEnumComboBox.h"
#include "Surface.h"
#include "SystemUtilities.h"
#include "WuQFactory.h"
#include "WuQGroupBoxExclusiveWidget.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

#include <limits>

static const int INDEX_SPIN_BOX_WIDTH = 100;



/**
 * Constructor.
 */
IdentifyBrainordinateDialog::IdentifyBrainordinateDialog(QWidget* parent)
: WuQDialogNonModal("Identify Brainordinate",
                    parent)
{
    /*
     * Surface Vertex widgets
     */
    m_surfaceVertexWidget = createSurfaceVertexlWidget();
    
    /*
     * Filter file types for CIFTI type files
     */
    std::vector<DataFileTypeEnum::Enum> allDataFileTypes;
    DataFileTypeEnum::getAllEnums(allDataFileTypes,
                                  DataFileTypeEnum::OPTIONS_INCLUDE_CONNECTIVITY_DENSE_DYNAMIC);
    
    std::vector<DataFileTypeEnum::Enum> supportedCiftiRowFileTypes;
    std::vector<DataFileTypeEnum::Enum> supportedLabelFileTypes;
    
    for (std::vector<DataFileTypeEnum::Enum>::iterator dtIter = allDataFileTypes.begin();
         dtIter != allDataFileTypes.end();
         dtIter++) {
        const DataFileTypeEnum::Enum dataFileType = *dtIter;
        bool ciftiRowFlag  = false;
        bool labelFileFlag = false;
        ParcelSourceDimension parcelSourceDimension = PARCEL_SOURCE_INVALID_DIMENSION;
        
        switch (dataFileType) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                ciftiRowFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                ciftiRowFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                labelFileFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                ciftiRowFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                parcelSourceDimension = PARCEL_SOURCE_LOADING_DIMENSION;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                parcelSourceDimension = PARCEL_SOURCE_LOADING_DIMENSION;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                parcelSourceDimension = PARCEL_SOURCE_MAPPING_DIMENSION;
                labelFileFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                parcelSourceDimension = PARCEL_SOURCE_MAPPING_DIMENSION;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                parcelSourceDimension = PARCEL_SOURCE_MAPPING_DIMENSION;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                ciftiRowFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                ciftiRowFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                ciftiRowFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::LABEL:
                labelFileFlag = true;
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SCENE:
                break;
            case DataFileTypeEnum::SPECIFICATION:
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::UNKNOWN:
                break;
            case DataFileTypeEnum::VOLUME:
                break;
        }
        
        if (parcelSourceDimension != PARCEL_SOURCE_INVALID_DIMENSION) {
            CaretAssert(parcelSourceDimension != PARCEL_SOURCE_INVALID_DIMENSION);
            m_parcelSourceDimensionMap.insert(std::make_pair(dataFileType,
                                                             parcelSourceDimension));
        }
        if (ciftiRowFlag) {
            supportedCiftiRowFileTypes.push_back(dataFileType);
        }
        if (labelFileFlag) {
            supportedLabelFileTypes.push_back(dataFileType);
        }
    }
    
    /*
     * CIFTI row widgets
     */
    m_ciftiRowWidget = createCiftiRowWidget(supportedCiftiRowFileTypes);
    
    /*
     * Label files widget
     */
    m_labelFileWidgets.m_widget = createLabelFilesWidget(supportedLabelFileTypes);
    
    /*
     * CIFTI Parcel Widgets
     */
    m_ciftiParcelWidget = createCiftiParcelWidget();
    
    m_widgetBox = new WuQGroupBoxExclusiveWidget();
    m_widgetBox->addWidget(m_ciftiRowWidget, "Identify Brainordinate from CIFTI File Row");
    m_widgetBox->addWidget(m_ciftiParcelWidget, "Identify CIFTI File Parcel");
    m_widgetBox->addWidget(m_labelFileWidgets.m_widget, "Identify Label");
    m_widgetBox->addWidget(m_surfaceVertexWidget, "Identify Surface Vertex");
    QObject::connect(m_widgetBox, SIGNAL(currentChanged(int32_t)),
                     this, SLOT(selectedWidgetChanged()));
    
    setCentralWidget(m_widgetBox,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    updateDialog();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
IdentifyBrainordinateDialog::~IdentifyBrainordinateDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_ciftiRowFileSelectionModel;
    m_ciftiRowFileSelectionModel = NULL;
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
IdentifyBrainordinateDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        uiEvent->setEventProcessed();
        updateDialog();
    }
}

/**
 * Create and return the CIFTI Parcel Widget
 *
 * @param supportedFileTypes
 *    The supported file types for this widget.
 * @return 
 *    The CIFTI Parcel Widget
 */
QWidget*
IdentifyBrainordinateDialog::createCiftiParcelWidget()
{
    int columnCount = 0;
    const int COLUMN_LABEL        = columnCount++;
    const int COLUMN_MAP_LEFT     = columnCount++;
    const int COLUMN_MAP_RIGHT    = columnCount++;
    
    m_ciftiParcelFileLabel = new QLabel("File");
    m_ciftiParcelFileMapLabel = new QLabel("Map");
    
    std::vector<DataFileTypeEnum::Enum> supportedParcelFileTypes;
    for (std::map<DataFileTypeEnum::Enum, ParcelSourceDimension>::iterator iter =  m_parcelSourceDimensionMap.begin();
         iter != m_parcelSourceDimensionMap.end();
         iter++) {
        const DataFileTypeEnum::Enum dataFileType = iter->first;
        CaretAssert(dataFileType != DataFileTypeEnum::UNKNOWN);
        supportedParcelFileTypes.push_back(dataFileType);
    }
    m_ciftiParcelFileSelector = new CaretMappableDataFileAndMapSelectorObject(supportedParcelFileTypes,
                                                                              CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                              this);
    QObject::connect(m_ciftiParcelFileSelector, SIGNAL(selectionWasPerformed()),
                     this, SLOT(slotParcelFileOrMapSelectionChanged()));
    
    
    m_ciftiParcelFileSelector->getWidgetsForAddingToLayout(m_ciftiParcelFileComboBox,
                                                           m_ciftiParcelFileMapSpinBox,
                                                           m_ciftiParcelFileMapComboBox);
    m_ciftiParcelFileMapSpinBox->setFixedWidth(INDEX_SPIN_BOX_WIDTH);
    m_ciftiParcelFileMapSpinBox->setToolTip("Map indices start at one.");
    m_ciftiParcelFileParcelLabel = new QLabel("Parcel");
    m_ciftiParcelFileParcelNameComboBox = new CiftiParcelSelectionComboBox(this);
    
    /*
     * Widget and layout
     */
    QWidget* widget = new QWidget();
    QGridLayout* ciftiParcelLayout = new QGridLayout(widget);
    ciftiParcelLayout->addWidget(m_ciftiParcelFileLabel, 0, COLUMN_LABEL);
    ciftiParcelLayout->addWidget(m_ciftiParcelFileComboBox, 0, COLUMN_MAP_LEFT, 1, 2);
    ciftiParcelLayout->addWidget(m_ciftiParcelFileMapLabel, 1, COLUMN_LABEL);
    ciftiParcelLayout->addWidget(m_ciftiParcelFileMapSpinBox, 1, COLUMN_MAP_LEFT); //, Qt::AlignLeft);
    ciftiParcelLayout->addWidget(m_ciftiParcelFileMapComboBox, 1, COLUMN_MAP_RIGHT); //, Qt::AlignLeft);
    ciftiParcelLayout->addWidget(m_ciftiParcelFileParcelLabel, 2, COLUMN_LABEL);
    ciftiParcelLayout->addWidget(m_ciftiParcelFileParcelNameComboBox->getWidget(), 2, COLUMN_MAP_LEFT, 1, 2);
    
    return widget;
}

/**
 * Create and return the CIFTI Row Widget
 *
 * @param supportedFileTypes
 *    The supported file types for this widget.
 * @return
 *    The CIFTI Row Widget
 */
QWidget*
IdentifyBrainordinateDialog::createCiftiRowWidget(const std::vector<DataFileTypeEnum::Enum>& supportedFileTypes)
{
    int columnCount = 0;
    const int COLUMN_LABEL        = columnCount++;
    const int COLUMN_MAP_LEFT     = columnCount++;
    
    /*
     * CIFTI Row selection
     */
    m_ciftiRowFileLabel = new QLabel("File");
    m_ciftiRowFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes(supportedFileTypes);
    m_ciftiRowFileComboBox = new CaretDataFileSelectionComboBox(this);
    
    m_ciftiRowFileIndexLabel = new QLabel("Row Index");
    m_ciftiRowFileIndexSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI(),
                                                                      std::numeric_limits<int>::max(),
                                                                      1);
    QObject::connect(m_ciftiRowFileIndexSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(apply()));
    m_ciftiRowFileIndexSpinBox->setFixedWidth(INDEX_SPIN_BOX_WIDTH);
    switch (CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()) {
        case 0:
            m_ciftiRowFileIndexSpinBox->setToolTip("Row indices start at zero.");
            break;
        case 1:
            m_ciftiRowFileIndexSpinBox->setToolTip("Row indices start at one.");
            break;
        default:
            CaretAssert(0);
            m_ciftiRowFileIndexSpinBox->setToolTip("PROGRAM ERROR CIFTI FILE ROW/COLUMN BASE INDEX SHOULD BE ONE OR ZERO.");
    }
    
    QWidget* widget = new QWidget();
    QGridLayout* ciftiRowLayout = new QGridLayout(widget);
    ciftiRowLayout->addWidget(m_ciftiRowFileLabel, 0, COLUMN_LABEL);
    ciftiRowLayout->addWidget(m_ciftiRowFileComboBox->getWidget(), 0, COLUMN_MAP_LEFT, 1, 2);
    ciftiRowLayout->addWidget(m_ciftiRowFileIndexLabel, 1, COLUMN_LABEL);
    ciftiRowLayout->addWidget(m_ciftiRowFileIndexSpinBox, 1, COLUMN_MAP_LEFT);
    
    return widget;
}

/**
 * Create and return the Label Files Widget
 *
 * @param supportedFileTypes
 *    The supported file types for this widget.
 * @return
 *    The Label Files Row Widget
 */
QWidget*
IdentifyBrainordinateDialog::createLabelFilesWidget(const std::vector<DataFileTypeEnum::Enum>& supportedFileTypes)
{
    int columnCount = 0;
    const int COLUMN_LABEL        = columnCount++;
    const int COLUMN_MAP_LEFT     = columnCount++;
    const int COLUMN_MAP_RIGHT    = columnCount++;
    
    m_labelFileWidgets.m_fileLabel = new QLabel("File");
    m_labelFileWidgets.m_fileMapLabel = new QLabel("Map");
    
    m_labelFileWidgets.m_fileSelector = new CaretMappableDataFileAndMapSelectorObject(supportedFileTypes,
                                                                              CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                              this);
    QObject::connect(m_labelFileWidgets.m_fileSelector, SIGNAL(selectionWasPerformed()),
                     this, SLOT(slotLabelFileOrMapSelectionChanged()));
    
    
    m_labelFileWidgets.m_fileSelector->getWidgetsForAddingToLayout(m_labelFileWidgets.m_fileComboBox,
                                                           m_labelFileWidgets.m_fileMapSpinBox,
                                                           m_labelFileWidgets.m_fileMapComboBox);
    m_labelFileWidgets.m_fileMapSpinBox->setFixedWidth(INDEX_SPIN_BOX_WIDTH);
    m_labelFileWidgets.m_fileMapSpinBox->setToolTip("Map indices start at one.");
    m_labelFileWidgets.m_fileLabellLabel = new QLabel("Label");
    m_labelFileWidgets.m_fileLabelComboBox = new GiftiLabelTableSelectionComboBox(this);
    
    /*
     * Widget and layout
     */
    QWidget* widget = new QWidget();
    QGridLayout* labelWidgetLayout = new QGridLayout(widget);
    labelWidgetLayout->addWidget(m_labelFileWidgets.m_fileLabel, 0, COLUMN_LABEL);
    labelWidgetLayout->addWidget(m_labelFileWidgets.m_fileComboBox, 0, COLUMN_MAP_LEFT, 1, 2);
    labelWidgetLayout->addWidget(m_labelFileWidgets.m_fileMapLabel, 1, COLUMN_LABEL);
    labelWidgetLayout->addWidget(m_labelFileWidgets.m_fileMapSpinBox, 1, COLUMN_MAP_LEFT); //, Qt::AlignLeft);
    labelWidgetLayout->addWidget(m_labelFileWidgets.m_fileMapComboBox, 1, COLUMN_MAP_RIGHT); //, Qt::AlignLeft);
    labelWidgetLayout->addWidget(m_labelFileWidgets.m_fileLabellLabel, 2, COLUMN_LABEL);
    labelWidgetLayout->addWidget(m_labelFileWidgets.m_fileLabelComboBox->getWidget(), 2, COLUMN_MAP_LEFT, 1, 2);
    
    return widget;
}

/**
 * Called when the user changes the label file or map.
 */
void
IdentifyBrainordinateDialog::slotLabelFileOrMapSelectionChanged()
{
    updateDialog();
}


/**
 * @return Create a Surface Vertex Widget
 */
QWidget*
IdentifyBrainordinateDialog::createSurfaceVertexlWidget()
{
    int columnCount = 0;
    const int COLUMN_LABEL        = columnCount++;
    const int COLUMN_MAP_LEFT     = columnCount++;
    
    m_vertexStructureLabel = new QLabel("Structure");
    m_vertexStructureComboBox = new StructureEnumComboBox(this);
    m_vertexStructureComboBox->listOnlyValidStructures();
    
    m_vertexIndexLabel = new QLabel("Vertex Index");
    m_vertexIndexSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0,
                                                                std::numeric_limits<int>::max(),
                                                                1);
    m_vertexIndexSpinBox->setFixedWidth(INDEX_SPIN_BOX_WIDTH);
    m_vertexIndexSpinBox->setToolTip("Vertex indices start at zero.");
    
    QWidget* widget = new QWidget();
    QGridLayout* surfaceVertexLayout = new QGridLayout(widget);
    surfaceVertexLayout->addWidget(m_vertexStructureLabel, 0, COLUMN_LABEL);
    surfaceVertexLayout->addWidget(m_vertexStructureComboBox->getWidget(), 0, COLUMN_MAP_LEFT, 1, 2);
    surfaceVertexLayout->addWidget(m_vertexIndexLabel, 1, COLUMN_LABEL);
    surfaceVertexLayout->addWidget(m_vertexIndexSpinBox, 1, COLUMN_MAP_LEFT);
    
    return widget;
}

/**
 * Called when the user changes the parcel file or map.
 */
void
IdentifyBrainordinateDialog::slotParcelFileOrMapSelectionChanged()
{
    updateDialog();
}

/**
 * Called when the selected widget changes.
 */
void
IdentifyBrainordinateDialog::selectedWidgetChanged()
{
    updateDialog();
}

/**
 * Update the dialog.
 */
void
IdentifyBrainordinateDialog::updateDialog()
{
    CaretMappableDataFileAndMapSelectionModel* parcelFileModel = m_ciftiParcelFileSelector->getModel();
    m_ciftiParcelFileSelector->updateFileAndMapSelector(parcelFileModel);
    CaretMappableDataFile* parcelFile = parcelFileModel->getSelectedFile();
    
    bool parcelsMapValidFlag = false;
    if (parcelFile != NULL) {
        const int32_t mapIndex = parcelFileModel->getSelectedMapIndex();
        if ((mapIndex >= 0)
            && (mapIndex < parcelFile->getNumberOfMaps())) {
            CiftiMappableDataFile* ciftiMapFile = dynamic_cast<CiftiMappableDataFile*>(parcelFile);
            if (ciftiMapFile != NULL) {
                const ParcelSourceDimension parcelSourceDimension = getParcelSourceDimensionFromFile(parcelFile);
                switch (parcelSourceDimension) {
                    case PARCEL_SOURCE_INVALID_DIMENSION:
                        CaretAssertMessage(0, "Should never be invalid.");
                        break;
                    case PARCEL_SOURCE_LOADING_DIMENSION:
                        m_ciftiParcelFileParcelNameComboBox->updateComboBox(ciftiMapFile->getCiftiParcelsMapForLoading());
                        parcelsMapValidFlag = true;
                        break;
                    case PARCEL_SOURCE_MAPPING_DIMENSION:
                        m_ciftiParcelFileParcelNameComboBox->updateComboBox(ciftiMapFile->getCiftiParcelsMapForBrainordinateMapping());
                        parcelsMapValidFlag = true;
                        break;
                }
            }
        }
    }
    if ( ! parcelsMapValidFlag) {
        m_ciftiParcelFileParcelNameComboBox->updateComboBox(NULL);
    }
    
    CaretMappableDataFileAndMapSelectionModel* labelFileModel = m_labelFileWidgets.m_fileSelector->getModel();
    m_labelFileWidgets.m_fileSelector->updateFileAndMapSelector(labelFileModel);
    CaretMappableDataFile* labelFile = labelFileModel->getSelectedFile();
    bool labelTableComboBoxValid = false;
    if (labelFile != NULL) {
        const int32_t mapIndex = labelFileModel->getSelectedMapIndex();
        if ((mapIndex >= 0)
            && (mapIndex < labelFile->getNumberOfMaps())) {
            GiftiLabelTable* labelTable = labelFile->getMapLabelTable(mapIndex);
            m_labelFileWidgets.m_fileLabelComboBox->updateContent(labelTable);
            labelTableComboBoxValid = true;
        }
    }
    if ( ! labelTableComboBoxValid) {
        m_labelFileWidgets.m_fileLabelComboBox->updateContent(NULL);
    }
    
    
    m_ciftiRowFileComboBox->updateComboBox(m_ciftiRowFileSelectionModel);
    
    m_vertexStructureComboBox->listOnlyValidStructures();
}

/**
 * Get the source of parcels for a caret data file.
 *
 * @param mapFile
 *    The caret data file.
 * @return 
 *    The source for parcels (loading or mapping dimension).
 */
IdentifyBrainordinateDialog::ParcelSourceDimension
IdentifyBrainordinateDialog::getParcelSourceDimensionFromFile(const CaretMappableDataFile* mapFile)
{
    ParcelSourceDimension parcelSourceDim = PARCEL_SOURCE_INVALID_DIMENSION;
    std::map<DataFileTypeEnum::Enum, ParcelSourceDimension>::iterator typeIter =  m_parcelSourceDimensionMap.find(mapFile->getDataFileType());
    if (typeIter != m_parcelSourceDimensionMap.end()) {
        parcelSourceDim = typeIter->second;
    }
    CaretAssert(parcelSourceDim != PARCEL_SOURCE_INVALID_DIMENSION);
    
    return parcelSourceDim;
}

/**
 * Gets called when Apply button is clicked.
 */
void
IdentifyBrainordinateDialog::applyButtonClicked()
{
    AString errorMessage;
    
    QWidget* selectedWidget = m_widgetBox->currentWidget();
    if (m_surfaceVertexWidget == selectedWidget) {
        processSurfaceVertexWidget(errorMessage);
    }
    else if (m_ciftiParcelWidget == selectedWidget) {
        processCiftiParcelWidget(errorMessage);
    }
    else if (m_ciftiRowWidget == selectedWidget) {
        processCiftiRowWidget(errorMessage);
    }
    else if (m_labelFileWidgets.m_widget == selectedWidget) {
        processLabelFileWidget(errorMessage);
    }
    else {
        const QString msg("Choose one of the methods for identifying a brainordinate.");
        errorMessage = (WuQtUtilities::createWordWrappedToolTipText(msg));
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    WuQDialogNonModal::applyButtonClicked();
}

/**
 * Update coloring and redraw all windows.
 */
void
IdentifyBrainordinateDialog::updateColoringAndDrawAllWindows()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Flash the brainordinate highlighting region of interest.
 *
 * @param brainROI
 *     brainordinate highlighting region of interest that is flashed.
 */
void
IdentifyBrainordinateDialog::flashBrainordinateHighlightingRegionOfInterest(BrainordinateRegionOfInterest* brainROI)
{
    const float   flashDelayTime = 0.25;
    const int32_t flashCount     = 4;
    for (int32_t iFlash = 0; iFlash < flashCount; iFlash++) {
        brainROI->setBrainordinateHighlightingEnabled(true);
        updateColoringAndDrawAllWindows();
        SystemUtilities::sleepSeconds(flashDelayTime);
        brainROI->setBrainordinateHighlightingEnabled(false);
        updateColoringAndDrawAllWindows();
        SystemUtilities::sleepSeconds(flashDelayTime);
    }
}

/**
 * Process user's selectons in the Label File Widget
 *
 * @param errorMessageOut
 *    Output containing error message.
 */
void
IdentifyBrainordinateDialog::processLabelFileWidget(AString& errorMessageOut)
{
    Brain* brain = GuiManager::get()->getBrain();
    
    CaretMappableDataFile* mapFile = m_labelFileWidgets.m_fileSelector->getModel()->getSelectedFile();
    const int32_t mapIndex = m_labelFileWidgets.m_fileSelector->getModel()->getSelectedMapIndex();
    if (mapFile != NULL) {
        const AString labelName = m_labelFileWidgets.m_fileLabelComboBox->getSelectedLabelName();
        
        BrainordinateRegionOfInterest* brainROI = brain->getBrainordinateHighlightRegionOfInterest();
        if (brainROI->setWithLabelFileLabel(mapFile,
                                        mapIndex,
                                        labelName,
                                            errorMessageOut)) {
            flashBrainordinateHighlightingRegionOfInterest(brainROI);
        }
    }
}

/**
 * Process user's selectons in the CIFTI Parcel Widget
 *
 * @param errorMessageOut
 *    Output containing error message.
 */
void
IdentifyBrainordinateDialog::processCiftiParcelWidget(AString& errorMessageOut)
{
    Brain* brain = GuiManager::get()->getBrain();
    
    CaretMappableDataFile* mapFile = m_ciftiParcelFileSelector->getModel()->getSelectedFile();
    const int32_t mapIndex = m_ciftiParcelFileSelector->getModel()->getSelectedMapIndex();
    if (mapFile != NULL) {
        CiftiMappableDataFile* ciftiFile = dynamic_cast<CiftiMappableDataFile*>(mapFile);
        const QString parcelName = m_ciftiParcelFileParcelNameComboBox->getSelectedParcelName();
        const ParcelSourceDimension parcelSourceDimension = getParcelSourceDimensionFromFile(ciftiFile);

        BrainordinateRegionOfInterest* brainROI = brain->getBrainordinateHighlightRegionOfInterest();
        switch (parcelSourceDimension) {
            case PARCEL_SOURCE_INVALID_DIMENSION:
                CaretAssertMessage(0, "Should never be invalid.");
                break;
            case PARCEL_SOURCE_LOADING_DIMENSION:
                brainROI->setWithCiftiParcelLoadingBrainordinates(ciftiFile,
                                                                  mapIndex,
                                                                  parcelName,
                                                                  errorMessageOut);
                break;
            case PARCEL_SOURCE_MAPPING_DIMENSION:
                brainROI->setWithCiftiParcelMappingBrainordinates(ciftiFile,
                                                                  mapIndex,
                                                                  parcelName,
                                                                  errorMessageOut);
                break;
        }
        
        
        if (brainROI->hasSurfaceNodes()
            || brainROI->hasVolumeVoxels()) {
            /*
             * Need to load data?
             */
            if (parcelSourceDimension == PARCEL_SOURCE_LOADING_DIMENSION) {
                CiftiMappableConnectivityMatrixDataFile* connMatrixFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(ciftiFile);
                if (connMatrixFile != NULL) {
                    SelectionManager* selectionManager = brain->getSelectionManager();
                    selectionManager->reset();
                    selectionManager->setAllSelectionsEnabled(true);
                    SelectionItemCiftiConnectivityMatrixRowColumn* selectionCiftiRowColumn = selectionManager->getCiftiConnectivityMatrixRowColumnIdentification();

                    const CiftiParcelsMap* ciftiParcelsMap = connMatrixFile->getCiftiParcelsMapForLoading();
                    const int64_t parcelIndex = ciftiParcelsMap->getIndexFromNumberOrName(parcelName);
                    if (parcelIndex >= 0) {
                        switch (connMatrixFile->getChartMatrixLoadingDimension()) {
                            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                                selectionCiftiRowColumn->setFileColumn(connMatrixFile,
                                                                       parcelIndex);
                                break;
                            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                                selectionCiftiRowColumn->setFileRow(connMatrixFile,
                                                                    parcelIndex);
                                break;
                        }
                        
                        GuiManager::get()->processIdentification(-1, // invalid tab index
                                                                 selectionManager,
                                                                 this);
                    }
                    else {
                        errorMessageOut = ("Parcel name="
                                           + parcelName
                                           + " not found in parcels map for file "
                                           + connMatrixFile->getFileNameNoPath());
                    }
                    
                    

                }
            }
            
            /*
             * Highlight the selected parcel
             */
            flashBrainordinateHighlightingRegionOfInterest(brainROI);
        }
        else {
            brainROI->setBrainordinateHighlightingEnabled(false);
            updateColoringAndDrawAllWindows();
        }
        
    }
    else {
        errorMessageOut = ("No parcel-type file is selected.");
    }
}

/**
 * Process user's selectons in the
 *
 * @param errorMessageOut
 *    Output containing error message.
 */
void
IdentifyBrainordinateDialog::processCiftiRowWidget(AString& errorMessageOut)
{
    Brain* brain = GuiManager::get()->getBrain();
    
    SelectionManager* selectionManager = brain->getSelectionManager();
    selectionManager->reset();
    selectionManager->setAllSelectionsEnabled(true);
    
    CaretDataFile* dataFile = m_ciftiRowFileSelectionModel->getSelectedFile();
    if (dataFile != NULL) {
        CiftiMappableDataFile*    ciftiMapFile  = dynamic_cast<CiftiMappableDataFile*>(dataFile);
        CiftiFiberTrajectoryFile* ciftiTrajFile = dynamic_cast<CiftiFiberTrajectoryFile*>(dataFile);
        
        const int32_t selectedCiftiRowIndex = m_ciftiRowFileIndexSpinBox->value() - m_ciftiRowFileIndexSpinBox->minimum();
        
        try {
            StructureEnum::Enum surfaceStructure;
            int32_t surfaceNodeIndex;
            int32_t surfaceNumberOfNodes;
            bool surfaceNodeValid = false;
            int64_t voxelIJK[3];
            float voxelXYZ[3];
            bool voxelValid = false;
            if (ciftiMapFile != NULL) {
                ciftiMapFile->getBrainordinateFromRowIndex(selectedCiftiRowIndex,
                                                           surfaceStructure,
                                                           surfaceNodeIndex,
                                                           surfaceNumberOfNodes,
                                                           surfaceNodeValid,
                                                           voxelIJK,
                                                           voxelXYZ,
                                                           voxelValid);
            }
            else if (ciftiTrajFile != NULL) {
                ciftiTrajFile->getBrainordinateFromRowIndex(selectedCiftiRowIndex,
                                                            surfaceStructure,
                                                            surfaceNodeIndex,
                                                            surfaceNumberOfNodes,
                                                            surfaceNodeValid,
                                                            voxelIJK,
                                                            voxelXYZ,
                                                            voxelValid);
            }
            else {
                errorMessageOut = "Neither CIFTI Mappable nor CIFTI Trajectory file.  Has new file type been added?";
            }
            
            
            if (surfaceNodeValid) {
                SelectionItemSurfaceNode* surfaceID = selectionManager->getSurfaceNodeIdentification();
                const Surface* surface = brain->getPrimaryAnatomicalSurfaceForStructure(surfaceStructure);
                if (surface != NULL) {
                    if ((surfaceNodeIndex >= 0)
                        && (surfaceNodeIndex < surface->getNumberOfNodes())) {
                        surfaceID->setSurface(const_cast<Surface*>(surface));
                        surfaceID->setBrain(brain);
                        const float* xyz = surface->getCoordinate(surfaceNodeIndex);
                        const double doubleXYZ[3] = { xyz[0], xyz[1], xyz[2] };
                        surfaceID->setModelXYZ(doubleXYZ);
                        surfaceID->setNodeNumber(surfaceNodeIndex);
                        
                        GuiManager::get()->processIdentification(-1, // invalid tab index
                                                                 selectionManager,
                                                                 this);
                    }
                    else {
                        errorMessageOut = ("Surface vertex index "
                                        + AString::number(surfaceNodeIndex)
                                        + " is not valid for surface "
                                        + surface->getFileNameNoPath());
                    }
                }
                else{
                    errorMessageOut = ("No surfaces are loaded for structure "
                                    + StructureEnum::toGuiName(surfaceStructure));
                }
                
            }
            else if (voxelValid) {
                SelectionItemVoxel* voxelID = selectionManager->getVoxelIdentification();
                voxelID->setBrain(brain);
                voxelID->setEnabledForSelection(true);
                voxelID->setVoxelIdentification(brain,
                                                ciftiMapFile,
                                                voxelIJK,
                                                0.0);
                const double doubleXYZ[3] = { voxelXYZ[0], voxelXYZ[1], voxelXYZ[2] };
                voxelID->setModelXYZ(doubleXYZ);
                
                GuiManager::get()->processIdentification(-1, // invalid tab index
                                                         selectionManager,
                                                         this);
            }
        }
        catch (const DataFileException& dfe) {
            errorMessageOut = dfe.whatString();
        }
    }
}

/**
 * Process user's selectons in the
 *
 * @param errorMessageOut
 *    Output containing error message.
 */
void
IdentifyBrainordinateDialog::processSurfaceVertexWidget(AString& errorMessageOut)
{
    Brain* brain = GuiManager::get()->getBrain();
    
    SelectionManager* selectionManager = brain->getSelectionManager();
    selectionManager->reset();
    
    
    const StructureEnum::Enum selectedStructure   = m_vertexStructureComboBox->getSelectedStructure();
    const int32_t selectedVertexIndex = m_vertexIndexSpinBox->value();
    
    BrainStructure* bs = brain->getBrainStructure(selectedStructure,
                                                  false);
    if (bs != NULL) {
        if (selectedVertexIndex < bs->getNumberOfNodes()) {
            Surface* surface = bs->getPrimaryAnatomicalSurface();
            if (surface != NULL) {
                SelectionItemSurfaceNode* nodeID = selectionManager->getSurfaceNodeIdentification();
                nodeID->setBrain(brain);
                nodeID->setNodeNumber(selectedVertexIndex);
                nodeID->setSurface(surface);
                const float* fxyz = surface->getCoordinate(selectedVertexIndex);
                const double xyz[3] = { fxyz[0], fxyz[1], fxyz[2] };
                nodeID->setModelXYZ(xyz);
                GuiManager::get()->processIdentification(-1,
                                                         selectionManager,
                                                         this);
            }
            else {
                errorMessageOut = ("PROGRAM ERROR: Primary Anatomical Surface not found for structure "
                                + StructureEnum::toName(selectedStructure));
            }
        }
        else {
            errorMessageOut = ("Vertex Index "
                               + AString::number(selectedVertexIndex)
                               + " is out of range [0, "
                               + AString::number(bs->getNumberOfNodes() - 1)
                               + "] for "
                               + StructureEnum::toGuiName(selectedStructure));
        }
    }
    else {
        errorMessageOut = ("Structure "
                        + StructureEnum::toName(selectedStructure)
                        + " not found.");
    }
}


