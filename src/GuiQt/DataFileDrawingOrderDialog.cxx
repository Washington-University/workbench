
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __DATA_FILE_DRAWING_ORDER_DIALOG_DECLARE__
#include "DataFileDrawingOrderDialog.h"
#undef __DATA_FILE_DRAWING_ORDER_DIALOG_DECLARE__

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>

#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "WuQMessageBoxTwo.h"

using namespace caret;


    
/**
 * \class caret::DataFileDrawingOrderDialog 
 * \brief Dialog for setting the drawing order of data files.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param dataFileType
 *    Data file types for sorting
 * @param parent
 *    Parent widget
 */
DataFileDrawingOrderDialog::DataFileDrawingOrderDialog(const DataFileTypeEnum::Enum dataFileType,
                                                       QWidget* parent)
: WuQDialogModal("Data Drawing Order",
                 parent),
m_dataFileType(dataFileType)
{
    m_dataFilesListWidget = new QListWidget();
    m_dataFilesListWidget->setDragDropMode(QListWidget::DragDropMode::InternalMove);
    
    QObject::connect(m_dataFilesListWidget->model(), &QAbstractItemModel::rowsMoved,
                     this, [=]() { orderOfFilesChanged(); } );
    
    const QString instructionsText("* Drag file names to change the drawing order\n"
                                   "* File at top is drawn on top\n"
                                   "* File at bottom is drawn on bottom\n"
                                   "* Changes must be saved to a scene");
    QLabel* instructionsLabel(new QLabel(instructionsText));
    
    QWidget* dialogWidget(new QWidget());
    QGridLayout* gridLayout(new QGridLayout(dialogWidget));
    gridLayout->setRowStretch(1000, 100);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->addWidget(m_dataFilesListWidget, 1, 0, 1, 2);
    gridLayout->addWidget(instructionsLabel, 2, 0, 1, 2);
    
    setCentralWidget(dialogWidget, ScrollAreaStatus::SCROLL_AREA_AS_NEEDED);
    setCancelButtonText("");
    
    dataFileTypeSelected();
}

/**
 * Destructor.
 */
DataFileDrawingOrderDialog::~DataFileDrawingOrderDialog()
{
}

/**
 * Called when the order of the files is changed
 */
void
DataFileDrawingOrderDialog::orderOfFilesChanged()
{
    std::vector<AString> dataFileNames;
    
    const int32_t numFiles(m_dataFilesListWidget->count());
    for (int32_t iRow = (numFiles - 1); iRow >= 0; iRow--) {
        dataFileNames.push_back(m_dataFilesListWidget->item(iRow)->data(Qt::UserRole).toString());
    }
    Brain* brain(GuiManager::get()->getBrain());
    CaretAssert(brain);
    const FunctionResult result(brain->setAllDataFilesWithDataFileTypeOrder(m_dataFileType,
                                                                            dataFileNames));
    if (result.isError()) {
        WuQMessageBoxTwo::critical(this,
                                   "Error",
                                   result.getErrorMessage());
    }
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}


/**
 * Called when a data file type is selected
 */
void
DataFileDrawingOrderDialog::dataFileTypeSelected()
{
    switch (m_dataFileType) {
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        default:
            CaretAssertMessage(0, ("Unsupported data file type: "
                                   + DataFileTypeEnum::toName(m_dataFileType)));
            break;
    }
    
    Brain* brain(GuiManager::get()->getBrain());
    CaretAssert(brain);
    std::vector<CaretDataFile*> dataFiles(brain->getAllDataFilesWithDataFileType(m_dataFileType));
    
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(dataFiles,
                                                                            displayNames);
    CaretAssert(dataFiles.size() == displayNames.size());

    m_dataFilesListWidget->clear();
    const int32_t numDataFiles(dataFiles.size());
    for (int32_t iFile = (numDataFiles - 1); iFile >= 0; iFile--) {
        const CaretDataFile* cdf(dataFiles[iFile]);
        CaretAssert(cdf);
        CaretAssertVectorIndex(displayNames, iFile);
        QListWidgetItem* item (new QListWidgetItem(displayNames[iFile]));
        item->setData(Qt::UserRole, cdf->getFileName());
        m_dataFilesListWidget->addItem(item);
    }
}

/**
 * Called when the Ok button is clicked
 */
void
DataFileDrawingOrderDialog::okButtonClicked()
{
    
    WuQDialogModal::okButtonClicked();
}

