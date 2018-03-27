
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

#include <QBoxLayout>
#include <QCheckBox>
#include <QFocusEvent>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>

#define __OVERLAY_SETTINGS_EDITOR_DIALOG_DECLARE__
#include "OverlaySettingsEditorDialog.h"
#undef __OVERLAY_SETTINGS_EDITOR_DIALOG_DECLARE__

#include "CaretMappableDataFile.h"
#include "ChartTwoOverlay.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "EventChartOverlayValidate.h"
#include "EventDataFileDelete.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventOverlayValidate.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabelTableEditor.h"
#include "MapSettingsChartTwoLineHistoryWidget.h"
#include "MapSettingsColorBarWidget.h"
#include "MapSettingsFiberTrajectoryWidget.h"
#include "MapSettingsLabelsWidget.h"
#include "MapSettingsLayerWidget.h"
#include "MapSettingsPaletteColorMappingWidget.h"
#include "MapSettingsParcelsWidget.h"
#include "Overlay.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::__OVERLAY_SETTINGS_EDITOR_DIALOG_DECLARE__ 
 * \brief Dialog for editing an overlay's settings
 * \ingroup GuiQt
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
OverlaySettingsEditorDialog::OverlaySettingsEditorDialog(QWidget* parent)
: WuQDialogNonModal("Overlay and Map Settings",
                    parent),
  EventListenerInterface()
{
    /*
     * No context menu, it screws things up
     */
    this->setContextMenuPolicy(Qt::NoContextMenu);
    
    this->setDeleteWhenClosed(false);

    m_caretMappableDataFile = NULL;
    m_selectedMapFileIndex = -1;
    m_brainordinateOverlay = NULL;
    m_chartOverlay = NULL;
    m_chartOverlaySelectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
    
    /*
     * No apply button
     */
    this->setApplyButtonText("");
    
    QWidget* mapNameWidget = createMapFileAndNameSection();
    
    m_colorBarWidget = new MapSettingsColorBarWidget();
    
    m_fiberTrajectoryWidget = new MapSettingsFiberTrajectoryWidget();
    
    m_layerWidget = new MapSettingsLayerWidget();
    
    m_paletteColorMappingWidget = new MapSettingsPaletteColorMappingWidget();
    
    m_parcelsWidget = new MapSettingsParcelsWidget();
    
    QWidget* windowOptionsWidget = this->createWindowOptionsSection();
    
    m_labelsWidget = new MapSettingsLabelsWidget();
    
    m_lineHistoryWidget = new MapSettingsChartTwoLineHistoryWidget();
    
    m_tabWidget = new QTabWidget();
    
    m_colorBarWidgetTabIndex = m_tabWidget->addTab(m_colorBarWidget,
                                                   "Color Bar");
    
    m_labelsWidgetTabIndex = m_tabWidget->addTab(m_labelsWidget,
                      "Labels");
    m_tabWidget->setTabEnabled(m_tabWidget->count() - 1, false);

    m_layersWidgetTabIndex = m_tabWidget->addTab(m_layerWidget,
                      "Layer");
    
    m_lineHistoryWidgetTabIndex = m_tabWidget->addTab(m_lineHistoryWidget,
                                                      "Lines");
    m_metadataWidgetTabIndex = m_tabWidget->addTab(new QWidget(),
                      "Metadata");
    m_tabWidget->setTabEnabled(m_tabWidget->count() - 1, false);
    
    m_paletteWidgetTabIndex = m_tabWidget->addTab(m_paletteColorMappingWidget,
                      "Palette");
    
    m_parcelsWidgetTabIndex = m_tabWidget->addTab(m_parcelsWidget,
                                          "Parcels");
    
    m_trajectoryWidgetTabIndex = m_tabWidget->addTab(m_fiberTrajectoryWidget,
                      "Trajectory");
    
    m_tabWidget->setCurrentIndex(m_tabWidget->count() - 1);
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    this->setLayoutSpacingAndMargins(layout);
    layout->addWidget(mapNameWidget);
    layout->addWidget(m_tabWidget);
    //layout->addWidget(windowOptionsWidget);

    this->setCentralWidget(w,
                           WuQDialog::SCROLL_AREA_NEVER);
    this->addWidgetToLeftOfButtons(windowOptionsWidget);
    
    disableAutoDefaultForAllPushButtons();

    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_DATA_FILE_DELETE);
}

/**
 * Destructor.
 */
OverlaySettingsEditorDialog::~OverlaySettingsEditorDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
OverlaySettingsEditorDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_DELETE) {
        updateDialog();
    }
}

/**
 * @return Create and return map file and name section of the dialog.
 *
 */
QWidget*
OverlaySettingsEditorDialog::createMapFileAndNameSection()
{
    QLabel* mapFileNameLabel = new QLabel("Map File: ");
    m_selectedMapFileNameLabel = new QLabel("");
    QLabel* mapNameLabel = new QLabel("Map Name: ");
    m_selectedMapNameLabel = new QLabel("");
    
    QGroupBox* groupBox = new QGroupBox();
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->addWidget(mapFileNameLabel, 0, 0);
    gridLayout->addWidget(m_selectedMapFileNameLabel, 0, 1, Qt::AlignLeft);
    gridLayout->addWidget(mapNameLabel, 1, 0);
    gridLayout->addWidget(m_selectedMapNameLabel, 1, 1, Qt::AlignLeft);
    
    return groupBox;
}

/**
 * Called for focus events.  Since this dialog stores a pointer
 * to the overlay, we need to be aware that the overlay's parameters
 * may change or the overlay may even be deleted.  So, when
 * this dialog gains focus, validate the overlay and then update
 * the dialog.
 *
 * @param event
 *     The focus event.
 */
void
OverlaySettingsEditorDialog::focusInEvent(QFocusEvent* event)
{
    if (event->reason() == Qt::PopupFocusReason) {
        /*
         * This occurs when a combo box is popped up Mac,
         * causes a problem, and can be ignored.
         */
        return;
    }
    
    updateDialog();
}

/**
 * Update if the given overlay is displayed in the dialog.
 * 
 * @param brainordinateOverlay
 *    Brainordinate overlay for the dialog.
 * @param chartOverlay
 *    Chart overlay for the dialog.
 */
void
OverlaySettingsEditorDialog::updateIfThisOverlayIsInDialog(Overlay* brainordinateOverlay,
                                                           ChartTwoOverlay* chartOverlay)
{
    if (m_brainordinateOverlay != NULL) {
        if (m_brainordinateOverlay == brainordinateOverlay) {
            updateDialogContent(m_brainordinateOverlay,
                                NULL);
        }
    }
    else if (m_chartOverlay != NULL) {
        if (m_chartOverlay == chartOverlay) {
            updateDialogContent(NULL,
                                m_chartOverlay);
        }
    }
}

/**
 * May be called to update the dialog's content.
 *
 * @param brainordinateOverlay
 *    Brainordinate overlay for the dialog.
 * @param chartOverlay
 *    Chart overlay for the dialog.
 */
void 
OverlaySettingsEditorDialog::updateDialogContent(Overlay* brainordinateOverlay,
                                                 ChartTwoOverlay* chartOverlay)
{
    updateDialogContentPrivate(brainordinateOverlay,
                               chartOverlay);
}

/**
 * Update the dialog's content.  Only one of the parameters
 * should be non-NULL.
 *
 * @param brainordinateOverlay
 *    Brainordinate overlay for the dialog.
 * @param chartOverlay
 *    Chart overlay for the dialog.
 */
void
OverlaySettingsEditorDialog::updateDialogContentPrivate(Overlay* brainordinateOverlay,
                                                        ChartTwoOverlay* chartOverlay)
{
    if ((brainordinateOverlay != NULL)
        && (chartOverlay != NULL)) {
        CaretAssert(0);  // only one should be non-NULL
    }
    
    const int32_t selectedTabIndex = m_tabWidget->currentIndex();
    m_brainordinateOverlay = brainordinateOverlay;
    m_chartOverlay = chartOverlay;
    
    m_caretMappableDataFile = NULL;
    m_chartOverlaySelectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
    m_selectedMapFileIndex = -1;
    if (m_brainordinateOverlay != NULL) {
        m_brainordinateOverlay->getSelectionData(m_caretMappableDataFile,
                                                 m_selectedMapFileIndex);
    }
    if (m_chartOverlay != NULL) {
        m_chartOverlay->getSelectionData(m_caretMappableDataFile,
                                         m_chartOverlaySelectedIndexType,
                                         m_selectedMapFileIndex);
    }
    
    bool isLabelsValid = false;
    bool isMetadataValid = false;
    GiftiMetaData* metadata = NULL;
    bool isPaletteValid = false;
    bool isParcelsValid = false;
    bool isFiberTrajectoryValid = false;
    bool isVolumeLayer = false;
    bool isLinesValid = false;
    
    QString mapFileName = "";
    QString mapName = "";
    
    if (m_caretMappableDataFile != NULL) {
        if (m_brainordinateOverlay != NULL) {
            if ((m_selectedMapFileIndex >= 0)
                && (m_selectedMapFileIndex < m_caretMappableDataFile->getNumberOfMaps())) {
                /*
                 * Get name of file and map
                 */
                mapFileName = m_caretMappableDataFile->getFileNameNoPath();
                if (m_selectedMapFileIndex >= 0) {
                    mapName = m_caretMappableDataFile->getMapName(m_selectedMapFileIndex);
                }
                
                /*
                 * Update layer widget
                 */
                m_layerWidget->updateContent(m_brainordinateOverlay);
                
                if (m_caretMappableDataFile->isMappedWithLabelTable()) {
                    if (m_caretMappableDataFile->getMapLabelTable(m_selectedMapFileIndex) != NULL) {
                        /*
                         * Update label editor
                         */
                        isLabelsValid = true;
                        m_labelsWidget->updateContent(m_brainordinateOverlay);
                    }
                }
                
                metadata = m_caretMappableDataFile->getMapMetaData(m_selectedMapFileIndex);
                if (metadata != NULL) {
                    /*
                     * TODO: Update metadata widget
                     */
                    //isMetadataValid = true;
                }
                
                if (m_caretMappableDataFile->isMappedWithPalette()) {
                    PaletteColorMapping* paletteColorMapping = m_caretMappableDataFile->getMapPaletteColorMapping(m_selectedMapFileIndex);
                    if (paletteColorMapping != NULL) {
                        /*
                         * Update palette settings
                         */
                        isPaletteValid = true;
                        m_paletteColorMappingWidget->updateEditor(m_caretMappableDataFile,
                                                                  m_selectedMapFileIndex);
                        AnnotationColorBar* colorBar = NULL;
                        if (m_brainordinateOverlay != NULL) {
                            colorBar = m_brainordinateOverlay->getColorBar();
                        }
                        else if (m_chartOverlay != NULL) {
                            colorBar = m_chartOverlay->getColorBar();
                        }
                        m_colorBarWidget->updateContent(m_caretMappableDataFile,
                                                        m_selectedMapFileIndex,
                                                        colorBar,
                                                        paletteColorMapping);
                    }
                }
                
                CiftiFiberTrajectoryFile* trajFile = dynamic_cast<CiftiFiberTrajectoryFile*>(m_caretMappableDataFile);
                if (trajFile != NULL) {
                    /*
                     * Update trajectory
                     */
                    isFiberTrajectoryValid = true;
                    m_fiberTrajectoryWidget->updateEditor(trajFile);
                }
                
                CiftiConnectivityMatrixParcelFile* parcelsFile = dynamic_cast<CiftiConnectivityMatrixParcelFile*>(m_caretMappableDataFile);
                if (parcelsFile != NULL) {
                    /*
                     * Update parcels
                     */
                    isParcelsValid = true;
                    m_parcelsWidget->updateEditor(parcelsFile);
                }
                
                /*
                 * Is volume mappable
                 */
                if (m_caretMappableDataFile->isVolumeMappable()) {
                    if (isFiberTrajectoryValid) {
                        /* nothing */
                    }
                    else {
                        isVolumeLayer = true;
                    }
                }
            }
        }
        else if (m_chartOverlay != NULL) {
            bool hasMapsFlag = false;
            bool hasHistoryFlag = false;
            switch (m_chartOverlay->getChartTwoDataType()) {
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                    hasMapsFlag = true;
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                    hasHistoryFlag = true;
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    hasMapsFlag = true;
                    break;
            }
            
            int32_t selectedMapIndex = -1;
            bool mapTypeFlag = false;
            if (hasMapsFlag) {
                switch (m_chartOverlaySelectedIndexType) {
                    case ChartTwoOverlay::SelectedIndexType::INVALID:
                        CaretAssert(0);
                        break;
                    case ChartTwoOverlay::SelectedIndexType::MAP:
                        if ((m_selectedMapFileIndex >= 0)
                            && (m_selectedMapFileIndex < m_caretMappableDataFile->getNumberOfMaps())) {
                            selectedMapIndex = m_selectedMapFileIndex;
                            mapTypeFlag = true;
                        }
                        break;
                    case ChartTwoOverlay::SelectedIndexType::COLUMN:
                    case ChartTwoOverlay::SelectedIndexType::ROW:
                        if (m_selectedMapFileIndex >= 0) {
                            if (m_caretMappableDataFile->getNumberOfMaps() == 1) {
                                m_selectedMapFileIndex = 0;
                            }
                            selectedMapIndex = m_selectedMapFileIndex;
                        }
                        break;
                }
                if ((selectedMapIndex >= 0)
                    && (selectedMapIndex < m_caretMappableDataFile->getNumberOfMaps())) {
                    /*
                     * Get name of file and map
                     */
                    mapFileName = m_caretMappableDataFile->getFileNameNoPath();
                    if (m_selectedMapFileIndex >= 0) {
                        mapName = m_caretMappableDataFile->getMapName(selectedMapIndex);
                    }
                    
                    if (mapTypeFlag) {
                        metadata = m_caretMappableDataFile->getMapMetaData(selectedMapIndex);
                        if (metadata != NULL) {
                            /*
                             * TODO: Update metadata widget
                             */
                            //isMetadataValid = true;
                        }
                    }
                    
                    if (m_caretMappableDataFile->isMappedWithPalette()) {
                        PaletteColorMapping* paletteColorMapping = m_caretMappableDataFile->getMapPaletteColorMapping(selectedMapIndex);
                        if (paletteColorMapping != NULL) {
                            /*
                             * Update palette settings
                             */
                            isPaletteValid = true;
                            m_paletteColorMappingWidget->updateEditor(m_caretMappableDataFile,
                                                                      selectedMapIndex);
                            AnnotationColorBar* colorBar = NULL;
                            if (m_brainordinateOverlay != NULL) {
                                colorBar = m_brainordinateOverlay->getColorBar();
                            }
                            else if (m_chartOverlay != NULL) {
                                colorBar = m_chartOverlay->getColorBar();
                            }
                            m_colorBarWidget->updateContent(m_caretMappableDataFile,
                                                            selectedMapIndex,
                                                            colorBar,
                                                            paletteColorMapping);
                        }
                    }
                    
                    CiftiConnectivityMatrixParcelFile* parcelsFile = dynamic_cast<CiftiConnectivityMatrixParcelFile*>(m_caretMappableDataFile);
                    if (parcelsFile != NULL) {
                        /*
                         * Update parcels
                         */
                        isParcelsValid = true;
                        m_parcelsWidget->updateEditor(parcelsFile);
                    }
                }
            }
            else if (hasHistoryFlag) {
                isLinesValid = true;
                m_lineHistoryWidget->updateContent(m_chartOverlay);
            }
        }
        else {
            CaretAssert(0);
        }
    }

  
    /*
     * Set enabled status of tabs
     */
    m_tabWidget->setTabEnabled(m_colorBarWidgetTabIndex,
                               isPaletteValid);
    m_tabWidget->setTabEnabled(m_labelsWidgetTabIndex,
                               isLabelsValid);
    m_tabWidget->setTabEnabled(m_layersWidgetTabIndex,
                               isVolumeLayer);
    m_tabWidget->setTabEnabled(m_metadataWidgetTabIndex,
                               isMetadataValid);
    m_tabWidget->setTabEnabled(m_paletteWidgetTabIndex,
                               isPaletteValid);
    m_tabWidget->setTabEnabled(m_parcelsWidgetTabIndex,
                               isParcelsValid);
    m_tabWidget->setTabEnabled(m_trajectoryWidgetTabIndex,
                               isFiberTrajectoryValid);
    m_tabWidget->setTabEnabled(m_lineHistoryWidgetTabIndex,
                               isLinesValid);

    /*
     * When the selected tab is invalid, we want to select the
     * "best" tab that is enabled.  The order in this vector
     * is the priority of the tabs.
     */
    std::vector<int32_t> priorityTabIndices;
    priorityTabIndices.push_back(m_paletteWidgetTabIndex);
    priorityTabIndices.push_back(m_lineHistoryWidgetTabIndex);
    priorityTabIndices.push_back(m_colorBarWidgetTabIndex);
    priorityTabIndices.push_back(m_labelsWidgetTabIndex);
    priorityTabIndices.push_back(m_parcelsWidgetTabIndex);
    priorityTabIndices.push_back(m_trajectoryWidgetTabIndex);
    priorityTabIndices.push_back(m_layersWidgetTabIndex);
    priorityTabIndices.push_back(m_metadataWidgetTabIndex);
    CaretAssertMessage((static_cast<int>(priorityTabIndices.size()) == m_tabWidget->count()),
                       "Number of elements in priorityTabIndices is different "
                       "than number of tab indices.  Was a new tab added?");
    
    /*
     * Make sure an enabled tab is selected using the tabs
     * in the priority order
     */
    if (selectedTabIndex >= 0) {
        if (m_tabWidget->isTabEnabled(selectedTabIndex) == false) {
            const int32_t numPriorityTabs = static_cast<int32_t>(priorityTabIndices.size());
            for (int32_t i = 0; i < numPriorityTabs; i++) {
                const int32_t tabIndex = priorityTabIndices[i];
                if (m_tabWidget->isTabEnabled(tabIndex)) {
                    m_tabWidget->setCurrentIndex(tabIndex);
                    break;
                }
            }
        }
    }
    
    m_selectedMapFileNameLabel->setText(mapFileName);
    m_selectedMapNameLabel->setText(mapName);
}

/**
 * May be called to update the dialog.
 */
void
OverlaySettingsEditorDialog::updateDialog()
{
    /*
     * Validate overlay to prevent crash
     */
    
    if (m_brainordinateOverlay != NULL) {
        EventOverlayValidate validateOverlayEvent(m_brainordinateOverlay);
        EventManager::get()->sendEvent(validateOverlayEvent.getPointer());
        if ( ! validateOverlayEvent.isValidOverlay()) {
            m_brainordinateOverlay = NULL;
        }
    }
    
    if (m_chartOverlay != NULL) {
        EventChartOverlayValidate validateChartOverlayEvent(m_chartOverlay);
        EventManager::get()->sendEvent(validateChartOverlayEvent.getPointer());
        if ( ! validateChartOverlayEvent.isValidChartOverlay()) {
            m_chartOverlay = NULL;
        }
    }

    updateDialogContentPrivate(m_brainordinateOverlay,
                               m_chartOverlay);
    
    if ((m_brainordinateOverlay == NULL)
        && (m_chartOverlay == NULL)) {
        close();
    }
}

/**
 * Update chart lines in the dialog
 */
void
OverlaySettingsEditorDialog::updateChartLinesInDialog()
{
    if (m_chartOverlay != NULL) {
        EventChartOverlayValidate validateChartOverlayEvent(m_chartOverlay);
        EventManager::get()->sendEvent(validateChartOverlayEvent.getPointer());
        if ( ! validateChartOverlayEvent.isValidChartOverlay()) {
            m_chartOverlay = NULL;
        }
        
        if (m_chartOverlay != NULL) {
            updateDialogContentPrivate(NULL,
                                       m_chartOverlay);
        }
    }
}


/**
 * Called when close button pressed.
 */ 
void
OverlaySettingsEditorDialog::closeButtonPressed()
{
    /*
     * Clear the content since it could be tied to an overlay
     * and we don't want the dialog updating if it is not
     * visible.
     */
    updateDialogContent(NULL,
                        NULL);
    
    /*
     * Allow this dialog to be reused (checked means DO NOT reuse)
     */
    m_doNotReplaceCheckBox->setCheckState(Qt::Unchecked);
    
    WuQDialogNonModal::closeButtonClicked();
}

/**
 * Set the layout margins.
 * @param layout
 *   Layout for which margins are set.
 */
void 
OverlaySettingsEditorDialog::setLayoutSpacingAndMargins(QLayout* layout)
{
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 5, 3);
}

/**
 * @return Is the Do Not Replace selected.  If so, this instance of the
 * dialog should not be replaced.
 */
bool 
OverlaySettingsEditorDialog::isDoNotReplaceSelected() const
{
    const bool checked = (m_doNotReplaceCheckBox->checkState() == Qt::Checked);
    return checked;
}

/**
 * Called when the state of the do not reply checkbox is changed.
 * @param state
 *    New state of checkbox.
 */
void 
OverlaySettingsEditorDialog::doNotReplaceCheckBoxStateChanged(int /*state*/)
{
//    const bool checked = (state == Qt::Checked);
}

/**
 * @return A widget containing the window options.
 */
QWidget*
OverlaySettingsEditorDialog::createWindowOptionsSection()
{
    m_doNotReplaceCheckBox = new QCheckBox("Do Not Replace");
    m_doNotReplaceCheckBox->setToolTip("If checked: \n"
                                           "   (1) this window remains displayed until it is\n"
                                           "       closed.\n"
                                           "   (2) if the user selects editing of another map's\n"
                                           "       palette, it will not replace the content of\n"
                                           "       this window.\n"
                                           "If NOT checked:\n"
                                           "   If the user selects editing of another map's \n"
                                           "   palette, it will replace the content of this\n"
                                           "   window.");
    QWidget* optionsWidget = new QWidget();
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsWidget);
    this->setLayoutSpacingAndMargins(optionsLayout);
    optionsLayout->addWidget(m_doNotReplaceCheckBox);
    optionsWidget->setSizePolicy(QSizePolicy::Fixed,
                                 QSizePolicy::Fixed);
    
    return optionsWidget;
}

/**
 * Called when the edit label table button is clicked.
 */
void
OverlaySettingsEditorDialog::editLabelTablePushButtonClicked()
{
    if (m_caretMappableDataFile != NULL) {
        if (m_caretMappableDataFile->isMappedWithLabelTable()) {
            if (m_selectedMapFileIndex >= 0) {
                GiftiLabelTableEditor labelTableEditor(m_caretMappableDataFile,
                                                       m_selectedMapFileIndex,
                                                       "Edit Labels",
                                                       GiftiLabelTableEditor::OPTION_ADD_APPLY_BUTTON,
                                                       m_editLabelTablePushButton);
                labelTableEditor.exec();
            }
        }
    }
}

/**
 * Create and return widget for editing label tables.
 */
QWidget*
OverlaySettingsEditorDialog::createLabelsSection()
{
    m_editLabelTablePushButton = new QPushButton("Edit");
    QObject::connect(m_editLabelTablePushButton, SIGNAL(clicked()),
                     this, SLOT(editLabelTablePushButtonClicked()));
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(m_editLabelTablePushButton,
                      0,
                      Qt::AlignLeft);
    layout->addStretch();
    
    return widget;
}


