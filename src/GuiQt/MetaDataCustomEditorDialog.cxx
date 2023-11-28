
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

#define __META_DATA_CUSTOM_EDITOR_DIALOG_DECLARE__
#include "MetaDataCustomEditorDialog.h"
#undef __META_DATA_CUSTOM_EDITOR_DIALOG_DECLARE__

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "Annotation.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretMappableDataFile.h"
#include "GiftiMetaData.h"
#include "MetaDataCustomEditorWidget.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::MetaDataCustomEditorDialog
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
MetaDataCustomEditorDialog::MetaDataCustomEditorDialog(Annotation* annotation,
                                           QWidget* parent)
: WuQDialogModal("",
                 parent)
{
    CaretAssert(annotation);
    
    const bool polyhedronSamplesFlag(true);
    std::vector<AString> metaDataNames;
    std::vector<AString> requiredMetaDataNames;
    Annotation::getDefaultMetaDataNamesForType(annotation->getType(),
                                               polyhedronSamplesFlag,
                                               metaDataNames,
                                               requiredMetaDataNames);
    const AString titlePrefix((annotation->getType() == AnnotationTypeEnum::POLYHEDRON)
                              ? "Edit Sample Metadata: "
                              : "Edit Annotation Metadata: ");
    initializeDialog((titlePrefix
                      + annotation->getName()),
                     metaDataNames,
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
MetaDataCustomEditorDialog::MetaDataCustomEditorDialog(CaretDataFile* caretDataFile,
                                           QWidget* parent)
: WuQDialogModal("",
                 parent)
{
    CaretAssert(caretDataFile);
    
    CaretAssertMessage(0, "Need metadata names");
    const std::vector<AString> metaDataNames;
    initializeDialog(("Edit File Metadata: "
                      + caretDataFile->getFileNameNoPath()),
                     metaDataNames,
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
MetaDataCustomEditorDialog::MetaDataCustomEditorDialog(CaretMappableDataFile* caretMappableDataFile,
                                           const int32_t mapIndex,
                                           QWidget* parent)
: WuQDialogModal("",
                 parent)
{
    CaretAssert(caretMappableDataFile);
    
    const AString mapName = caretMappableDataFile->getMapName(mapIndex);
    
    CaretAssertMessage(0, "Need metadata names");
    const std::vector<AString> metaDataNames;
    initializeDialog(("Edit Map Metadata: "
                      + mapName),
                     metaDataNames,
                     caretMappableDataFile->getMapMetaData(mapIndex));
}

/**
 * Destructor.
 */
MetaDataCustomEditorDialog::~MetaDataCustomEditorDialog()
{
    
}

/*
 * Initialize the dialog
 * @param dialogTitle
 *    Title of dialog
 * @param metaDataNames
 *    Names of metadata entries
 * @param metaData
 *    The metadata
 */
void
MetaDataCustomEditorDialog::initializeDialog(const AString& dialogTitle,
                                             const std::vector<AString>& metaDataNames,
                                             GiftiMetaData* metaData)
{
    CaretAssert(metaData);
    
    setWindowTitle(dialogTitle);
    
    std::vector<AString> dummyRequiredMetaNames;
    m_metaDataEditorWidget = new MetaDataCustomEditorWidget(metaDataNames,
                                                            dummyRequiredMetaNames,
                                                            metaData,
                                                            this);
    
    setCentralWidget(m_metaDataEditorWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
//    m_metaDataEditorWidget->loadMetaData(metaData);
}

/**
 * Called when OK button clicked.
 */
void
MetaDataCustomEditorDialog::okButtonClicked()
{
    m_metaDataEditorWidget->saveMetaData();
    WuQDialogModal::okButtonClicked();
}

/**
 * Called when Cancel button clicked.
 */
void
MetaDataCustomEditorDialog::cancelButtonClicked()
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


