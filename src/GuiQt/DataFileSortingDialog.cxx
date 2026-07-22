
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

#define __DATA_FILE_SORTING_DIALOG_DECLARE__
#include "DataFileSortingDialog.h"
#undef __DATA_FILE_SORTING_DIALOG_DECLARE__

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>

#include "Brain.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQMessageBoxTwo.h"

using namespace caret;


    
/**
 * \class caret::DataFileSortingDialog 
 * \brief Dialog for sorting data files.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param dataFileTypes
 *    Data file types for sorting
 * @param parent
 *    Parent widget
 */
DataFileSortingDialog::DataFileSortingDialog(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                                             const DataFileTypeEnum::Enum defaultDataFileType,
                                             QWidget* parent)
: WuQDialogModal("Sort Data Files",
                 parent)
{
    QLabel* fileTypeLabel(new QLabel("File Type "));
    m_dataFileTypeComboBox = new EnumComboBoxTemplate(this);
    m_dataFileTypeComboBox->setupWithItems<DataFileTypeEnum, DataFileTypeEnum::Enum>(dataFileTypes);
    QObject::connect(m_dataFileTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &DataFileSortingDialog::dataFileTypeSelected);
    
    m_dataFilesListWidget = new QListWidget();
    m_dataFilesListWidget->setDragDropMode(QListWidget::DragDropMode::InternalMove);
    
    QObject::connect(m_dataFilesListWidget->model(), &QAbstractItemModel::rowsMoved,
                     this, [=]() { orderOfFilesChanged(); } );
    
    const QString instructionsText("* Drag file names to reorder the files\n"
                                   "* Changes must be saved to a scene\n"
                                   "* File at top is drawn on top\n"
                                   "* File at bottom is drawn on bottom\n"
                                   "* For 3D surface:\n"
                                   "  - Draw borders as polylines or spheres\n"
                                   "  - Draw foci as spheres");
    QLabel* instructionsLabel(new QLabel(instructionsText));
    
    QWidget* dialogWidget(new QWidget());
    QGridLayout* gridLayout(new QGridLayout(dialogWidget));
    gridLayout->setRowStretch(1000, 100);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->addWidget(fileTypeLabel, 0, 0);
    gridLayout->addWidget(m_dataFileTypeComboBox->getWidget(), 0, 1, Qt::AlignLeft);
    gridLayout->addWidget(m_dataFilesListWidget, 1, 0, 1, 2);
    gridLayout->addWidget(instructionsLabel, 2, 0, 1, 2);
    
    setCentralWidget(dialogWidget, ScrollAreaStatus::SCROLL_AREA_AS_NEEDED);
    setCancelButtonText("");
    setOkButtonText("Close");
    
    if (std::find(dataFileTypes.begin(),
                  dataFileTypes.end(),
                  defaultDataFileType) != dataFileTypes.end()) {
        m_dataFileTypeComboBox->setSelectedItem<DataFileTypeEnum, DataFileTypeEnum::Enum>(defaultDataFileType);
    }
    
    dataFileTypeSelected();
}

/**
 * Destructor.
 */
DataFileSortingDialog::~DataFileSortingDialog()
{
}

/**
 * Called when the order of the files is changed
 */
void
DataFileSortingDialog::orderOfFilesChanged()
{
    std::vector<AString> dataFileNames;
    
    const int32_t numFiles(m_dataFilesListWidget->count());
    for (int32_t iRow = (numFiles - 1); iRow >= 0; iRow--) {
        dataFileNames.push_back(m_dataFilesListWidget->item(iRow)->data(Qt::UserRole).toString());
    }
    Brain* brain(GuiManager::get()->getBrain());
    CaretAssert(brain);
    const FunctionResult result(brain->setAllDataFilesWithDataFileTypeOrder(getDataFileTypeSelected(),
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
DataFileSortingDialog::dataFileTypeSelected()
{
    const DataFileTypeEnum::Enum dataFileType(getDataFileTypeSelected());
    
    switch (dataFileType) {
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        default:
            CaretAssertMessage(0, ("Unsupported data file type: "
                                   + DataFileTypeEnum::toName(dataFileType)));
            break;
    }
    
    Brain* brain(GuiManager::get()->getBrain());
    CaretAssert(brain);
    std::vector<CaretDataFile*> dataFiles(brain->getAllDataFilesWithDataFileType(dataFileType));
    
    m_dataFilesListWidget->clear();
    const int32_t numDataFiles(dataFiles.size());
    for (int32_t iFile = (numDataFiles - 1); iFile >= 0; iFile--) {
        const CaretDataFile* cdf(dataFiles[iFile]);
        CaretAssert(cdf);
        QListWidgetItem* item (new QListWidgetItem(cdf->getFileNameNoPath()));
        item->setData(Qt::UserRole, cdf->getFileName());
        m_dataFilesListWidget->addItem(item);
    }
}

/**
 * @return Data file type selected
 */
DataFileTypeEnum::Enum
DataFileSortingDialog::getDataFileTypeSelected() const
{
    DataFileTypeEnum::Enum dataFileType(DataFileTypeEnum::UNKNOWN);
    
    if (m_dataFileTypeComboBox->getComboBox()->count() > 0) {
        dataFileType = m_dataFileTypeComboBox->getSelectedItem<DataFileTypeEnum, DataFileTypeEnum::Enum>();
    }
    return dataFileType;
}


/**
 * Called when the Ok button is clicked
 */
void
DataFileSortingDialog::okButtonClicked()
{
    
    WuQDialogModal::okButtonClicked();
}

