
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __SCENE_PREVIEW_DIALOG_DECLARE__
#include "ScenePreviewDialog.h"
#undef __SCENE_PREVIEW_DIALOG_DECLARE__

#include <QLabel>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "ImageFile.h"
#include "Scene.h"
#include "SceneDialog.h"
#include "SceneInfo.h"

using namespace caret;


    
/**
 * \class caret::ScenePreviewDialog 
 * \brief Non-modal dialog that shows preview image of a scene.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ScenePreviewDialog::ScenePreviewDialog(const Scene* scene,
                                       QWidget* parent)
: WuQDialogNonModal(scene->getName(),
                    parent)
{
    /*
     * Dialog will be deleted when closed.
     */
    setDeleteWhenClosed(true);
    
    setApplyButtonText("");
    setCloseButtonText("Close");

    QByteArray imageByteArray;
    AString imageBytesFormat;
    scene->getSceneInfo()->getImageBytes(imageByteArray,
                                         imageBytesFormat);
    QLabel* imageLabel = new QLabel();
    bool imageValidFlag = false;
    try {
        if (imageByteArray.length() > 0) {
            ImageFile imageFile;
            imageFile.setImageFromByteArray(imageByteArray,
                                            imageBytesFormat);
            const QImage* image = imageFile.getAsQImage();
            if (image != NULL) {
                if (image->isNull()) {
                    CaretLogSevere("Preview image is invalid (isNull)");
                }
                else {
                    imageLabel->setPixmap(QPixmap::fromImage(*image));
                    imageValidFlag = true;
                }
            }
        }
    }
    catch (const DataFileException& dfe) {
        CaretLogSevere("Converting preview to image: "
                       + dfe.whatString());
    }
    
    if ( ! imageValidFlag) {
        imageLabel->setText("No image available");
    }
    
    AString nameText;
    AString sceneIdText;
    AString descriptionText;
    const int32_t negativeIsUnlimitedNumberOfLines = -1;
    SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(scene->getSceneInfo(),
                                                                     -1,
                                                                     nameText,
                                                                     sceneIdText,
                                                                     descriptionText,
                                                                     negativeIsUnlimitedNumberOfLines);
    QLabel* nameLabel = new QLabel(nameText);
    
    QLabel* sceneIdLabel = new QLabel(sceneIdText);
    
    QLabel* descriptionLabel = NULL;
    if (! descriptionText.isEmpty()) {
        descriptionLabel = new QLabel(descriptionText);
        descriptionLabel->setWordWrap(true);
    }
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(imageLabel);
    layout->addWidget(nameLabel);
    layout->addWidget(sceneIdLabel);
    if (descriptionLabel != NULL) {
        layout->addWidget(descriptionLabel);
    }
    
    setCentralWidget(widget, SCROLL_AREA_ALWAYS);
}

/**
 * Destructor.
 */
ScenePreviewDialog::~ScenePreviewDialog()
{
}

/** May be called requesting the dialog to update its content */
void
ScenePreviewDialog::updateDialog()
{
    /* nothing to update */
}
