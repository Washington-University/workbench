
#ifndef __WU_Q_FILE_DIALOG_H__
#define __WU_Q_FILE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

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

#include <QDialog>
#include <QDir>
#include <QMap>
#include <QUrl>


class QAbstractButton;
class QAction;
class QByteArray;
class QComboBox;
class QFileSystemWatcher;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLayout;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSplitter;
class QStackedWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QVBoxLayout;

namespace caret {
    /// class for a useful, extendable file selection dialog
    class WuQFileDialog : public QDialog {
        Q_OBJECT
        
    public:
        /// type used for mapping file extensions to file type names
        typedef QMap<QString,QString> TypeExtensionToTypeNameMap;
        
        /// opening or closing a file 
        enum AcceptMode { 
            AcceptOpen, 
            AcceptSave
        };
        
        /// enum used to set dialog button or label texts
        enum DialogLabel { 
            LookIn, 
            FileName, 
            FileType, 
            Accept, 
            Reject 
        };
        
        /// determines what user may select and what dialog returns if accepted
        enum FileMode { 
            /// name of file, whether it exists or not
            AnyFile, 
            /// name of a single existing file
            ExistingFile, 
            /// name of a directory and both files and directories are displayed
            Directory, 
            /// names of zero or more existing files
            ExistingFiles, 
            /// name of a directory and only directories are displayed
            DirectoryOnly 
        };
        
        enum Option {
            ShowDirsOnly = 0x01,
            DontResolveSymlinks = 0x02,
            DontConfirmOverwrite = 0x04,
            DontUseSheet = 0x08,
            DontUseNativeDialog = 0x10
        };
        typedef unsigned int Options;
        
        enum ViewMode { 
            Detail, 
            List 
        };
        
        // constructor
        WuQFileDialog(QWidget* parent,
                      Qt::WindowFlags f);
        
        // constructor
        WuQFileDialog(QWidget* parent = 0,
                      const QString& caption = QString(),
                      const QString& directory = QString(),
                      const QString& filter = QString());
        
        // destructor
        ~WuQFileDialog();
        
        /// get the accept mode
        AcceptMode acceptMode() const { return theAcceptMode; }
        
        /// get confirm overwrite
        bool confirmOverwrite() const { return confirmOverwriteFlag; }
        
        /// get the default suffix
        QString defaultSuffix() const { return theDefaultSuffix; }
        
        /// get the directory
        QDir directory() const;
        
        /// get the file mode
        FileMode fileMode() const { return theFileMode; }
        
        /// get the file filters
        QStringList filters() const;
        
        /// get the history (previous directories)
        QStringList history() const;
        
        /// get read only status
        bool isReadOnly() const { return readOnlyFlag; }
        
        /// get label text
        QString labelText(const DialogLabel label) const;
        
        // returns file filters that match the name
        virtual QStringList matchingFilters(const QString& name);
        
        // restores the dialog's layout, history and current directory to the state specified
        // returns false if there are errors
        bool restoreState(const QByteArray& state);
        
        // saves the state of the dialog's layout, history, current directory
        QByteArray saveState() const;
        
        /// select a file
        void selectFile(const QString& name);
        
        /// select the file filter
        void selectFilter(const QString& filter);
        
        // get the selected files
        QStringList selectedFiles() const;
        
        // get the selected file filter
        QString selectedFilter() const;
        
        // set the accept mode
        void setAcceptMode(const AcceptMode mode);
        
        /// set confirm overwrite
        void setConfirmOverwrite(const bool enabled) { confirmOverwriteFlag = enabled; }
        
        /// set the default suffix
        void setDefaultSuffix(const QString& suffix) { theDefaultSuffix = suffix; }
        
        // set the directory 
        void setDirectory(const QString& dir);
        
        // set the directory 
        void setDirectory(const QDir& dir);
        
        // set the file mode
        void setFileMode(const FileMode mode);
        
        // sets file name filters
        void setNameFilter(const QString& filter);
        
        // set the file name filters
        void setNameFilters(const QStringList& filters);
        
        // set the history (previous paths)
        void setHistory(const QStringList& paths);
        
        // set the label text
        void setLabelText(const DialogLabel label, const QString& text);
        
        // set read only 
        void setReadOnly(const bool enabled);
        
        // set the sidebar URLS
        void setSidebarUrls(const QList<QUrl>& urls);
        
        // set the view mode
        void setViewMode(const ViewMode viewMode);
        
        // get sidebar Urls
        QList<QUrl> sidebarUrls() const;
        
        // get the view mode
        ViewMode viewMode() const;
        
        // modal method to get open file name
        static QString getOpenFileName(QWidget *parent = 0,
                                       const QString &caption = QString(),
                                       const QString &dir = QString(),
                                       const QString &filter = QString(),
                                       QString *selectedFilter = 0,
                                       Options options = 0);
        
        // modal method to get save file name
        static QString getSaveFileName(QWidget *parent = 0,
                                       const QString &caption = QString(),
                                       const QString &dir = QString(),
                                       const QString &filter = QString(),
                                       QString *selectedFilter = 0,
                                       Options options = 0);
        
        // modal method to get directory name
        static QString getExistingDirectory(QWidget *parent = 0,
                                            const QString &caption = QString(),
                                            const QString &dir = QString(),
                                            Options options = ShowDirsOnly);
        
        // modal method to get open file names
        static QStringList getOpenFileNames(QWidget *parent = 0,
                                            const QString &caption = QString(),
                                            const QString &dir = QString(),
                                            const QString &filter = QString(),
                                            QString *selectedFilter = 0,
                                            Options options = 0);
        
        // initialize the default file extension to type name map
        static void initializeFileExtensionToTypeNameMap();
        
        /// get the file extension to type name map
        static TypeExtensionToTypeNameMap getFileExtensionToTypeNameMap() 
        { return fileExtensionToTypeNameMap; }
        
        /// set the file extension to type name map
        static void setFileExtensionToTypeNameMap(const TypeExtensionToTypeNameMap& typeMap) 
        { fileExtensionToTypeNameMap = typeMap; }
        
    signals:
        // When the current file changes, this signal is emitted with the new file
        // as the path paramter
        void currentChanged(const QString&);
        
        // This signal is emitted when the user enters a directory
        void directoryEntered(const QString&);
        
        // When the selection changes, this signal is emitted with the (possibly
        // empty) list of selected files
        void filesSelected(const QStringList&);
        
        // This signal is emitted when the user selects a filter
        void filterSelected(const QString&);
        
    public slots:      
        // Rereads the current directory shown in the file dialog
        void rereadDir();
        
    protected:
        // add a widgets to the dialog
        void addWidgets(QWidget* leftColumn, 
                        QWidget* centerColumn, 
                        QWidget* rightColumn);
        
        // add a toolbutton to the dialog
        void addToolButton(QAbstractButton* b,
                           const bool separator = false);
        
        private slots:
        // called when navigation back tool button clicked
        void slotNavigationBackAction();
        
        // called when navigation forward tool button clicked
        void slotNavigationForwardAction();
        
        // called when navigation up tool button clicked
        void slotNavigationUpAction();
        
        // called when navigation go to directory tool button clicked
        void slotNavigationGoToDirectoryAction();
        
        // called when navigation refresh tool button clicked
        void slotNavigationRefreshAction();
        
        // called when case sensitive tool button clicked
        void slotNavigationCaseSensitiveAction();
        
        // called when show hidden files tool button clicked
        void slotNavigationShowHiddenFilesAction();
        
        // called when navigation new directory tool button clicked
        void slotNavigationNewDirectoryAction();
        
        // called when navigation delete file tool button clicked
        void slotNavigationDeleteFileAction();
        
        // called when navigation rename file tool button clicked
        void slotNavigationRenameFileAction();
        
        // called when navigation view tool button clicked
        void slotNavigationViewActionTriggered(QAction*);
        
        // called when a common directory item selected
        void slotCommonDirectoryListWidget(QListWidgetItem*);
        
        // called when a file selection list widget item clicked
        void slotFileSelectionListWidgetItemClicked(QListWidgetItem*);
        
        // called when a file selection list widget item double clicked
        void slotFileSelectionListWidgetItemDoubleClicked(QListWidgetItem*);
        
        // called when a file selection tree widget item clicked
        void slotFileSelectionTreeWidgetItemClicked(QTreeWidgetItem*, int);
        
        // called when a file selection tree widget item double clicked
        void slotFileSelectionTreeWidgetItemDoubleClicked(QTreeWidgetItem*, int);
        
        // called when a navigation history selection is made
        void slotNavigationHistoryComboBox(const QString&);
        
        // called when accept button is pressed
        void slotAcceptPushButton();
        
        // called when reject button is pressed
        void slotRejectPushButton();
        
        // called when file type combo box selection is made
        void slotFileTypeComboBox(const QString&);
        
        // called when file name line edit is changed
        void slotFileNameLineEditChanged(const QString&);
        
    private:
        // add to common directory
        void addToCommonDirectory(const QString& directoryPath,
                                  const QString& labelName);
        
        // initialize the dialog
        void initializeDialog();
        
        // update the title, labels, and buttons
        void updateTitleLabelsButtons();
        
        // update selected file line edit
        void updateSelectedFileLineEdit(const QString& s);
        
        // create the navigation section of the dialog
        QLayout* createNavigationSection();
        
        // create the common directory section of the dialog
        QWidget* createCommonDirectorySection();
        
        // create the file selection section of the dialog
        QWidget* createFileSelectionSection();
        
        // load the common directory section
        void loadCommonDirectorySection();
        
        // set the directory 
        void setDirectory(const QString& dirPath,
                          const bool selectionFromHistoryFlag);
        
        /// file name line edit
        QLineEdit* fileNameLineEdit;
        
        /// file type combo box
        QComboBox* fileTypeComboBox;
        
        /// accept push button
        QPushButton* acceptPushButton;
        
        /// reject push button
        QPushButton* rejectPushButton;
        
        /// look in label
        QLabel* lookInLabel;
        
        /// file name label
        QLabel* fileNameLabel;
        
        /// file type label;
        QLabel* fileTypeLabel;
        
        /// navigation history combo box
        QComboBox* navigationHistoryComboBox;
        
        /// list widget for common directories
        QListWidget* commonDirectoryListWidget;
        
        /// layout for labels, combo boxes, and push buttons
        QGridLayout* nameFilterButtonsGridLayout;
        
        /// index of first sidebar URL in "commonDirectoryListWidget"
        int firstSideBarUrlIndex;
        
        /// stacked widget for file name list and detail views
        QStackedWidget* fileListAndDetailStackedWidget;
        
        /// file selection list widget
        QListWidget* fileSelectionListWidget;
        
        /// file selection tree widget
        QTreeWidget* fileSelectionTreeWidget;
        
        /// width has already been set for name in tree
        bool fileSelectionTreeNameWidthAdjustedFlag;
        
        /// navigation toolbutton layout
        QHBoxLayout* navigationToolButtonLayout;
        
        /// navigation back action
        QAction* navigationBackAction;
        
        /// navigation forward action
        QAction* navigationForwardAction;
        
        /// navigation up action
        QAction* navigationUpAction;
        
        /// navigation go to directory action
        QAction* navigationGoToDirectoryAction;
        
        /// navigation refresh action
        QAction* navigationRefreshAction;
        
        /// navigation case sensitive action
        QAction* navigationCaseSensitiveFileAction;
        
        /// navigation show hidden files action
        QAction* navigationShowHiddensFilesAction;
        
        /// navigation new directory action
        QAction* navigationNewDirectoryAction;
        
        /// delete file action
        QAction* navigationDeleteFileAction;
        
        /// rename the file action
        QAction* navigationRenameFileAction;
        
        /// navigation list view action
        QAction* navigationListViewAction;
        
        /// navigation detail view tool button
        QAction* navigationDetailViewAction;
        
        /// the directory being navigated
        QDir theDirectory;      
        
        /// the accept mode
        AcceptMode theAcceptMode;
        
        /// confirm overwrite flag
        bool confirmOverwriteFlag;
        
        /// the default suffix
        QString theDefaultSuffix;
        
        /// the file mode
        FileMode theFileMode;
        
        /// user set label for "look in"
        bool userSetLookInLabel;
        
        /// user set label for "file name"
        bool userSetFileNameLabel;
        
        /// user set label for "file type"
        bool userSetFileTypeLabel;
        
        /// user set label for "accept"
        bool userSetAcceptLabel;
        
        /// user set label for "reject"
        bool userSetRejectLabel;
        
        /// read only flag
        bool readOnlyFlag;
        
        /// the file system watcher
        QFileSystemWatcher* fileSystemWatcher;
        
        /// splits between side bar and files
        QSplitter* dirFileSplitter;
        
        /// the debugging flag
        bool debugFlag;
        
        /// the map that maps file extensions to type names
        static TypeExtensionToTypeNameMap fileExtensionToTypeNameMap;
    };

#ifdef _WU_Q_FILE_DIALOG_MAIN_H_
   WuQFileDialog::TypeExtensionToTypeNameMap WuQFileDialog::fileExtensionToTypeNameMap;
#endif // _WU_Q_FILE_DIALOG_MAIN_H_

} // namespace caret
#endif  // __WU_Q_FILE_DIALOG_H__

