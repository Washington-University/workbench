
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

#include <iostream>

#define __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__
#include "CiftiConnectivityMatrixViewController.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QSignalMapper>

#include "Brain.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiConnectivityMatrixParcelDynamicFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiParcelScalarFile.h"
#include "CiftiConnectivityMatrixParcelDynamicFile.h"
#include "ConnectivityCorrelationSettingsMenu.h"
#include "CursorDisplayScoped.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FiberTrajectoryMapProperties.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "MetricDynamicConnectivityFile.h"
#include "VolumeDynamicConnectivityFile.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

static const char* FILE_POINTER_PROPERTY_NAME = "filePointer";

/**
 * \class caret::CiftiConnectivityMatrixViewController 
 * \brief View-Controller connectivity files
 * \ingroup GuiQt
 */
/**
 * Constructor.
 *
 * @param parentObjectName
 *    Name of parent object for macros
 * @param parent
 *    The parent widget
 */
CiftiConnectivityMatrixViewController::CiftiConnectivityMatrixViewController(const QString& parentObjectName,
                                                                             QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(parentObjectName
                   + ":Connectivity")
{
    m_gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_gridLayout, 2, 2);
    m_gridLayout->setColumnStretch(COLUMN_ENABLE_CHECKBOX, 0);
    m_gridLayout->setColumnStretch(COLUMN_LAYER_CHECKBOX, 0);
    m_gridLayout->setColumnStretch(COLUMN_COPY_BUTTON, 0);
    m_gridLayout->setColumnStretch(COLUMN_OPTIONS_BUTTON, 0);
    m_gridLayout->setColumnStretch(COLUMN_NAME_LINE_EDIT, 100);
    m_gridLayout->setColumnStretch(COLUMN_ORIENTATION_FILE_COMBO_BOX, 100);
    const int titleRow = m_gridLayout->rowCount();
    m_gridLayout->addWidget(new QLabel("Load"),
                            titleRow, COLUMN_ENABLE_CHECKBOX);
    m_gridLayout->addWidget(new QLabel("Layer"),
                            titleRow, COLUMN_LAYER_CHECKBOX);
    m_gridLayout->addWidget(new QLabel("Copy"),
                            titleRow, COLUMN_COPY_BUTTON);
    m_gridLayout->addWidget(new QLabel("Options"),
                            titleRow, COLUMN_OPTIONS_BUTTON);
    m_gridLayout->addWidget(new QLabel("Connectivity File"),
                            titleRow, COLUMN_NAME_LINE_EDIT);
    m_gridLayout->addWidget(new QLabel("Fiber Orientation File"),
                            titleRow, COLUMN_ORIENTATION_FILE_COMBO_BOX);
    
    m_signalMapperFileEnableCheckBox = new QSignalMapper(this);
#if QT_VERSION >= 0x060000
    QObject::connect(m_signalMapperFileEnableCheckBox, &QSignalMapper::mappedInt,
                     this, &CiftiConnectivityMatrixViewController::enabledCheckBoxClicked);
#else
    QObject::connect(m_signalMapperFileEnableCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(enabledCheckBoxClicked(int)));
#endif
    
    m_signalMapperLayerCheckBox = new QSignalMapper(this);
#if QT_VERSION >= 0x060000
    QObject::connect(m_signalMapperLayerCheckBox, &QSignalMapper::mappedInt,
                     this, &CiftiConnectivityMatrixViewController::layerCheckBoxClicked);
#else
    QObject::connect(m_signalMapperLayerCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(layerCheckBoxClicked(int)));
#endif
    
    m_signalMapperFileCopyToolButton = new QSignalMapper(this);
#if QT_VERSION >= 0x060000
    QObject::connect(m_signalMapperFileCopyToolButton, &QSignalMapper::mappedInt,
                     this, &CiftiConnectivityMatrixViewController::copyToolButtonClicked);
#else
    QObject::connect(m_signalMapperFileCopyToolButton, SIGNAL(mapped(int)),
                     this, SLOT(copyToolButtonClicked(int)));
#endif
    
    m_signalMapperOptionsToolButton = new QSignalMapper(this);
#if QT_VERSION >= 0x060000
    QObject::connect(m_signalMapperOptionsToolButton, &QSignalMapper::mappedInt,
                     this, &CiftiConnectivityMatrixViewController::optionsButtonClicked);
#else
    QObject::connect(m_signalMapperOptionsToolButton, SIGNAL(mapped(int)),
                     this, SLOT(optionsButtonClicked(int)));
#endif
    
    m_signalMapperFiberOrientationFileComboBox = new QSignalMapper(this);
#if QT_VERSION >= 0x060000
    QObject::connect(m_signalMapperFiberOrientationFileComboBox, &QSignalMapper::mappedInt,
                     this, &CiftiConnectivityMatrixViewController::fiberOrientationFileComboBoxActivated);
#else
    QObject::connect(m_signalMapperFiberOrientationFileComboBox, SIGNAL(mapped(int)),
                     this, SLOT(fiberOrientationFileComboBoxActivated(int)));
#endif
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(m_gridLayout);
    layout->addStretch();
    
    s_allCiftiConnectivityMatrixViewControllers.insert(this);

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixViewController::~CiftiConnectivityMatrixViewController()
{
    s_allCiftiConnectivityMatrixViewControllers.erase(this);
    
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update this view controller.
 */
void 
CiftiConnectivityMatrixViewController::updateViewController()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    std::vector<CaretMappableDataFile*> files;
    
    std::vector<CiftiFiberTrajectoryFile*> trajFiles;
    brain->getConnectivityFiberTrajectoryFiles(trajFiles);
    for (std::vector<CiftiFiberTrajectoryFile*>::iterator trajIter = trajFiles.begin();
         trajIter != trajFiles.end();
         trajIter++) {
        files.push_back(*trajIter);
    }
    
    std::vector<CiftiMappableConnectivityMatrixDataFile*> matrixFiles;
    brain->getAllCiftiConnectivityMatrixFiles(matrixFiles);
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator matrixIter = matrixFiles.begin();
         matrixIter != matrixFiles.end();
         matrixIter++) {
        files.push_back(*matrixIter);
    }

    std::vector<MetricDynamicConnectivityFile*> metricDynConnFiles;
    brain->getMetricDynamicConnectivityFiles(metricDynConnFiles);
    files.insert(files.end(),
                 metricDynConnFiles.begin(), metricDynConnFiles.end());
    
    std::vector<VolumeDynamicConnectivityFile*> volumeDynConnFiles;
    brain->getVolumeDynamicConnectivityFiles(volumeDynConnFiles);
    files.insert(files.end(),
                 volumeDynConnFiles.begin(), volumeDynConnFiles.end());
    
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    const int32_t numFiles = static_cast<int32_t>(files.size());

    for (int32_t i = 0; i < numFiles; i++) {
        QCheckBox* checkBox = NULL;
        QCheckBox* layerCheckBox = NULL;
        QLineEdit* lineEdit = NULL;
        QToolButton* copyToolButton = NULL;
        QToolButton* optionsToolButton = NULL;
        QComboBox* comboBox = NULL;
        
        if (i < static_cast<int32_t>(m_fileEnableCheckBoxes.size())) {
            checkBox          = m_fileEnableCheckBoxes[i];
            layerCheckBox     = m_layerCheckBoxes[i];
            lineEdit          = m_fileNameLineEdits[i];
            copyToolButton    = m_fileCopyToolButtons[i];
            optionsToolButton = m_optionsToolButtons[i];
            comboBox          = m_fiberOrientationFileComboBoxes[i];
        }
        else {
            
            const QString objectNamePrefix(m_objectNamePrefix
                                           + QString("%1").arg((int)i+1, 2, 10, QLatin1Char('0'))
                                           + ":");
            const QString descriptivePrefix("connectivity file " + QString::number(i+1));
            
            checkBox = new QCheckBox("");
            checkBox->setToolTip("When selected, load data during\n"
                                 "an identification operation");
            m_fileEnableCheckBoxes.push_back(checkBox);
            checkBox->setObjectName(objectNamePrefix
                                         + "Enable");
            macroManager->addMacroSupportToObject(checkBox,
                                                  "Enable " + descriptivePrefix);
            
            const AString dynToolTip("This option is enabled only for dynamic connectivity files.  "
                                     "When checked, this dynamic connectivity file will appear in the Overlay Layers' File selection combo box.  "
                                     "Dynamic connectivity files do not explicitly exist but allow dynamic "
                                     "computation of connectivity from a CIFTI data-series, metric, or volume file.  "
                                     "In Preferences, one may set the default to show/hide dynamic connectivity files.");
            layerCheckBox = new QCheckBox("");
            WuQtUtilities::setWordWrappedToolTip(layerCheckBox, dynToolTip);
            m_layerCheckBoxes.push_back(layerCheckBox);
            layerCheckBox->setObjectName(objectNamePrefix
                                         + "EnableLayer");
            macroManager->addMacroSupportToObject(layerCheckBox,
                                                  "Enable dynamic connectivity for " + descriptivePrefix);
            
            lineEdit = new QLineEdit();
            lineEdit->setReadOnly(true);
            m_fileNameLineEdits.push_back(lineEdit);
            
            copyToolButton = new QToolButton();
            copyToolButton->setText("Copy");
            copyToolButton->setToolTip("Copy loaded connectivity data into a writable file that is added to layers");
            m_fileCopyToolButtons.push_back(copyToolButton);
            copyToolButton->setObjectName(objectNamePrefix
                                         + "CopyButton");
            macroManager->addMacroSupportToObject(copyToolButton,
                                                  "Copy load row to new CIFTI scalar or Volume file for " + descriptivePrefix);
            
            optionsToolButton = new QToolButton();
            optionsToolButton->setText("Opts");
            optionsToolButton->setToolTip("Dynamic connectivity options for correlation and covariance");
            m_optionsToolButtons.push_back(optionsToolButton);
            optionsToolButton->setObjectName(objectNamePrefix
                                             + "OptionsButton");
            macroManager->addMacroSupportToObject(optionsToolButton,
                                                  "Options for dynamic connectivity");
            
            comboBox = new QComboBox();
            m_fiberOrientationFileComboBoxes.push_back(comboBox);
            comboBox->setToolTip("Select Fiber Orientation File");
            comboBox->setObjectName(objectNamePrefix
                                         + "FiberOrientationFile");
            macroManager->addMacroSupportToObject(comboBox,
                                                  "Select fiber orientation for " + descriptivePrefix);
            
            QObject::connect(copyToolButton, SIGNAL(clicked()),
                             m_signalMapperFileCopyToolButton, SLOT(map()));
            m_signalMapperFileCopyToolButton->setMapping(copyToolButton, i);
            
            QObject::connect(optionsToolButton, SIGNAL(clicked()),
                             m_signalMapperOptionsToolButton, SLOT(map()));
            m_signalMapperOptionsToolButton->setMapping(optionsToolButton, i);
            
            QObject::connect(checkBox, SIGNAL(clicked(bool)),
                             m_signalMapperFileEnableCheckBox, SLOT(map()));
            m_signalMapperFileEnableCheckBox->setMapping(checkBox, i);
            
            QObject::connect(layerCheckBox, SIGNAL(clicked(bool)),
                             m_signalMapperLayerCheckBox, SLOT(map()));
            m_signalMapperLayerCheckBox->setMapping(layerCheckBox, i);
            
            QObject::connect(comboBox, SIGNAL(activated(int)),
                             m_signalMapperFiberOrientationFileComboBox, SLOT(map()));
            m_signalMapperFiberOrientationFileComboBox->setMapping(comboBox, i);
            
            const int row = m_gridLayout->rowCount();
            m_gridLayout->addWidget(checkBox,
                                    row, COLUMN_ENABLE_CHECKBOX);
            m_gridLayout->addWidget(layerCheckBox,
                                    row, COLUMN_LAYER_CHECKBOX);
            m_gridLayout->addWidget(copyToolButton,
                                    row, COLUMN_COPY_BUTTON);
            m_gridLayout->addWidget(optionsToolButton,
                                    row, COLUMN_OPTIONS_BUTTON);
            m_gridLayout->addWidget(lineEdit,
                                    row, COLUMN_NAME_LINE_EDIT);
            m_gridLayout->addWidget(comboBox,
                                    row, COLUMN_ORIENTATION_FILE_COMBO_BOX);
        }
        
        const CiftiMappableConnectivityMatrixDataFile* matrixFile = dynamic_cast<const CiftiMappableConnectivityMatrixDataFile*>(files[i]);
        const CiftiFiberTrajectoryFile* trajFile = dynamic_cast<const CiftiFiberTrajectoryFile*>(files[i]);
        const VolumeDynamicConnectivityFile* volDynConnFile = dynamic_cast<VolumeDynamicConnectivityFile*>(files[i]);
        const MetricDynamicConnectivityFile* metricDynConnFile = dynamic_cast<MetricDynamicConnectivityFile*>(files[i]);
        const CiftiConnectivityMatrixDenseDynamicFile* dynConnFile = dynamic_cast<const CiftiConnectivityMatrixDenseDynamicFile*>(files[i]);
        const CiftiConnectivityMatrixParcelDynamicFile* parcelDynConnFile = dynamic_cast<const CiftiConnectivityMatrixParcelDynamicFile*>(files[i]);

        bool checkStatus = false;
        if (dynConnFile != NULL) {
            checkStatus = dynConnFile->isMapDataLoadingEnabled(0);
        }
        else if (parcelDynConnFile != NULL) {
            checkStatus = parcelDynConnFile->isMapDataLoadingEnabled(0);
        }
        else if (matrixFile != NULL) {
            checkStatus = matrixFile->isMapDataLoadingEnabled(0);
        }
        else if (trajFile != NULL) {
            checkStatus = trajFile->isDataLoadingEnabled();
        }
        else if (volDynConnFile != NULL) {
            checkStatus = volDynConnFile->isDataLoadingEnabled();
        }
        else if (metricDynConnFile != NULL) {
            checkStatus = metricDynConnFile->isDataLoadingEnabled();
        }
        else {
            CaretAssertMessage(0, "Has a new file type been added?");
        }
        
        checkBox->setChecked(checkStatus);
        checkBox->setProperty(FILE_POINTER_PROPERTY_NAME,
                              QVariant::fromValue((void*)files[i]));

        bool dynFileFlag(false);
        if (dynConnFile != NULL) {
            dynFileFlag = true;
            layerCheckBox->setChecked(dynConnFile->isEnabledAsLayer());
        }
        else if (volDynConnFile != NULL) {
            dynFileFlag = true;
            layerCheckBox->setChecked(volDynConnFile->isEnabledAsLayer());
        }
        else if (metricDynConnFile != NULL) {
            dynFileFlag = true;
            layerCheckBox->setChecked(metricDynConnFile->isEnabledAsLayer());
        }
        else if (parcelDynConnFile != NULL) {
            dynFileFlag = true;
            layerCheckBox->setChecked(parcelDynConnFile->isEnabledAsLayer());
        }
        else if (matrixFile != NULL) {
            layerCheckBox->setChecked(matrixFile->isEnabledAsLayer());
        }
        else {
            layerCheckBox->setChecked(false);
        }
        
        lineEdit->setText(files[i]->getFileName());
        optionsToolButton->setEnabled(dynFileFlag);
    }


    const int32_t numItems = static_cast<int32_t>(m_fileEnableCheckBoxes.size());
    for (int32_t i = 0; i < numItems; i++) {
        bool layerCheckBoxValid = false;
        bool showRow = false;
        bool showOrientationComboBox = false;
        if (i < numFiles) {
            showRow = true;
            if (dynamic_cast<CiftiFiberTrajectoryFile*>(files[i]) != NULL) {
                showOrientationComboBox = true;
            }
            if (dynamic_cast<CiftiConnectivityMatrixDenseDynamicFile*>(files[i]) != NULL) {
                layerCheckBoxValid = true;
            }
            else if (dynamic_cast<MetricDynamicConnectivityFile*>(files[i]) != NULL) {
                layerCheckBoxValid = true;
            }
            else if (dynamic_cast<VolumeDynamicConnectivityFile*>(files[i]) != NULL) {
                layerCheckBoxValid = true;
            }
        }
        
        m_fileEnableCheckBoxes[i]->setVisible(showRow);
        m_layerCheckBoxes[i]->setVisible(showRow);
        m_layerCheckBoxes[i]->setEnabled(layerCheckBoxValid);
        m_fileCopyToolButtons[i]->setVisible(showRow);
        m_fileNameLineEdits[i]->setVisible(showRow);
        m_fiberOrientationFileComboBoxes[i]->setVisible(showOrientationComboBox);
        m_fiberOrientationFileComboBoxes[i]->setEnabled(showOrientationComboBox);
        m_optionsToolButtons[i]->setVisible(showRow);
    }
    
    updateFiberOrientationComboBoxes();
}

/**
 * Update the fiber orientation combo boxes.
 */
void
CiftiConnectivityMatrixViewController::updateFiberOrientationComboBoxes()
{
    std::vector<CiftiFiberOrientationFile*> orientationFiles;
    GuiManager::get()->getBrain()->getConnectivityFiberOrientationFiles(orientationFiles);
    const int32_t numOrientationFiles = static_cast<int32_t>(orientationFiles.size());
    
    const int32_t numItems = static_cast<int32_t>(m_fiberOrientationFileComboBoxes.size());
    for (int32_t i = 0; i < numItems; i++) {
        QComboBox* comboBox = m_fiberOrientationFileComboBoxes[i];
        CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
        CiftiConnectivityMatrixDenseDynamicFile* ciftiDenseDynConnFile = NULL;
        CiftiConnectivityMatrixParcelDynamicFile* ciftiParcelDynConnFile(NULL);
        CiftiFiberTrajectoryFile* trajFile = NULL;
        MetricDynamicConnectivityFile* metricDynConnFile(NULL);
        VolumeDynamicConnectivityFile* volDynConnFile = NULL;
        
        if (comboBox->isEnabled()) {
            getFileAtIndex(i,
                           matrixFile,
                           ciftiDenseDynConnFile,
                           ciftiParcelDynConnFile,
                           trajFile,
                           metricDynConnFile,
                           volDynConnFile);
        }
        
        if (trajFile != NULL) {
            int32_t selectedIndex = 0;
            CiftiFiberOrientationFile* selectedOrientationFile = trajFile->getMatchingFiberOrientationFile();
            
            comboBox->clear();
            
            for (int32_t iOrient = 0; iOrient < numOrientationFiles; iOrient++) {
                CiftiFiberOrientationFile* orientFile = orientationFiles[iOrient];
                if (trajFile->isFiberOrientationFileCombatible(orientFile)) {
                    if (orientFile == selectedOrientationFile) {
                        selectedIndex = iOrient;
                    }
                    
                    comboBox->addItem(orientFile->getFileNameNoPath(),
                                      QVariant::fromValue((void*)orientFile));
                }
            }
            
            if ((selectedIndex >= 0)
                && (selectedIndex < comboBox->count())) {
                comboBox->setCurrentIndex(selectedIndex);
            }
        }
        else {
            comboBox->clear();
        }
    }
}

/**
 * Called when an enabled check box changes state.
 *
 * @param indx
 *    Index of checkbox that was clicked.
 */
void
CiftiConnectivityMatrixViewController::enabledCheckBoxClicked(int indx)
{
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    const bool newStatus = m_fileEnableCheckBoxes[indx]->isChecked();
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiConnectivityMatrixDenseDynamicFile* ciftiDenseDynConnFile = NULL;
    CiftiConnectivityMatrixParcelDynamicFile* ciftiParcelDynConnFile(NULL);
    CiftiFiberTrajectoryFile* trajFile = NULL;
    VolumeDynamicConnectivityFile* volDynConnFile(NULL);
    MetricDynamicConnectivityFile* metricDynConnFile(NULL);

    getFileAtIndex(indx,
                   matrixFile,
                   ciftiDenseDynConnFile,
                   ciftiParcelDynConnFile,
                   trajFile,
                   metricDynConnFile,
                   volDynConnFile);
    
    if (matrixFile != NULL) {
        matrixFile->setMapDataLoadingEnabled(0,
                                             newStatus);
    }
    else if (metricDynConnFile != NULL) {
        metricDynConnFile->setDataLoadingEnabled(newStatus);
    }
    else if (trajFile != NULL) {
        trajFile->setDataLoadingEnabled(newStatus);
    }
    else if (volDynConnFile != NULL) {
        volDynConnFile->setDataLoadingEnabled(newStatus);
    }
    else {
        CaretAssertMessage(0, "Has a new file type been added?");
    }
    
    updateOtherCiftiConnectivityMatrixViewControllers();
}

/**
 * Called when a layer check box changes state.
 *
 * @param indx
 *    Index of checkbox that was clicked.
 */
void
CiftiConnectivityMatrixViewController::layerCheckBoxClicked(int indx)
{
    CaretAssertVectorIndex(m_layerCheckBoxes, indx);
    const bool newStatus = m_layerCheckBoxes[indx]->isChecked();
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiConnectivityMatrixDenseDynamicFile* ciftiDenseDynConnFile = NULL;
    CiftiConnectivityMatrixParcelDynamicFile* ciftiParcelDynConnFile(NULL);
    CiftiFiberTrajectoryFile* trajFile = NULL;
    MetricDynamicConnectivityFile* metricDynConnFile(NULL);
    VolumeDynamicConnectivityFile* volDynConnFile(NULL);

    getFileAtIndex(indx,
                   matrixFile,
                   ciftiDenseDynConnFile,
                   ciftiParcelDynConnFile,
                   trajFile,
                   metricDynConnFile,
                   volDynConnFile);
    
    if (matrixFile != NULL) {
        CiftiConnectivityMatrixDenseDynamicFile* dynConnFile = dynamic_cast<CiftiConnectivityMatrixDenseDynamicFile*>(matrixFile);
        if (dynConnFile != NULL) {
            dynConnFile->setEnabledAsLayer(newStatus);
        }
        CiftiConnectivityMatrixParcelDynamicFile* parcelDynConnFile(dynamic_cast<CiftiConnectivityMatrixParcelDynamicFile*>(matrixFile));
        if (parcelDynConnFile != NULL) {
            parcelDynConnFile->setEnabledAsLayer(newStatus);
        }
    }
    else if (metricDynConnFile != NULL) {
        metricDynConnFile->setEnabledAsLayer(newStatus);
    }
    else if (volDynConnFile != NULL) {
        volDynConnFile->setEnabledAsLayer(newStatus);
    }
    else if (trajFile != NULL) {
        CaretAssertMessage(0, "Should never get caled for fiber trajectory file");
    }
    else {
        CaretAssertMessage(0, "Has a new file type been added?");
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Get the file associated with the given index.  One of the output files
 * will be NULL and the other will be non-NULL.
 *
 * @param indx
 *    The index.
 * @param ciftiMatrixFileOut
 *    If there is a CIFTI matrix file at the given index, this will be non-NULL.
 * @param ciftiDenseDynConnFileOut
 *    If ther eis a CIFTI dense dyn conn file at the given index, this will be non-NULL
 * @param ciftiParcelDynConnFileOut
 *    If ther eis a CIFTI parcel dyn conn file at the given index, this will be non-NULL
 * @param ciftiTrajFileOut
 *    If there is a CIFTI trajectory file at the given index, this will be non-NULL.
 * @param metricDynConnFileOut
 *    If there is a Metric Dynamic file at the given index, this will be non-NULL
 * @param volDynConnFileOut
 *    If there is a volume dynamnic connectivity files at the given index, this will be non-NULL
 */
void
CiftiConnectivityMatrixViewController::getFileAtIndex(const int32_t indx,
                                                      CiftiMappableConnectivityMatrixDataFile* &ciftiMatrixFileOut,
                                                      CiftiConnectivityMatrixDenseDynamicFile* &ciftiDenseDynConnFileOut,
                                                      CiftiConnectivityMatrixParcelDynamicFile* &ciftiParcelDynConnFileOut,
                                                      CiftiFiberTrajectoryFile* &ciftiTrajFileOut,
                                                      MetricDynamicConnectivityFile* &metricDynConnFileOut,
                                                      VolumeDynamicConnectivityFile* &volDynConnFileOut)
{
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    void* ptr = m_fileEnableCheckBoxes[indx]->property(FILE_POINTER_PROPERTY_NAME).value<void*>();
    CaretMappableDataFile* mapFilePointer = (CaretMappableDataFile*)ptr;
    
    ciftiMatrixFileOut  = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(mapFilePointer);
    ciftiDenseDynConnFileOut = dynamic_cast<CiftiConnectivityMatrixDenseDynamicFile*>(mapFilePointer);
    ciftiParcelDynConnFileOut = dynamic_cast<CiftiConnectivityMatrixParcelDynamicFile*>(mapFilePointer);
    ciftiTrajFileOut    = dynamic_cast<CiftiFiberTrajectoryFile*>(mapFilePointer);
    metricDynConnFileOut = dynamic_cast<MetricDynamicConnectivityFile*>(mapFilePointer);
    volDynConnFileOut   = dynamic_cast<VolumeDynamicConnectivityFile*>(mapFilePointer);
    
    AString name = "";
    if (mapFilePointer != NULL) {
        name = mapFilePointer->getFileNameNoPath();
    }

    if (ciftiMatrixFileOut != NULL) {
        /* OK */
    }
    else if (ciftiDenseDynConnFileOut != NULL) {
        /* OK */
    }
    else if (ciftiParcelDynConnFileOut != NULL) {
        /* OK */
    }
    else if (ciftiTrajFileOut != NULL) {
        /* OK */
    }
    else if (metricDynConnFileOut != NULL) {
        /* OK */
    }
    else if (volDynConnFileOut != NULL) {
        /* OK */
    }
    else {
        CaretAssertMessage(0,
                           "Has a new file type been added?");
    }
}

/**
 * Called when fiber orientation file combo box changed.
 *
 * @param indx
 *    Index of combo box that was changed.
 */
void
CiftiConnectivityMatrixViewController::fiberOrientationFileComboBoxActivated(int indx)
{
    CaretAssertVectorIndex(m_fiberOrientationFileComboBoxes, indx);
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiConnectivityMatrixDenseDynamicFile* ciftiDenseDynConnFile = NULL;
    CiftiConnectivityMatrixParcelDynamicFile* ciftiParcelDynConnFile(NULL);
    CiftiFiberTrajectoryFile* trajFile = NULL;
    MetricDynamicConnectivityFile* metricDynConnFile(NULL);
    VolumeDynamicConnectivityFile* volDynConnFile(NULL);
    
    getFileAtIndex(indx,
                   matrixFile,
                   ciftiDenseDynConnFile,
                   ciftiParcelDynConnFile,
                   trajFile,
                   metricDynConnFile,
                   volDynConnFile);
    
    CaretAssertMessage(trajFile,
                       "Selected orientation file but trajectory file is invalid.");
    
    QComboBox* cb = m_fiberOrientationFileComboBoxes[indx];
    void* ptr = cb->itemData(indx,
                             Qt::UserRole).value<void*>();
    CaretAssert(ptr);
    CiftiFiberOrientationFile* orientFile = (CiftiFiberOrientationFile*)ptr;
    
    std::vector<CiftiFiberOrientationFile*> orientationFiles;
    GuiManager::get()->getBrain()->getConnectivityFiberOrientationFiles(orientationFiles);
    if (std::find(orientationFiles.begin(),
                  orientationFiles.end(),
                  orientFile) == orientationFiles.end()) {
        CaretAssertMessage(0,
                           "Selected fiber orientation file is no longer valid.");
        return;
    }
    
    trajFile->setMatchingFiberOrientationFile(orientFile);
    
    updateOtherCiftiConnectivityMatrixViewControllers();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}


/**
 * Called when copy tool button is clicked.
 *
 * @param indx
 *    Index of copy tool button that was clicked.
 */
void
CiftiConnectivityMatrixViewController::copyToolButtonClicked(int indx)
{
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiConnectivityMatrixDenseDynamicFile* ciftiDenseDynConnFile = NULL;
    CiftiConnectivityMatrixParcelDynamicFile* ciftiParcelDynConnFile(NULL);
    CiftiFiberTrajectoryFile* trajFile = NULL;
    MetricDynamicConnectivityFile* metricDynConnFile(NULL);
    VolumeDynamicConnectivityFile* volDynConnFile(NULL);

    getFileAtIndex(indx,
                   matrixFile,
                   ciftiDenseDynConnFile,
                   ciftiParcelDynConnFile,
                   trajFile,
                   metricDynConnFile,
                   volDynConnFile);
    
    CiftiConnectivityMatrixParcelFile* parcelMatrixFile = dynamic_cast<CiftiConnectivityMatrixParcelFile*>(matrixFile);
    
    bool errorFlag = false;
    AString errorMessage;
    
    const AString directoryName = GuiManager::get()->getBrain()->getCurrentDirectory();
    if ((ciftiParcelDynConnFile != NULL)
        || (parcelMatrixFile != NULL)) {
        CiftiParcelScalarFile* parcelScalarFile(CiftiParcelScalarFile::newInstanceFromRowInCiftiConnectivityMatrixFile(matrixFile,
                                                                                                                       directoryName,
                                                                                                                       errorMessage));
        if (parcelScalarFile != NULL) {
            EventDataFileAdd dataFileAdd(parcelScalarFile);
            EventManager::get()->sendEvent(dataFileAdd.getPointer());
            
            if (dataFileAdd.isError()) {
                errorMessage = dataFileAdd.getErrorMessage();
                errorFlag = true;
            }
        }
        else {
            errorFlag = true;
        }
    }
    else if (matrixFile != NULL) {
        CiftiBrainordinateScalarFile* scalarFile =
        CiftiBrainordinateScalarFile::newInstanceFromRowInCiftiConnectivityMatrixFile(matrixFile,
                                                                                      directoryName,
                                                                                      errorMessage);
        if (scalarFile != NULL) {
            EventDataFileAdd dataFileAdd(scalarFile);
            EventManager::get()->sendEvent(dataFileAdd.getPointer());
            
            if (dataFileAdd.isError()) {
                errorMessage = dataFileAdd.getErrorMessage();
                errorFlag = true;
            }
        }
        else {
            errorFlag = true;
        }
    }
    else if (volDynConnFile != NULL) {
        VolumeFile* newVolumeFile = volDynConnFile->newVolumeFileFromLoadedData(directoryName,
                                                                               errorMessage);
        if (newVolumeFile != NULL) {
            EventDataFileAdd dataFileAdd(newVolumeFile);
            EventManager::get()->sendEvent(dataFileAdd.getPointer());
            
            if (dataFileAdd.isError()) {
                errorMessage = dataFileAdd.getErrorMessage();
                errorFlag = true;
            }
        }
        else {
            errorFlag = true;
        }
    }
    else if (metricDynConnFile != NULL) {
        MetricFile* newMetricFile = metricDynConnFile->newMetricFileFromLoadedData(directoryName,
                                                                                errorMessage);
        if (newMetricFile != NULL) {
            EventDataFileAdd dataFileAdd(newMetricFile);
            EventManager::get()->sendEvent(dataFileAdd.getPointer());
            
            if (dataFileAdd.isError()) {
                errorMessage = dataFileAdd.getErrorMessage();
                errorFlag = true;
            }
        }
        else {
            errorFlag = true;
        }
    }
    else if (trajFile != NULL) {
        CiftiFiberTrajectoryFile* newTrajFile = trajFile->newFiberTrajectoryFileFromLoadedRowData(directoryName,
                                                                                                  errorMessage);

        if (newTrajFile != NULL) {
            EventDataFileAdd dataFileAdd(newTrajFile);
            EventManager::get()->sendEvent(dataFileAdd.getPointer());
            
            if (dataFileAdd.isError()) {
                errorMessage = dataFileAdd.getErrorMessage();
                errorFlag = true;
            }
        }
        else {
            errorFlag = true;
        }
    }
    else {
        CaretAssertMessage(0,
                           "Has a new file type been added?");
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    cursor.restoreCursor();
    
    if (errorFlag) {
        WuQMessageBox::errorOk(m_fileCopyToolButtons[indx],
                               errorMessage);
    }
}

/**
 * Called when options tool button is clicked.
 *
 * @param indx
 *    Index of option tool button that was clicked.
 */
void
CiftiConnectivityMatrixViewController::optionsButtonClicked(int indx)
{
    CaretAssertVectorIndex(m_optionsToolButtons, indx);
    QToolButton* toolButton(m_optionsToolButtons[indx]);

    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiConnectivityMatrixDenseDynamicFile* ciftiDenseDynConnFile = NULL;
    CiftiConnectivityMatrixParcelDynamicFile* ciftiParcelDynConnFile(NULL);
    CiftiFiberTrajectoryFile* trajFile = NULL;
    MetricDynamicConnectivityFile* metricDynConnFile(NULL);
    VolumeDynamicConnectivityFile* volDynConnFile(NULL);
    
    getFileAtIndex(indx,
                   matrixFile,
                   ciftiDenseDynConnFile,
                   ciftiParcelDynConnFile,
                   trajFile,
                   metricDynConnFile,
                   volDynConnFile);
    ConnectivityCorrelationSettings* settings(NULL);
    if (ciftiDenseDynConnFile != NULL) {
        settings = ciftiDenseDynConnFile->getCorrelationSettings();
    }
    else if (ciftiParcelDynConnFile != NULL) {
        settings = ciftiParcelDynConnFile->getCorrelationSettings();
    }
    else if (metricDynConnFile != NULL) {
        settings = metricDynConnFile->getCorrelationSettings();
    }
    else if (volDynConnFile != NULL) {
        settings = volDynConnFile->getCorrelationSettings();
    }
    
    if (settings != NULL) {
        ConnectivityCorrelationSettingsMenu menu(settings,
                                                 this);
        menu.exec(toolButton->mapToGlobal(QPoint(0,0)));
    }
}

/**
 * Update other connectivity view controllers other than 'this' instance
 * that contain the same connectivity file.
 */
void 
CiftiConnectivityMatrixViewController::updateOtherCiftiConnectivityMatrixViewControllers()
{
    for (std::set<CiftiConnectivityMatrixViewController*>::iterator iter = s_allCiftiConnectivityMatrixViewControllers.begin();
         iter != s_allCiftiConnectivityMatrixViewControllers.end();
         iter++) {
        CiftiConnectivityMatrixViewController* clvc = *iter;
        if (clvc != this) {
            clvc->updateViewController();
        }
    }
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
CiftiConnectivityMatrixViewController::receiveEvent(Event* event)
{
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isConnectivityUpdate()
            || uiEvent->isToolBoxUpdate()) {
            this->updateViewController();
            uiEvent->setEventProcessed();
        }
    }
}



