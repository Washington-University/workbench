
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

#include "AnnotationArrow.h"
#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationLine.h"
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
#include "EventAnnotation.h"
#include "EventDataFileAdd.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
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
: WuQDialogModal("Create Annotation",
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
    QWidget* spaceSelectionWidget = createSpaceSelectionWidget();
    QWidget* textWidget = ((m_annotationType == AnnotationTypeEnum::TEXT)
                           ? createTextWidget()
                           : NULL);

    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
    if (m_fileSelectionWidget != NULL) {
        layout->addWidget(m_fileSelectionWidget);
    }
    layout->addWidget(spaceSelectionWidget);
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
    m_annotationFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileType(GuiManager::get()->getBrain(),
                                                                                                  DataFileTypeEnum::ANNOTATION);
    if (s_previousAnnotationFile != NULL) {
        m_annotationFileSelectionModel->setSelectedFile(s_previousAnnotationFile);
    }
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
 * @return New instance of the space selection widget.
 */
QWidget*
AnnotationCreateDialog::createSpaceSelectionWidget()
{
    bool enableModelSpaceFlag   = false;
    bool enableSurfaceSpaceFlag = false;
    bool enableTabSpaceFlag     = true;
    bool enableWindowSpaceFlag  = true;
    
    switch (m_annotationType) {
        case AnnotationTypeEnum::ARROW:
            break;
        case AnnotationTypeEnum::BOX:
            enableModelSpaceFlag   = true;
            enableSurfaceSpaceFlag = true;
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            enableModelSpaceFlag   = true;
            enableSurfaceSpaceFlag = true;
            break;
        case AnnotationTypeEnum::TEXT:
            enableModelSpaceFlag   = true;
            enableSurfaceSpaceFlag = true;
            break;
    }
    
    m_spaceButtonGroup = new QButtonGroup(this);
    
    int columnIndex = 0;
    const int COLUMN_RADIO_BUTTON = columnIndex++;
    const int COLUMN_COORD_X      = columnIndex++;
    const int COLUMN_COORD_Y      = columnIndex++;
    const int COLUMN_COORD_Z      = columnIndex++;
    const int COLUMN_EXTRA        = columnIndex++;
    
    QWidget* widget = new QGroupBox("Coordinate Space");
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->setColumnStretch(COLUMN_RADIO_BUTTON, 0);
    gridLayout->setColumnStretch(COLUMN_COORD_X,      0);
    gridLayout->setColumnStretch(COLUMN_COORD_Y,      0);
    gridLayout->setColumnStretch(COLUMN_COORD_Z,      0);
    gridLayout->setColumnStretch(COLUMN_EXTRA,      100);
    
    const int titleRow = gridLayout->rowCount();
    gridLayout->addWidget(new QLabel("Space"),
                          titleRow, COLUMN_RADIO_BUTTON);
    gridLayout->addWidget(new QLabel("X"),
                          titleRow, COLUMN_COORD_X);
    gridLayout->addWidget(new QLabel("Y"),
                          titleRow, COLUMN_COORD_Y);
    gridLayout->addWidget(new QLabel("Z"),
                          titleRow, COLUMN_COORD_Z);
    
    if (m_coordInfo.m_modelXYZValid
        && enableModelSpaceFlag) {
        QRadioButton* rb = createRadioButtonForSpace(AnnotationCoordinateSpaceEnum::MODEL);
        m_spaceButtonGroup->addButton(rb,
                                      AnnotationCoordinateSpaceEnum::toIntegerCode(AnnotationCoordinateSpaceEnum::MODEL));
        
        const int rowNum = gridLayout->rowCount();
        gridLayout->addWidget(rb,
                              rowNum, COLUMN_RADIO_BUTTON);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_modelXYZ[0])),
                              rowNum, COLUMN_COORD_X);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_modelXYZ[1])),
                              rowNum, COLUMN_COORD_Y);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_modelXYZ[2])),
                              rowNum, COLUMN_COORD_Z);
        enableModelSpaceFlag = false;
    }
    
    if ((m_coordInfo.m_tabIndex >= 0)
        && enableTabSpaceFlag) {
        QRadioButton* rb = createRadioButtonForSpace(AnnotationCoordinateSpaceEnum::TAB);
        rb->setText(rb->text()
                    + " "
                    + AString::number(m_coordInfo.m_tabIndex + 1));
        m_spaceButtonGroup->addButton(rb,
                                      AnnotationCoordinateSpaceEnum::toIntegerCode(AnnotationCoordinateSpaceEnum::TAB));
        
        const int rowNum = gridLayout->rowCount();
        gridLayout->addWidget(rb,
                              rowNum, COLUMN_RADIO_BUTTON);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_tabXYZ[0])),
                              rowNum, COLUMN_COORD_X);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_tabXYZ[1])),
                              rowNum, COLUMN_COORD_Y);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_tabXYZ[2])),
                              rowNum, COLUMN_COORD_Z);
    }
    
    if ((m_coordInfo.m_windowIndex >= 0)
        && enableWindowSpaceFlag) {
        QRadioButton* rb = createRadioButtonForSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        rb->setText(rb->text()
                    + " "
                    + AString::number(m_coordInfo.m_windowIndex + 1));
        m_spaceButtonGroup->addButton(rb,
                                      AnnotationCoordinateSpaceEnum::toIntegerCode(AnnotationCoordinateSpaceEnum::WINDOW));

        
        const int rowNum = gridLayout->rowCount();
        gridLayout->addWidget(rb,
                              rowNum, COLUMN_RADIO_BUTTON);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_windowXYZ[0])),
                              rowNum, COLUMN_COORD_X);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_windowXYZ[1])),
                              rowNum, COLUMN_COORD_Y);
        gridLayout->addWidget(new QLabel(AString::number(m_coordInfo.m_windowXYZ[2])),
                              rowNum, COLUMN_COORD_Z);
    }
    
    if (m_coordInfo.m_surfaceNodeValid
        && enableSurfaceSpaceFlag) {
        
        QRadioButton* rb = createRadioButtonForSpace(AnnotationCoordinateSpaceEnum::SURFACE);
        m_spaceButtonGroup->addButton(rb,
                                      AnnotationCoordinateSpaceEnum::toIntegerCode(AnnotationCoordinateSpaceEnum::SURFACE));

        
        const int rowNum = gridLayout->rowCount();
        gridLayout->addWidget(rb,
                              rowNum, COLUMN_RADIO_BUTTON);
        const AString infoText(StructureEnum::toGuiName(m_coordInfo.m_surfaceStructure)
                               + " Vertex: "
                               +AString::number(m_coordInfo.m_surfaceNodeIndex));
        gridLayout->addWidget(new QLabel(infoText),
                              rowNum, COLUMN_COORD_X, 1, 4);
    }
    
    /*
     * This switch statment does nothing.  But, if a new space is added
     * the missing enumerated value in the switch statement will cause a
     * compilation error which may indicate the code in this method
     * needs to be updated.
     */
    const AnnotationCoordinateSpaceEnum::Enum space = AnnotationCoordinateSpaceEnum::TAB;
    switch (space) {
        case AnnotationCoordinateSpaceEnum::MODEL:
        case AnnotationCoordinateSpaceEnum::PIXELS:
        case AnnotationCoordinateSpaceEnum::SURFACE:
        case AnnotationCoordinateSpaceEnum::TAB:
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
    }
    
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
    s_previousAnnotationFile = newFile;
    m_annotationFileSelectionModel->setSelectedFile(s_previousAnnotationFile);
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
        //WuQMessageBox::errorOk(this, );
        //return;
    }
    const int checkedButtonID = m_spaceButtonGroup->checkedId();
    if (checkedButtonID < 0) {
        errorMessage.appendWithNewLine("A coordinate space must be selected.");
        //WuQMessageBox::errorOk(this, "A space must be selected.");
        //return;
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
    const AnnotationCoordinateSpaceEnum::Enum space = AnnotationCoordinateSpaceEnum::fromIntegerCode(checkedButtonID,
                                                                                                     &valid);
    if ( ! valid) {
        const QString msg("PROGRAM ERROR: Annotation coordinate space is not valid.  This should not happen.");
        WuQMessageBox::errorOk(this,
                               msg);
        return;
    }
    CaretAssert(annotationFile);
    
    
    CaretPointer<Annotation> annotation;
    annotation.grabNew(NULL);
    
    switch (m_annotationType) {
        case AnnotationTypeEnum::ARROW:
            annotation.grabNew(new AnnotationArrow());
            break;
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
    
    AnnotationTwoDimensionalShape* twoDimAnnotation = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation.getPointer());
    AnnotationOneDimensionalShape* oneDimAnnotation = dynamic_cast<AnnotationOneDimensionalShape*>(annotation.getPointer());
    
    AnnotationCoordinate* firstCoordinate  = NULL;
    AnnotationCoordinate* secondCoordinate = NULL;
    
    if (twoDimAnnotation != NULL) {
        firstCoordinate = twoDimAnnotation->getCoordinate();
    }
    else if (oneDimAnnotation != NULL) {
        firstCoordinate = oneDimAnnotation->getStartCoordinate();
        secondCoordinate = oneDimAnnotation->getEndCoordinate();
    }

    switch (space) {
        case AnnotationCoordinateSpaceEnum::MODEL:
            if (m_coordInfo.m_modelXYZValid) {
                firstCoordinate->setXYZ(m_coordInfo.m_modelXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MODEL);
            }
            break;
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (m_coordInfo.m_surfaceNodeValid) {
                firstCoordinate->setSurfaceSpace(m_coordInfo.m_surfaceStructure,
                                                 m_coordInfo.m_surfaceNumberOfNodes,
                                                 m_coordInfo.m_surfaceNodeIndex,
                                                 m_coordInfo.m_surfaceNodeOffset);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (m_coordInfo.m_tabIndex >= 0) {
                firstCoordinate->setXYZ(m_coordInfo.m_tabXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                annotation->setTabIndex(m_coordInfo.m_tabIndex);
                
                if (secondCoordinate != NULL) {
                    double xyz[3] = {
                        m_coordInfo.m_tabXYZ[0],
                        m_coordInfo.m_tabXYZ[1],
                        m_coordInfo.m_tabXYZ[2]
                    };
                    if (xyz[1] > 0.5) {
                        xyz[1] -= 0.25;
                    }
                    else {
                        xyz[1] += 0.25;
                    }
                    secondCoordinate->setXYZ(xyz);
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (m_coordInfo.m_windowIndex >= 0) {
                firstCoordinate->setXYZ(m_coordInfo.m_windowXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                annotation->setWindowIndex(m_coordInfo.m_windowIndex);
                
                if (secondCoordinate != NULL) {
                    double xyz[3] = {
                        m_coordInfo.m_tabXYZ[0],
                        m_coordInfo.m_tabXYZ[1],
                        m_coordInfo.m_tabXYZ[2]
                    };
                    if (xyz[1] > 0.5) {
                        xyz[1] -= 0.25;
                    }
                    else {
                        xyz[1] += 0.25;
                    }
                    secondCoordinate->setXYZ(xyz);
                }
            }
            break;
    }
    
    /*
     * Need to release annotation from its CaretPointer since the
     * annotation file will take ownership of the annotation.
     */
    Annotation* annotationPointer = annotation.releasePointer();
    
    annotationFile->addAnnotation(annotationPointer);

    annotationPointer->setSelected(true);
    EventManager::get()->sendEvent(EventAnnotation().setModeEditAnnotation(m_coordInfo.m_windowIndex,
                                                                           annotationPointer).getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_coordInfo.m_windowIndex).getPointer());
    
    WuQDialog::okButtonClicked();
}

