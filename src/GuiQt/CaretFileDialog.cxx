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

#include <iostream>

#include <QFileSystemModel>
#include <QUrl>

#define __CARET_FILE_DIALOG_DECLARE__
#include "CaretFileDialog.h"
#undef __CARET_FILE_DIALOG_DECLARE__

#include "Brain.h"
#include "GuiManager.h"

using namespace caret;

FilterFilesProxyModel::FilterFilesProxyModel()
{
    m_dataFileType = DataFileTypeEnum::UNKNOWN;
}

FilterFilesProxyModel::~FilterFilesProxyModel()
{
}


/**
 * On Macs, Qt shows files that do not match the filter as disabled.  This
 * method looks for disabled files and prevents them from being displayed.
 *
 * @return True to display file, else false.
 */
bool 
FilterFilesProxyModel::filterAcceptsRow ( int sourceRow, const QModelIndex & sourceParent ) const {
    
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
        
        /*
         * All CIFTI files use a 'CIFTI prefix' followed by the NIFTI volume
         * extension.  As a result, when the File Dialog's filter is set
         * to volume files, both volume and CIFTI files are displayed.
         * So, when the filter is volume files, inhibit the display CIFTI
         * files in the file selection dialog.
         */
        if (showIt) {
            if (m_dataFileType == DataFileTypeEnum::VOLUME) {
                const AString name = fileModel->fileName(modelIndex);
                bool isValid = false;
                const DataFileTypeEnum::Enum fileType = DataFileTypeEnum::fromFileExtension(name,
                                                                                            &isValid);
                if (isValid) {
                    if (fileType != DataFileTypeEnum::VOLUME) {
                        showIt = false;
                    }
                }
            }
        }
    }
    
    return showIt;
}

void
FilterFilesProxyModel::setDataFileTypeForFiltering(const DataFileTypeEnum::Enum dataFileType)
{
    m_dataFileType = dataFileType;
}

/**
 * \class caret::CaretFileDialog 
 * \brief Adds additional functionality over Qt's QFileDialog.
 */

/**
 * Constructor.
 */
CaretFileDialog::CaretFileDialog(QWidget* parent,
                                 Qt::WindowFlags f)
: QFileDialog(parent,
                f)
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
: QFileDialog(parent,
              caption,
              directory,
              filter)
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
    /*
     * We MUST use a non-native dialog.
     * Otherwise, the file filter proxy model 
     * will not work.
     */
    setOption(QFileDialog::DontUseNativeDialog);
    
    /*
     * Create a proxy model for filtering the data files.
     * The proxy is used to limit the displayed files to 
     * only those with the proper file extension.  In addition,
     * since CIFTI and NIFTI files end in ".nii", filtering is
     * performed so that CIFTI files are not displayed when
     * the user selectes Volume files.
     */
    m_filterFilesProxyModel = new FilterFilesProxyModel();
    this->setProxyModel(m_filterFilesProxyModel);
    
#ifdef Q_OS_MACX
    /*
     * On Macs, add /Volumes to the sidebar URLs 
     * so that mounted disks can be accessed.
     */
    QList<QUrl> urls = this->sidebarUrls();
    urls.append(QUrl::fromLocalFile("/Volumes"));
    this->setSidebarUrls(urls);
#endif // Q_OS_MACX
    
    QObject::connect(this, SIGNAL(filterSelected(const QString&)),
                     this, SLOT(fileFilterWasChanged(const QString&)));
}

/**
 * Overrides parent's setVisible to ensure file filter is properly set
 *
 * @param visible
 *     New visibility status.
 */
void
CaretFileDialog::setVisible(bool visible)
{
    if (visible) {
        fileFilterWasChanged(selectedNameFilter());
    }
    
    QFileDialog::setVisible(visible);
}

/**
 * Gets called when the file filter is changed.
 *
 * @param filter
 *    Newly selected file filter.
 */
void
CaretFileDialog::fileFilterWasChanged(const QString& filter)
{
    bool isValid = false;
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromQFileDialogFilter(filter,
                                                                                  &isValid);
    
    if ( ! isValid) {
        dataFileType = DataFileTypeEnum::UNKNOWN;
    }
    
    m_filterFilesProxyModel->setDataFileTypeForFiltering(dataFileType);
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
        cfd.selectNameFilter(*selectedFilter);
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
        cfd.selectNameFilter(*selectedFilter);
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
 * Like QFileDialog::getSaveFileName() except that this
 * NEVER uses the native file dialog thus providing 
 * a consistent user-interface across platforms.
 *
 * The file filter will be from the given data file type.
 * The returned file will contain a valid extension from the
 * given data file type.
 *
 * @param dataFileType
 *    Type of file being saved.
 * @param parent
 *    Parent on which this dialog is displayed.
 * @param caption
 *    Caption for dialog (if not provided a default caption is shown)
 * @param dir
 *    Directory show by dialog (Brain's current directory if empty string)
 * @param options
 *    Options (see QFileDialog).
 * @return
 *    Name of file selected or empty string if user cancelled.  If there is
 *    no file extension or it is invalid, a valid extension will be added.
 */
QString 
CaretFileDialog::getSaveFileNameDialog(const DataFileTypeEnum::Enum dataFileType,
                                       QWidget *parent,
                                       const QString &caption,
                                       const QString &dir,
                                       Options options)
{
    CaretFileDialog cfd(parent,
                        caption,
                        dir,
                        DataFileTypeEnum::toQFileDialogFilter(dataFileType));
    cfd.selectNameFilter(DataFileTypeEnum::toQFileDialogFilter(dataFileType));
    cfd.setOptions(options);
    cfd.setAcceptMode(QFileDialog::AcceptSave);
    cfd.setFileMode(CaretFileDialog::AnyFile);
    
    if (cfd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = cfd.selectedFiles();
        if (selectedFiles.size() > 0) {
            AString filename = DataFileTypeEnum::addFileExtensionIfMissing(selectedFiles[0], 
                                                                           dataFileType);
            return filename;
        }
    }
    
    return QString();
}

/**
 * Like QFileDialog::getSaveFileName() except that this
 * NEVER uses the native file dialog thus providing
 * a consistent user-interface across platforms.  It also will
 * display "Choose" for the selection button.
 *
 * The file filter will be from the given data file type.
 * The returned file will contain a valid extension from the
 * given data file type.
 *
 * @param dataFileType
 *    Type of file being saved.
 * @param directoryOrFileName
 *    Name of directory or name of file.
 * @param parent
 *    Parent on which this dialog is displayed.
 * @param caption
 *    Caption for dialog (if not provided a default caption is shown)
 * @param dir
 *    Directory show by dialog (Brain's current directory if empty string)
 * @param options
 *    Options (see QFileDialog).
 * @return
 *    Name of file selected or empty string if user cancelled.  If there is
 *    no file extension or it is invalid, a valid extension will be added.
 */
QString
CaretFileDialog::getChooseFileNameDialog(const DataFileTypeEnum::Enum dataFileType,
                                         const QString& directoryOrFileName,
                                         QWidget *parent)
{
    CaretFileDialog fd(parent);
    if (dataFileType != DataFileTypeEnum::UNKNOWN) {
        fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilter(dataFileType));
    }
    fd.selectNameFilter(DataFileTypeEnum::toQFileDialogFilter(dataFileType));
    fd.setAcceptMode(CaretFileDialog::AcceptSave);
    fd.setFileMode(CaretFileDialog::AnyFile);
    fd.setViewMode(CaretFileDialog::List);
    
    if (directoryOrFileName.isEmpty() == false) {
        QFileInfo fileInfo(directoryOrFileName);
        if (fileInfo.isDir()) {
            fd.setDirectory(directoryOrFileName);
        }
        else {
            fd.selectFile(directoryOrFileName);
        }
    }
    else {
        fd.setDirectory(GuiManager::get()->getBrain()->getCurrentDirectory());
    }
    
    fd.setLabelText(CaretFileDialog::Accept, "Choose");
    fd.setWindowTitle("Choose File Name");
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = fd.selectedFiles();
        if (selectedFiles.size() > 0) {
            AString filename = DataFileTypeEnum::addFileExtensionIfMissing(selectedFiles[0],
                                                                           dataFileType);
            return filename;
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
        cfd.selectNameFilter(*selectedFilter);
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

/**
 * Save the dialog settings (directory, filename filter, and geometry)
 * for next time dialog displayed.
 *
 * @param settingsName
 *     Name for settings.
 */
void
CaretFileDialog::saveDialogSettings(const AString& settingsName)
{
    PreviousDialogSettings previousSettings(selectedNameFilter(),
                                            directory().absolutePath(),
                                            saveGeometry());
    
    std::map<AString, PreviousDialogSettings>::iterator iter = s_previousDialogSettingsMap.find(settingsName);
    if (iter != s_previousDialogSettingsMap.end()) {
        iter->second = previousSettings;
    }
    else {
        s_previousDialogSettingsMap.insert(std::make_pair(settingsName,
                                                          previousSettings));
    }
}

/**
 * Restore the dialog settings (directory, filename filter, and geometry)
 * for next time dialog displayed.
 *
 * @param settingsName
 *     Name for settings used when settings were saved.
 */
void
CaretFileDialog::restoreDialogSettings(const AString& settingsName)
{
    std::map<AString, PreviousDialogSettings>::iterator iter = s_previousDialogSettingsMap.find(settingsName);
    if (iter != s_previousDialogSettingsMap.end()) {
        std::cout << "Found and restoring settings for " << qPrintable(settingsName) << std::endl;
        
        const PreviousDialogSettings previousSettings = iter->second;
        
        FileInformation fileInfo(previousSettings.m_directoryName);
        if (fileInfo.exists()) {
            setDirectory(previousSettings.m_directoryName);
        }
        
        QStringList dialogFilters = nameFilters();
        QStringListIterator filterIter(dialogFilters);
        while (filterIter.hasNext()) {
            const AString filterName = filterIter.next();
            if (filterName == previousSettings.m_fileFilterName) {
                selectNameFilter(previousSettings.m_fileFilterName);
                break;
            }
        }
        
        restoreGeometry(previousSettings.m_dialogGeometry);
    }
}


