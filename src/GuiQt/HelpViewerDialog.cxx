
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
#include <QFile>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextStream>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CommandOperation.h"
#include "CommandOperationManager.h"

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
        
    //
    // Create the tree widget for the indices
    //
    QStringList indexTreeHeaderLabels;
    indexTreeHeaderLabels << "Help Page Index" << "Location";
    m_topicTreeWidget = new QTreeWidget();
    m_topicTreeWidget->setHeaderHidden(true);
//    m_topicTreeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    m_topicTreeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    m_topicTreeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_topicTreeWidget->setColumnCount(2);
    m_topicTreeWidget->setHeaderLabels(indexTreeHeaderLabels);
    m_topicTreeWidget->setColumnHidden(0, false);
    m_topicTreeWidget->setColumnHidden(1, true);
    QObject::connect(m_topicTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(topicTreeItemSelected(QTreeWidgetItem*,int)));
    
    m_helpBrowserWidget = new QTextBrowser();
    
    QWidget* leftColumnWidget = new QWidget();
    QVBoxLayout* leftColumnLayout = new QVBoxLayout(leftColumnWidget);
    leftColumnLayout->addWidget(topicLabel, 0, Qt::AlignHCenter);
    leftColumnLayout->addWidget(m_topicTreeWidget, 100);
    leftColumnLayout->addStretch();
    
    m_splitter = new QSplitter();
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(leftColumnWidget);
    m_splitter->addWidget(m_helpBrowserWidget);
    QList<int> sizeList;
    sizeList << 225 << 400;
    m_splitter->setSizes(sizeList);
    setCentralWidget(m_splitter,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    loadHelpTopics();

//    /*
//     * Set the 
//     */
//    //m_topicTreeWidget->resizeToFitContent();
//    const int topicTreeWidgetWidth = m_topicTreeWidget->sizeHint().width();
//    if (topicTreeWidgetWidth > 50) {
//        const int leftWidgetWidth = leftColumnWidget->sizeHint().width();
//        if (leftWidgetWidth > 50) {
//            const int browserWidgetWidth = topicTreeWidgetWidth * 4;
//            QList<int> widgetSizes;
//            widgetSizes << (leftWidgetWidth + 20) << browserWidgetWidth;
//            m_splitter->setSizes(widgetSizes);
//        }
//    }
    
    setMinimumHeight(500);
}

/**
 * Destructor.
 */
HelpViewerDialog::~HelpViewerDialog()
{
}

/**
 * Create a help page item for HTML loaded from a file.
 *
 * @param topicName
 *    Name displayed in topics list.
 * @param filePath
 *    Path to the file.
 * @param parent
 *    Parent of item that is created.
 * @return
 *    Item that was created.
 */
QTreeWidgetItem*
HelpViewerDialog::createHelpPageFileItem(const AString& topicName,
                                     const AString& filePath,
                                     QTreeWidgetItem* parent) const
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent,
                                                TREE_ITEM_HELP_PAGE);
    item->setText(0, topicName);
    item->setData(0, Qt::UserRole, filePath);
    
    return item;
}

/**
 * Load the help topics into the list widget.
 */
void
HelpViewerDialog::loadHelpTopics()
{
    m_topicTreeWidget->blockSignals(true);
    
    
    QTreeWidgetItem* workbenchItem = new QTreeWidgetItem(m_topicTreeWidget,
                                                         TREE_ITEM_NONE);
    workbenchItem->setText(0, "Workbench");
    
    QTreeWidgetItem* menuItem = new QTreeWidgetItem(workbenchItem,
                                                    TREE_ITEM_NONE);
    menuItem->setText(0, "Menus");    
    createHelpPageFileItem("File Menu", ":/HelpFiles/File_Menu_new.htm", menuItem);
    createHelpPageFileItem("Data Menu", ":/HelpFiles/Data_Menu.htm", menuItem);
    
    createHelpPageFileItem("Preferences", ":/HelpFiles/Preferences_filt.htm", workbenchItem);
    createHelpPageFileItem("Splash Screen", ":/HelpFiles/Splash_Screen_filt.htm", workbenchItem);
    
//    QDir resourceDir(":/");
//    if (resourceDir.exists()) {
//        QDir helpFilesDir = resourceDir;
//        helpFilesDir.cd("HelpFiles");
//        if (helpFilesDir.exists()) {
//            QStringList htmlFileFilter;
//            htmlFileFilter << "*.htm" << "*.html";
//            QFileInfoList fileList = helpFilesDir.entryInfoList(htmlFileFilter,
//                                                               QDir::Files,
//                                                               QDir::Name);
//            if (fileList.size() > 0) {
//                for (int i = 0; i < fileList.size(); i++) {
//                    const QString pathName = fileList.at(i).absoluteFilePath();
//                    const QString indexName = fileList.at(i).baseName().replace('_', ' ');
//                    
//                    const QString resourcePathName = "qrc" + pathName;
//                    QTreeWidgetItem* twi = new QTreeWidgetItem(workbenchItem,
//                                                               TREE_ITEM_HELP_PAGE);
//                    twi->setText(0, indexName);
//                    twi->setData(0, Qt::UserRole, resourcePathName);
//                }
//            }
//            else {
//                CaretLogSevere("No HTML (*.html, *.htm) Help Files Found in Resource directory "
//                               + helpFilesDir.absolutePath()
//                               + " not found.");
//            }
//        }
//        else {
//            CaretLogSevere("Help Files Resource directory "
//                           + helpFilesDir.absolutePath()
//                           + " not found.");
//        }
//    }
//    else {
//        CaretLogSevere("Resource directory "
//                       + resourceDir.absolutePath()
//                       + " not found.");
//    }
    
    m_topicTreeWidget->setItemExpanded(menuItem,
                              true);
    m_topicTreeWidget->setItemExpanded(workbenchItem,
                                       true);

    /*
     * Load commands
     */
    CommandOperationManager* commandOperationManager = CommandOperationManager::getCommandOperationManager();
    std::vector<CommandOperation*> commandOperations = commandOperationManager->getCommandOperations();
    
    if ( ! commandOperations.empty()) {
        std::map<AString, CommandOperation*> cmdMap;
        const uint64_t numberOfCommands = commandOperations.size();
        for (uint64_t i = 0; i < numberOfCommands; i++) {
            CommandOperation* op = commandOperations[i];
            cmdMap[op->getOperationShortDescription()] = op;
        }
        
        QTreeWidgetItem* wbCommandItem = new QTreeWidgetItem(m_topicTreeWidget,
                                                             TREE_ITEM_NONE);
        wbCommandItem->setText(0, "wb_command");
        
        for (std::map<AString, CommandOperation*>::iterator iter = cmdMap.begin();
             iter != cmdMap.end();
             iter++) {
            QTreeWidgetItem* twi = new QTreeWidgetItem(wbCommandItem,
                                                       TREE_ITEM_WB_COMMAND);
            twi->setText(0, iter->second->getOperationShortDescription());
            QVariant commandPointer = qVariantFromValue((void*)iter->second);
            twi->setData(0, Qt::UserRole, commandPointer);
        }
    }

    m_topicTreeWidget->blockSignals(false);
}

/**
 * Show the help page with the given name (Name.
 */
void
HelpViewerDialog::showHelpPageWithName(const AString& helpPageName)
{
    const AString pageName = QString(helpPageName).replace('_', ' ');
    if (pageName.isEmpty()) {
        return;
    }
    
//    for (int i = 0; i < m_topicTreeWidget->count(); i++) {
//        QListWidgetItem* lwi = m_workbenchIndexListWidget->item(i);
//        if (lwi->text() == pageName) {
//            m_workbenchIndexListWidget->setCurrentItem(lwi);
//            workbenchIndexListWidgetItemClicked(lwi);
//            return;
//        }
//    }
    
    CaretLogSevere("Could not find help page \""
                   + helpPageName
                   + "\" for loading.");
}

/**
 * Called when a help topic is clicked in the index list widget which
 * causes display of its help information.
 *
 * @param item
 *    List widget item that was clicked.
 */
void
HelpViewerDialog::topicTreeItemSelected(QTreeWidgetItem* item,int column)
{
    if (item != NULL) {
        switch (item->type()) {
            case TREE_ITEM_NONE:
                break;
            case TREE_ITEM_HELP_PAGE:
            {
                QVariant itemData = item->data(column, Qt::UserRole);
                
                const QString resourceFileName = itemData.toString();
                
                if (resourceFileName.startsWith("qrc:")) {
                    QString urlText = itemData.toString();
                    if (! urlText.isEmpty()) {
                         QUrl url(urlText);
                         m_helpBrowserWidget->setSource(url);
                    }
                }
                else {
                    QFile file(resourceFileName);
                    if (file.exists()) {
                        QUrl url = QUrl::fromLocalFile(resourceFileName);
                        m_helpBrowserWidget->setSource(url);
                    }
                    else {
                        const QString msg("Help page \""
                                          + resourceFileName
                                          + "\": not found");
                        CaretLogSevere(msg);
                    }
                    
                    
//                    if (file.open(QFile::ReadOnly)) {
//                        QTextStream stream(&file);
//                        m_helpBrowserWidget->setHtml(stream.readAll());
//                    }
//                    else {
//                        const QString msg("Unable to open help page from \""
//                                          + resourceFileName
//                                          + "\": "
//                                          + file.errorString());
//                        CaretLogSevere(msg);
//                    }
                }
                
            }
                break;
            case TREE_ITEM_WB_COMMAND:
            {
                QVariant itemData = item->data(column, Qt::UserRole);
                void* ptr = itemData.value<void*>();
                CommandOperation* command = (CommandOperation*)ptr;
                m_helpBrowserWidget->setText(command->getHelpInformation("wb_command"));
            }
                break;
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

