
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __HELP_DIALOG_DECLARE__
#include "HelpViewerDialog.h"
#undef __HELP_DIALOG_DECLARE__

#include <QDir>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSplitter>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "HelpViewerTopicEnum.h"

using namespace caret;


    
/**
 * \class caret::HelpViewerDialog 
 * \brief Dialog for display of help information.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     Parent widget on which dialog is displayed.
 * @param f
 *     The widget flags.
 *
 */
HelpViewerDialog::HelpViewerDialog(QWidget* parent,
                       Qt::WindowFlags f)
: WuQDialogNonModal("Help",
                    parent,
                    f)
{
    m_helpDataHasBeenLoadedFlag = false;
    setApplyButtonText("");
    
    QLabel* topicLabel = new QLabel("Topic");
    
    m_indexListWidget = new QListWidget();
    m_indexListWidget->setSelectionMode(QListWidget::SingleSelection);
    QObject::connect(m_indexListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                     this, SLOT(indexListWidgetItemClicked(QListWidgetItem*)));
    
    m_helpBrowserWidget = new QTextBrowser();
    
    QWidget* leftColumnWidget = new QWidget();
    QVBoxLayout* leftColumnLayout = new QVBoxLayout(leftColumnWidget);
    leftColumnLayout->addWidget(topicLabel, 0, Qt::AlignHCenter);
    leftColumnLayout->addWidget(m_indexListWidget, 100);
    leftColumnLayout->addStretch();
    
    m_splitter = new QSplitter();
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(leftColumnWidget);
    m_splitter->addWidget(m_helpBrowserWidget);
    setCentralWidget(m_splitter,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    loadHelpTopics();

    /*
     * Set the 
     */
    const int indexListWidgetWidth = m_indexListWidget->sizeHint().width();
    if (indexListWidgetWidth > 50) {
        m_indexListWidget->setMaximumWidth(indexListWidgetWidth);
        const int leftWidgetWidth = leftColumnWidget->sizeHint().width();
        if (leftWidgetWidth > 50) {
            const int browserWidgetWidth = indexListWidgetWidth * 4;
            QList<int> widgetSizes;
            widgetSizes << (leftWidgetWidth + 20) << browserWidgetWidth;
            m_splitter->setSizes(widgetSizes);
        }
    }
    
    // READ ALL RESOURCE TO FIND HELP PAGES
    
    setMinimumHeight(500);
}

/**
 * Destructor.
 */
HelpViewerDialog::~HelpViewerDialog()
{
}

/**
 * Load the help topics into the list widget.
 */
void
HelpViewerDialog::loadHelpTopics()
{
    m_indexListWidget->blockSignals(true);
    
//    QListWidgetItem* defaultHelpItem = NULL;
//    std::vector<HelpViewerTopicEnum::Enum> helpTopics;
//    HelpViewerTopicEnum::getAllEnums(helpTopics);
//    for (std::vector<HelpViewerTopicEnum::Enum>::iterator iter = helpTopics.begin();
//         iter != helpTopics.end();
//         iter++) {
//        HelpViewerTopicEnum::Enum topic = *iter;
//        const QString text         = HelpViewerTopicEnum::toGuiName(topic);
//        const QString resourceName = HelpViewerTopicEnum::toResourceFileName(topic);
//        
//        if (( ! text.isEmpty())
//            && (! resourceName.isEmpty())) {
//            QListWidgetItem* lwi = new QListWidgetItem(text);
//            lwi->setData(Qt::UserRole, resourceName);
//            m_indexListWidget->addItem(lwi);
//            
//            if (defaultHelpItem == NULL) {
//                defaultHelpItem = lwi;
//            }
//        }
//    }
//
//    if (defaultHelpItem != NULL) {
//        m_indexListWidget->setCurrentItem(defaultHelpItem);
//        indexListWidgetItemClicked(defaultHelpItem);
//    }
//    
//    std::map<QString, QString> helpTitleAndFileMap;
    
    QDir resourceDir(":/");
    if (resourceDir.exists()) {
        QDir helpFilesDir = resourceDir;
        helpFilesDir.cd("HelpFiles");
        if (helpFilesDir.exists()) {
            QStringList htmlFileFilter;
            htmlFileFilter << "*.htm" << "*.html";
            QFileInfoList fileList = helpFilesDir.entryInfoList(htmlFileFilter,
                                                               QDir::Files,
                                                               QDir::Name);
            if (fileList.size() > 0) {
                for (int i = 0; i < fileList.size(); i++) {
                    const QString pathName = fileList.at(i).absoluteFilePath();
                    const QString indexName = fileList.at(i).baseName().replace('_', ' ');
                    //                helpTitleAndFileMap.insert(std::make_pair<QString, QString>(indexName, pathName));
                    
                    const QString resourcePathName = "qrc" + pathName;
                    //                std::cout << "File " << i << ": " << qPrintable(resourcePathName) << std::endl;
                    QListWidgetItem* lwi = new QListWidgetItem(indexName);
                    lwi->setData(Qt::UserRole, resourcePathName);
                    m_indexListWidget->addItem(lwi);
                }
            }
            else {
                CaretLogSevere("No HTML (*.html, *.htm) Help Files Found in Resource directory "
                               + helpFilesDir.absolutePath()
                               + " not found.");
            }
        }
        else {
            CaretLogSevere("Help Files Resource directory "
                           + helpFilesDir.absolutePath()
                           + " not found.");
        }
    }
    else {
        CaretLogSevere("Resource directory "
                       + resourceDir.absolutePath()
                       + " not found.");
    }

    /*
     * Select the first item in the help index
     */
    if (m_indexListWidget->count() > 0) {
        QListWidgetItem* firstItem = m_indexListWidget->item(0);
        m_indexListWidget->setCurrentItem(firstItem);
        indexListWidgetItemClicked(firstItem);
    }
    
    m_indexListWidget->blockSignals(false);
}

/**
 * Called when a help topic is clicked in the index list widget which
 * causes display of its help information.
 *
 * @param item
 *    List widget item that was clicked.
 */
void
HelpViewerDialog::indexListWidgetItemClicked(QListWidgetItem* item)
{
    if (item != NULL) {
        QVariant itemData = item->data(Qt::UserRole);
        QString urlText = itemData.toString();
        if (! urlText.isEmpty()) {
            QUrl url(urlText);
            m_helpBrowserWidget->setSource(url);
        }
    }
}

/**
 * Update the content of the dialog.
 */
void
HelpViewerDialog::updateDialog()
{
}

