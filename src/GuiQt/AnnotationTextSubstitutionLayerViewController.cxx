
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __ANNOTATION_TEXT_SUBSTITUTION_LAYER_VIEW_CONTROLLER_DECLARE__
#include "AnnotationTextSubstitutionLayerViewController.h"
#undef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_VIEW_CONTROLLER_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QSpinBox>
#include <QToolButton>

#include "AnnotationTextSubstitutionFile.h"
#include "AnnotationTextSubstitutionLayer.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "EventAnnotationTextSubstitutionInvalidate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventUserInterfaceUpdate.h"
#include "MapYokingGroupComboBox.h"
#include "WuQGridLayoutGroup.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTextSubstitutionLayerViewController 
 * \brief View controller for a text substitution layer
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationTextSubstitutionLayerViewController::AnnotationTextSubstitutionLayerViewController(QGridLayout* gridLayout,
                                                                                             const int32_t layerIndex,
                                                                                             const QString& parentObjectName,
                                                                                             QObject* parent)
: QObject(parent),
m_layerIndex(layerIndex)
{
    /*
     * May hide construction button if layers are not editable
     */
    const bool showConstructionToolButtonFlag(false);
    
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    
    QString objectNamePrefix = QString(parentObjectName
                                       + ":AnnTextSubsLayer%1"
                                       + ":").arg((int)(m_layerIndex + 1), 2, 10, QLatin1Char('0'));
    const QString descriptivePrefix = QString("layer %1").arg(m_layerIndex + 1);

    m_enabledCheckBox = new QCheckBox();
    m_enabledCheckBox->setToolTip("Enable text substitutions using selected file");
    QObject::connect(m_enabledCheckBox, &QCheckBox::clicked,
                     this, &AnnotationTextSubstitutionLayerViewController::enabledCheckBoxClicked);
    m_enabledCheckBox->setObjectName(objectNamePrefix + ":m_enabledCheckBox");
    macroManager->addMacroSupportToObject(m_enabledCheckBox, "Enable CheckBox");
    
    /*
     * Construction Tool Button
     * Note: macro support is on each action in menu in 'createConstructionMenu'
     */
    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                               constructionIcon);
    m_constructionAction = new QAction(this);
    if (constructionIconValid) {
        m_constructionAction->setIcon(constructionIcon);
    }
    else {
        m_constructionAction->setText("M");
    }
    m_constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(m_constructionToolButton,
                                                     descriptivePrefix,
                                                     (objectNamePrefix
                                                      + "ConstructionMenu:"));
    m_constructionAction->setMenu(constructionMenu);
    m_constructionToolButton->setDefaultAction(m_constructionAction);
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);

    if ( ! showConstructionToolButtonFlag) {
        m_constructionToolButton->setVisible(false);
    }
    
    m_groupIdComboBox = new QComboBox();
    m_groupIdComboBox->setToolTip("Select annotation text substitution Group ID");
    m_groupIdComboBox->setObjectName(objectNamePrefix + "m_groupIdComboBox");
    macroManager->addMacroSupportToObject(m_groupIdComboBox, "Group ID Combo Box");
    
    QObject::connect(m_groupIdComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &AnnotationTextSubstitutionLayerViewController::groupIdComboBoxActivated);
    
    m_yokingComboBox = new MapYokingGroupComboBox(this);
    QObject::connect(m_yokingComboBox, &MapYokingGroupComboBox::itemActivated,
                     this, &AnnotationTextSubstitutionLayerViewController::yokingGroupComboBoxActivated);
    m_yokingComboBox->getWidget()->setObjectName(objectNamePrefix + "m_yokingComboBox");
    macroManager->addMacroSupportToObject(m_yokingComboBox->getWidget(), "Map Yoking Group");
    
    m_rowIndexSpinBox = new QSpinBox();
    m_rowIndexSpinBox->setToolTip("Select row index from annotation text substitution file");
    m_rowIndexSpinBox->setMinimumWidth(60);
    m_rowIndexSpinBox->setObjectName(objectNamePrefix + "m_rowIndexSpinBox");
    macroManager->addMacroSupportToObject(m_rowIndexSpinBox, "Row Index Spin Box");
#if QT_VERSION >= 0x050700
    QObject::connect(m_rowIndexSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &AnnotationTextSubstitutionLayerViewController::rowIndexSpinBoxValueChanged);
#else
    QObject::connect(m_rowIndexSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &AnnotationTextSubstitutionLayerViewController::rowIndexSpinBoxValueChanged);
#endif

    m_fileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    m_fileSelectionComboBox->getWidget()->setToolTip("Select annotation text substitution file");
    m_fileSelectionComboBox->getWidget()->setObjectName(objectNamePrefix + "m_fileSelectionComboBox");
    macroManager->addMacroSupportToObject(m_fileSelectionComboBox->getWidget(), "Substitution File Combo Box");
    QObject::connect(m_fileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     this, &AnnotationTextSubstitutionLayerViewController::fileComboBoxSelected);
    
    /*
     * Use layout group so that items can be shown/hidden
     */
    
    m_gridLayoutGroup = new WuQGridLayoutGroup(gridLayout, this);
    
    const bool multiRowLayoutFlag(true);
    if (multiRowLayoutFlag) {
        int row(m_gridLayoutGroup->rowCount());
        if (layerIndex == 0) {
            QLabel* onLabel       = new QLabel("On");
            QLabel* modLabel      = new QLabel("Mod");
            QLabel* groupIDLabel  = new QLabel("Group ID");
            QLabel* yokeLabel     = new QLabel("Yoke");
            QLabel* indexLabel    = new QLabel("Row");
            m_gridLayoutGroup->addWidget(onLabel, row, 0, Qt::AlignCenter);
            m_gridLayoutGroup->addWidget(modLabel, row, 1, Qt::AlignHCenter);
            m_gridLayoutGroup->addWidget(groupIDLabel, row, 2, Qt::AlignHCenter);
            m_gridLayoutGroup->addWidget(yokeLabel, row, 3, Qt::AlignHCenter);
            m_gridLayoutGroup->addWidget(indexLabel, row, 4, Qt::AlignHCenter);
            
            gridLayout->setColumnStretch(5, 100);
            gridLayout->setVerticalSpacing(gridLayout->verticalSpacing() / 2);
            
            ++row;
            
            if ( ! showConstructionToolButtonFlag) {
                modLabel->setVisible(false);
            }
        }
        m_gridLayoutGroup->addWidget(m_enabledCheckBox, row, 0, 2, 1, Qt::AlignCenter);
        m_gridLayoutGroup->addWidget(m_constructionToolButton, row, 1, 2, 1, Qt::AlignCenter);
        m_gridLayoutGroup->addWidget(m_groupIdComboBox, row, 2);
        m_gridLayoutGroup->addWidget(m_yokingComboBox->getWidget(), row, 3, Qt::AlignHCenter);
        m_gridLayoutGroup->addWidget(m_rowIndexSpinBox, row, 4, Qt::AlignLeft);
        ++row;
        m_gridLayoutGroup->addWidget(m_fileSelectionComboBox->getWidget(), row, 2, 1, 4);
    }
    else {
        int row(m_gridLayoutGroup->rowCount());
        if (layerIndex == 0) {
            QLabel* onLabel       = new QLabel("On");
            QLabel* modLabel      = new QLabel("Mod");
            QLabel* groupIDLabel  = new QLabel("Group ID");
            QLabel* yokeLabel     = new QLabel("Yoke");
            QLabel* indexLabel    = new QLabel("Row");
            QLabel* subsFileLabel = new QLabel("Substitution File");
            m_gridLayoutGroup->addWidget(onLabel, row, 0, Qt::AlignHCenter);
            m_gridLayoutGroup->addWidget(modLabel, row, 1, Qt::AlignHCenter);
            m_gridLayoutGroup->addWidget(groupIDLabel, row, 2, Qt::AlignHCenter);
            m_gridLayoutGroup->addWidget(yokeLabel, row, 3, Qt::AlignHCenter);
            m_gridLayoutGroup->addWidget(indexLabel, row, 4, Qt::AlignHCenter);
            m_gridLayoutGroup->addWidget(subsFileLabel, row, 5, Qt::AlignLeft);
            
            ++row;

            if ( ! showConstructionToolButtonFlag) {
                modLabel->setVisible(false);
            }
        }
        m_gridLayoutGroup->addWidget(m_enabledCheckBox, row, 0, Qt::AlignHCenter);
        m_gridLayoutGroup->addWidget(m_constructionToolButton, row, 1, Qt::AlignHCenter);
        m_gridLayoutGroup->addWidget(m_groupIdComboBox, row, 2);
        m_gridLayoutGroup->addWidget(m_yokingComboBox->getWidget(), row, 3, Qt::AlignHCenter);
        m_gridLayoutGroup->addWidget(m_rowIndexSpinBox, row, 4);
        m_gridLayoutGroup->addWidget(m_fileSelectionComboBox->getWidget(), row, 5);
    }
}

/**
 * Destructor.
 */
AnnotationTextSubstitutionLayerViewController::~AnnotationTextSubstitutionLayerViewController()
{
}

/**
 * Update the content
 * @param layer
 *    Layer for use in this view controll (may be NULL)
 */
void
AnnotationTextSubstitutionLayerViewController::updateViewController(AnnotationTextSubstitutionLayer* layer)
{
    m_layer = layer;
    
    bool validLayerFlag(false);
    if (m_layer != NULL) {
        m_enabledCheckBox->setChecked(m_layer->isEnabled());
        
        const AString selectedGroupID(m_layer->getGroupIdentifer());
        int32_t selectedGroupIndex(0);
        const std::vector<AString>& allGroupIDs(m_layer->getAllGroupIdentifiers());
        const int32_t numGroupIDs(allGroupIDs.size());
        m_groupIdComboBox->clear();
        for (int32_t i = 0; i < numGroupIDs; i++) {
            CaretAssertVectorIndex(allGroupIDs, i);
            m_groupIdComboBox->addItem(allGroupIDs[i]);
            if (selectedGroupID == allGroupIDs[i]) {
                selectedGroupIndex = i;
            }
        }
        if ((selectedGroupIndex >= 0)
            && (selectedGroupIndex < numGroupIDs)) {
            m_groupIdComboBox->setCurrentIndex(selectedGroupIndex);
        }

        int32_t numRows(0);
        const AnnotationTextSubstitutionFile* annSubsFile(m_layer->getSelectedSubstitutionFile());
        if (annSubsFile != NULL) {
            numRows = annSubsFile->getNumberOfMaps();
            validLayerFlag = (numRows > 0);
        }
        m_rowIndexSpinBox->setRange(0, numRows - 1);
        m_yokingComboBox->setMapYokingGroup(m_layer->getMapYokingGroup());
        m_rowIndexSpinBox->setValue(m_layer->getSubstitutionFileRowIndex());
        m_fileSelectionComboBox->updateComboBox(m_layer->getSubstitutionFileSelectionModel());
    }
    
    m_enabledCheckBox->setEnabled(validLayerFlag);
    m_groupIdComboBox->setEnabled(validLayerFlag);
    m_rowIndexSpinBox->setEnabled(validLayerFlag);
    m_yokingComboBox->getWidget()->setEnabled(validLayerFlag);
    m_fileSelectionComboBox->getWidget()->setEnabled(validLayerFlag);
}

/**
 * Set the visibility of this view controller
 */
void
AnnotationTextSubstitutionLayerViewController::setVisible(bool visible)
{
    CaretAssert(m_gridLayoutGroup);
    m_gridLayoutGroup->setVisible(visible);
}

/**
 * Update graphics after controls changed
 */
void
AnnotationTextSubstitutionLayerViewController::updateGraphics()
{
    /* Text substitutions may have changed so need to invalidate all substitutions */
    EventManager::get()->sendEvent(EventAnnotationTextSubstitutionInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Update the user interface
 */
void
AnnotationTextSubstitutionLayerViewController::updateUserInterface()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Create the construction menu.
 * @param parent
 *    Parent widget.
 * @param descriptivePrefix
 *    Descriptive prefix
 * @param menuActionNamePrefix
 *    Prefix for macros
 */
QMenu*
AnnotationTextSubstitutionLayerViewController::createConstructionMenu(QWidget* /*parent*/,
                                                                      const AString& /*descriptivePrefix*/,
                                                                      const AString& /*menuActionNamePrefix*/)
{
    QMenu* menu(new QMenu());
    return menu;
}

/**
 * Called when enabled check box is clicked
 * @param checked
 *    New checked stats
 */
void
AnnotationTextSubstitutionLayerViewController::enabledCheckBoxClicked(bool checked)
{
    if (m_layer != NULL) {
        m_layer->setEnabled(checked);
        updateGraphics();
    }
}

/**
 * Called when group ID combo box is changed
 * @param index
 *    Index of items selected
 */
void
AnnotationTextSubstitutionLayerViewController::groupIdComboBoxActivated(const int index)
{
    if (m_layer != NULL) {
        if ((index >= 0)
            && (index < m_groupIdComboBox->count())) {
            m_layer->setGroupIdentifer(m_groupIdComboBox->currentText());
        }
        updateGraphics();
    }
}

/**
 * Called when index spin box value is changed
 * @param index
 *    Index of items selected
 */
void
AnnotationTextSubstitutionLayerViewController::rowIndexSpinBoxValueChanged(const int index)
{
    if (m_layer != NULL) {
        MapYokingGroupEnum::Enum mapYoking = m_yokingComboBox->getMapYokingGroup();
        if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            CaretMappableDataFile* nullMapFile(NULL);
            HistologySlicesFile* nullHistologySlicesFile(NULL);
            MediaFile* nullMediaFile(NULL);
            AnnotationTextSubstitutionFile* nullTextSubFile(NULL);
            EventMapYokingSelectMap selectMapEvent(mapYoking,
                                                   nullMapFile,
                                                   nullTextSubFile,
                                                   nullHistologySlicesFile,
                                                   nullMediaFile,
                                                   index,
                                                   MapYokingGroupEnum::MediaAllFramesStatus::ALL_FRAMES_OFF,
                                                   true);
            EventManager::get()->sendEvent(selectMapEvent.getPointer());
            updateUserInterface();
        }
        else {
            m_layer->setSubstitutionFileRowIndex(index);
        }
        updateGraphics();
    }
}

/**
 * Called when map yoking group combo box is changed
 */
void
AnnotationTextSubstitutionLayerViewController::yokingGroupComboBoxActivated()
{
    if (m_layer != NULL) {
        AnnotationTextSubstitutionFile* atsf(m_layer->getSelectedSubstitutionFile());
        if (atsf != NULL) {
            m_yokingComboBox->validateYokingChange(m_layer);
            updateGraphics();
            updateUserInterface();
        }
    }
}

/**
 * Called when file combo box is changed
 * @param caretDataFile
 *    File that was selected
 */
void
AnnotationTextSubstitutionLayerViewController::fileComboBoxSelected(CaretDataFile* /*caretDataFile*/)
{
    if (m_layer != NULL) {
        /*
         * The file selection model gets set by the combo box
         */
        updateGraphics();
    }
}
