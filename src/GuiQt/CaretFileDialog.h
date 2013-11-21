#ifndef __CARET_FILE_DIALOG__H_
#define __CARET_FILE_DIALOG__H_

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
        
    public slots:
        virtual void setVisible(bool visible);
        
    private slots:
        void fileFilterWasChanged(const QString& filter);
        
    private:
        CaretFileDialog(const CaretFileDialog&);

        CaretFileDialog& operator=(const CaretFileDialog&);
        
        void initializeCaretFileDialog();
        
        FilterFilesProxyModel* m_filterFilesProxyModel;
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
            bool filterAcceptsRow ( int sourceRow, const QModelIndex & sourceParent ) const;
            
        private:
            DataFileTypeEnum::Enum m_dataFileType;
            
        };
        
#ifdef __CARET_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__CARET_FILE_DIALOG__H_
