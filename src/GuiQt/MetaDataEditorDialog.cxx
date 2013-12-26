
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __META_DATA_EDITOR_DIALOG_DECLARE__
#include "MetaDataEditorDialog.h"
#undef __META_DATA_EDITOR_DIALOG_DECLARE__

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

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


