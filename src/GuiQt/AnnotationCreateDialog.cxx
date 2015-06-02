
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

#define __ANNOTATION_CREATE_DIALOG_DECLARE__
#include "AnnotationCreateDialog.h"
#undef __ANNOTATION_CREATE_DIALOG_DECLARE__

#include <QButtonGroup>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationCoordinateSelectionWidget.h"
#include "AnnotationLine.h"
#include "AnnotationManager.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
#include "AnnotationFile.h"
#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretFileDialog.h"
#include "CaretPointer.h"
#include "EventDataFileAdd.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MouseEvent.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "Surface.h"
#include "UserInputModeAnnotations.h"
#include "WuQtUtilities.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCreateDialog 
 * \brief Dialog used for creating new annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotationType
 *      Type of annotation that is being created.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog::AnnotationCreateDialog(const MouseEvent& mouseEvent,
                                               const AnnotationTypeEnum::Enum annotationType,
                                               QWidget* parent)
: WuQDialogModal("Insert Annotation",
                 parent),
m_annotationType(annotationType)
{
    m_textLineEdit = NULL;
    
    /*
     * Get coordinates at the mouse location.
     */
    UserInputModeAnnotations::getCoordinatesFromMouseLocation(mouseEvent,
                                                              m_coordInfo);
    
    m_fileSelectionWidget = createFileSelectionWidget();
    m_coordinateSelectionWidget = new AnnotationCoordinateSelectionWidget(annotationType,
                                                                          m_coordInfo);
    QGroupBox* coordGroupBox = new QGroupBox("Coordinate Space");
    QVBoxLayout* coordGroupLayout = new QVBoxLayout(coordGroupBox);
    coordGroupLayout->setMargin(0);
    coordGroupLayout->addWidget(m_coordinateSelectionWidget);
    
    if (s_previousSelections.m_valid) {
        m_coordinateSelectionWidget->selectCoordinateSpace(s_previousSelections.m_coordinateSpace);
    }
    
    QWidget* textWidget = ((m_annotationType == AnnotationTypeEnum::TEXT)
                           ? createTextWidget()
                           : NULL);

    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
    if (m_fileSelectionWidget != NULL) {
        layout->addWidget(m_fileSelectionWidget);
    }
    layout->addWidget(coordGroupBox);
    if (textWidget != NULL) {
        layout->addWidget(textWidget);
    }
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
    
    if (annotationType == AnnotationTypeEnum::TEXT) {
        CaretAssert(m_textLineEdit);
        m_textLineEdit->setFocus();
    }
}

/**
 * Destructor.
 */
AnnotationCreateDialog::~AnnotationCreateDialog()
{
    delete m_annotationFileSelectionModel;
}

/**
 * @return New instance of the file selection widget.
 */
QWidget*
AnnotationCreateDialog::createFileSelectionWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    m_annotationFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileType(brain,
                                                                                                  DataFileTypeEnum::ANNOTATION);
    m_annotationFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    m_annotationFileSelectionComboBox->updateComboBox(m_annotationFileSelectionModel);
    
    QAction* newFileAction = WuQtUtilities::createAction("New",
                                                         "Create a new border file",
                                                         this,
                                                         this,
                                                         SLOT(newAnnotationFileButtonClicked()));
    QToolButton* newFileToolButton = new QToolButton();
    newFileToolButton->setDefaultAction(newFileAction);
    
    m_sceneAnnotationFileRadioButton = new QRadioButton("Scene Annotation File");
    m_brainAnnotationFileRadioButton = new QRadioButton("File");
    
    QButtonGroup* fileButtonGroup = new QButtonGroup(this);
    fileButtonGroup->addButton(m_brainAnnotationFileRadioButton);
    fileButtonGroup->addButton(m_sceneAnnotationFileRadioButton);
    
    if (s_previousSelections.m_valid) {
        /*
         * Default using previous selections
         */
        if (s_previousSelections.m_annotationFile == brain->getSceneAnnotationFile()) {
            m_sceneAnnotationFileRadioButton->setChecked(true);
        }
        else {
            m_annotationFileSelectionModel->setSelectedFile(s_previousSelections.m_annotationFile);
            m_annotationFileSelectionComboBox->updateComboBox(m_annotationFileSelectionModel);
            if (m_annotationFileSelectionModel->getSelectedFile() == s_previousSelections.m_annotationFile) {
                m_brainAnnotationFileRadioButton->setChecked(true);
            }
        }
    }
    else {
        /*
         * First time displayed, default to scene annotation file
         */
        m_sceneAnnotationFileRadioButton->setChecked(true);
    }
    
    QWidget* widget = new QGroupBox("Annotation File");
    QGridLayout* layout = new QGridLayout(widget);
    layout->setColumnStretch(0,   0);
    layout->setColumnStretch(1, 100);
    layout->setColumnStretch(2,   0);
    layout->addWidget(m_brainAnnotationFileRadioButton,
                      0, 0);
    layout->addWidget(m_annotationFileSelectionComboBox->getWidget(),
                      0, 1);
    layout->addWidget(newFileToolButton,
                      0, 2);
    layout->addWidget(m_sceneAnnotationFileRadioButton,
                      1, 0,
                      1, 3,
                      Qt::AlignLeft);
    
    return widget;
}

/**
 * Create a radio button that displays the text for and contains the 
 * enumerated value in a property.
 */
QRadioButton*
AnnotationCreateDialog::createRadioButtonForSpace(const AnnotationCoordinateSpaceEnum::Enum space)
{
    const AString spaceGuiName = AnnotationCoordinateSpaceEnum::toGuiName(space);
    const QString spaceEnumName = AnnotationCoordinateSpaceEnum::toName(space);
    QRadioButton* rb = new QRadioButton(spaceGuiName);
    rb->setProperty(s_SPACE_PROPERTY_NAME.toAscii().constData(),
                    spaceEnumName);
    
    return rb;
}
/**
 * Gets called when "New" file button is clicked.
 */
void
AnnotationCreateDialog::newAnnotationFileButtonClicked()
{
    /*
     * Let user choose a different path/name
     */
    AnnotationFile* newFile = new AnnotationFile();
    AString newFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::ANNOTATION,
                                                                       this,
                                                                       "Choose Annotation File Name",
                                                                       newFile->getFileName());
    /*
     * If user cancels, delete the new border file and return
     */
    if (newFileName.isEmpty()) {
        delete newFile;
        return;
    }
    
    /*
     * Set name of new border file, add file to brain, and make
     * file the selected annotation file.
     */
    newFile->setFileName(newFileName);
    EventManager::get()->sendEvent(EventDataFileAdd(newFile).getPointer());
    m_annotationFileSelectionModel->setSelectedFile(newFile);
    m_annotationFileSelectionComboBox->updateComboBox(m_annotationFileSelectionModel);
}

/**
 * @return New instance of text widget.
 */
QWidget*
AnnotationCreateDialog::createTextWidget()
{
    m_textLineEdit = new QLineEdit();
    m_textLineEdit->setText("");
    m_textLineEdit->selectAll();
    
    QGroupBox* groupBox = new QGroupBox("Text");
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    layout->addWidget(m_textLineEdit, 100);
    
    return groupBox;
}


/**
 * Gets called when the OK button is clicked.
 */
void
AnnotationCreateDialog::okButtonClicked()
{
    AString errorMessage;
    
    AnnotationFile* annotationFile = NULL;
    if (m_brainAnnotationFileRadioButton->isChecked()) {
        annotationFile = m_annotationFileSelectionModel->getSelectedFileOfType<AnnotationFile>();
        if (annotationFile == NULL) {
            errorMessage.appendWithNewLine("An annotation file must be selected.");
            //WuQMessageBox::errorOk(this, "An annotation file must be selected.");
            //return;
        }
    }
    else if (m_sceneAnnotationFileRadioButton->isChecked()) {
        annotationFile = GuiManager::get()->getBrain()->getSceneAnnotationFile();
    }
    else {
        errorMessage.appendWithNewLine("Type of anotation file is not selected.");
    }
    
    QString userText;
    if (m_annotationType == AnnotationTypeEnum::TEXT) {
        userText = m_textLineEdit->text().trimmed();
        if (userText.isEmpty()) {
            errorMessage.appendWithNewLine("Text is missing.");
        }
        
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    bool valid = false;
    const AnnotationCoordinateSpaceEnum::Enum space = m_coordinateSelectionWidget->getSelectedCoordinateSpace(valid);
    if ( ! valid) {
        const QString msg("A coordinate space has not been selected.");
        WuQMessageBox::errorOk(this,
                               msg);
        return;
    }
    CaretAssert(annotationFile);
    
    /*
     * Save for next time
     */
    s_previousSelections.m_annotationFile  = annotationFile;
    s_previousSelections.m_coordinateSpace = space;
    s_previousSelections.m_valid = true;
    
    CaretPointer<Annotation> annotation;
    annotation.grabNew(NULL);
    
    switch (m_annotationType) {
        case AnnotationTypeEnum::BOX:
            annotation.grabNew(new AnnotationBox());
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            annotation.grabNew(new AnnotationLine());
            break;
        case AnnotationTypeEnum::OVAL:
            annotation.grabNew(new AnnotationOval());
            break;
        case AnnotationTypeEnum::TEXT:
        {
            AnnotationText* text = new AnnotationText();
            text->setText(userText);
            annotation.grabNew(text);
        }
            break;
    }
    
    if ( ! m_coordinateSelectionWidget->setCoordinateForNewAnnotation(annotation,
                                                                      errorMessage)) {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    /*
     * Need to release annotation from its CaretPointer since the
     * annotation file will take ownership of the annotation.
     */
    Annotation* annotationPointer = annotation.releasePointer();
    
    annotationFile->addAnnotation(annotationPointer);

    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    annotationManager->selectAnnotation(AnnotationManager::SELECTION_MODE_SINGLE,
                                        annotationPointer);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    WuQDialog::okButtonClicked();
}

