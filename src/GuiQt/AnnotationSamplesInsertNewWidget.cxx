
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
#include <QMenu>
#include <QToolButton>

#include "AnnotationManager.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationSampleMetaData.h"
#include "AnnotationSamplesMetaDataDialog.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "BrainBrowserWindow.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "DataFileException.h"
#include "DisplayPropertiesSamples.h"
#include "DrawingViewportContent.h"
#include "EnumComboBoxTemplate.h"
#include "EventAnnotationAddToRemoveFromFile.h"
#include "EventAnnotationCreateNewType.h"
#include "EventAnnotationGetBeingDrawnInWindow.h"
#include "EventAnnotationGetSelectedInsertNewFile.h"
#include "EventAnnotationPolyhedronGetByLinkedIdentifier.h"
#include "EventDataFileAdd.h"
#include "EventDrawingViewportContentGet.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SamplesDrawingSettings.h"
#include "SamplesFile.h"
#include "WuQMessageBox.h"
#include "WuQMessageBoxTwo.h"
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

    QLabel* newLabel(new QLabel("New:"));
    
    m_newDesiredSampleAction = new QAction();
    m_newDesiredSampleAction->setText("Desired");
    m_newDesiredSampleAction->setToolTip(getNewSampleToolTip(AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE));
    QObject::connect(m_newDesiredSampleAction, &QAction::triggered,
                     this, &AnnotationSamplesInsertNewWidget::newDesiredSampleActionTriggered);
    m_newDesiredSampleToolButton = new QToolButton();
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_newDesiredSampleToolButton);
    m_newDesiredSampleToolButton->setDefaultAction(m_newDesiredSampleAction);
    
    m_samplesDrawingModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_samplesDrawingModeEnumComboBox->setToolTip(SamplesDrawingModeEnum::getToolTip());
    m_samplesDrawingModeEnumComboBox->setup<SamplesDrawingModeEnum,SamplesDrawingModeEnum::Enum>();
    QObject::connect(m_samplesDrawingModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &AnnotationSamplesInsertNewWidget::samplesDrawingModeEnumComboBoxItemActivated);
    
    m_newActualSampleAction = new QAction();
    m_newActualSampleAction->setText("Actual");
    m_newActualSampleAction->setToolTip(getNewSampleToolTip(AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE));
    QObject::connect(m_newActualSampleAction, &QAction::triggered,
                     this, &AnnotationSamplesInsertNewWidget::newActualSampleActionTriggered);
    m_newActualSampleToolButton = new QToolButton();
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_newActualSampleToolButton);
    m_newActualSampleToolButton->setDefaultAction(m_newActualSampleAction);
    
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
    
    m_selectAction = new QAction("Metadata...");
    m_selectAction->setToolTip("Display menu listing all Samples; \nselect to edit metadata");
    QObject::connect(m_selectAction, &QAction::triggered,
                     this, &AnnotationSamplesInsertNewWidget::selectActionTriggered);
    m_selectToolButton = new QToolButton();
    m_selectToolButton->setDefaultAction(m_selectAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_selectToolButton);
    
    QWidget* samplesWidget(new QWidget());
    QHBoxLayout* samplesLayout(new QHBoxLayout(samplesWidget));
    WuQtUtilities::setLayoutSpacingAndMargins(samplesLayout, 2, 0);
    samplesLayout->addWidget(newLabel);
    samplesLayout->addWidget(m_newDesiredSampleToolButton);
    samplesLayout->addWidget(m_newActualSampleToolButton);
    samplesLayout->addWidget(m_samplesDrawingModeEnumComboBox->getWidget());
    samplesLayout->addSpacing(4);
    samplesLayout->addWidget(m_upperSliceOffsetLabel);
    samplesLayout->addWidget(m_upperSliceOffsetSpinBox);
    samplesLayout->addWidget(m_lowerSliceOffsetLabel);
    samplesLayout->addWidget(m_lowerSliceOffsetSpinBox);
    samplesLayout->addStretch();
    samplesLayout->addWidget(m_selectToolButton);

    QWidget* fileWidget(new QWidget());
    QHBoxLayout* fileLayout(new QHBoxLayout(fileWidget));
    WuQtUtilities::setLayoutSpacingAndMargins(fileLayout, 2, 0);
    fileLayout->addWidget(fileLabel);
    fileLayout->addWidget(m_fileSelectionComboBox->getWidget(), 100);
    fileLayout->addWidget(newFileToolButton);
    fileLayout->addWidget(saveFileToolButton);
    
    QVBoxLayout* layout(new QVBoxLayout(this));
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
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
        const BrowserWindowContent* windowContent(bbw->getBrowerWindowContent());
        if (windowContent != NULL) {
            
            const SamplesDrawingSettings* samplesSettings(windowContent->getSamplesDrawingSettings());
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
    
    m_selectAction->setEnabled( ! getAllPolyhedrons().empty());
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
 * Called when new actual sample action triggered
 */
void
AnnotationSamplesInsertNewWidget::newActualSampleActionTriggered()
{
    if ( ! isVolumeSliceMontageEnabledInSelectedTab(m_newActualSampleToolButton)) {
        return;
    }
    
    SamplesFile* samplesFile(getSelectedSamplesFile());
    if (samplesFile == NULL) {
        WuQMessageBoxTwo::warning(m_newActualSampleToolButton,
                                  "Warning",
                                  "No Samples File is selcted");
        return;
    }
    
    const std::vector<AnnotationPolyhedron*> selectedDesiredPolyhedrons(getSelectedDesiredSamples());
    const int32_t numDesiredPolygonsSelected(selectedDesiredPolyhedrons.size());
    if (numDesiredPolygonsSelected == 0) {
        const AString msg("No Desired Sample is selected.  Do you want to create an Actual "
                          "Sample that is not associated with a Desired Sample.   If not, "
                          "click No to close this dialog, select a Desired Sample, and "
                          "click the Actual button again.");
        const WuQMessageBoxTwo::StandardButton button(WuQMessageBoxTwo::warning(m_newActualSampleToolButton,
                                                                                "Warning",
                                                                                msg,
                                                                                ((int)WuQMessageBoxTwo::StandardButton::Yes
                                                                                 | (int)WuQMessageBoxTwo::StandardButton::No),
                                                                                WuQMessageBoxTwo::StandardButton::No));
        if (button == WuQMessageBoxTwo::StandardButton::Yes) {
            AnnotationPolyhedron* linkedPolyhedron(NULL);
            createNewSample(AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE,
                            linkedPolyhedron);
        }
    }
    else if (numDesiredPolygonsSelected == 1) {
        CaretAssertVectorIndex(selectedDesiredPolyhedrons, 0);
        AnnotationPolyhedron* desiredPolyhedron(selectedDesiredPolyhedrons[0]);
        CaretAssert(desiredPolyhedron);

        const AString desiredPolyLinkedIdentifier(desiredPolyhedron->getLinkedPolyhedronIdentifier());
        EventAnnotationPolyhedronGetByLinkedIdentifier polyEvent(NULL,
                                                                 AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE,
                                                                 desiredPolyLinkedIdentifier);
        EventManager::get()->sendEvent(polyEvent.getPointer());
        if (polyEvent.getPolyhedron() != NULL) {
            const AString msg("There is an ACTUAL SAMPLE linked to the selected DESIRED SAMPLE.  "
                              "Only one ACTUAL SAMPLE may be linked to a DESIRED SAMPLE.  Name of "
                              "the existing ACTUAL SAMPLE is \""
                              + polyEvent.getPolyhedron()->getName() + "\"");
            WuQMessageBoxTwo::critical(this, "ERROR", msg);
            return;
        }
        
        QMenu menu(this);
        QAction* copyAction = menu.addAction("Create as Copy of Desired Sample Polyhedron");
        QAction* drawAction = menu.addAction("Draw Actual Sample as New Polyhedron");
        QAction* actionSelected(menu.exec(m_newActualSampleToolButton->mapToGlobal(QPoint(0, 0))));
        if (actionSelected == copyAction) {
            AnnotationPolyhedron* actualPolyhedron(new AnnotationPolyhedron(*desiredPolyhedron));
            actualPolyhedron->setPolyhedronType(AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE);
            actualPolyhedron->setLinkedPolyhedronIdentifier(desiredPolyLinkedIdentifier);
            actualPolyhedron->getSampleMetaData()->setActualSampleEditDate(AnnotationSampleMetaData::getCurrentDateInString());
            CaretAssert(samplesFile);
            EventAnnotationAddToRemoveFromFile addEvent(EventAnnotationAddToRemoveFromFile::MODE_CREATE,
                                                        samplesFile,
                                                        actualPolyhedron);
            EventManager::get()->sendEvent(addEvent.getPointer());
            
            AnnotationManager* annMan(GuiManager::get()->getBrain()->getAnnotationManager(UserInputModeEnum::Enum::SAMPLES_EDITING));
            std::vector<Annotation*> selectedAnnotations { actualPolyhedron };
            annMan->setAnnotationsForEditing(m_browserWindowIndex,
                                             selectedAnnotations);
            
            updateContent();
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        }
        else if (actionSelected == drawAction) {
            createNewSample(AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE,
                            desiredPolyhedron);
        }
    }
    else {
        const AString msg("There is more than one Desired Sample selected.  There must be exactly one "
                          "selected Desired Sample when creating an associated Actual Sample or no "
                          "selected Desired Sample to create an unassociated Actual Sample.");
        WuQMessageBoxTwo::warning(m_newActualSampleToolButton,
                                  "Warning",
                                  msg);
    }
}

/**
 * Called when new desired sample action triggered
 */
void
AnnotationSamplesInsertNewWidget::newDesiredSampleActionTriggered()
{
    if (isVolumeSliceMontageEnabledInSelectedTab(m_newDesiredSampleToolButton)) {
        AnnotationPolyhedron* linkedPolyhedron(NULL);
        createNewSample(AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE,
                        linkedPolyhedron);
    }
}

/**
 * @return True if a volume slice montage is enabled in the selected tab
 * @param parent
 *    Parent widget for dialogs
 */
bool
AnnotationSamplesInsertNewWidget::isVolumeSliceMontageEnabledInSelectedTab(QWidget* parent) const
{
    const BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex));
    CaretAssert(bbw);
    const BrowserTabContent* tabContent(bbw->getBrowserTabContent());
    if (tabContent != NULL) {
        FunctionResult result(tabContent->isVolumeSlicesMontageDisplayed());
        if (result.isOk()) {
            return true;
        }
        WuQMessageBoxTwo::critical(parent,
                                   "Error",
                                   result.getErrorMessage());
    }
    return false;
}

/**
 * Create a new sample of the given polyhedron type
 * @param polyhedronType
 *    Type of polyhedron to create
 * @param linkedPolyhedron
 *    Polyhedron that is linked to the newly created polyhedron
 */
void
AnnotationSamplesInsertNewWidget::createNewSample(const AnnotationPolyhedronTypeEnum::Enum polyhedronType,
                                                  AnnotationPolyhedron* linkedPolyhedron)
{
    SamplesFile* samplesFile(getSelectedSamplesFile());
    if (samplesFile == NULL) {
        WuQMessageBox::errorOk(this, "No file is selected.  Click the New button to create a new samples file.");
        return;
    }
    
    BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex));
    CaretAssert(bbw);
    if (bbw != NULL) {
        BrowserWindowContent* bwc(bbw->getBrowerWindowContent());
        CaretAssert(bwc);
        if (bwc != NULL) {
            SamplesDrawingSettings* samplesSettings(bwc->getSamplesDrawingSettings());
            CaretAssert(samplesSettings);
            samplesSettings->setPolyhedronDrawingType(polyhedronType);
            if (linkedPolyhedron != NULL) {
                samplesSettings->setLinkedPolyhedronIdentifier(linkedPolyhedron->getLinkedPolyhedronIdentifier());
            }
            else {
                samplesSettings->setLinkedPolyhedronIdentifier("");
            }
        }
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
    CaretAssert(bbw);
    if (bbw != NULL) {
        BrowserWindowContent* bwc(bbw->getBrowerWindowContent());
        CaretAssert(bwc);
        if (bwc != NULL) {
            SamplesDrawingSettings* samplesSettings(bwc->getSamplesDrawingSettings());
            CaretAssert(samplesSettings);
            const SamplesDrawingModeEnum::Enum drawingMode(m_samplesDrawingModeEnumComboBox->getSelectedItem<SamplesDrawingModeEnum,SamplesDrawingModeEnum::Enum>());
            samplesSettings->setDrawingMode(drawingMode);
            EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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
    CaretAssert(bbw);
    if (bbw != NULL) {
        BrowserWindowContent* bwc(bbw->getBrowerWindowContent());
        CaretAssert(bwc);
        if (bwc != NULL) {
            SamplesDrawingSettings* samplesSettings(bwc->getSamplesDrawingSettings());
            CaretAssert(samplesSettings);
            samplesSettings->setLowerSliceOffset(value);
            EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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
    CaretAssert(bbw);
    if (bbw != NULL) {
        BrowserWindowContent* bwc(bbw->getBrowerWindowContent());
        CaretAssert(bwc);
        if (bwc != NULL) {
            SamplesDrawingSettings* samplesSettings(bwc->getSamplesDrawingSettings());
            CaretAssert(samplesSettings);
            samplesSettings->setUpperSliceOffset(value);
            EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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

/**
 * @return All polyhedrons in memory
 */
std::vector<AnnotationPolyhedron*>
AnnotationSamplesInsertNewWidget::getAllPolyhedrons()
{
    std::vector<AnnotationPolyhedron*> polyhedrons;
    
    const AnnotationManager* annMan(GuiManager::get()->getBrain()->getAnnotationManager(UserInputModeEnum::Enum::SAMPLES_EDITING));
    const std::vector<Annotation*> allAnnotations(annMan->getAllAnnotations());
    for (const auto& ann : allAnnotations) {
        AnnotationPolyhedron* ap(ann->castToPolyhedron());
        if (ap != NULL) {
            polyhedrons.push_back(ap);
        }
    }

    return polyhedrons;
}

/**
 * @return All selected desired samples
 */
std::vector<AnnotationPolyhedron*>
AnnotationSamplesInsertNewWidget::getSelectedDesiredSamples()
{
    std::vector<AnnotationPolyhedron*> polyhedrons;
    
    const AnnotationManager* annMan(GuiManager::get()->getBrain()->getAnnotationManager(UserInputModeEnum::Enum::SAMPLES_EDITING));
    const std::vector<Annotation*> selectedAnnotations(annMan->getAnnotationsSelectedForEditing(m_browserWindowIndex));
    for (const auto& ann : selectedAnnotations) {
        AnnotationPolyhedron* ap(ann->castToPolyhedron());
        if (ap != NULL) {
            switch (ap->getPolyhedronType()) {
                case AnnotationPolyhedronTypeEnum::INVALID:
                    break;
                case AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE:
                    break;
                case AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE:
                    polyhedrons.push_back(ap);
                    break;
            }
        }
    }
    
    return polyhedrons;
}

/**
 * Called when select tool button action is triggered
 */
void
AnnotationSamplesInsertNewWidget::selectActionTriggered()
{
    std::vector<std::pair<QAction*, AnnotationPolyhedron*>> menuActionSamples;
    
    QMenu menu(this);
    
    std::vector<AnnotationPolyhedron*> polyhedrons(getAllPolyhedrons());
    for (AnnotationPolyhedron* ap : polyhedrons) {
        const AnnotationSampleMetaData* asmd(ap->getSampleMetaData());
        CaretAssert(asmd);
        AString sampleNumber(asmd->getSampleNumber());
        if (sampleNumber.isEmpty()) {
            sampleNumber = "No-Num";
        }
        AString sampleName(asmd->getSampleName());
        if (sampleName.isEmpty()) {
            sampleName = "No-Name";
        }
        const AString text(sampleNumber
                           + " "
                           + sampleName);
        
        menuActionSamples.emplace_back(menu.addAction(text),
                                       ap);
    }
    
    QAction* actionSelected(menu.exec(m_selectToolButton->mapToGlobal(QPoint(0, 0))));
    if (actionSelected != NULL) {
        for (const auto& ap : menuActionSamples) {
            if (ap.first == actionSelected) {
                AnnotationSamplesMetaDataDialog dialog(ap.second,
                                                       m_selectToolButton);
                dialog.exec();
                EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
            }
        }
    }
}

/**
 * @return Tooltip for new sample action
 * @param polyhedronType
 *    Type of polyhedron to create
 */
AString
AnnotationSamplesInsertNewWidget::getNewSampleToolTip(const AnnotationPolyhedronTypeEnum::Enum polyhedronType)
{
    const AString typeString(AnnotationPolyhedronTypeEnum::toGuiName(polyhedronType));
    const AString sampleToolTipText("<html>"
                                    "Click this button to initiate the drawing of a " + typeString + " polyhedron.<br>"
                                    "Use the <b>Slice</b> controls to exclude slices from drawing (a large 'X' "
                                    "will appear over the excluded slices).  <br><br>"
                                    "To draw a sample polyhedron:"
                                    "<ul>"
                                    "<li> <i>Click</i> the mouse to insert coordinates and create straight, possibly longer lines"
                                    "<li> <i>Drag</i> (move with left button down) the mouse to create curved lines "
                                    "<li> Note that one can intermix clicks and drags while drawing"
                                    "<li> To remove the most recently entered coordinate, click the <b>X</b> "
                                    "button in the <b>Drawing</b> section of the Toolbar"
                                    "<li> When finished, <i>click</i> the <b>Finish</b> button in the <b>Drawing</b> "
                                    "section of the Toolbar OR <i>shift-click</i> the mouse to conclude drawing of "
                                    "the sample polyhedron"
                                    "<li> To cancel drawing of the sample polyhedron, <i>click</i> the <b>Cancel</b> button "
                                    "in the <b>Drawing</b> section of the Toolbar"
                                    "</ul>"
                                    "</html>");
    return sampleToolTipText;
}

