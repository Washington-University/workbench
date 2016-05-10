
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

#include <QDesktopServices>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QUrl>
#include <QVBoxLayout>

#define __BALSA_DATABASE_DIALOG_DECLARE__
#include "BalsaDatabaseDialog.h"
#undef __BALSA_DATABASE_DIALOG_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "UsernamePasswordWidget.h"
#include "WuQtUtilities.h"
using namespace caret;


    
/**
 * \class caret::BalsaDatabaseDialog 
 * \brief Dialog for interaction with BALSA Database
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BalsaDatabaseDialog::BalsaDatabaseDialog(QWidget* parent)
: WuQDialogNonModal("BALSA Database",
                    parent)
{
    /*
     * No apply buton
     */
    setApplyButtonText("");
    
    QLabel* uploadLabel = new QLabel("<html>"
                                     "Upload Scene File to "
                                     "<bold><a href=\"https://balsa.wustl.edu\">BALSA Database</a></bold>"
                                     "</html>");
    QObject::connect(uploadLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));

    QWidget* loginWidget = createUsernamePasswordWidget();
    
    QWidget* controlsWidget = new QWidget();
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->addWidget(uploadLabel, 0, Qt::AlignHCenter);
    controlsLayout->addWidget(loginWidget, 0, Qt::AlignHCenter);
    //WuQtUtilities::setLayoutSpacingAndMargins(controlsLayout, 4, 2);
    
    setCentralWidget(controlsWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    /*
     * Need to use a "processed" event listener for file read and reload events
     * so that our receiveEvent() method is called after the files have been
     * read.
     */
    EventManager::get()->addProcessedEventListener(this,
                                                   EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addProcessedEventListener(this,
                                                   EventTypeEnum::EVENT_DATA_FILE_RELOAD);
}

/**
 * Destructor.
 */
BalsaDatabaseDialog::~BalsaDatabaseDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
BalsaDatabaseDialog::receiveEvent(Event* event)
{
//    SceneFile* lastSceneFileRead = NULL;
//    
//    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
//        EventUserInterfaceUpdate* uiEvent =
//        dynamic_cast<EventUserInterfaceUpdate*>(event);
//        CaretAssert(uiEvent);
//        
//        updateDialog();
//        uiEvent->setEventProcessed();
//    }
//    else if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_READ) {
//        EventDataFileRead* readEvent =
//        dynamic_cast<EventDataFileRead*>(event);
//        CaretAssert(readEvent);
//        
//        /*
//         * Determine if a scene file was read
//         */
//        const int32_t numFilesRead = readEvent->getNumberOfDataFilesToRead();
//        for (int32_t i = (numFilesRead - 1); i >= 0; i--) {
//            if ( ! readEvent->isFileError(i)) {
//                if (readEvent->getDataFileType(i) == DataFileTypeEnum::SCENE) {
//                    CaretDataFile* dataFileRead = readEvent->getDataFileRead(i);
//                    lastSceneFileRead = dynamic_cast<SceneFile*>(dataFileRead);
//                    if (lastSceneFileRead != NULL) {
//                        break;
//                    }
//                }
//            }
//        }
//    }
//    else if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_RELOAD) {
//        EventDataFileReload* reloadEvent =
//        dynamic_cast<EventDataFileReload*>(event);
//        CaretAssert(reloadEvent);
//        
//        if ( ! reloadEvent->isError()) {
//            CaretDataFile* dataFileRead = reloadEvent->getCaretDataFile();
//            lastSceneFileRead = dynamic_cast<SceneFile*>(dataFileRead);
//        }
//    }
//    
//    /*
//     * If a scene file was read, make it the selected scene file
//     * in this dialog.
//     */
//    if (lastSceneFileRead != NULL) {
//        loadSceneFileComboBox(lastSceneFileRead);
//        loadScenesIntoDialog(NULL);
//        highlightSceneAtIndex(0);
//        m_sceneSelectionScrollArea->horizontalScrollBar()->setSliderPosition(0);
//    }
}

/**
 * Update the scene dialog.
 */
void
BalsaDatabaseDialog::updateDialog()
{
//    loadSceneFileComboBox(NULL);
//    loadScenesIntoDialog(NULL);
}

/**
 * @return Widget for entering username and password.
 */
QWidget*
BalsaDatabaseDialog::createUsernamePasswordWidget()
{

    const int minimumLineEditWidth = 250;
    
    QLabel* usernameLabel = new QLabel("User Name: ");
    m_usernameLineEdit = new QLineEdit();
    m_usernameLineEdit->setMinimumWidth(minimumLineEditWidth);
    
    QLabel* passwordLabel = new QLabel("Password: ");
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    
    QLabel* forgotUsernameLabel = new QLabel("<html>"
                                             "<bold><a href=\"https://balsa.wustl.edu/register/forgotUsername\">Forgot Username</a></bold>"
                                             "</html>");
    QObject::connect(forgotUsernameLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    QLabel* forgotPasswordLabel = new QLabel("<html>"
                                             "<bold><a href=\"https://balsa.wustl.edu/register/forgotPassword\">Forgot Password</a></bold>"
                                             "</html>");
    QObject::connect(forgotPasswordLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    QLabel* registerLabel = new QLabel("<html>"
                                       "<bold><a href=\"https://balsa.wustl.edu/register/register\">Register</a></bold>"
                                       "</html>");
    QObject::connect(registerLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    QHBoxLayout* linkLabelsLayout = new QHBoxLayout();
    linkLabelsLayout->addSpacing(5);
    linkLabelsLayout->addWidget(forgotUsernameLabel);
    linkLabelsLayout->addStretch();
    linkLabelsLayout->addWidget(forgotPasswordLabel);
    linkLabelsLayout->addStretch();
    linkLabelsLayout->addWidget(registerLabel);
    linkLabelsLayout->addSpacing(5);
    
    
    QGroupBox* widget = new QGroupBox();
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    int row = 0;
    gridLayout->addWidget(usernameLabel, row, 0);
    gridLayout->addWidget(m_usernameLineEdit, row, 1);
    row++;
    gridLayout->addWidget(passwordLabel, row, 0);
    gridLayout->addWidget(m_passwordLineEdit, row, 1);
    row++;
    gridLayout->addLayout(linkLabelsLayout, row, 0, 1, 2);
    
    widget->setSizePolicy(QSizePolicy::Fixed,
                          QSizePolicy::Fixed);

    return widget;
}

/**
 * Gets called when the user clicks a link in the forgot username
 * or password label.
 */
void
BalsaDatabaseDialog::labelHtmlLinkClicked(const QString& linkURL)
{
    if (linkURL.isEmpty() == false) {
        QDesktopServices::openUrl(QUrl(linkURL));
    }
}


