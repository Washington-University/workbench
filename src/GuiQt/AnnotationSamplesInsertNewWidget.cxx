
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

#define __ANNOTATION_SAMPLES_INSERT_NEW_WIDGET_DECLARE__
#include "AnnotationSamplesInsertNewWidget.h"
#undef __ANNOTATION_SAMPLES_INSERT_NEW_WIDGET_DECLARE__

#include <QAction>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

#include "AnnotationPolyhedron.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "DataFileException.h"
#include "DisplayPropertiesSamples.h"
#include "DrawingViewportContent.h"
#include "EnumComboBoxTemplate.h"
#include "EventAnnotationCreateNewType.h"
#include "EventAnnotationGetBeingDrawnInWindow.h"
#include "EventAnnotationGetSelectedInsertNewFile.h"
#include "EventDataFileAdd.h"
#include "EventDrawingViewportContentGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SamplesDrawingSettings.h"
#include "SamplesFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSamplesInsertNewWidget 
 * \brief Toolbar widget for inserring new samples
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param userInputMode
 *    The user input mode
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parent
 *     Parent of this widget.
 */
AnnotationSamplesInsertNewWidget::AnnotationSamplesInsertNewWidget(const UserInputModeEnum::Enum userInputMode,
                                                                   const int32_t browserWindowIndex,
                                                                   QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    CaretAssert(userInputMode == UserInputModeEnum::Enum::SAMPLES_EDITING);
    
    const AString sampleToolTipText("<html>"
                                    "Click this button to initiate the drawing of a sample polyhedron.<br>"
                                    "Use the <b>Slices</b> control to set the number of slices that the "
                                    "polyhedron spans.  The <b>Slices</b> value may be negative or positive.  "
                                    "To draw a polyhedron:"
                                    "<ul>"
                                    "<li> <i>Click</i> the mouse to insert coordinates and create straight, possibly longer lines"
                                    "<li> <i>Drag</i> (move with left button down) the mouse to create curved lines "
                                    "<li> Note that one can intermix clicks and drags while drawing"
                                    "<li> When finished, <i>shift-click</i> the mouse to finalize the polygon (does NOT add "
                                    "another coordinate) or click the <b>Finish</b> button"
                                    "</ul>"
                                    "</html>");
    
    QLabel* fileLabel(new QLabel("File"));
    
    m_fileSelectionModel.reset(CaretDataFileSelectionModel::newInstanceForCaretDataFileType(DataFileTypeEnum::SAMPLES));
    m_fileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel.get());
    m_fileSelectionComboBox->setNoFilesText("Click \"New\" to create a file");
    m_fileSelectionComboBox->setToolTip("New samples are added to this file");
    QObject::connect(m_fileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     this, &AnnotationSamplesInsertNewWidget::fileSelectionComboBoxFileSelected);
    
    QAction* newFileAction(new QAction());
    newFileAction->setText("New File");
    newFileAction->setToolTip("Create a new Samples File");
    QObject::connect(newFileAction, &QAction::triggered,
                     this, &AnnotationSamplesInsertNewWidget::newFileActionTriggered);
    QToolButton* newFileToolButton(new QToolButton());
    newFileToolButton->setDefaultAction(newFileAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(newFileToolButton);

    const AString saveToolTip("<html>"
                              "Save the selected Samples File.<br>"
                              "To change the name of the file, use the File Menu's "
                              "Save/Manage Files menu item."
                              "</html>");
    m_saveFileAction = new QAction();
    m_saveFileAction->setText("Save File");
    m_saveFileAction->setToolTip(saveToolTip);
    QObject::connect(m_saveFileAction, &QAction::triggered,
                     this, &AnnotationSamplesInsertNewWidget::saveFileActionTriggered);
    QToolButton* saveFileToolButton(new QToolButton());
    saveFileToolButton->setDefaultAction(m_saveFileAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(saveFileToolButton);

    m_newSampleAction = new QAction();
    m_newSampleAction->setText("Insert New Sample");
    m_newSampleAction->setToolTip(sampleToolTipText);
    QObject::connect(m_newSampleAction, &QAction::triggered,
                     this, &AnnotationSamplesInsertNewWidget::newSampleActionTriggered);
    QToolButton* newSampleToolButton(new QToolButton());
    WuQtUtilities::setToolButtonStyleForQt5Mac(newSampleToolButton);
    newSampleToolButton->setDefaultAction(m_newSampleAction);
    
    m_samplesDrawingModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_samplesDrawingModeEnumComboBox->setToolTip(SamplesDrawingModeEnum::getToolTip());
    m_samplesDrawingModeEnumComboBox->setup<SamplesDrawingModeEnum,SamplesDrawingModeEnum::Enum>();
    QObject::connect(m_samplesDrawingModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &AnnotationSamplesInsertNewWidget::samplesDrawingModeEnumComboBoxItemActivated);
    
    const QString lowerToolTip("<html>Inhibit drawing on slices starting from bottom-right</html>");
    m_lowerSliceOffsetLabel = new QLabel("Lower");
    m_lowerSliceOffsetLabel->setToolTip(lowerToolTip);
    m_lowerSliceOffsetSpinBox = new QSpinBox();
    m_lowerSliceOffsetSpinBox->setMinimum(0);
    m_lowerSliceOffsetSpinBox->setMaximum(99);
    m_lowerSliceOffsetSpinBox->setSingleStep(1);
    m_lowerSliceOffsetSpinBox->setToolTip(lowerToolTip);
    QObject::connect(m_lowerSliceOffsetSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &AnnotationSamplesInsertNewWidget::lowerSliceOffsetSpinBoxValueChanged);
    
    const QString upperToolTip("<html>Inhibit drawing on slices starting from top-left</html>");
    m_upperSliceOffsetLabel = new QLabel("Upper");
    m_upperSliceOffsetLabel->setToolTip(upperToolTip);
    m_upperSliceOffsetSpinBox = new QSpinBox();
    m_upperSliceOffsetSpinBox->setMinimum(0);
    m_upperSliceOffsetSpinBox->setMaximum(99);
    m_upperSliceOffsetSpinBox->setSingleStep(1);
    m_upperSliceOffsetSpinBox->setToolTip(upperToolTip);
    QObject::connect(m_upperSliceOffsetSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &AnnotationSamplesInsertNewWidget::upperSliceOffsetSpinBoxValueChanged);
    
    WuQtUtilities::matchWidgetWidths(m_lowerSliceOffsetSpinBox,
                                     m_upperSliceOffsetSpinBox);
    
    QWidget* samplesWidget(new QWidget());
    QHBoxLayout* samplesLayout(new QHBoxLayout(samplesWidget));
    WuQtUtilities::setLayoutSpacingAndMargins(samplesLayout, 2, 2);
    samplesLayout->addWidget(newSampleToolButton);
    samplesLayout->addStretch();
    samplesLayout->addWidget(m_samplesDrawingModeEnumComboBox->getWidget());
    samplesLayout->addSpacing(4);
    samplesLayout->addWidget(m_upperSliceOffsetLabel);
    samplesLayout->addWidget(m_upperSliceOffsetSpinBox);
    samplesLayout->addWidget(m_lowerSliceOffsetLabel);
    samplesLayout->addWidget(m_lowerSliceOffsetSpinBox);
    
    QWidget* fileWidget(new QWidget());
    QHBoxLayout* fileLayout(new QHBoxLayout(fileWidget));
    WuQtUtilities::setLayoutSpacingAndMargins(fileLayout, 2, 0);
    fileLayout->addWidget(fileLabel);
    fileLayout->addWidget(m_fileSelectionComboBox->getWidget(), 100);
    fileLayout->addWidget(newFileToolButton);
    fileLayout->addWidget(saveFileToolButton);
    
    QVBoxLayout* layout(new QVBoxLayout(this));
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(fileWidget);
    layout->addWidget(samplesWidget);
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_NEW_DRAWING_POLYHEDRON_SLICE_DEPTH);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE);
}

/**
 * Destructor.
 */
AnnotationSamplesInsertNewWidget::~AnnotationSamplesInsertNewWidget()
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
AnnotationSamplesInsertNewWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE) {
        EventAnnotationGetSelectedInsertNewFile* fileEvent(dynamic_cast<EventAnnotationGetSelectedInsertNewFile*>(event));
        CaretAssert(fileEvent);
        if (fileEvent->getUserInputMode() == m_userInputMode) {
            SamplesFile* samplesFile(getSelectedSamplesFile());
            if (samplesFile != NULL) {
                fileEvent->setAnnotationFile(samplesFile);
                fileEvent->setEventProcessed();
            }
        }
    }
}

/**
 * Update the content.
 */
void
AnnotationSamplesInsertNewWidget::updateContent()
{
    const BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex));
    if (bbw != NULL) {
        const BrowserTabContent* tabContent(bbw->getBrowserTabContent());
        if (tabContent != NULL) {
            
            const SamplesDrawingSettings* samplesSettings(tabContent->getSamplesDrawingSettings());
            CaretAssert(samplesSettings);
            
            const SamplesDrawingModeEnum::Enum drawingMode(samplesSettings->getDrawingMode());

            m_samplesDrawingModeEnumComboBox->setSelectedItem<SamplesDrawingModeEnum,SamplesDrawingModeEnum::Enum>(drawingMode);
            QSignalBlocker lowBlocker(m_lowerSliceOffsetSpinBox);
            m_lowerSliceOffsetSpinBox->setValue(samplesSettings->getLowerSliceOffset());

            QSignalBlocker highBlocker(m_upperSliceOffsetSpinBox);
            m_upperSliceOffsetSpinBox->setValue(samplesSettings->getUpperSliceOffset());
            
            bool enableIndexSpinBoxesFlag(false);
            switch (drawingMode) {
                case SamplesDrawingModeEnum::ALL_SLICES:
                    break;
                case SamplesDrawingModeEnum::EXCLUDE:
                    enableIndexSpinBoxesFlag = true;
                    break;
            }
            m_lowerSliceOffsetLabel->setEnabled(enableIndexSpinBoxesFlag);
            m_lowerSliceOffsetSpinBox->setEnabled(enableIndexSpinBoxesFlag);
            m_upperSliceOffsetLabel->setEnabled(enableIndexSpinBoxesFlag);
            m_upperSliceOffsetSpinBox->setEnabled(enableIndexSpinBoxesFlag);

            EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_userInputMode,
                                                              m_browserWindowIndex);
            EventManager::get()->sendEvent(annDrawEvent.getPointer());
            const bool noDrawingFlag(annDrawEvent.getAnnotation() == NULL);
            m_samplesDrawingModeEnumComboBox->getWidget()->setEnabled(noDrawingFlag);
            const bool enableRangeFlag((drawingMode == SamplesDrawingModeEnum::EXCLUDE)
                                        && noDrawingFlag);
            m_lowerSliceOffsetLabel->setEnabled(enableRangeFlag);
            m_lowerSliceOffsetSpinBox->setEnabled(enableRangeFlag);
            m_upperSliceOffsetLabel->setEnabled(enableRangeFlag);
            m_upperSliceOffsetSpinBox->setEnabled(enableRangeFlag);
        }
    }
    
    
    m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel.get());
        
    bool saveEnabledFlag(false);
    const SamplesFile* samplesFile(getSelectedSamplesFile());
    if (samplesFile != NULL) {
        if (samplesFile->isModified()) {
            saveEnabledFlag = ( ! samplesFile->getFileName().isEmpty());
        }
    }
    m_saveFileAction->setEnabled(saveEnabledFlag);
}

/**
 * Called when a file is selected from the file combo box
 * @param caretDataFile
 *    File selected
 */
void
AnnotationSamplesInsertNewWidget::fileSelectionComboBoxFileSelected(CaretDataFile* /*caretDataFile*/)
{
    updateContent();
}

/**
 * Called to create a new samples file
 */
void
AnnotationSamplesInsertNewWidget::newFileActionTriggered()
{
    const AString fileDialogSettingsName("AnnotDiskFileDialog");
    
    /*
     * Setup file selection dialog.
     */
    CaretFileDialog fd(CaretFileDialog::Mode::MODE_SAVE,
                       this);
    fd.setAcceptMode(CaretFileDialog::AcceptSave);
    fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilterForWriting(DataFileTypeEnum::SAMPLES));
    fd.selectFile(SamplesFile().getFileNameNoPath());
    fd.setFileMode(CaretFileDialog::AnyFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.setLabelText(CaretFileDialog::Accept, "Choose"); /* OK button shows Insert */
    fd.restoreDialogSettings(fileDialogSettingsName);
    
    AString errorMessages;
    
    if (fd.exec() == CaretFileDialog::Accepted) {
        fd.saveDialogSettings(fileDialogSettingsName);
        
        QStringList selectedFiles = fd.selectedFiles();
        if ( ! selectedFiles.empty()) {
            const AString newFileName = selectedFiles.at(0);
            SamplesFile* newFile = new SamplesFile();
            newFile->setFileName(newFileName);
            EventManager::get()->sendEvent(EventDataFileAdd(newFile).getPointer());
            m_fileSelectionModel->setSelectedFile(newFile);
            m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel.get());
            newFile->clearModified();
        }
    }
    
    updateContent();
}

/**
 * Called when new sample action triggered
 */
void
AnnotationSamplesInsertNewWidget::newSampleActionTriggered()
{
    SamplesFile* samplesFile(getSelectedSamplesFile());
    if (samplesFile == NULL) {
        WuQMessageBox::errorOk(this, "No file is selected.  Click the New button to create a new samples file.");
        return;
    }
    
    const AnnotationCoordinateSpaceEnum::Enum annSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
    const AnnotationTypeEnum::Enum annShape(AnnotationTypeEnum::POLYHEDRON);
    EventAnnotationCreateNewType::PolyhedronDrawingMode polyhedronDrawingMode
    = EventAnnotationCreateNewType::PolyhedronDrawingMode::SAMPLES_DRAWING;
    
    DisplayPropertiesSamples* dps(GuiManager::get()->getBrain()->getDisplayPropertiesSamples());
    CaretAssert(dps);
    dps->setDisplaySamples(true);
    
    EventManager::get()->sendEvent(EventAnnotationCreateNewType(m_browserWindowIndex,
                                                                m_userInputMode,
                                                                samplesFile,
                                                                annSpace,
                                                                annShape,
                                                                polyhedronDrawingMode).getPointer());
    
    updateContent();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * @return The selected samples file
 */
SamplesFile*
AnnotationSamplesInsertNewWidget::getSelectedSamplesFile() const
{
    CaretAssert(m_fileSelectionModel);
    SamplesFile* samplesFile(NULL);
    CaretDataFile* caretDataFile(m_fileSelectionModel->getSelectedFile());
    if (caretDataFile != NULL) {
        samplesFile = dynamic_cast<SamplesFile*>(caretDataFile);
    }
    return samplesFile;
}

/**
 * Called when mode combo box selection is made
 * @param index
 *    Index of item selected
 */
void
AnnotationSamplesInsertNewWidget::samplesDrawingModeEnumComboBoxItemActivated()
{
    BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex));
    if (bbw != NULL) {
        BrowserTabContent* tabContent(bbw->getBrowserTabContent());
        if (tabContent != NULL) {
            SamplesDrawingSettings* samplesSettings(tabContent->getSamplesDrawingSettings());
            CaretAssert(samplesSettings);
            const SamplesDrawingModeEnum::Enum drawingMode(m_samplesDrawingModeEnumComboBox->getSelectedItem<SamplesDrawingModeEnum,SamplesDrawingModeEnum::Enum>());
            samplesSettings->setDrawingMode(drawingMode);
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            updateContent();
        }
    }
}

/**
 * Called when low slice index spin box value is changed
 * @param value
 */
void
AnnotationSamplesInsertNewWidget::lowerSliceOffsetSpinBoxValueChanged(int value)
{
    BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex));
    if (bbw != NULL) {
        BrowserTabContent* tabContent(bbw->getBrowserTabContent());
        if (tabContent != NULL) {
            SamplesDrawingSettings* samplesSettings(tabContent->getSamplesDrawingSettings());
            CaretAssert(samplesSettings);
            samplesSettings->setLowerSliceOffset(value);
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            updateContent();
        }
    }
}

/**
 * Called when high slice index spin box value is changed
 * @param value
 */
void
AnnotationSamplesInsertNewWidget::upperSliceOffsetSpinBoxValueChanged(int value)
{
    BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex));
    if (bbw != NULL) {
        BrowserTabContent* tabContent(bbw->getBrowserTabContent());
        if (tabContent != NULL) {
            SamplesDrawingSettings* samplesSettings(tabContent->getSamplesDrawingSettings());
            CaretAssert(samplesSettings);
            samplesSettings->setUpperSliceOffset(value);
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            updateContent();
        }
    }
}

/**
 * Called to save the selected file.
 */
void
AnnotationSamplesInsertNewWidget::saveFileActionTriggered()
{
    SamplesFile* samplesFile(getSelectedSamplesFile());
    if (samplesFile != NULL) {
        try {
            samplesFile->writeFile(samplesFile->getFileName());
        }
        catch (const DataFileException& dfe) {
            WuQMessageBox::critical(this,
                                    "Save Error",
                                    dfe.whatString());
        }
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

