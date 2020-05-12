#ifndef __CARET_FILE_DIALOG__H_
#define __CARET_FILE_DIALOG__H_

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

#include <QFileDialog>
#include <QSortFilterProxyModel>

#include "DataFileTypeEnum.h"

namespace caret {

    class FilterFilesProxyModel;
    
    class CaretFileDialog : public QFileDialog {
        Q_OBJECT
        
    public:
        CaretFileDialog(QWidget* parent,
                        Qt::WindowFlags f);
        
        CaretFileDialog(QWidget* parent = 0,
                      const QString& caption = QString(),
                      const QString& directory = QString(),
                      const QString& filter = QString());
        
        virtual ~CaretFileDialog();
        
        // modal method to get open file name
        static QString getOpenFileNameDialog(QWidget *parent = 0,
                                       const QString &caption = QString(),
                                       const QString &dir = QString(),
                                       const QString &filter = QString(),
                                       QString *selectedFilter = 0,
                                       Options options = 0);
        
        // model method to get open file name
        static QString getOpenFileNameDialog(const DataFileTypeEnum::Enum dataFileType,
                                             QWidget* parent = 0,
                                             const QString& caption = QString(),
                                             const QString& dir = QString(),
                                             Options options = 0);
        
        // modal method to get save file name
        static QString getSaveFileNameDialog(QWidget *parent = 0,
                                       const QString &caption = QString(),
                                       const QString &dir = QString(),
                                       const QString &filter = QString(),
                                       QString *selectedFilter = 0,
                                       Options options = 0);
        
        // modal method to get save file name
        static QString getSaveFileNameDialog(const DataFileTypeEnum::Enum dataFileType,
                                             QWidget *parent = 0,
                                             const QString &caption = QString(),
                                             const QString &dir = QString(),
                                             Options options = 0);
        
        // modal method to get choose file name
        static QString getChooseFileNameDialog(const DataFileTypeEnum::Enum dataFileType,
                                               const QString& directoryOrFileName,
                                               QWidget *parent = 0);
        
        // modal method to get directory name
        static QString getExistingDirectoryDialog(QWidget *parent = 0,
                                            const QString &caption = QString(),
                                            const QString &dir = QString(),
                                            Options options = ShowDirsOnly);
        
        // modal method to get open file names
        static QStringList getOpenFileNamesDialog(QWidget *parent = 0,
                                            const QString &caption = QString(),
                                            const QString &dir = QString(),
                                            const QString &filter = QString(),
                                            QString *selectedFilter = 0,
                                            Options options = 0);
        
        void saveDialogSettings(const AString& settingsName);
        
        void restoreDialogSettings(const AString& settingsName);
        
    public slots:
        virtual void setVisible(bool visible);
        
    private slots:
        void fileFilterWasChanged(const QString& filter);
        
    private:
        CaretFileDialog(const CaretFileDialog&);

        CaretFileDialog& operator=(const CaretFileDialog&);
        
        void initializeCaretFileDialog();
        
        FilterFilesProxyModel* m_filterFilesProxyModel;

        class PreviousDialogSettings {
        public:
            PreviousDialogSettings(const AString& fileFilterName,
                                   const AString& directoryName,
                                   const QByteArray& dialogGeometry)
            : m_fileFilterName(fileFilterName),
            m_directoryName(directoryName),
            m_dialogGeometry(dialogGeometry)
            {
                
            }
            
            AString m_fileFilterName;
            AString m_directoryName;
            QByteArray m_dialogGeometry;
        };
        
        static std::map<AString, PreviousDialogSettings> s_previousDialogSettingsMap;
    };
    
        /**
         * May be fully implemented to provide additional filtering of files.
         */
        class FilterFilesProxyModel : public QSortFilterProxyModel {
            
        public:
            FilterFilesProxyModel();
            
            virtual ~FilterFilesProxyModel();
            
            void setDataFileTypeForFiltering(const DataFileTypeEnum::Enum dataFileType);
            
        protected:
            bool filterAcceptsRow ( int sourceRow, const QModelIndex & sourceParent ) const override;
            
            void sort(int column, Qt::SortOrder order) override;
        private:
            DataFileTypeEnum::Enum m_dataFileType;
            
        };
    
#ifdef __CARET_FILE_DIALOG_DECLARE__
    std::map<AString, CaretFileDialog::PreviousDialogSettings> CaretFileDialog::s_previousDialogSettingsMap;
#endif // __CARET_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__CARET_FILE_DIALOG__H_
