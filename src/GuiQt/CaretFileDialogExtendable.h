#ifndef __CARET_FILE_DIALOG_EXTENDABLE__H_
#define __CARET_FILE_DIALOG_EXTENDABLE__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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


#include <QDialog>
#include <QFileDialog>

class AbstractItemDelegate;
class AbstractProxyModel;
class QVBoxLayout;

namespace caret {
    class CaretFileDialog;

    class CaretFileDialogExtendable : public QDialog {
        
        Q_OBJECT

    public:
        CaretFileDialogExtendable(QWidget* parent,
                                  Qt::WindowFlags f);
        
        CaretFileDialogExtendable(QWidget* parent = 0,
                                  const QString& caption = QString(),
                                  const QString& directory = QString(),
                                  const QString& filter = QString());
        
        virtual ~CaretFileDialogExtendable();
        
        void addWidget(QWidget* widget);
        
        QFileDialog* getQFileDialog();
        
        const QFileDialog* getQFileDialog() const;
        
        QFileDialog::AcceptMode acceptMode() const;
        
        bool confirmOverwrite() const;
        
        QString defaultSuffix() const;
        
        QDir directory() const;
        
        QFileDialog::FileMode fileMode() const;
        
        QStringList filters() const;
        
        QStringList history() const;
        
        QFileIconProvider* iconProvider() const;
        
        bool isReadOnly() const;
        
        QAbstractItemDelegate* itemDelegate() const;
        
        QString labelText(const QFileDialog::DialogLabel label) const;
        
        QStringList nameFilters() const;

        void open(QObject* receiver, const char* member);
        
        QFileDialog::Options options() const;
        
        QAbstractProxyModel* proxyModel() const;
        
        bool resolveSymlinks() const;
        
        bool restoreState(const QByteArray& state);
        
        QByteArray saveState() const;
        
        void selectFile(const QString& name);
        
        void selectNameFilter(const QString& filter);
        
        QStringList selectedFiles() const;
        
        QString selectedNameFilter() const;
        
        void setAcceptMode(const QFileDialog::AcceptMode mode);
        
        void setConfirmOverwrite(const bool enabled);
        
        void setDefaultSuffix(const QString& suffix);
        
        void setDirectory(const QString& dir);
        
        void setDirectory(const QDir& dir);
        
        void setFileMode(const QFileDialog::FileMode mode);
        
        void setFilter(const QString& filter);
        
        void setHistory(const QStringList& paths);
        
        void setIconProvider(QFileIconProvider* provider);
        
        void setItemDelegate(AbstractItemDelegate* delegate);
        
        void setLabelText(const QFileDialog::DialogLabel label, const QString& text);
        
        void setNameFilter(const QString& filter);
        
        void setNameFilterDetailsVisible(bool enabled);
        
        void setNameFilters(const QStringList& filters);
        
        void setOption(QFileDialog::Option option, bool on = true);
        
        void setOptions(QFileDialog::Options options);
        
        void setProxyModel(AbstractProxyModel* proxyModel);
        
        void setReadOnly(const bool enabled);
        
        void setResolveSymlinks(bool enabled);
        
        void setSidebarUrls(const QList<QUrl>& urls);
        
        void setViewMode(const QFileDialog::ViewMode viewMode);
        
        QList<QUrl> sidebarUrls() const;
        
        bool testOption(QFileDialog::Option option) const;
        
        QFileDialog::ViewMode viewMode() const;
        
    signals:
        void currentChanged(const QString& path);
        
        void directoryEntered(const QString& directory);
        
        void fileSelected(const QString& file);
        
        void filesSelected(const QStringList& selected);
        
        void filterSelected(const QString& filter);
        
    private:
        CaretFileDialogExtendable(const CaretFileDialogExtendable&);

        CaretFileDialogExtendable& operator=(const CaretFileDialogExtendable&);

        void createDialog();
        
        QVBoxLayout* m_dialogLayout;
        
        CaretFileDialog* m_caretFileDialog;
    };
    
#ifdef __CARET_FILE_DIALOG_EXTENDABLE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_FILE_DIALOG_EXTENDABLE_DECLARE__

} // namespace
#endif  //__CARET_FILE_DIALOG_EXTENDABLE__H_
