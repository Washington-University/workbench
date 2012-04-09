
/*LICENSE_START*/
/*
 * Copyright 2011 Washington University,
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

#include <iostream>

#include <QFileSystemModel>
#include <QUrl>

#define __CARET_FILE_DIALOG_DECLARE__
#include "CaretFileDialog.h"
#undef __CARET_FILE_DIALOG_DECLARE__

#include "Brain.h"
#include "GuiManager.h"

using namespace caret;

#ifdef USE_QT_FILE_DIALOG

/**
 * On Macs, Qt shows files that do not match the filter as disabled.  This
 * method looks for disabled files and prevents them from being displayed.
 *
 * @return True to display file, else false.
 */
bool 
HideFilesProxyModel::filterAcceptsRow ( int sourceRow, const QModelIndex & sourceParent ) const {
    
    /*
     * See if the 'super' allows file to be displayed.
     */
    bool showIt = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    if (showIt) {
        /*
         * See if item is disabled, and if so, do not show it.
         */
        QModelIndex modelIndex = sourceModel()->index(sourceRow, 0, sourceParent);
        QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());
        Qt::ItemFlags flags = fileModel->flags(modelIndex);
        if((flags & Qt::ItemIsEnabled) == 0) {
            showIt = false;
        }
    }
    
    return showIt;
}

#endif // USE_QT_FILE_DIALOG


/**
 * \class caret::CaretFileDialog 
 * \brief Adds additional functionality over Qt's QFileDialog.
 */

/**
 * Constructor.
 */
CaretFileDialog::CaretFileDialog(QWidget* parent,
                                 Qt::WindowFlags f)
#ifdef USE_QT_FILE_DIALOG
: QFileDialog(parent,
                f)
#else
: WuQFileDialog(parent,
                f)
#endif
{
    this->initializeCaretFileDialog();
        
    this->setDirectory(GuiManager::get()->getBrain()->getCurrentDirectory());
}
/**
 * Constructor.
 */
CaretFileDialog::CaretFileDialog(QWidget* parent,
                                 const QString& caption,
                                 const QString& directory,
                                 const QString& filter)
#ifdef USE_QT_FILE_DIALOG
: QFileDialog(parent,
              caption,
              directory,
              filter)
#else
: WuQFileDialog(parent,
                caption,
                directory,
                filter)
#endif
{
    this->initializeCaretFileDialog();
    
    if (directory.isEmpty()) {
        this->setDirectory(GuiManager::get()->getBrain()->getCurrentDirectory());
    }
}

/**
 * Destructor.
 */
CaretFileDialog::~CaretFileDialog()
{
}

/**
 * Initialize the file dialog.
 */
void 
CaretFileDialog::initializeCaretFileDialog()
{
#ifdef USE_QT_FILE_DIALOG
    /*
     * Create a proxy model that hides files that do not match the file filter.
     * On Macs, Qt shows files that do not match the file filter as disabled
     * but we don't want them displayed.  The dialog will take ownership of 
     * the proxy model so it does not need to be deleted by this instance.
     */
    HideFilesProxyModel* fileFilterProxyModel = new HideFilesProxyModel();
    this->setProxyModel(fileFilterProxyModel);
    
    /*
     * This appears that it should work but it still shows files disabled on Mac.
     *
     QFileSystemModel* model = new QFileSystemModel(this);
     model->setNameFilterDisables(false);
     QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
     proxyModel->setSourceModel(model);
     model->setNameFilterDisables(false);
     this->setProxyModel(proxyModel);
     */
#endif

#ifdef USE_QT_FILE_DIALOG
#ifdef Q_OS_MACX
    /*
     * On Macs, add /Volumes to the sidebar URLs 
     * so that mounted disks can be accessed.
     */
    QList<QUrl> urls = this->sidebarUrls();
    urls.append(QUrl::fromLocalFile("/Volumes"));
    this->setSidebarUrls(urls);
#endif // Q_OS_MACX
#endif
}

/**
 * Like QFileDialog::getOpenFileName() except that this
 * NEVER uses the native file dialog thus providing 
 * a consistent user-interface across platforms.
 *
 * @param parent
 *    Parent on which this dialog is displayed.
 * @param caption
 *    Caption for dialog (if not provided a default caption is shown)
 * @param dir
 *    Directory show by dialog (Brain's current directory if empty string)
 * @param filter
 *    Filter for file file selection.
 * @param selectedFilter
 *    Optional selected filter.
 * @param options
 *    Options (see QFileDialog).
 * @return
 *    Name of file selected or empty string if user cancelled.
 */
QString 
CaretFileDialog::getOpenFileNameDialog(QWidget *parent,
                                     const QString &caption,
                                     const QString &dir,
                                     const QString &filter,
                                     QString *selectedFilter,
                                     Options options )
{
    CaretFileDialog cfd(parent,
                        caption,
                        dir,
                        filter);
    if (selectedFilter != 0) {
        cfd.selectFilter(*selectedFilter);
    }
    cfd.setOptions(options);
    cfd.setAcceptMode(CaretFileDialog::AcceptOpen);
    cfd.setFileMode(CaretFileDialog::AnyFile);
    
    if (cfd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = cfd.selectedFiles();
        if (selectedFiles.size() > 0) {
            return selectedFiles[0];
        }
    }
    
    return QString();
}

/**
 * Like QFileDialog::getSaveFileName() except that this
 * NEVER uses the native file dialog thus providing 
 * a consistent user-interface across platforms.
 *
 * @param parent
 *    Parent on which this dialog is displayed.
 * @param caption
 *    Caption for dialog (if not provided a default caption is shown)
 * @param dir
 *    Directory show by dialog (Brain's current directory if empty string)
 * @param filter
 *    Filter for file file selection.
 * @param selectedFilter
 *    Optional selected filter.
 * @param options
 *    Options (see QFileDialog).
 * @return
 *    Name of file selected or empty string if user cancelled.
 */
QString 
CaretFileDialog::getSaveFileNameDialog(QWidget *parent,
                                     const QString &caption,
                                     const QString &dir,
                                     const QString &filter,
                                     QString *selectedFilter,
                                     Options options)
{
    CaretFileDialog cfd(parent,
                        caption,
                        dir,
                        filter);
    if (selectedFilter != 0) {
        cfd.selectFilter(*selectedFilter);
    }
    cfd.setOptions(options);
    cfd.setAcceptMode(QFileDialog::AcceptSave);
    cfd.setFileMode(CaretFileDialog::AnyFile);
    
    if (cfd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = cfd.selectedFiles();
        if (selectedFiles.size() > 0) {
            return selectedFiles[0];
        }
    }
    
    return QString();
}

/**
 * Like QFileDialog::getExistingDirectory() except that this
 * NEVER uses the native file dialog thus providing 
 * a consistent user-interface across platforms.
 *
 * @param parent
 *    Parent on which this dialog is displayed.
 * @param caption
 *    Caption for dialog (if not provided a default caption is shown)
 * @param dir
 *    Directory show by dialog (Brain's current directory if empty string)
 * @param filter
 *    Filter for file file selection.
 * @param selectedFilter
 *    Optional selected filter.
 * @param options
 *    Options (see QFileDialog).
 * @return
 *    Name of directory selected or empty string if user cancelled.
 */
QString 
CaretFileDialog::getExistingDirectoryDialog(QWidget *parent,
                                          const QString &caption,
                                          const QString &dir,
                                          Options options)
{
    CaretFileDialog cfd(parent,
                        caption,
                        dir,
                        "");
    cfd.setOptions(options);
    cfd.setAcceptMode(CaretFileDialog::AcceptOpen);
    cfd.setFileMode(CaretFileDialog::Directory);
    cfd.setOption(CaretFileDialog::ShowDirsOnly, true);
    
    if (cfd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = cfd.selectedFiles();
        if (selectedFiles.size() > 0) {
            return selectedFiles[0];
        }
    }
    
    return QString();
}

/**
 * Like QFileDialog::getOpenFileNames() except that this
 * NEVER uses the native file dialog thus providing 
 * a consistent user-interface across platforms.
 *
 * @param parent
 *    Parent on which this dialog is displayed.
 * @param caption
 *    Caption for dialog (if not provided a default caption is shown)
 * @param dir
 *    Directory show by dialog (Brain's current directory if empty string)
 * @param filter
 *    Filter for file file selection.
 * @param selectedFilter
 *    Optional selected filter.
 * @param options
 *    Options (see QFileDialog).
 * @return
 *    StringList of file(s) selected or empty list if user cancelled.
 */
QStringList 
CaretFileDialog::getOpenFileNamesDialog(QWidget *parent,
                                          const QString &caption,
                                          const QString &dir,
                                          const QString &filter,
                                          QString *selectedFilter,
                                          Options options)
{
    CaretFileDialog cfd(parent,
                        caption,
                        dir,
                        filter);
    if (selectedFilter != 0) {
        cfd.selectFilter(*selectedFilter);
    }
    cfd.setOptions(options);
    cfd.setAcceptMode(CaretFileDialog::AcceptOpen);
    cfd.setFileMode(CaretFileDialog::ExistingFiles);
    
    if (cfd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = cfd.selectedFiles();
        if (selectedFiles.size() > 0) {
            return selectedFiles;
        }
    }
    
    QStringList sl;
    return sl;
}


