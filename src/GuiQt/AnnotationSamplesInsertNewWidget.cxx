
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
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

#include "AnnotationPolyhedron.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "EventAnnotationCreateNewType.h"
#include "EventAnnotationGetBeingDrawnInWindow.h"
#include "EventAnnotationGetDrawingPolyhedronSliceDepth.h"
#include "EventAnnotationGetSelectedInsertNewFile.h"
#include "EventDataFileAdd.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
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
    
    const AString sampleToolTipText("To draw a polyhedron:"
                                    "Click this button to insert a new polyhedron and then click or drag the mouse to draw "
                                    "the first face of the polyhedron.  As points are added, points are added on additional "
                                    "volume slices."
                                    "<ul>"
                                    "<li> <i>Click</i> the mouse to insert coordinates and create straight, possibly longer lines"
                                    "<li> <i>Drag</i> (move with left button down) the mouse to create curved lines "
                                    "<li> Note that one can intermix clicks and drags while drawing"
                                    "<li> When finished, <i>shift-click</i> the mouse to finalize the polygon (does NOT add "
                                    "another coordinate) or click the <i>Finish</i> button"
                                    "</ul>");
    
    QLabel* fileLabel(new QLabel("File"));
    
    m_fileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileType(DataFileTypeEnum::SAMPLES);
    m_fileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel);
    m_fileSelectionComboBox->setFixedWidth(220);
    m_fileSelectionComboBox->setNoFilesText("Click \"New\" to create a file");
    
    QAction* newFileAction(new QAction());
    newFileAction->setText("New...");
    newFileAction->setToolTip("Create a new Samples File");
    QObject::connect(newFileAction, &QAction::triggered,
                     this, &AnnotationSamplesInsertNewWidget::newFileActionTriggered);
    QToolButton* newFileToolButton(new QToolButton());
    newFileToolButton->setDefaultAction(newFileAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(newFileToolButton);
    
    m_newSampleAction = new QAction();
    m_newSampleAction->setText("Insert New Sample");
    m_newSampleAction->setToolTip(sampleToolTipText);
    QObject::connect(m_newSampleAction, &QAction::triggered,
                     this, &AnnotationSamplesInsertNewWidget::newSampleActionTriggered);
    QToolButton* newSampleToolButton(new QToolButton());
    WuQtUtilities::setToolButtonStyleForQt5Mac(newSampleToolButton);
    newSampleToolButton->setDefaultAction(m_newSampleAction);
    
    m_slicesLabel = new QLabel(" Slices");
    m_newSampleDepthSpinBox = new QSpinBox();
    m_newSampleDepthSpinBox->setMinimum(-99);
    m_newSampleDepthSpinBox->setMaximum(99);
    m_newSampleDepthSpinBox->setSingleStep(1);
    m_newSampleDepthSpinBox->setValue(m_previousNewSampleDepthSpinBoxValue);
    QObject::connect(m_newSampleDepthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &AnnotationSamplesInsertNewWidget::newSampleDepthValueChanged);
    m_newSampleDepthSpinBox->setToolTip("Polyhedron spans this number of slices");

    QHBoxLayout* samplesLayout(new QHBoxLayout());
    WuQtUtilities::setLayoutSpacingAndMargins(samplesLayout, 2, 2);
    samplesLayout->addWidget(newSampleToolButton);
    samplesLayout->addWidget(m_slicesLabel);
    samplesLayout->addWidget(m_newSampleDepthSpinBox);
    samplesLayout->addStretch();
    
    QGridLayout* layout(new QGridLayout(this));
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    layout->setColumnStretch(2, 0);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(fileLabel,
                      0, 0);
    layout->addWidget(m_fileSelectionComboBox->getWidget(),
                      0, 1);
    layout->addWidget(newFileToolButton,
                      0, 2);
    layout->addLayout(samplesLayout,
                      1, 0, 1, 2, Qt::AlignLeft);
    
    setSizePolicy(QSizePolicy::Fixed,
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
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_NEW_DRAWING_POLYHEDRON_SLICE_DEPTH) {
        EventAnnotationGetDrawingPolyhedronSliceDepth* depthEvent(dynamic_cast<EventAnnotationGetDrawingPolyhedronSliceDepth*>(event));
        CaretAssert(event);
        if ((depthEvent->getUserInputMode() == m_userInputMode)
            && (depthEvent->getWindowIndex() == m_browserWindowIndex)) {
            if (m_newSampleDepthSpinBox != NULL) {
                depthEvent->setNumberOfSlicesDepth(m_newSampleDepthSpinBox->value());
                depthEvent->setEventProcessed();
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE) {
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
    EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_browserWindowIndex);
    EventManager::get()->sendEvent(annDrawEvent.getPointer());
    
    m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel);
    
    bool annDrawingFlag(false);
    const Annotation* annDraw(annDrawEvent.getAnnotation());
    if (annDraw != NULL) {
        annDrawingFlag = (annDraw->castToPolyhedron() != NULL);
    }
    
    m_slicesLabel->setEnabled(annDrawingFlag);
    m_newSampleDepthSpinBox->setEnabled(annDrawingFlag);
}

/**
 * Called when a file is selected from the file combo box
 * @param caretDataFile
 *    File selected
 */
void
AnnotationSamplesInsertNewWidget::fileSelectionComboBoxFileSelected(CaretDataFile* /*caretDataFile*/)
{
    
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
            m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel);
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
    
    EventManager::get()->sendEvent(EventAnnotationCreateNewType(m_browserWindowIndex,
                                                                m_userInputMode,
                                                                samplesFile,
                                                                annSpace,
                                                                annShape,
                                                                polyhedronDrawingMode).getPointer());
    
    updateContent();
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
 * Called when the depth value for the new sample is  changed
 * @param value
 *    The new value
 */
void
AnnotationSamplesInsertNewWidget::newSampleDepthValueChanged(int value)
{
    /*
     * Do not allow a zero value
     * If user transitions to zero from positive one, set to negative one
     * If user transitions to zero from negative one, set to positive one
     */
    if (value == 0) {
        if (m_previousNewSampleDepthSpinBoxValue > 0) {
            value = -1;
        }
        else if (m_previousNewSampleDepthSpinBoxValue < 0) {
            value = 1;
        }
        else {
            value = 1;
        }
        QSignalBlocker blocker(m_newSampleDepthSpinBox);
        m_newSampleDepthSpinBox->setValue(value);
    }
    
    m_previousNewSampleDepthSpinBoxValue = value;
    
    /*
     * Depth value is requested while the annotation is being drawn by graphics
     * so request a graphics update
     */
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
