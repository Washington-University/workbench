
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __LABEL_SELECTION_WIDGET_DECLARE__
#include "LabelSelectionWidget.h"
#undef __LABEL_SELECTION_WIDGET_DECLARE__

#include <QGridLayout>
#include <QLabel>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiBrainordinateLabelFile.h"
#include "GiftiLabelTableSelectionComboBox.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "CiftiParcelLabelFile.h"
#include "GuiManager.h"
#include "LabelFile.h"
#include "MetaVolumeFile.h"
#include "VolumeFile.h"

using namespace caret;
    
/**
 * \class caret::LabelSelectionWidget 
 * \brief Widget for selection of a label from a label-type file
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param mode
 *    Mode of the widget
 * @param saveRestoreStateName
 *    Used to save the state of the widget so that state is restored next time widget is displayed
 * @param parent
 *    Optional parent widget
 */
LabelSelectionWidget::LabelSelectionWidget(const Mode mode,
                                           const QString& saveRestoreStateName,
                                           QWidget* parent)
: QWidget(parent),
m_mode(mode),
m_saveRestoreStateName(saveRestoreStateName)
{
    bool showLabelSelectionFlag(false);
    switch (m_mode) {
        case Mode::FILE_AND_MAP:
            break;
        case Mode::FILE_MAP_AND_LABEL:
            showLabelSelectionFlag = true;
            break;
    }
    
    std::vector<CaretDataFile*> allDataFiles;
    GuiManager::get()->getBrain()->getAllDataFiles(allDataFiles);
    
    std::vector<CaretMappableDataFile*> labelFiles;
    
    for (auto df : allDataFiles) {
        CaretDataFile* caretDataFile(NULL);
        
        switch (df->getDataFileType()) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                caretDataFile = df;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                caretDataFile = df;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SPARSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_MAPS:
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                break;
            case DataFileTypeEnum::CZI_IMAGE_FILE:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::HISTOLOGY_SLICES:
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::LABEL:
                caretDataFile = df;
                break;
            case DataFileTypeEnum::META_VOLUME:
            {
                MetaVolumeFile* mvf(dynamic_cast<MetaVolumeFile*>(df));
                CaretAssert(mvf);
                if (mvf->isMappedWithLabelTable()) {
                    const AString msg("Label Selection has not been implemented for MetaVolumeFile");
                    CaretLogSevere(msg);
                    CaretAssertMessage(0, msg);
                }
            }
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::METRIC_DYNAMIC:
                break;
            case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SAMPLES:
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
            {
                VolumeFile* vf(dynamic_cast<VolumeFile*>(df));
                if (vf != NULL) {
                    if (vf->getType() == SubvolumeAttributes::LABEL) {
                        caretDataFile = df;
                    }
                }
            }
                break;
            case DataFileTypeEnum::VOLUME_DYNAMIC:
                break;
        }
        
        if (caretDataFile != NULL) {
            CaretMappableDataFile* mapFile(dynamic_cast<CaretMappableDataFile*>(caretDataFile));
            if (mapFile != NULL) {
                labelFiles.push_back(mapFile);
            }
        }
    }
    
    std::vector<DataFileTypeEnum::Enum> labelFileTypes;
    labelFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL);
    labelFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL);
    labelFileTypes.push_back(DataFileTypeEnum::LABEL);
    labelFileTypes.push_back(DataFileTypeEnum::VOLUME);
    
    std::vector<SubvolumeAttributes::VolumeType> volumeTypes;
    volumeTypes.push_back(SubvolumeAttributes::LABEL);

    int columnCount = 0;
    const int COLUMN_LABEL        = columnCount++;
    const int COLUMN_MAP_LEFT     = columnCount++;
    const int COLUMN_MAP_RIGHT    = columnCount++;
    
    QLabel* fileLabel = new QLabel("File");
    QLabel* fileMapLabel = new QLabel("Map");
    
    m_fileSelector = new CaretMappableDataFileAndMapSelectorObject(labelFileTypes,
                                                                   volumeTypes,
                                                                   CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                   this);
    QObject::connect(m_fileSelector, &CaretMappableDataFileAndMapSelectorObject::selectionWasPerformed,
                     this, &LabelSelectionWidget::labelFileOrMapSelectionChanged);
    
    
    QWidget* fileComboBox(NULL);
    QWidget* fileMapSpinBox(NULL);
    QWidget* fileMapComboBox(NULL);
    
    m_fileSelector->getWidgetsForAddingToLayout(fileComboBox,
                                                fileMapSpinBox,
                                                fileMapComboBox);
    CaretAssert(fileComboBox);
    CaretAssert(fileMapSpinBox);
    CaretAssert(fileMapComboBox);
    const int INDEX_SPIN_BOX_WIDTH(100);
    fileMapSpinBox->setFixedWidth(INDEX_SPIN_BOX_WIDTH);
    fileMapSpinBox->setToolTip("Map indices start at one.");
    QLabel* fileLabellLabel = NULL;
    m_fileLabelComboBox = NULL;
    if (showLabelSelectionFlag) {
        fileLabellLabel     = new QLabel("Label");
        m_fileLabelComboBox = new GiftiLabelTableSelectionComboBox(this);
    }
    
    /*
     * Widget and layout
     */
    QGridLayout* labelWidgetLayout = new QGridLayout(this);
    labelWidgetLayout->addWidget(fileLabel, 0, COLUMN_LABEL);
    labelWidgetLayout->addWidget(fileComboBox, 0, COLUMN_MAP_LEFT, 1, 2);
    labelWidgetLayout->addWidget(fileMapLabel, 1, COLUMN_LABEL);
    labelWidgetLayout->addWidget(fileMapSpinBox, 1, COLUMN_MAP_LEFT);
    labelWidgetLayout->addWidget(fileMapComboBox, 1, COLUMN_MAP_RIGHT);
    if (fileLabellLabel != NULL) {
        labelWidgetLayout->addWidget(fileLabellLabel, 2, COLUMN_LABEL);
    }
    if (m_fileLabelComboBox != NULL) {
        labelWidgetLayout->addWidget(m_fileLabelComboBox->getWidget(), 2, COLUMN_MAP_LEFT, 1, 2);
    }
    labelWidgetLayout->setRowStretch(1000, 1000);
    labelWidgetLayout->setColumnStretch(1000, 1000);
    
    updateWidget();
    if ( ! m_saveRestoreStateName.isEmpty()) {
        restoreSelections(m_saveRestoreStateName);
    }
}

/**
 * Destructor.
 */
LabelSelectionWidget::~LabelSelectionWidget()
{
    if ( ! m_saveRestoreStateName.isEmpty()) {
        saveSelections(m_saveRestoreStateName);
    }
}

/**
 * Called when label file or map selection is made
 */
void
LabelSelectionWidget::labelFileOrMapSelectionChanged()
{
    updateWidget();
}

/**
 * Update the widget' content
 */
void
LabelSelectionWidget::updateWidget()
{
    CaretMappableDataFileAndMapSelectionModel* labelFileModel = m_fileSelector->getModel();
    m_fileSelector->updateFileAndMapSelector(labelFileModel);
    CaretMappableDataFile* labelFile = labelFileModel->getSelectedFile();
    bool labelTableComboBoxValid = false;
    if (labelFile != NULL) {
        const int32_t mapIndex = labelFileModel->getSelectedMapIndex();
        if ((mapIndex >= 0)
            && (mapIndex < labelFile->getNumberOfMaps())) {
            if (m_fileLabelComboBox != NULL) {
                GiftiLabelTable* labelTable = labelFile->getMapLabelTable(mapIndex);
                CaretAssert(labelTable);
                m_fileLabelComboBox->updateContent(labelTable);
                labelTableComboBoxValid = true;
            }
        }
    }
    if ( ! labelTableComboBoxValid) {
        if (m_fileLabelComboBox != NULL) {
            m_fileLabelComboBox->updateContent(NULL);
        }
    }
}

/**
 * @return The selected label
 */
AString
LabelSelectionWidget::getSelectedLabel() const
{
    AString labelName;
    if (m_fileLabelComboBox != NULL) {
        labelName = m_fileLabelComboBox->getSelectedLabelName();
    }
    return labelName;
}

/**
 * @return The selected file
 */
CaretMappableDataFile*
LabelSelectionWidget::getSelectedFile() const
{
    if (m_fileSelector != NULL) {
        CaretMappableDataFileAndMapSelectionModel* model(m_fileSelector->getModel());
        if (model != NULL) {
            CaretMappableDataFile* cmdf(model->getSelectedFile());
            return cmdf;
        }
    }
    return NULL;
}


/**
 * @return The selected file name
 */
AString
LabelSelectionWidget::getSelectedFileName() const
{
    const CaretMappableDataFile* cmdf(getSelectedFile());
    if (cmdf != NULL) {
        return cmdf->getFileName();
    }
    return NULL;
}

/**
 * @return The selected file name without a path
 */
AString
LabelSelectionWidget::getSelectedFileNameNoPath() const
{
    const CaretMappableDataFile* cmdf(getSelectedFile());
    if (cmdf != NULL) {
        return cmdf->getFileNameNoPath();
    }
    return NULL;
}

/**
 * @return The selected map name
 */
AString
LabelSelectionWidget::getSelectedMapName() const
{
    const CaretMappableDataFile* cmdf(getSelectedFile());
    if (cmdf != NULL) {
        const int32_t mapIndex(getSelectedMapIndex());
        if ((mapIndex >= 0)
            && (mapIndex < cmdf->getNumberOfMaps())) {
            return cmdf->getMapName(mapIndex);
        }
    }
    return "";
}

/**
 * @return The selected map index
 */
int32_t
LabelSelectionWidget::getSelectedMapIndex() const
{
    if (m_fileSelector != NULL) {
        CaretMappableDataFileAndMapSelectionModel* model(m_fileSelector->getModel());
        if (model != NULL) {
            return model->getSelectedMapIndex();
        }
    }
    return -1;
}

/**
 * Restor the selections using the given name
 * @param selectionName
 *    Name for selections
 */
void
LabelSelectionWidget::restoreSelections(const QString& selectionName)
{
    CaretAssert( ! selectionName.isEmpty());
    decltype(s_previousStates)::iterator iter(s_previousStates.find(selectionName));
    if (iter != s_previousStates.end()) {
        const QString stateString(iter->second);
        QStringList list(stateString.split(s_stateSeparator));
        if (list.size() == 3) {
            const QString filename(list[0]);
            const int32_t mapIndex(list[1].toInt());
            const QString labelName(list[2]);
            
            m_fileSelector->getModel()->setSelectedFileName(filename);
            m_fileSelector->getModel()->setSelectedMapIndex(mapIndex);
            m_fileSelector->updateFileAndMapSelector(m_fileSelector->getModel());
            updateWidget();
            if (m_fileLabelComboBox != NULL) {
                m_fileLabelComboBox->setSelectedLabelName(labelName);
            }
        }
    }
}

/**
 * Save the selections using the given name
 * @param selectionName
 *    Name for selections
 */
void
LabelSelectionWidget::saveSelections(const QString& selectionName)
{
    if (m_fileSelector->getModel()->getSelectedFile() != NULL) {
        CaretAssert( ! selectionName.isEmpty());
        
        const QString filename(m_fileSelector->getModel()->getSelectedFile()->getFileName());
        const int32_t mapIndex(m_fileSelector->getModel()->getSelectedMapIndex());
        const QString labelName(getSelectedLabel());
        
        const QString stateString(filename
                                  + s_stateSeparator
                                  + QString::number(mapIndex)
                                  + s_stateSeparator
                                  + labelName);
        s_previousStates[selectionName] = stateString;
    }
}
