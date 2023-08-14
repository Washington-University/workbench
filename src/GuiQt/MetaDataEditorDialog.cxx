
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

#define __META_DATA_EDITOR_DIALOG_DECLARE__
#include "MetaDataEditorDialog.h"
#undef __META_DATA_EDITOR_DIALOG_DECLARE__

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "Annotation.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretMappableDataFile.h"
#include "GiftiMetaData.h"
#include "MetaDataEditorWidget.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::MetaDataEditorDialog 
 * \brief Dialog for editing metadata.
 * \ingroup GuiQt
 */

/**
 * Constructor for editing an annotation's metadata.
 *
 * @param annotation
 *    Annotation that will have its metadata edited.
 * @param parent
 *    Widget on which this dialog is displayed.
 */
MetaDataEditorDialog::MetaDataEditorDialog(Annotation* annotation,
                                           QWidget* parent)
: WuQDialogModal("",
                 parent)
{
    CaretAssert(annotation);
    
    initializeDialog(("Edit Annotation Metadata: "
                      + annotation->getName()),
                     annotation->getMetaData());
}


/**
 * Constructor for editing a file's metadata.
 *
 * @param caretDataFile
 *    Caret Data File that will have its metadata edited.
 * @param parent
 *    Widget on which this dialog is displayed.
 */
MetaDataEditorDialog::MetaDataEditorDialog(CaretDataFile* caretDataFile,
                                           QWidget* parent)
: WuQDialogModal("",
                 parent)
{
    CaretAssert(caretDataFile);
    
    initializeDialog(("Edit File Metadata: "
                      + caretDataFile->getFileNameNoPath()),
                     caretDataFile->getFileMetaData());
}

/**
 * Constructor for editing a map's metadata.
 *
 * @param caretMappableDataFile
 *    Caret Data File that will have its map's metadata edited.
 * @param mapIndex
 *    Index of map that will have its metadata edited.
 * @param parent
 *    Widget on which this dialog is displayed.
 */
MetaDataEditorDialog::MetaDataEditorDialog(CaretMappableDataFile* caretMappableDataFile,
                                           const int32_t mapIndex,
                                           QWidget* parent)
: WuQDialogModal("",
                 parent)
{
    CaretAssert(caretMappableDataFile);
    
    const AString mapName = caretMappableDataFile->getMapName(mapIndex);
    
    initializeDialog(("Edit Map Metadata: "
                      + mapName),
                     caretMappableDataFile->getMapMetaData(mapIndex));
}

/**
 * Destructor.
 */
MetaDataEditorDialog::~MetaDataEditorDialog()
{
    
}

void
MetaDataEditorDialog::initializeDialog(const AString& dialogTitle,
                                       GiftiMetaData* metaData)
{
    CaretAssert(metaData);
    
    setWindowTitle(dialogTitle);
    
    m_metaDataEditorWidget = new MetaDataEditorWidget(this);
    
    setCentralWidget(m_metaDataEditorWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    m_metaDataEditorWidget->loadMetaData(metaData);
}

/**
 * Called when OK button clicked.
 */
void
MetaDataEditorDialog::okButtonClicked()
{
    const AString errorMessage = m_metaDataEditorWidget->saveMetaData();
    if (errorMessage.isEmpty() == false) {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    WuQDialogModal::okButtonClicked();
}

/**
 * Called when Cancel button clicked.
 */
void
MetaDataEditorDialog::cancelButtonClicked()
{
    if (m_metaDataEditorWidget->isMetaDataModified()) {
        const AString errorMessage = ("The metadata has been modified.  Discard changes?");
        if (WuQMessageBox::warningOkCancel(this, errorMessage)) {
            WuQDialogModal::cancelButtonClicked();
        }
    }
    else {
        WuQDialogModal::cancelButtonClicked();
    }
}


