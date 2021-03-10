
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

#define __ANNOTATION_CHANGE_COORDINATE_DIALOG_DECLARE__
#include "AnnotationChangeCoordinateDialog.h"
#undef __ANNOTATION_CHANGE_COORDINATE_DIALOG_DECLARE__

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include "Annotation.h"
#include "AnnotationCoordinate.h"
#include "AnnotationCoordinateInformation.h"
#include "AnnotationCoordinateSelectionWidget.h"
#include "CaretAssert.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationChangeCoordinateDialog 
 * \brief Dialog for changing an annotation coordinate.
 * \ingroup GuiQt
 *
 * When the user changes a coordinate it may be desirable to 
 * change the coordinate space such as moving from one tab
 * to another tab.
 */

/**
 * Constructor.
 */
AnnotationChangeCoordinateDialog::AnnotationChangeCoordinateDialog(const AnnotationCoordinateInformation& coordInfo,
                                                                   Annotation* annotation,
                                                                   AnnotationCoordinate* coordinate,
                                                                   AnnotationCoordinate* secondCoordinate,
                                                                   QWidget* parent)
: WuQDialogModal("Change Coordinate",
                 parent),
m_coordInfo(coordInfo),
m_annotation(annotation),
m_coordinate(coordinate),
m_secondCoordinate(secondCoordinate)
{
    CaretAssert(annotation);
    CaretAssert(coordinate);
    
    QWidget* currentCoordinateWidget = createCurrentCoordinateWidget();
    QWidget* newCoordinateWidget     = createNewCoordinateWidget();
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(currentCoordinateWidget);
    layout->addWidget(newCoordinateWidget);
    
    setCentralWidget(widget,
                     SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
AnnotationChangeCoordinateDialog::~AnnotationChangeCoordinateDialog()
{
}

/**
 * @return Widget containing new coordinate selection
 */
QWidget*
AnnotationChangeCoordinateDialog::createNewCoordinateWidget()
{
    m_coordinateSelectionWidget = new AnnotationCoordinateSelectionWidget(m_annotation->getType(),
                                                                          m_coordInfo,
                                                                          NULL);
    m_coordinateSelectionWidget->selectCoordinateSpace(m_annotation->getCoordinateSpace());
    QGroupBox* newCoordGroupBox = new QGroupBox("New Coordinate");
    QVBoxLayout* newCoordGroupLayout = new QVBoxLayout(newCoordGroupBox);
    newCoordGroupLayout->setContentsMargins(0, 0, 0, 0);
    newCoordGroupLayout->addWidget(m_coordinateSelectionWidget);
    
    return newCoordGroupBox;
}

/**
 * @return Widget containing current coordinate.
 */
QWidget*
AnnotationChangeCoordinateDialog::createCurrentCoordinateWidget()
{
    QString spaceText(AnnotationCoordinateSpaceEnum::toGuiName(m_annotation->getCoordinateSpace())
                      + " ");

    bool useXyzFlag = true;
    switch (m_annotation->getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            spaceText += m_annotation->getSpacerTabIndex().getRowColumnGuiText();
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
        {
            useXyzFlag = false;
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t numberOfNodes = -1;
            int32_t nodeIndex = -1;
            m_coordinate->getSurfaceSpace(structure,
                                          numberOfNodes,
                                          nodeIndex);
            spaceText += ("Structure: "
                          + StructureEnum::toGuiName(structure)
                          + " Node Index="
                          + QString::number(nodeIndex));
        }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            spaceText += (AString::number(m_annotation->getTabIndex() + 1) + " ");
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            spaceText += (AString::number(m_annotation->getWindowIndex() + 1) + " ");
            break;
    }

    QGroupBox* groupBox = new QGroupBox("Coordinate");
    if (useXyzFlag) {
        float xyz[3];
        m_coordinate->getXYZ(xyz);
        
        QGridLayout* gridLayout = new QGridLayout(groupBox);
        int row = gridLayout->rowCount();
        gridLayout->addWidget(new QLabel("Space"),
                              row, 0,
                              Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("X"),
                              row, 1,
                              Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Y"),
                              row, 2,
                              Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Z"),
                              row, 3,
                              Qt::AlignHCenter);
        row++;
        gridLayout->addWidget(new QLabel(spaceText),
                              row, 0);
        gridLayout->addWidget(new QLabel(QString::number(xyz[0], 'f', 1)),
                              row, 1,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(xyz[1], 'f', 1)),
                              row, 2,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(xyz[2], 'f', 1)),
                              row, 3,
                              Qt::AlignRight);
    }
    else {
        QLabel* spaceLabel = new QLabel(spaceText);
        QVBoxLayout* layout = new QVBoxLayout(groupBox);
        layout->addWidget(spaceLabel);
    }
    
    return groupBox;
}


/**
 * Called when the OK button is clicked.
 */
void
AnnotationChangeCoordinateDialog::okButtonClicked()
{
    QString errorMessage;
    if ( ! m_coordinateSelectionWidget->changeAnnotationCoordinate(m_annotation,
                                                                   errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
 
    WuQDialogModal::okButtonClicked();
}

