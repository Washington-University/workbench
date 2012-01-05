
/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <iostream>

#include <QAction>
#include <QByteArray>
#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QFileSystemWatcher>
#include <QGlobalStatic>
#include <QGridLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRegExp>
#include <QSet>
#include <QSplitter>
#include <QStackedWidget>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolButton>

#define _WU_Q_FILE_DIALOG_MAIN_H_
#include "WuQFileDialog.h"
#include "WuQFileDialogIcons.h"
#undef _WU_Q_FILE_DIALOG_MAIN_H_

#include "Brain.h"
#include "CaretPreferences.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "SessionManager.h"

using namespace caret;

static const qint32 WUQFileDialogMagic = 0x57554644;  //'WUFD';
static const qint32 WUQFileDialogVersion = 1;

/**
 * constructor.
 */
WuQFileDialog::WuQFileDialog(QWidget* parent,
                                 Qt::WindowFlags f)
   : QDialog(parent, f)
{
   initializeDialog();
}

/**
 * constructor.
 */
WuQFileDialog::WuQFileDialog(QWidget* parent,
                                 const QString& caption,
                                 const QString& directoryName,
                                 const QString& filter)
   : QDialog(parent, 0)
{
   initializeDialog();
   
   if (caption.isEmpty() == false) {
      setWindowTitle(caption);
   }
   if (directoryName.isEmpty() == false) {
      setDirectory(directoryName);
   }
   if (filter.isEmpty() == false) {
      setNameFilter(filter);
   }
}

/**
 * initialize the dialog.
 */
void 
WuQFileDialog::initializeDialog()
{      
   debugFlag = false;
   
   confirmOverwriteFlag = true;
   theFileMode = AnyFile;
   theDefaultSuffix = "";
   firstSideBarUrlIndex = 500000;
   readOnlyFlag = false;
   
   userSetLookInLabel = false;
   userSetFileNameLabel = false;
   userSetFileTypeLabel = false;
   userSetAcceptLabel = false;
   userSetRejectLabel = false;
   
   fileSelectionTreeNameWidthAdjustedFlag = false;
   
   //
   // Create the file system watcher
   //
   fileSystemWatcher = new QFileSystemWatcher(this);
   QObject::connect(fileSystemWatcher, SIGNAL(directoryChanged(const QString&)),
                    this, SLOT(rereadDir()));
                    
   //
   // Initialize the directory
   //
   theDirectory.setPath(QDir::currentPath());
   
   //
   // file name line edit and file type combo box
   //
   fileNameLineEdit = new QLineEdit;
   QObject::connect(fileNameLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotFileNameLineEditChanged(const QString&)));
   fileTypeComboBox = new QComboBox;
   QObject::connect(fileTypeComboBox, SIGNAL(activated(const QString&)),
                    this, SLOT(slotFileTypeComboBox(const QString&)));
   QObject::connect(fileTypeComboBox, SIGNAL(activated(const QString&)),
                    this, SIGNAL(filterSelected(const QString&)));
   
   //
   // Labels
   //
   fileNameLabel = new QLabel("");
   fileTypeLabel = new QLabel("");
   
   //
   // push buttons
   //
   acceptPushButton = new QPushButton("");
   acceptPushButton->setAutoDefault(true);
   QObject::connect(acceptPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAcceptPushButton()));
   rejectPushButton   = new QPushButton("");
   QObject::connect(rejectPushButton, SIGNAL(clicked()),
                    this, SLOT(slotRejectPushButton()));
   
   //
   // Splitter for common directory and file lists
   //
   dirFileSplitter = new QSplitter;
   dirFileSplitter->addWidget(createCommonDirectorySection());
   dirFileSplitter->addWidget(createFileSelectionSection());
   dirFileSplitter->setStretchFactor(0, 1);
   dirFileSplitter->setStretchFactor(1, 1000);
   
   //
   // Layout file name, filter, buttons, additional widgets
   //
   nameFilterButtonsGridLayout = new QGridLayout;
   nameFilterButtonsGridLayout->addWidget(fileNameLabel, 0, 0, Qt::AlignLeft);
   nameFilterButtonsGridLayout->addWidget(fileNameLineEdit, 0, 1);
   nameFilterButtonsGridLayout->addWidget(acceptPushButton, 0, 2);
   nameFilterButtonsGridLayout->addWidget(fileTypeLabel, 1, 0, Qt::AlignLeft);
   nameFilterButtonsGridLayout->addWidget(fileTypeComboBox, 1, 1);
   nameFilterButtonsGridLayout->addWidget(rejectPushButton, 1, 2);
   
   //
   // Layout the dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addLayout(createNavigationSection());
   dialogLayout->addWidget(dirFileSplitter);
   dialogLayout->addLayout(nameFilterButtonsGridLayout);
   
   setAcceptMode(AcceptOpen);

   //
   // Initialize the dialog
   //
   loadCommonDirectorySection();
   QStringList filterList;
   filterList << "*";
   setNameFilters(filterList);
   setDirectory(theDirectory.absolutePath(), false);
   slotFileNameLineEditChanged("");
   
   updateTitleLabelsButtons();
   
   
   //
   // Initialize file extension to type map
   //
   static bool firstTime = true;
   if (firstTime) {
      initializeFileExtensionToTypeNameMap();
      firstTime = false;
   }
   
    /*
     * Previous directories and set to current directory
     */
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->addToPreviousOpenFileDirectories(GuiManager::get()->getBrain()->getCurrentDirectory());
    QStringList previousDirectoriesList;
    prefs->getPreviousOpenFileDirectories(previousDirectoriesList);
    if (previousDirectoriesList.empty() == false) {
        this->setHistory(previousDirectoriesList);
        this->setDirectory(previousDirectoriesList.at(0));
    }
    else {
        this->setDirectory(GuiManager::get()->getBrain()->getCurrentDirectory());
    }
}

/**
 * destructor.
 */
WuQFileDialog::~WuQFileDialog()
{
} 

/**
 * modal method to get open file name.
 */
QString 
WuQFileDialog::getOpenFileName(QWidget *parent,
                             const QString &caption,
                             const QString &dir,
                             const QString &filter,
                             QString *selectedFilter,
                             Options /*options*/)
{
   QString name;
   
   WuQFileDialog cfd(parent);
   cfd.setWindowTitle(caption);
   cfd.setDirectory(dir);
   cfd.setNameFilters(filter.split(";;", QString::SkipEmptyParts));
   cfd.setFileMode(ExistingFile);
   cfd.setAcceptMode(AcceptOpen);
   cfd.rereadDir();
   if (cfd.exec() == Accepted) {
      if (cfd.selectedFiles().count() > 0) {
         name = cfd.selectedFiles().at(0);
         if (selectedFilter != NULL) {
            (*selectedFilter) = cfd.selectedFilter();
         }
      }
   }
   
   return name;
}

/**
 * modal method to get save file name.
 */
QString 
WuQFileDialog::getSaveFileName(QWidget *parent,
                             const QString &caption,
                             const QString &dir,
                             const QString &filter,
                             QString *selectedFilter,
                             Options options)
{
   QString name;
   
   WuQFileDialog cfd(parent);
   cfd.setWindowTitle(caption);
   cfd.setDirectory(dir);
   cfd.setNameFilters(filter.split(";;", QString::SkipEmptyParts));
   cfd.setFileMode(AnyFile);
   cfd.setAcceptMode(AcceptSave);
   if (options & DontConfirmOverwrite) {
      cfd.setConfirmOverwrite(false);
   }
   cfd.rereadDir();
   if (cfd.exec() == Accepted) {
      if (cfd.selectedFiles().count() > 0) {
         name = cfd.selectedFiles().at(0);
         if (selectedFilter != NULL) {
            (*selectedFilter) = cfd.selectedFilter();
         }
      }
   }
   
   return name;
}

/**
 * modal method to get directory name.
 */
QString 
WuQFileDialog::getExistingDirectory(QWidget *parent,
                                  const QString &caption,
                                  const QString &dir,
                                  Options options)
{
   QString name;
   
   WuQFileDialog cfd(parent);
   cfd.setWindowTitle(caption);
   cfd.setDirectory(dir);
   if (options & ShowDirsOnly) {
      cfd.setFileMode(DirectoryOnly);
   }
   else {
      cfd.setFileMode(Directory);
   }
   cfd.rereadDir();
   if (cfd.exec() == Accepted) {
      name = cfd.directory().absolutePath();
   }
   
   return name;
}

/**
 * modal method to get open file names.
 */
QStringList 
WuQFileDialog::getOpenFileNames(QWidget *parent,
                                  const QString &caption,
                                  const QString &dir,
                                  const QString &filter,
                                  QString *selectedFilter,
                                  Options /*options*/)
{
   QStringList names;
   
   WuQFileDialog cfd(parent);
   cfd.setWindowTitle(caption);
   cfd.setDirectory(dir);
   cfd.setNameFilters(filter.split(";;", QString::SkipEmptyParts));
   cfd.setFileMode(ExistingFile);
   cfd.setAcceptMode(AcceptOpen);
   cfd.rereadDir();
   if (cfd.exec() == Accepted) {
      if (cfd.selectedFiles().count() > 0) {
         names = cfd.selectedFiles();
         if (selectedFilter != NULL) {
            (*selectedFilter) = cfd.selectedFilter();
         }
      }
   }
   
   return names;
}

/**
 * called when accept button is pressed.
 */
void 
WuQFileDialog::slotAcceptPushButton()
{
   if (debugFlag) {
      std::cout << "Selected Directory: {"
                << directory().absolutePath().toAscii().constData()
                << "}"
                << std::endl;
   }
       
   bool fileFlag = false;
   
   switch (fileMode()) {
      case AnyFile:
         fileFlag = true;
         break;
      case ExistingFile: 
         fileFlag = true;
         break;
      case Directory:
         break;
      case ExistingFiles: 
         fileFlag = true;
         break;
      case DirectoryOnly:
         break;
   }

   if (fileFlag) {   
      if (selectedFiles().count() <= 0) {
         QMessageBox::critical(this,
                               tr("Error"),
                               tr("No file selected"),
                               QMessageBox::Ok);
         return;
      }

      switch (acceptMode()) {
         case AcceptOpen:
            break;
         case AcceptSave:
            if (confirmOverwrite()) {
               QFileInfo fi(selectedFiles().at(0));
               if (fi.exists()) {
                  QString fileName = fi.fileName();
                  const QString message(fileName 
                                        + tr(" already exists.\n")
                                        + tr("Do you want to replace it?"));
                  const QMessageBox::StandardButton buttonPressed = 
                     QMessageBox::warning(this,
                                          tr("Overwrite File"),
                                          message,
                                          QMessageBox::Yes |
                                             QMessageBox::No);
                  if (buttonPressed == QMessageBox::No) {
                     return;
                  }
               }
            }
            break;
      }
   }
  
   accept();
    
    /*
     * For any file with absolute path, add the path to the
     * previous directories
     */
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    QStringListIterator nameIter(this->selectedFiles());
    while (nameIter.hasNext()) {
        AString name = nameIter.next();
        
        FileInformation fileInfo(name);
        if (fileInfo.isAbsolute()) {
            prefs->addToPreviousOpenFileDirectories(fileInfo.getPathName());
        }
    }
/*
   if (isModal()) {
      accept();
   }
   else {
      close();
   } 
*/
}

/**
 * called when reject button is pressed.
 */
void 
WuQFileDialog::slotRejectPushButton()
{
   reject();
/*
   if (isModal()) {
      reject();
   }
   else {
      close();
   }
*/
}
      
/**
 * called when file name line edit is changed.
 */
void 
WuQFileDialog::slotFileNameLineEditChanged(const QString& text)
{
   bool dirFlag  = false;
   bool fileFlag = false;
   bool renameFlag = false;
   switch (fileMode()) {
      case AnyFile:
      case ExistingFile:
      case ExistingFiles:
         if (text.isEmpty() == false) {
            fileFlag = true;
         } 
         renameFlag = (selectedFiles().count() == 1);
         break;
      case Directory:
      case DirectoryOnly: 
         dirFlag = true;
         break;
   }
   
   acceptPushButton->setEnabled(dirFlag || fileFlag);
   navigationDeleteFileAction->setEnabled(fileFlag 
                                          && (readOnlyFlag == false));
   navigationRenameFileAction->setEnabled(fileFlag 
                                          && renameFlag
                                          && (readOnlyFlag == false));
}
      
/**
 * get the directory.
 */
QDir 
WuQFileDialog::directory() const 
{ 
   QDir dirOut = theDirectory;
   
   switch (fileMode()) {
      case AnyFile:
      case ExistingFile:
      case ExistingFiles: 
         break;
      case Directory:
      case DirectoryOnly:
         { 
            if (fileListAndDetailStackedWidget->currentWidget() == fileSelectionListWidget) {
               const QList<QListWidgetItem*> selItems = fileSelectionListWidget->selectedItems();
               if (selItems.count() > 0) {
                  const QListWidgetItem* item = selItems.at(0);
                  const QString name(item->data(Qt::UserRole).toString());
                  QFileInfo fi(theDirectory, name);
                  if (fi.isDir()) {
                     dirOut = QDir(fi.absoluteFilePath());
                  }
               }
            }
            else if (fileListAndDetailStackedWidget->currentWidget() == fileSelectionTreeWidget) {
               const QList<QTreeWidgetItem*> selItems = fileSelectionTreeWidget->selectedItems();
               if (selItems.count() > 0) {
                  const QTreeWidgetItem* item = selItems.at(0);
                  const QString name(item->data(0, Qt::UserRole).toString());
                  QFileInfo fi(theDirectory, name);
                  if (fi.isDir()) {
                     dirOut = QDir(fi.absoluteFilePath());
                  }
               }
            }
         }
   }
   return dirOut; 
}
      
/**
 * get the selected files.
 */
QStringList 
WuQFileDialog::selectedFiles() const
{
   QStringList fileList;
   
   const QString s = fileNameLineEdit->text().trimmed();
   if (s.isEmpty() == false) {
      int firstQuoteIndex = s.indexOf('"');
      if (firstQuoteIndex >= 0) {
         const QStringList sl = s.split('"');
         for (int i = 0; i < sl.count(); i++) {
            const QString s = sl.at(i).trimmed();
            if (s.isEmpty() == false) {
               if (s != "\"") {
                  fileList << s;
               }
            }
         }
      }
      else {
         fileList << s;
      }
   }
   
   //
   // Insert directory into each file name
   //
   for (int i = 0; i < fileList.count(); i++) {
      QFileInfo fi(theDirectory, fileList.at(i));
      fileList[i] = fi.absoluteFilePath();
   }
   
   //
   // May need to add default suffix if saving
   //
   if (theDefaultSuffix.isEmpty() == false) {
      switch (acceptMode()) {
         case AcceptOpen:
            break;
         case AcceptSave:
            for (int i = 0; i < fileList.count(); i++) {
               if (fileList[i].endsWith(theDefaultSuffix) == false) {
                  fileList[i] += theDefaultSuffix;
               }
            }
            break;
      }
   }
   
   return fileList;
}

/**
 * update selected file line edit.
 */
void 
WuQFileDialog::updateSelectedFileLineEdit(const QString& s)
{
   fileNameLineEdit->setText(s);
   emit filesSelected(selectedFiles());
   if (selectedFiles().count() > 0) {
      emit currentChanged(selectedFiles().at(0));
   }
   else {
      emit currentChanged("");
   }
}
      
/**
 * update the title, labels, and buttons.
 */
void 
WuQFileDialog::updateTitleLabelsButtons()
{
   //
   // Set any buttons and labels that have not been set by the user
   //
   if (userSetLookInLabel == false) {
      lookInLabel->setText(tr("Look In"));
   }
   if (userSetFileNameLabel == false) {
      fileNameLabel->setText(tr("File Name"));
   }
   if (userSetFileTypeLabel == false) {
      fileTypeLabel->setText(tr("File Type"));
   }
   if (userSetRejectLabel == false) {
      rejectPushButton->setText(tr("Cancel"));
   }
   if (userSetAcceptLabel == false) {
      switch (fileMode()) {
         case AnyFile:
         case ExistingFile:
         case ExistingFiles: 
            switch (acceptMode()) {
               case AcceptOpen:
                  acceptPushButton->setText(tr("Open"));
                  break;
               case AcceptSave:
                  acceptPushButton->setText(tr("Save"));
                  break;
            }   
            break;
         case Directory:
         case DirectoryOnly: 
            acceptPushButton->setText(tr("Choose"));
            break;
      }
   }

   const QString openModeTitle(tr("Open"));
   const QString saveModeTitle(tr("Save"));
   const QString directoryModeTitle(tr("Find Directory"));
   if (windowTitle().isEmpty() ||
       (windowTitle() == openModeTitle) ||
       (windowTitle() == saveModeTitle) ||
       (windowTitle() == directoryModeTitle)) {
      switch (fileMode()) {
         case AnyFile:
         case ExistingFile:
         case ExistingFiles: 
            switch (theAcceptMode) {
               case AcceptOpen:
                  setWindowTitle(openModeTitle);
                  break;
               case AcceptSave:
                  setWindowTitle(saveModeTitle);
                  break;
            }   
            break;
         case Directory:
         case DirectoryOnly: 
            setWindowTitle(directoryModeTitle);
            break;
      }
   }
}
      
/**
 * set the accept mode.
 */
void 
WuQFileDialog::setAcceptMode(const AcceptMode mode)
{
   theAcceptMode = mode;
   updateTitleLabelsButtons();
}

/**
 * get label text.
 */
QString 
WuQFileDialog::labelText(const DialogLabel label) const
{
   QString text;
   switch (label) {
      case LookIn:
         text = lookInLabel->text();
         break;
      case FileName:
         text = fileNameLabel->text();
         break;
      case FileType:
         text = fileTypeLabel->text();
         break;
      case Accept:
         text = acceptPushButton->text();
         break;
      case Reject:
         text = rejectPushButton->text();
         break;
    }
    return text;
}

/**
 * set the label text.
 */
void 
WuQFileDialog::setLabelText(const DialogLabel label, const QString& text)
{
   switch (label) {
      case LookIn:
         lookInLabel->setText(text);
         userSetLookInLabel = true;
         break;
      case FileName:
         fileNameLabel->setText(text);
         userSetFileNameLabel = true;
         break;
      case FileType:
         fileTypeLabel->setText(text);
         userSetFileTypeLabel = true;
         break;
      case Accept:
         acceptPushButton->setText(text);
         userSetAcceptLabel = true;
         break;
      case Reject:
         rejectPushButton->setText(text);
         userSetRejectLabel = true;
         break;
    }
    updateTitleLabelsButtons();
}
      
/**
 * create the navigation section of the dialog.
 */
QLayout* 
WuQFileDialog::createNavigationSection()
{
   //
   // combo box for navigation history
   //
   navigationHistoryComboBox = new QComboBox;
   navigationHistoryComboBox->setInsertPolicy(QComboBox::InsertAtTop);
   QObject::connect(navigationHistoryComboBox, SIGNAL(activated(const QString&)),
                    this, SLOT(slotNavigationHistoryComboBox(const QString&)));
                    
   //
   // back tool button
   //
   navigationBackAction = new QAction(this);
   navigationBackAction->setToolTip(tr("Go Back To\n"
                                        "Previous\n"
                                        "Directory"));
   navigationBackAction->setIcon(QPixmap(back_xpm));
   QObject::connect(navigationBackAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationBackAction()));
   QToolButton* navigationBackToolButton = new QToolButton;
   navigationBackToolButton->setDefaultAction(navigationBackAction);

   //
   // forward tool button
   //
   navigationForwardAction = new QAction(this);
   navigationForwardAction->setToolTip(tr("Go Forward\n"
                                           "To Directory"));
   navigationForwardAction->setIcon(QPixmap(forward_xpm));
   QObject::connect(navigationForwardAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationForwardAction()));
   QToolButton* navigationForwardToolButton = new QToolButton;
   navigationForwardToolButton->setDefaultAction(navigationForwardAction);

   //
   // up tool button
   //
   navigationUpAction = new QAction(this);
   navigationUpAction->setToolTip(tr("Go Up To Parent\n"
                                     "Directory"));
   navigationUpAction->setIcon(QPixmap(up_xpm));
   QObject::connect(navigationUpAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationUpAction()));
   QToolButton* navigationUpToolButton = new QToolButton;
   navigationUpToolButton->setDefaultAction(navigationUpAction);

   //
   // go to directory tool button
   //
   navigationGoToDirectoryAction = new QAction(this);
   navigationGoToDirectoryAction->setText("Dir");
   navigationGoToDirectoryAction->setToolTip(tr("Enter")
                                             + "\n"
                                             + tr("Directory")
                                             + "\n"
                                             + tr("Name"));
   //navigationGoToDirectoryAction->setIcon(QPixmap(goto_xpm));
   QObject::connect(navigationGoToDirectoryAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationGoToDirectoryAction()));
   QToolButton* navigationGoToDirectoryToolButton = new QToolButton;
   navigationGoToDirectoryToolButton->setDefaultAction(navigationGoToDirectoryAction);
   
   //
   // refresh tool button
   //
   navigationRefreshAction = new QAction(this);
   navigationRefreshAction->setToolTip(tr("Refresh the\n"
                                          "File Listing"));
   navigationRefreshAction->setIcon(QPixmap(refresh_xpm));
   QObject::connect(navigationRefreshAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationRefreshAction()));
   QToolButton* navigationRefreshToolButton = new QToolButton;
   navigationRefreshToolButton->setDefaultAction(navigationRefreshAction);

   //
   // new directory tool button
   //
   navigationNewDirectoryAction = new QAction(this);
   navigationNewDirectoryAction->setToolTip(tr("Create New\n"
                                                "Directory"));
   navigationNewDirectoryAction->setIcon(QPixmap(newfolder_xpm));
   QObject::connect(navigationNewDirectoryAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationNewDirectoryAction()));
   QToolButton* navigationNewDirectoryToolButton = new QToolButton;
   navigationNewDirectoryToolButton->setDefaultAction(navigationNewDirectoryAction);

   //
   // Delete file action and tool button
   //
   navigationDeleteFileAction = new QAction(this);
   navigationDeleteFileAction->setIcon(QPixmap(trash_xpm));
   navigationDeleteFileAction->setToolTip(tr("Delete\n"
                                             "Selected\n"
                                             "Files"));
   QObject::connect(navigationDeleteFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationDeleteFileAction()));
   QToolButton* navigationDeleteFileToolButton = new QToolButton;
   navigationDeleteFileToolButton->setDefaultAction(navigationDeleteFileAction);
   
   navigationRenameFileAction = new QAction(this);
   navigationRenameFileAction->setText(tr("A2B"));
   //navigationRenameFileAction->setIcon(QPixmap(rename_xpm));
   navigationRenameFileAction->setToolTip(tr("Rename\n"
                                             "Selected\n"
                                             "File"));
   QObject::connect(navigationRenameFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationRenameFileAction()));
   QToolButton* navigationRenameFileToolButton = new QToolButton;
   navigationRenameFileToolButton->setDefaultAction(navigationRenameFileAction);
   
   //
   // Case sensitive action and tool button
   //
   navigationCaseSensitiveFileAction = new QAction(this);
   navigationCaseSensitiveFileAction->setText(tr("Aa"));
   navigationCaseSensitiveFileAction->setToolTip(tr("Case Sensitive") +
                                                 "\n" +
                                                 tr("(List View Only)"));
   navigationCaseSensitiveFileAction->setCheckable(true);
   navigationCaseSensitiveFileAction->setChecked(true);
   QObject::connect(navigationCaseSensitiveFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationCaseSensitiveAction()));
   QToolButton* navigationCaseSensitiveToolButton = new QToolButton;
   navigationCaseSensitiveToolButton->setDefaultAction(navigationCaseSensitiveFileAction);
   
   //
   // Show hidden files action and tool button
   //
   navigationShowHiddensFilesAction = new QAction(this);
   navigationShowHiddensFilesAction->setText(tr("."));
   navigationShowHiddensFilesAction->setToolTip(tr("Show Hidden Files"));
   navigationShowHiddensFilesAction->setCheckable(true);
   navigationShowHiddensFilesAction->setChecked(false);
   QObject::connect(navigationShowHiddensFilesAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNavigationShowHiddenFilesAction()));
   QToolButton* navigationShowHiddenFilesToolButton = new QToolButton;
   navigationShowHiddenFilesToolButton->setDefaultAction(navigationShowHiddensFilesAction);
   
   //
   // list view tool button
   //
   navigationListViewAction = new QAction(this);
   navigationListViewAction->setCheckable(true);
   navigationListViewAction->setToolTip(tr("List View"));
   navigationListViewAction->setIcon(QPixmap(mclistview_xpm));
   QToolButton* navigationListViewToolButton = new QToolButton;
   navigationListViewToolButton->setDefaultAction(navigationListViewAction);

   //
   // detail view tool button
   //
   navigationDetailViewAction = new QAction(this);
   navigationDetailViewAction->setCheckable(true);
   navigationDetailViewAction->setToolTip(tr("Detail View"));
   navigationDetailViewAction->setIcon(QPixmap(detailedview_xpm));
   QToolButton* navigationDetailViewToolButton = new QToolButton;
   navigationDetailViewToolButton->setDefaultAction(navigationDetailViewAction);

   //
   // Action group for list/detail views
   //
   QActionGroup* navigationViewActionGroup = new QActionGroup(this);
   navigationViewActionGroup->addAction(navigationListViewAction);
   navigationViewActionGroup->addAction(navigationDetailViewAction);
   QObject::connect(navigationViewActionGroup, SIGNAL(triggered(QAction*)),
                    this, SLOT(slotNavigationViewActionTriggered(QAction*)));

   //
   // Look in Label
   //
   lookInLabel   = new QLabel("");

   //
   // History row
   //
   QHBoxLayout* navigationHistoryLayout = new QHBoxLayout;
   navigationHistoryLayout->addWidget(lookInLabel, 0);
   navigationHistoryLayout->addWidget(navigationHistoryComboBox, 1000);
   
   //
   // Toolbutton row
   //
   navigationToolButtonLayout = new QHBoxLayout;
   navigationToolButtonLayout->addWidget(navigationBackToolButton, 0);
   navigationToolButtonLayout->addWidget(navigationForwardToolButton, 0);
   navigationToolButtonLayout->addWidget(navigationUpToolButton, 0);
   navigationToolButtonLayout->addWidget(navigationGoToDirectoryToolButton, 0);
   navigationToolButtonLayout->addWidget(navigationRefreshToolButton, 0);
   navigationToolButtonLayout->addWidget(new QLabel(" "));
   navigationToolButtonLayout->addWidget(navigationCaseSensitiveToolButton, 0);
   navigationToolButtonLayout->addWidget(navigationShowHiddenFilesToolButton, 0);
   navigationToolButtonLayout->addWidget(new QLabel(" "));
   navigationToolButtonLayout->addWidget(navigationNewDirectoryToolButton, 0);
   navigationToolButtonLayout->addWidget(navigationRenameFileToolButton, 0);
   navigationToolButtonLayout->addWidget(navigationDeleteFileToolButton, 0);
   navigationToolButtonLayout->addWidget(new QLabel(" "));
   navigationToolButtonLayout->addWidget(navigationListViewToolButton, 0);
   navigationToolButtonLayout->addWidget(navigationDetailViewToolButton, 0);
   
   //
   // Layout for the two rows
   //
   QVBoxLayout* navigationLayout = new QVBoxLayout;
   navigationLayout->addLayout(navigationToolButtonLayout);
   navigationLayout->addLayout(navigationHistoryLayout);
   navigationLayout->setAlignment(navigationToolButtonLayout, Qt::AlignLeft);
   
   return navigationLayout;
}

/**
 * create the common directory section of the dialog.
 */
QWidget*
WuQFileDialog::createCommonDirectorySection()
{
   //
   // List widget for directories
   //
   commonDirectoryListWidget = new QListWidget;
   commonDirectoryListWidget->setSelectionMode(QListWidget::NoSelection);
   commonDirectoryListWidget->setMinimumWidth(120);
   QObject::connect(commonDirectoryListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                    this, SLOT(slotCommonDirectoryListWidget(QListWidgetItem*)));
                    
   return commonDirectoryListWidget;
}

/**
 * called when a common directory item selected.
 */
void 
WuQFileDialog::slotCommonDirectoryListWidget(QListWidgetItem* item)
{
   const QString directoryName = item->data(Qt::UserRole).toString();
   if (debugFlag) {
      std::cout << "Directory: " << directoryName.toAscii().constData() << std::endl;
   }
   setDirectory(directoryName, false);
}
      
/**
 * set the sidebar URLS.
 */
void 
WuQFileDialog::setSidebarUrls(const QList<QUrl>& urls)
{
   loadCommonDirectorySection();
   
   for (int i = 0; i < urls.count(); i++) {
      const QString dirName(urls.at(i).toLocalFile());
      addToCommonDirectory(dirName, dirName);
   }
}

/**
 * get sidebar Urls.
 */
QList<QUrl> 
WuQFileDialog::sidebarUrls() const
{
   QList<QUrl> urls;
   
   for (int i = firstSideBarUrlIndex; i < commonDirectoryListWidget->count(); i++) {
      urls << QUrl::fromLocalFile(commonDirectoryListWidget->item(i)->text());
   }
   
   return urls;
}
      
/**
 * load the common directory section.
 */
void 
WuQFileDialog::loadCommonDirectorySection()
{
   //
   // clear the current items
   //
   commonDirectoryListWidget->clear();
   
   //
   // Add the Computer
   //
   addToCommonDirectory(QDir::rootPath(), tr("Computer"));
   
   //
   // Add the user's home directory
   //
   const QString homeDir(QDir::homePath());
   addToCommonDirectory(homeDir, "Home");
   
#ifdef Q_OS_MACX   
   //
   // Add Desktop
   //
   const QString desktopPath(homeDir 
                             + QDir::separator()
                             + "Desktop");
   addToCommonDirectory(desktopPath, tr("Desktop"));

   //
   // Add Documents
   //
   const QString documentsPath(homeDir 
                             + QDir::separator()
                             + "Documents");
   addToCommonDirectory(documentsPath, tr("Documents"));

   //
   // Add Downloads
   //
   const QString downloadsPath(homeDir 
                             + QDir::separator()
                             + "Downloads");
   addToCommonDirectory(downloadsPath, tr("Downloads"));

   //
   // Add Volumes
   //
   const QString volumesPath("/Volumes");
   addToCommonDirectory(volumesPath, tr("Volumes"));
#endif // Q_OS_MACX

#ifdef Q_OS_WIN32
   //
   // Add Desktop
   //
   const QString desktopPath(homeDir 
                             + QDir::separator()
                             + "Desktop");
   addToCommonDirectory(desktopPath, tr("Desktop"));

   //
   // Add Document
   //
   const QString documentsPath(homeDir 
                             + QDir::separator()
                             + "My Documents");
   addToCommonDirectory(documentsPath, tr("My Documents"));
#endif // Q_OS_WIN32

   //
   // Add drives
   //
   QFileInfoList drivesList = QDir::drives();
   for (int i = 0; i < drivesList.count(); i++) {
      const QFileInfo fi = drivesList.at(i);
      const QString driveName = fi.absoluteFilePath();
#ifdef Q_OS_MACX   
      if (driveName == "/") {
         continue;
      }
#endif // Q_OS_MACX
      addToCommonDirectory(driveName, driveName);
   }
   
   //
   // Anything else will be sidebar URLS
   //
   firstSideBarUrlIndex = commonDirectoryListWidget->count();
}
      
/**
 * add to common directory.
 */
void 
WuQFileDialog::addToCommonDirectory(const QString& directoryPath,
                                      const QString& labelName)
{
   if (QFile::exists(directoryPath)) {
      QListWidgetItem* item = new QListWidgetItem(QPixmap(folder_xpm), 
                                                           labelName);
      item->setData(Qt::UserRole, directoryPath);
      commonDirectoryListWidget->addItem(item);
   }
}
                                
/**
 * create the file selection section of the dialog.
 */
QWidget* 
WuQFileDialog::createFileSelectionSection()
{
   const int minHeight = 150;
   
   //
   // list widget for list view file selection
   //
   fileSelectionListWidget = new QListWidget;
   fileSelectionListWidget->setMinimumHeight(minHeight);
   fileSelectionListWidget->setFlow(QListWidget::TopToBottom);
   fileSelectionListWidget->setWrapping(true);
   fileSelectionListWidget->setSortingEnabled(false);
   QObject::connect(fileSelectionListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                    this, SLOT(slotFileSelectionListWidgetItemClicked(QListWidgetItem*)));
   QObject::connect(fileSelectionListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                    this, SLOT(slotFileSelectionListWidgetItemDoubleClicked(QListWidgetItem*)));
                    
   //
   // tree widget detail view file selection
   //
   fileSelectionTreeWidget = new QTreeWidget;
   fileSelectionTreeWidget->setMinimumHeight(minHeight);
   fileSelectionTreeWidget->setSortingEnabled(true);
   fileSelectionTreeWidget->sortItems(0, Qt::AscendingOrder);
   QObject::connect(fileSelectionTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                    this, SLOT(slotFileSelectionTreeWidgetItemClicked(QTreeWidgetItem*,int)));
   QObject::connect(fileSelectionTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                    this, SLOT(slotFileSelectionTreeWidgetItemDoubleClicked(QTreeWidgetItem*,int)));
                    
   //
   // stacked widget for list and detail views
   //
   fileListAndDetailStackedWidget = new QStackedWidget;
   fileListAndDetailStackedWidget->addWidget(fileSelectionListWidget);
   fileListAndDetailStackedWidget->addWidget(fileSelectionTreeWidget);
   return fileListAndDetailStackedWidget;
}

/**
 * called when a file selection tree widget item clicked.
 */
void 
WuQFileDialog::slotFileSelectionTreeWidgetItemClicked(QTreeWidgetItem* /*item*/, int)
{
   //
   // Get all selected files and put in set so names unique
   //
   QSet<QString> fileNamesSorted;   
   const QList<QTreeWidgetItem*> selItems = fileSelectionTreeWidget->selectedItems();
   for (int i = 0; i < selItems.count(); i++) {
      const QTreeWidgetItem* item = selItems.at(i);
      const QString name(item->data(0, Qt::UserRole).toString());
      if (debugFlag) {
         std::cout << "Item Clicked: " << name.toAscii().constData() << std::endl;
      }
      
      QFileInfo fi(name);
      if (fi.isFile()) {
         fileNamesSorted << fi.fileName();
      }
   }
   
   //
   // Load files into file name line edit
   // If more than one selected file, put them in double quotes
   //
   QStringList fileNameList = QStringList::fromSet(fileNamesSorted);
   QString fileNames;
   const int numFiles = fileNameList.count();
   if (numFiles == 1) {
      fileNames = fileNameList.at(0);
   }
   else {
      for (int i = 0; i < numFiles; i++) {   
         const QString quotedName = "\"" + fileNameList.at(i) + "\"";
         if (fileNames.isEmpty() == false) {
            fileNames += " ";
         }
         fileNames.append(quotedName);
      }
   }
   updateSelectedFileLineEdit(fileNames);
}

/**
 * called when a file selection tree widget item double clicked.
 */
void 
WuQFileDialog::slotFileSelectionTreeWidgetItemDoubleClicked(QTreeWidgetItem* item, 
                                                              int column)
{
   //
   // Get name
   //
   const QString name(item->data(0, Qt::UserRole).toString());
   
   //
   // If directory, go into it
   //
   QFileInfo fi(name);
   if (fi.isDir()) {
      setDirectory(QDir(QFileInfo(theDirectory, name).absoluteFilePath()));
      return;
   }
   
   //
   // if file, select it and close dialog
   // 
   if (fi.isFile()) {
      slotFileSelectionTreeWidgetItemClicked(item, column);
      if (selectedFiles().count() > 0) {
         slotAcceptPushButton();
      }
   }
}
      
/**
 * called when a file selection widget item clicked.
 */
void 
WuQFileDialog::slotFileSelectionListWidgetItemClicked(QListWidgetItem* /*item*/)
{
   //
   // Get all selected files
   //
   QStringList fileNameList;   
   const QList<QListWidgetItem*> selItems = fileSelectionListWidget->selectedItems();
   for (int i = 0; i < selItems.count(); i++) {
      const QListWidgetItem* item = selItems.at(i);
      const QString name(item->data(Qt::UserRole).toString());
      if (debugFlag) {
         std::cout << "Item Clicked: " << name.toAscii().constData() << std::endl;
      }
      
      QFileInfo fi(name);
      if (fi.isFile()) {
         fileNameList << fi.fileName();
      }
   }
   
   //
   // Load files into file name line edit
   // If more than one selected file, put them in double quotes
   //
   QString fileNames;
   const int numFiles = fileNameList.count();
   if (numFiles == 1) {
      fileNames = fileNameList.at(0);
   }
   else {
      for (int i = 0; i < numFiles; i++) {   
         const QString quotedName = "\"" + fileNameList.at(i) + "\"";
         if (fileNames.isEmpty() == false) {
            fileNames += " ";
         }
         fileNames.append(quotedName);
      }
   }
   updateSelectedFileLineEdit(fileNames);
}

/**
 * called when a file selection widget item double clicked.
 */
void 
WuQFileDialog::slotFileSelectionListWidgetItemDoubleClicked(QListWidgetItem* item)
{
   //
   // Get name
   //
   const QString name(item->data(Qt::UserRole).toString());
   
   //
   // If directory, go into it
   //
   QFileInfo fi(name);
   if (fi.isDir()) {
      setDirectory(QDir(QFileInfo(theDirectory, name).absoluteFilePath()));
      return;
   }
   
   //
   // if file, select it and close dialog
   // 
   if (fi.isFile()) {
      slotFileSelectionListWidgetItemClicked(item);
      if (selectedFiles().count() > 0) {
         slotAcceptPushButton();
      }
   }
}
      
/**
 * called when file type combo box selection is made.
 */
void 
WuQFileDialog::slotFileTypeComboBox(const QString& s)
{
   selectFilter(s);
}
      
/**
 * select a file filter.
 */
void 
WuQFileDialog::selectFilter(const QString& filterName)
{
   fileTypeComboBox->blockSignals(true);
   for (int i = 0; i < fileTypeComboBox->count(); i++) {
      if (filterName == fileTypeComboBox->itemText(i)) {
         fileTypeComboBox->setCurrentIndex(i);
         break;
      }
   }
   fileTypeComboBox->blockSignals(false);
   rereadDir();
}
      
/**
 * get the file filters.
 */
QStringList 
WuQFileDialog::filters() const
{
   QStringList sl;
   
   for (int i = 0; i < fileTypeComboBox->count(); i++) {
      sl << fileTypeComboBox->itemText(i);
   }
   
   return sl;
}
      
/**
 * sets file name filters.
 */
void 
WuQFileDialog::setNameFilter(const QString& filter)
{
   setNameFilters(QStringList(filter));
}

/**
 * set the file name filters.
 */
void 
WuQFileDialog::setNameFilters(const QStringList& filters)
{
   fileTypeComboBox->clear();
   
   for (int i = 0; i < filters.count(); i++) {
      fileTypeComboBox->addItem(filters.at(i));
   }
   
   if (fileTypeComboBox->count() <= 0) {
      fileTypeComboBox->addItem(tr("Any File (*)"));
   }
   
   rereadDir();
}

/**
 * get the selected file filter.
 */
QString 
WuQFileDialog::selectedFilter() const
{
   QString s;
   if (fileTypeComboBox->count() > 0) {
      s = fileTypeComboBox->currentText();
   }
   return s;
}
      
/**
 * select a file.
 */
void 
WuQFileDialog::selectFile(const QString& name)
{
   bool validFilesOnly = false;
   switch (fileMode()) {
      case AnyFile:
         break;
      case ExistingFile: 
         validFilesOnly = true;
         break;
      case Directory:
         break;
      case ExistingFiles: 
         validFilesOnly = true;
         break;
      case DirectoryOnly:
         break;
   }
   
   QFileInfo fi(name);
   
   const QString fileName = fi.fileName();
   const QString path     = fi.absolutePath();
   
   if (path.isEmpty()) {
      setDirectory(path, false);
      rereadDir();
   }
   
   QList<QListWidgetItem*> lwItems = fileSelectionListWidget->findItems(fileName, 
                                                         (Qt::MatchFixedString
                                                          | Qt::MatchCaseSensitive));

   if (lwItems.count() > 0) {
      fileSelectionListWidget->setCurrentItem(lwItems.at(0));
      updateSelectedFileLineEdit(fileName);
   }
   
   for (int i = 0; i < fileSelectionTreeWidget->topLevelItemCount(); i++) {
      QTreeWidgetItem* item = fileSelectionTreeWidget->topLevelItem(i);
      if (item->text(0) == fileName) {
         item->setSelected(true);
      }
      else {
         item->setSelected(false);
      }
   }
   
   if (validFilesOnly) {
      QFileInfo fullFilePath(theDirectory, fileName);
      if (fullFilePath.exists()) {
         updateSelectedFileLineEdit(fileName);
      }
   }
   else {
      updateSelectedFileLineEdit(fileName);
   }
}
      
/**
 * called when a navigation history selection is made.
 */
void
WuQFileDialog::slotNavigationHistoryComboBox(const QString& name)
{
   setDirectory(name, true);
}
      
/**
 * set the directory.
 */
void 
WuQFileDialog::setDirectory(const QString& dir)
{
   setDirectory(dir, false);
}

/**
 * set the directory.
 */
void 
WuQFileDialog::setDirectory(const QDir& dir)
{
   setDirectory(dir.absolutePath(), false);
}
      
/**
 * set the file mode.
 */
void 
WuQFileDialog::setFileMode(const FileMode mode) 
{ 
   theFileMode = mode; 
   
   QListWidget::SelectionMode listSelMode = QListWidget::SingleSelection;
   QTreeWidget::SelectionMode treeSelMode = QTreeWidget::SingleSelection;
   
   switch (fileMode()) {
      case AnyFile:
         break;
      case ExistingFile:
         break;
      case Directory:
         break;
      case ExistingFiles:
         listSelMode = QListWidget::ExtendedSelection;
         treeSelMode = QTreeWidget::ExtendedSelection;
         break;
      case DirectoryOnly:
         break;
   }
   
   fileSelectionListWidget->setSelectionMode(listSelMode);
   fileSelectionTreeWidget->setSelectionMode(treeSelMode);
   
   slotFileNameLineEditChanged(fileNameLineEdit->text());
   updateTitleLabelsButtons();
}
      
/**
 * get the history (previous directories).
 */
QStringList 
WuQFileDialog::history() const
{
   QStringList sl;
   
   for (int i = 0; i < navigationHistoryComboBox->count(); i++) {
      sl << navigationHistoryComboBox->itemText(i);
   }
   
   return sl;
}

/**
 * set the history (previous paths).
 */
void 
WuQFileDialog::setHistory(const QStringList& paths)
{
   navigationHistoryComboBox->clear();
   
   for (int i = 0; i < paths.count(); i++) {
      navigationHistoryComboBox->addItem(paths.at(i));
   }
}
      
/**
 * set read only.
 */
void 
WuQFileDialog::setReadOnly(const bool enabled)
{
   readOnlyFlag = enabled;
   slotFileNameLineEditChanged(fileNameLineEdit->text());
}
      
/**
 * set the directory path.
 */
void 
WuQFileDialog::setDirectory(const QString& dirPathIn,
                              const bool selectionFromHistoryFlag)
{
   //
   // Convert from ".", if needed
   //
   QString dirPath = dirPathIn;
   if (dirPath == ".") {
      dirPath = QDir::currentPath();
   }
   
   //
   // Set the directory
   //
   theDirectory.setPath(dirPath);
   
   //
   // Update navigation history combo box
   //
   if (selectionFromHistoryFlag == false) {
      navigationHistoryComboBox->blockSignals(true);

      //
      // If directory is first item in combo box, do not add it
      //
      const int dirIndex = navigationHistoryComboBox->findText(dirPath);
      if (dirIndex != 0) {
         //
         // Add to history
         //
         navigationHistoryComboBox->insertItem(0, dirPath);
         navigationHistoryComboBox->setCurrentIndex(0);
      }
      else if (dirIndex == 0) {
         navigationHistoryComboBox->setCurrentIndex(0);
      }
      
      navigationHistoryComboBox->blockSignals(false);
   }
   
   //
   // Remove previous directories from file system watcher
   // and add new directory
   //
   QStringList currentPaths(fileSystemWatcher->directories());
   if (currentPaths.count() > 0) {
      fileSystemWatcher->removePaths(currentPaths);
   }
   fileSystemWatcher->addPath(theDirectory.absolutePath());
   
   //
   // Update listing of files in dialog
   //
   rereadDir();
   
   //
   // Enable forward and back buttons
   //
   const int historyIndex = navigationHistoryComboBox->currentIndex();
   navigationBackAction->setEnabled(navigationHistoryComboBox->count()
                                        > (historyIndex + 1));
   navigationForwardAction->setEnabled(historyIndex > 0);
   
   emit directoryEntered(theDirectory.absolutePath());
}
   
/**
 * Rereads the current directory shown in the file dialog.
 */
void 
WuQFileDialog::rereadDir()
{
   //
   // Get the current file filter
   //
   QString filterText = selectedFilter();
   if (filterText.isEmpty()) {
      filterText = "*";
   }
   else {
      const int openParenIndex = filterText.indexOf("(");
      const int closeParenIndex = filterText.indexOf(")");
      if (openParenIndex >= 0) {
         if (closeParenIndex >=openParenIndex) {
            const int num = closeParenIndex - openParenIndex - 1;
            filterText = filterText.mid(openParenIndex + 1, num);
         }
         else {
            filterText = filterText.mid(openParenIndex + 1);
         }
      }
   }
   
   //
   // Get filters into a QStringList separate by white space and semicolons
   //
   QStringList filterStringList = filterText.split(QRegExp("[\\s;]"),
                                                   QString::SkipEmptyParts);
   if (debugFlag) {
      std::cout << "Filter: |" 
                << filterText.toAscii().constData()
                << "|"
                << std::endl;
      std::cout << "Filters: ";
      for (int i = 0; i < filterStringList.count(); i++) {
         std::cout << "|" 
                   << filterStringList.at(i).toAscii().constData()
                   << "|  ";
      }
      std::cout << std::endl;
   }
    
   //
   // Choose types of files to filter
   //
   bool showFiles = true;
   QDir::Filters filterFlags = QDir::NoDotAndDotDot | QDir::CaseSensitive;
   switch (fileMode()) {
      case AnyFile:
         filterFlags |= (QDir::AllDirs
                         | QDir::Files);
         break;
      case ExistingFile: 
         filterFlags |= (QDir::AllDirs
                         | QDir::Files);
         break;
      case Directory:
         filterFlags |= (QDir::AllDirs
                         | QDir::Files);
         break;
      case ExistingFiles: 
         filterFlags |= (QDir::AllDirs
                         | QDir::Files);
         break;
      case DirectoryOnly:
         filterFlags |= (QDir::AllDirs);
         showFiles = false;
         break;
   }
   if (navigationShowHiddensFilesAction->isChecked()) {
      filterFlags |= QDir::Hidden;
   }
   
   //
   // Sorting
   //
   QDir::SortFlags sortFlags = QDir::Name
                               | QDir::DirsFirst;
   if (navigationCaseSensitiveFileAction->isChecked() == false) {
      sortFlags |= QDir::IgnoreCase;
   }
                               
   //
   // Get list of directories and files
   //
   QFileInfoList dirList = theDirectory.entryInfoList(filterStringList,
                                                      filterFlags,
                                                      sortFlags);

   //
   // Remove all items in list widget
   //
   fileSelectionListWidget->clear();
   
   //
   // Add items to list widget
   //
   for (int i = 0; i < dirList.count(); i++) {
      const QFileInfo& fi = dirList.at(i);
      
      //
      // Skip files ?
      //
      if (showFiles == false) {
         if (fi.isFile()) {
            continue;
         }
      }
      
      const QString name = fi.fileName();
      
      QListWidgetItem* item = NULL;
      if (fi.isDir()) {
         item = new QListWidgetItem(QPixmap(folder_xpm), 
                                    name);
      }
      else {
         item = new QListWidgetItem(QPixmap(file_xpm), 
                                    name);
      }
      item->setData(Qt::UserRole, fi.absoluteFilePath());
      fileSelectionListWidget->addItem(item);
   }
   
   //
   // Remove all items in tree widget
   //
   fileSelectionTreeWidget->clear();
   
   //
   // Columns for different types of data
   //
   int columnCount = 0;
   const int nameColumn = columnCount++;
   const int typeColumn = columnCount++;
   const int dateColumn = columnCount++;
   const int sizeColumn = columnCount++;
   
   //
   // number of rows
   //
   const int rowCount = dirList.count();
   if (rowCount > 0) {
      //
      // set column size of tree
      //
      fileSelectionTreeWidget->setColumnCount(columnCount);
      
      if (fileSelectionTreeNameWidthAdjustedFlag == false) {
         fileSelectionTreeWidget->setColumnWidth(nameColumn, 300);
         fileSelectionTreeNameWidthAdjustedFlag = true;
      }
      
      //
      // Set column titles
      //
      QStringList columnTitles;
      columnTitles << tr("Name");
      columnTitles << tr("Type");
      columnTitles << tr("Date");
      columnTitles << tr("Size");
      fileSelectionTreeWidget->setHeaderLabels(columnTitles);
      
      //
      // Add items to list widget
      //
      for (int i = 0; i < rowCount; i++) {
         const QFileInfo& fi = dirList.at(i);

         //
         // Skip files ?
         //
         if (showFiles == false) {
            if (fi.isFile()) {
               continue;
            }
         }
      
         //
         // Create the tree widget item
         //         
         QTreeWidgetItem* treeItem = new QTreeWidgetItem(fileSelectionTreeWidget);

         //
         // Name
         //
         const QString name = fi.fileName();            
         treeItem->setText(nameColumn, name);
         if (fi.isDir()) {
            treeItem->setIcon(nameColumn, QPixmap(folder_xpm));
         }
         else {
            treeItem->setIcon(nameColumn, QPixmap(file_xpm)); 
         }
         treeItem->setData(nameColumn, Qt::UserRole, fi.absoluteFilePath());

         //
         // Type (file extension)
         //         
         QString typeName = fi.suffix();
         if (fi.isDir()) {
            typeName = "Directory";
         }
         else {
            QString extensionTypeName = "." + typeName.toLower();
            if (fileExtensionToTypeNameMap.contains(extensionTypeName)) {
               typeName = fileExtensionToTypeNameMap.value(extensionTypeName);
            }
         }
         treeItem->setText(typeColumn, typeName);
         treeItem->setData(typeColumn, Qt::UserRole, fi.absolutePath());
         
         //
         // Date
         //
         const QString dateString = fi.lastModified().toString("yyyy/MM/dd hh:mm:ss");
         treeItem->setText(dateColumn, dateString);

         //
         // size
         //
         QString sizeString;
         if (fi.isFile()) {
            const qint64 fileSize = fi.size();
            if (fileSize < 1024) {
               sizeString = (QString::number(fileSize) + " bytes");
            }
            else if (fileSize < (1024 * 1024)) {
               sizeString = (QString::number(fileSize / 1024) + " KB");
            }
            else {
               sizeString = (QString::number(fileSize / (1024*1024)) + " MB");
            }
         }
         treeItem->setText(sizeColumn, sizeString);
         
         //
         // Add to the tree
         //
         fileSelectionTreeWidget->addTopLevelItem(treeItem);
      }
   }
}
      
/**
 * returns file filters that match the name.
 */
QStringList 
WuQFileDialog::matchingFilters(const QString& name)
{
   QStringList matchingFilters;
   
   //
   // Loop through the filters
   //
   QStringList theFilters = filters();
   for (int i = 0; i < theFilters.size(); i++) {
      const QString filterText = theFilters.at(i);
      
      //
      // Loop for the part within the parenthesis (if any)
      // Example  "Word Files (*.doc *.rtf)"  ===>> "*.doc *.rtf"
      //
      const int openParenIndex = filterText.indexOf("(");
      const int closeParenIndex = filterText.indexOf(")");
      QString patternsText;
      if (openParenIndex >= 0) {
         if (closeParenIndex >=openParenIndex) {
            const int num = closeParenIndex - openParenIndex - 1;
            patternsText = filterText.mid(openParenIndex + 1, num);
         }
         else {
            patternsText = filterText.mid(openParenIndex + 1);
         }
      }
   
      //
      // Get patterns into a QStringList separate by white space and semicolons
      //
      QStringList patternsStringList = patternsText.split(QRegExp("[\\s;]"),
                                                          QString::SkipEmptyParts);
      for (int i = 0; i < patternsStringList.count(); i++) {
         const QString pattern = patternsStringList.at(i);
         QRegExp regExp(pattern, 
                        Qt::CaseSensitive,
                        QRegExp::Wildcard);
         if (regExp.exactMatch(name)) {
            matchingFilters += filterText;
         }
      }
   }
   
   return matchingFilters;
}
      
/**
 * add a toolbutton to the dialog.
 */
void 
WuQFileDialog::addToolButton(QAbstractButton* b,
                               const bool separator)
{
   if (b != NULL) {
      if (separator) {
         navigationToolButtonLayout->addWidget(new QLabel(" "));
         navigationToolButtonLayout->addWidget(b);
      }
   }
}
                         
/**
 * Add a widgets to the dialog.
 * The widgets are placed in a new row at the bottom of the dialog.
 * Widgets that are NULL are ignored.
 * A widget may be specified more than once.  For example, specifying the
 * "leftColumn" and the "centerColumn" with the same widget results in the 
 * widget occupying both the left and center column.
 */
void 
WuQFileDialog::addWidgets(QWidget* leftColumn, 
                          QWidget* centerColumn, 
                          QWidget* rightColumn)
{
   if ((leftColumn != NULL) ||
       (centerColumn != NULL) ||
       (rightColumn != NULL)) {
      const int numRows = nameFilterButtonsGridLayout->rowCount();
      
      if (leftColumn != NULL) {
         int leftColumnSpan = 1;
         if (centerColumn == leftColumn) {
            leftColumnSpan++;
            centerColumn = NULL;
            if (rightColumn == leftColumn) {
               leftColumnSpan++;
               rightColumn = NULL;
            }
         }
         Qt::Alignment alignment = Qt::AlignTop;
         if (leftColumnSpan == 1) {
            alignment |= Qt::AlignRight;
         }
         nameFilterButtonsGridLayout->addWidget(leftColumn, numRows, 0, 
                                                1, leftColumnSpan,
                                                alignment);
      }
      
      if (centerColumn != NULL) {
         int centerColumnSpan = 1;
         if (rightColumn == centerColumn) {
            centerColumnSpan++;
            rightColumn = NULL;
         }
         nameFilterButtonsGridLayout->addWidget(centerColumn, numRows, 1,
                                                1, centerColumnSpan, 
                                                Qt::AlignTop);
      }
      
      if (rightColumn != NULL) {
         nameFilterButtonsGridLayout->addWidget(rightColumn, numRows, 2, (Qt::AlignLeft
                                                                | Qt::AlignTop));
      }
      
      updateGeometry();
   }
}
      
/**
 * called when navigation back tool button clicked.
 */
void 
WuQFileDialog::slotNavigationBackAction()
{
   const int backIndex = navigationHistoryComboBox->currentIndex() + 1;
   if (navigationHistoryComboBox->count() > backIndex) {
      navigationHistoryComboBox->blockSignals(true);
      navigationHistoryComboBox->setCurrentIndex(backIndex);
      navigationHistoryComboBox->blockSignals(false);
      setDirectory(navigationHistoryComboBox->currentText(), true);
   }
}

/**
 * called when navigation forward tool button clicked.
 */
void
WuQFileDialog::slotNavigationForwardAction()
{
   const int forwardIndex = navigationHistoryComboBox->currentIndex() - 1;
   if (forwardIndex >= 0) {
      navigationHistoryComboBox->blockSignals(true);
      navigationHistoryComboBox->setCurrentIndex(forwardIndex);
      navigationHistoryComboBox->blockSignals(false);
      setDirectory(navigationHistoryComboBox->currentText(), true);
   }
}

/**
 * called when navigation go to directory tool button clicked.
 */
void 
WuQFileDialog::slotNavigationGoToDirectoryAction()
{
   QString dirName;
   bool doLoopFlag = true;
   while (doLoopFlag) {
      doLoopFlag = false;
      bool ok = false;
      dirName = QInputDialog::getText(this,
                                      tr("Go To Directory"),
                                      tr("Directory Name"),
                                      QLineEdit::Normal,
                                      dirName,
                                      &ok);
      if (ok) {
         //
         // Substitute home directory for ~
         //
         if (dirName.startsWith("~")) {
            dirName = QDir::homePath()
                      + dirName.mid(1);
         }
         QFileInfo fi(dirName);
         if (fi.exists()) {
            setDirectory(dirName);
         }
         else {
            QFileInfo fi(theDirectory, dirName);
            if (fi.exists()) {
               setDirectory(theDirectory.absolutePath() + "/" + dirName);
            }
            else {
               doLoopFlag = true;
               QMessageBox::critical(this,
                                     tr("Error"),
                                     tr("Invalid Directory Name"),
                                     QMessageBox::Ok);
            }
         }
      }
   }
}
      
/**
 * called when navigation up tool button clicked.
 */
void 
WuQFileDialog::slotNavigationUpAction()
{
   theDirectory.cdUp();
   setDirectory(theDirectory.absolutePath(), false);
}

/**
 * called when navigation refresh tool button clicked.
 */
void 
WuQFileDialog::slotNavigationRefreshAction()
{
   rereadDir();
}
      
/**
 * called when navigation new directory tool button clicked.
 */
void 
WuQFileDialog::slotNavigationNewDirectoryAction()
{
   //
   // Get name for new directory
   // 
   bool ok = false;
   const QString name = QInputDialog::getText(this,
                                              tr("New Directory"),
                                              tr("Enter name for new directory."),
                                              QLineEdit::Normal,
                                              "",
                                              &ok).trimmed();
   if (ok) {
      if (name.isEmpty() == false) {
         theDirectory.mkdir(name);
         rereadDir();
      }
   }
}

/**
 * set the view mode.
 */
void 
WuQFileDialog::setViewMode(const ViewMode viewMode)
{
   navigationListViewAction->blockSignals(true);
   navigationDetailViewAction->blockSignals(true);
   navigationListViewAction->setChecked(false);
   navigationDetailViewAction->setChecked(false);
   
   switch (viewMode) {
      case Detail:
         fileListAndDetailStackedWidget->setCurrentWidget(fileSelectionTreeWidget);
         navigationDetailViewAction->setChecked(true);
         break;
      case List:
          fileListAndDetailStackedWidget->setCurrentWidget(fileSelectionListWidget);
          navigationListViewAction->setChecked(true);
          break;
   }

   navigationListViewAction->blockSignals(false);
   navigationDetailViewAction->blockSignals(false);
}

/**
 * get the view mode.
 */
WuQFileDialog::ViewMode 
WuQFileDialog::viewMode() const
{
   if (fileListAndDetailStackedWidget->currentWidget() == fileSelectionListWidget) {
      return List;
   }
   
   return Detail;
}
      
/**
 * called when navigation rename file tool button clicked.
 */
void 
WuQFileDialog::slotNavigationRenameFileAction()
{
   const QStringList nameList = selectedFiles();
   
   if (nameList.count() == 1) {
      const QString oldFileName = nameList.at(0);
      bool ok = false;
      const QString newFileName = QInputDialog::getText(this,
                                                        tr("Rename File"),
                                                        tr("Enter new file name."),
                                                        QLineEdit::Normal,
                                                        oldFileName,
                                                        &ok);
      if (ok) {
         if (newFileName != oldFileName) {
            //
            // Rename the file
            //
            QFileInfo newInfo(theDirectory, newFileName);
            if (QFile::rename(oldFileName, newInfo.filePath()) == false) {
               QMessageBox::critical(this,
                                     tr("ERROR"),
                                     tr("Unable to rename file"),
                                     QMessageBox::Ok);
               return;
            }

            //
            // Refresh file listing
            //
            rereadDir();
         }
      }
   }   
}
      
/**
 * called when navigation delete file tool button clicked.
 */
void 
WuQFileDialog::slotNavigationDeleteFileAction()
{
   const QStringList nameList = selectedFiles();
   
   if (nameList.count() > 0) {
      QString confirmNames;
      
      //
      // Look for selected items that are not files
      //
      QStringList namesNoPath;
      for (int i = 0; i < nameList.count(); i++) {
         const QString fileName = nameList.at(i);         
         QFileInfo fi(fileName);
         const QString fileNameNoPath(fi.fileName());
         namesNoPath << fileNameNoPath;
         if (fi.isFile() == false) {
            QMessageBox::critical(this,
                                  tr("ERROR"),
                                  fileName + tr(" is not a file.  Unable to delete."),
                                  QMessageBox::Ok);
            return;
         }
         if (fi.exists() == false) {
            QMessageBox::critical(this,
                                  tr("ERROR"),
                                  fileNameNoPath + tr(" does not exist.  Unable to delete."),
                                  QMessageBox::Ok);
            return;
         }
      }
      
      //
      // CONFIRM
      //
      const QString msg = (tr("Are you sure you want to delete these files? \n")
                           + namesNoPath.join("\n"));
      if (QMessageBox::question(this,
                                tr("CONFIRM"),
                                msg,
                                QMessageBox::Ok,
                                QMessageBox::Cancel) == QMessageBox::Cancel) {
         return;
      }
      
      //
      // Delete the files
      //
      for (int i = 0; i < nameList.count(); i++) {
         const QString fileName = nameList.at(i);         
         QFile::remove(fileName);
      }
      
      //
      // Refresh file listing
      //
      rereadDir();
   }   
}
      
/**
 * called when navigation view tool button clicked.
 */
void 
WuQFileDialog::slotNavigationViewActionTriggered(QAction* action)
{
   if (action == navigationListViewAction) {
     setViewMode(List);
   }
   else {
     setViewMode(Detail);
   }
}

/**
 * called when case sensitive tool button clicked.
 */
void 
WuQFileDialog::slotNavigationCaseSensitiveAction()
{
   rereadDir();
}

/**
 * called when show hidden files tool button clicked.
 */
void 
WuQFileDialog::slotNavigationShowHiddenFilesAction()
{
   rereadDir();
}      
      
/**
 * restores the dialog's layout, history and current directory to the state specified
 * returns false if there are errors.
 */
bool 
WuQFileDialog::restoreState(const QByteArray& state)
{
   QByteArray sd = state;
   QDataStream stream(&sd, QIODevice::ReadOnly);
   stream.setVersion(QDataStream::Qt_4_3);
   if (stream.atEnd()) {
      return false;
   }
   
   qint32 marker, version;
   stream >> marker;
   stream >> version;
   if ((marker != WUQFileDialogMagic) ||
       (version != WUQFileDialogVersion)) {
      return false;
   }
   
   QByteArray splitterState;
   QByteArray headerState;
   QStringList historyState;
   QString directoryState; 
   qint32  viewModeState;
   qint32  caseSensitiveState;
   qint32  showHiddenState;
   stream >> splitterState
          >> historyState
          >> directoryState
          >> headerState
          >> viewModeState
          >> caseSensitiveState
          >> showHiddenState;
          
   if (dirFileSplitter->restoreState(splitterState) == false) {
      return false;
   }
   setHistory(historyState);
   setDirectory(directoryState);
   if (fileSelectionTreeWidget->header()->restoreState(headerState) == false) {
      return false;
   }
   setViewMode(ViewMode(viewModeState));
   navigationCaseSensitiveFileAction->setChecked(caseSensitiveState != 0);
   navigationShowHiddensFilesAction->setChecked(showHiddenState != 0);
   
   return true;
}

/**
 * saves the state of the dialog's layout, history, current directory.
 */
QByteArray 
WuQFileDialog::saveState() const
{
   QByteArray data;
   QDataStream stream(&data, QIODevice::WriteOnly);
   stream.setVersion(QDataStream::Qt_4_3);
   
   stream << qint32(WUQFileDialogMagic);
   stream << qint32(WUQFileDialogVersion);
   stream << dirFileSplitter->saveState();
   stream << history();
   stream << directory().absolutePath();
   stream << fileSelectionTreeWidget->header()->saveState();
   stream << qint32(viewMode());
   stream << qint32(navigationCaseSensitiveFileAction->isChecked());
   stream << qint32(navigationShowHiddensFilesAction->isChecked());
   
   return data;
}
      
void 
WuQFileDialog::initializeFileExtensionToTypeNameMap()
{
   fileExtensionToTypeNameMap[".doc"] = "Word";
   fileExtensionToTypeNameMap[".xls"] = "Excel";
   fileExtensionToTypeNameMap[".ppt"] = "PowerPoint";
   fileExtensionToTypeNameMap[".jpg"] = "Image - JPEG";
   fileExtensionToTypeNameMap[".tif"] = "Image - TIFF";
   fileExtensionToTypeNameMap[".tiff"] = "Image - TIFF";
   fileExtensionToTypeNameMap[".png"] = "Image - PNG";
   fileExtensionToTypeNameMap[".ppm"] = "Image - PPM";
   fileExtensionToTypeNameMap[".gif"] = "Image - GIF";
   fileExtensionToTypeNameMap[".sh"] = "Bourne Shell";
   fileExtensionToTypeNameMap[".csh"] = "C Shell";
   fileExtensionToTypeNameMap[".pl"] = "Perl";
   fileExtensionToTypeNameMap[".py"] = "Python";
   fileExtensionToTypeNameMap[".wrl"] = "VRML";
   fileExtensionToTypeNameMap[".bat"] = "MSDOS Batch";
   fileExtensionToTypeNameMap[".cpp"] = "C++";
   fileExtensionToTypeNameMap[".cxx"] = "C++";
   fileExtensionToTypeNameMap[".c"] = "C";
   fileExtensionToTypeNameMap[".o"] = "Header";
   fileExtensionToTypeNameMap[".obj"] = "Object";
   fileExtensionToTypeNameMap[".exe"] = "Executable";
   fileExtensionToTypeNameMap[".pdf"] = "PDF";
   fileExtensionToTypeNameMap[".zip"] = "Compressed";
   fileExtensionToTypeNameMap[".gz"] = "Compressed";
   fileExtensionToTypeNameMap[".pro"] = "QT Project";
   fileExtensionToTypeNameMap[".wav"] = "Audio Wave";
   fileExtensionToTypeNameMap[".mpg"] = "Audio MP3";
   fileExtensionToTypeNameMap[".mov"] = "Movie Quicktime";
   fileExtensionToTypeNameMap[".wmv"] = "Movie Windows Media";
   fileExtensionToTypeNameMap[".mpg"] = "Movie MPEG";
   fileExtensionToTypeNameMap[".mpeg"] = "Movie MPEG";
   fileExtensionToTypeNameMap[".avi"] = "Movie AVI";
   fileExtensionToTypeNameMap[".rar"] = "Archive";
   fileExtensionToTypeNameMap[".psd"] = "Photoshop";
   fileExtensionToTypeNameMap[".wma"] = "Audio Windows Media";
   fileExtensionToTypeNameMap[".sitx"] = "Stuffit X";
   fileExtensionToTypeNameMap[".sit"] = "Stuffit";
   fileExtensionToTypeNameMap[".eps"] = "Postscript";
   fileExtensionToTypeNameMap[".ps"] = "Postscript";
   fileExtensionToTypeNameMap[".rgb"] = "Rich Text";
   fileExtensionToTypeNameMap[".html"] = "HTML";
   fileExtensionToTypeNameMap[".tgz"] = "Archive Tar/Gzip";
}

