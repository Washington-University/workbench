
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __SCENE_CLASS_INFO_WIDGET_DECLARE__
#include "SceneClassInfoWidget.h"
#undef __SCENE_CLASS_INFO_WIDGET_DECLARE__

#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "AString.h"
#include "CaretAssert.h"
#include "GiftiMetaData.h"
#include "ImageFile.h"
#include "Scene.h"
#include "SceneInfo.h"
#include "WuQtUtilities.h"

using namespace caret;


/**
 * \class caret::SceneClassWidget
 * \brief Dialog for manipulation of scenes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
SceneClassInfoWidget::SceneClassInfoWidget()
: QGroupBox(0)
{
    m_scene = NULL;
    m_sceneIndex = -1;
    
    m_defaultBackgroundRole = backgroundRole();
    m_defaultAutoFillBackgroundStatus = autoFillBackground();
    
    m_activeSceneLabel = new QLabel();
    m_nameLabel = new QLabel();
    m_nameLabel->setWordWrap(true);
    
    m_descriptionLabel = new QLabel();
    m_descriptionLabel->setWordWrap(true);
    
    m_sceneIdLabel = new QLabel();
    
    m_previewImageLabel = new QLabel();
    m_previewImageLabel->setContentsMargins(0, 0, 0, 0);
    
    m_rightSideWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(m_rightSideWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(3);
    rightLayout->addWidget(m_activeSceneLabel);
    rightLayout->addWidget(m_nameLabel,1);
    rightLayout->addWidget(m_sceneIdLabel);
    rightLayout->addWidget(m_descriptionLabel, 100);
    rightLayout->addStretch();
    
    m_leftSideWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(m_leftSideWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(m_previewImageLabel);
    leftLayout->addStretch();
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 3, 0, 0);
    layout->setSpacing(3);
    layout->addWidget(m_leftSideWidget);
    layout->addWidget(m_rightSideWidget, 100, Qt::AlignTop);
}

/**
 * Destructor.
 */
SceneClassInfoWidget::~SceneClassInfoWidget()
{
    
}

/**
 * Set/reset the background so that the widget appears to be
 * selected/unselected.
 *
 * @param selected
 *     Selection status.
 */
void
SceneClassInfoWidget::setBackgroundForSelected(const bool selected)
{
    if (selected) {
        setAutoFillBackground(true);
        setBackgroundRole(QPalette::Highlight);
    }
    else {
        setAutoFillBackground(m_defaultAutoFillBackgroundStatus);
        setBackgroundRole(m_defaultBackgroundRole);
    }
}

/**
 * Update the content.
 *
 * @param scene
 *     Scene for display.
 * @param sceneIndex
 *     Index of the scene.
 * @param activeSceneFlag
 *     True if this is the active scene
 */
void
SceneClassInfoWidget::updateContent(Scene* scene,
                                    const int32_t sceneIndex,
                                    const bool activeSceneFlag)
{
    m_scene = scene;
    m_sceneIndex = sceneIndex;
    
    if ((m_scene != NULL)
        && (m_sceneIndex >= 0)) {
        AString nameText;
        AString sceneIdText;
        AString descriptionText;
        AString abbreviatedDescriptionText;
        AString fullDescriptionText;
        const bool scenePreviewDialogFlag(false);
        SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(scene->getSceneInfo(),
                                                                         sceneIndex,
                                                                         nameText,
                                                                         sceneIdText,
                                                                         abbreviatedDescriptionText,
                                                                         fullDescriptionText,
                                                                         scenePreviewDialogFlag);
        
        if (activeSceneFlag) {
            m_activeSceneLabel->setText("<html><font color=\"red\">Current Scene</font></html>");
            m_activeSceneLabel->setVisible(true);
        }
        else {
            m_activeSceneLabel->setText("");
            m_activeSceneLabel->setVisible(false);
        }
        m_nameLabel->setText(nameText);
        m_sceneIdLabel->setText(sceneIdText);
        m_descriptionLabel->setText(fullDescriptionText);
        
        QByteArray imageByteArray;
        AString imageBytesFormat;
        scene->getSceneInfo()->getImageBytes(imageByteArray,
                                             imageBytesFormat);
        
        
        const int previewImageWidth = 192;
        
        QImage  previewImage;
        bool    previewImageValid = false;
        
        if (imageByteArray.length() > 0) {
            ImageFile imageFile;
            imageFile.setImageFromByteArray(imageByteArray,
                                            imageBytesFormat);
            
            previewImage = *imageFile.getAsQImage();
            if ( ! previewImage.isNull()) {
                imageFile.resizeToWidth(previewImageWidth);
                QImage newPreviewImage = *imageFile.getAsQImage();
                if ( ! newPreviewImage.isNull()) {
                    previewImage = newPreviewImage;
                    previewImageValid = true;
                }
            }
        }
        
        m_previewImageLabel->clear();
        m_previewImageLabel->setAlignment(Qt::AlignHCenter
                                          | Qt::AlignTop);
        if (previewImageValid) {
            m_previewImageLabel->setPixmap(QPixmap::fromImage(previewImage));
        }
        else {
            m_previewImageLabel->setText("<html>No preview<br>image</html>");
        }
    }
}

/**
 * Examine the given string for newlines and remove
 * any lines that exceed the maximum allowed.
 *
 * @param textLines
 *     String containing lines of text separated by a newline character.
 * @param maximumNumberOfLines
 *     Maximum number of lines for the text.
 */
void
SceneClassInfoWidget::limitToNumberOfLines(AString& textLines,
                                           const int32_t maximumNumberOfLines)
{
    if (textLines.isEmpty()) {
        return;
    }
    
    const QString lineSeparator("\n");
    
#if QT_VERSION >= 0x060000
    QStringList descriptionLines = textLines.split(lineSeparator,
                                                   Qt::KeepEmptyParts);
#else
    QStringList descriptionLines = textLines.split(lineSeparator,
                                                   QString::KeepEmptyParts);
#endif
    const int32_t numLines = descriptionLines.size();
    const int32_t numLinesToRemove = numLines - maximumNumberOfLines;
    if (numLinesToRemove > 0) {
        for (int32_t i = 0; i < numLinesToRemove; i++) {
            descriptionLines.pop_back();
        }
    }
    textLines = descriptionLines.join(lineSeparator);
}

/**
 * Get formatted text for display of scene name and description.
 *
 * @param sceneInfo
 *    Info for the scene.
 * @param nameTextOut
 *    Text for name.
 * @param sceneIdTextOut
 *    Text for scene ID.
 * @param abbreviatedDescriptionTextOut
 *    Abbreviated text for description.
 * @param fullDescriptionTextOut
 *    Full text for description.
 * @param previewDialogFlag
 *    True if info will be displayed in scene preview dialog
 */
void
SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(const SceneInfo* sceneInfo,
                                                                 const int32_t sceneIndex,
                                                                 AString& nameTextOut,
                                                                 AString& sceneIdTextOut,
                                                                 AString& abbreviatedDescriptionTextOut,
                                                                 AString& fullDescriptionTextOut,
                                                                 const bool scenePreviewDialogFlag)
{
    CaretAssert(sceneInfo);
    
    AString name = sceneInfo->getName();
    if (name.isEmpty()) {
        name = "NAME IS MISSING !!!";
    }
    
    AString indexText;
    const bool showSceneIndexFlag = false;
    if (showSceneIndexFlag) {
        if (sceneIndex >= 0) {
            indexText = ("(" + AString::number(sceneIndex + 1) + ") ");
        }
    }
    
    nameTextOut = ("<html>"
                   + indexText
                   + "<b>NAME</b> ("
                   + AString::number(sceneIndex + 1)
                   + "):  "
                   + name
                   + "</html>");
    
    sceneIdTextOut = ("<html><b>BALSA SCENE ID</b>:  "
                      + sceneInfo->getBalsaSceneID()
                      + "</html>");
    
    fullDescriptionTextOut = sceneInfo->getDescription();
    abbreviatedDescriptionTextOut = fullDescriptionTextOut;
    const int32_t maximumLinesInDescription(scenePreviewDialogFlag
                                            ? 999999  /* unlimited for preview dialog*/
                                            : 9);
    SceneClassInfoWidget::limitToNumberOfLines(abbreviatedDescriptionTextOut,
                                               maximumLinesInDescription);
    
    if ( ! fullDescriptionTextOut.isEmpty()) {
        /*
         * HTML formatting is needed so text is properly displayed.
         * Want to put "Description" at beginning in bold but any
         * HTML tags are converted to text.  So, after conversion to
         * HTML, perform a replace to insert "Description" in bold.
         */
        const AString replaceWithDescriptionBoldText = "REPLACE_WITH_DESCRIPTION";
        fullDescriptionTextOut = WuQtUtilities::createWordWrappedToolTipText(replaceWithDescriptionBoldText
                                                                             + fullDescriptionTextOut);
        fullDescriptionTextOut.replace(replaceWithDescriptionBoldText,
                                       "<b>DESCRIPTION:</b> ");
    }
    
    if ( ! abbreviatedDescriptionTextOut.isEmpty()) {
        /*
         * HTML formatting is needed so text is properly displayed.
         * Want to put "Description" at beginning in bold but any
         * HTML tags are converted to text.  So, after conversion to
         * HTML, perform a replace to insert "Description" in bold.
         */
        const AString replaceWithDescriptionBoldText = "REPLACE_WITH_DESCRIPTION";
        abbreviatedDescriptionTextOut = WuQtUtilities::createWordWrappedToolTipText(replaceWithDescriptionBoldText
                                                                                    + abbreviatedDescriptionTextOut);
        abbreviatedDescriptionTextOut.replace(replaceWithDescriptionBoldText,
                                              "<b>DESCRIPTION:</b> ");
    }
    
    if (scenePreviewDialogFlag) {
        /*
         * Show metadata in scene preview dialog
         */
        std::map<AString, AString> metaDataMap(sceneInfo->getMetaData()->getAsMap());
        if ( ! metaDataMap.empty()) {
            fullDescriptionTextOut.appendWithNewLine("<p><b>MetaData</b>:<br>");
            for (auto& m : metaDataMap) {
                fullDescriptionTextOut.appendWithNewLine(" "
                                                         + m.first
                                                         + ": "
                                                         + m.second
                                                         + "<br>");
            }
        }
    }
}


/**
 * Called by Qt when the mouse is pressed.
 *
 * @param event
 *    The mouse event information.
 */
void
SceneClassInfoWidget::mousePressEvent(QMouseEvent* event)
{
    emit highlighted(m_sceneIndex);
    event->setAccepted(true);
}

/**
 * Called when the mouse is double clicked.
 *
 * @param event
 *    The mouse event information.
 */
void
SceneClassInfoWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit activated(m_sceneIndex);
    event->setAccepted(true);
}

/**
 * @return The scene.
 */
Scene*
SceneClassInfoWidget::getScene()
{
    return m_scene;
}

/**
 * @return The index of the scene.
 */
int32_t
SceneClassInfoWidget::getSceneIndex() const
{
    return m_sceneIndex;
}


/**
 * @return True if this scene class info widget is valid (has
 * a scene with a valid index).
 */
bool
SceneClassInfoWidget::isValid() const
{
    if ((m_scene != NULL)
        && (m_sceneIndex >= 0)) {
        return true;
    }
    
    return false;
}
