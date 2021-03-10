#ifndef __CARET_FILE_DIALOG_EXTENDABLE__H_
#define __CARET_FILE_DIALOG_EXTENDABLE__H_

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


#include <QDialog>
#include <QUrl>

#include "CaretFileDialog.h"

class QVBoxLayout;

namespace caret {
    
    class CaretFileDialogPrivate : public CaretFileDialog {
        Q_OBJECT
    public:
        CaretFileDialogPrivate(QWidget* parent,
                               Qt::WindowFlags f);
        
        CaretFileDialogPrivate(QWidget* parent = 0,
                               const QString& caption = QString(),
                               const QString& directory = QString(),
                               const QString& filter = QString());
        
        virtual ~CaretFileDialogPrivate();

    protected:  
        virtual void done(int result);
        
        virtual void closeEvent(QCloseEvent* event);
    };
    
    
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
        
        QFileDialog::AcceptMode acceptMode() const { return m_caretFileDialog->acceptMode(); }
        
        bool confirmOverwrite() const {
            //return m_caretFileDialog->confirmOverwrite();
            return ( ! m_caretFileDialog->testOption(QFileDialog::DontConfirmOverwrite));
        }
        
        QString defaultSuffix() const { return m_caretFileDialog->defaultSuffix(); }
        
        QDir directory() const { return m_caretFileDialog->directory(); }
        
        QFileDialog::FileMode fileMode() const { return m_caretFileDialog->fileMode(); }
        
        QStringList filters() const { return m_caretFileDialog->nameFilters(); }
        
        QStringList history() const { return m_caretFileDialog->history(); }
        
        QFileIconProvider* iconProvider() const { return m_caretFileDialog->iconProvider(); }
        
        bool isReadOnly() const {
            //return m_caretFileDialog->isReadOnly();
            return m_caretFileDialog->testOption(QFileDialog::ReadOnly);
        }
        
        QAbstractItemDelegate* itemDelegate() const { return m_caretFileDialog->itemDelegate(); }
        
        QString labelText(const QFileDialog::DialogLabel label) const { return m_caretFileDialog->labelText(label); }
        
        QStringList nameFilters() const { return m_caretFileDialog->nameFilters(); }

        //void open(QObject* receiver, const char* member) { m_caretFileDialog->open(receiver, member); }
        
        QFileDialog::Options options() const { return m_caretFileDialog->options(); }
        
        QAbstractProxyModel* proxyModel() const { return m_caretFileDialog->proxyModel(); }
        
        bool resolveSymlinks() const {
            //return m_caretFileDialog->resolveSymlinks();
            return ( ! m_caretFileDialog->testOption(QFileDialog::DontResolveSymlinks));

        }
        
        bool restoreState(const QByteArray& state) { return m_caretFileDialog->restoreState(state); }
        
        QByteArray saveState() const { return m_caretFileDialog->saveState(); }
        
        void selectFile(const QString& name) { m_caretFileDialog->selectFile(name); }
        
        void selectNameFilter(const QString& filter) { m_caretFileDialog->selectNameFilter(filter); }
        
        QStringList selectedFiles() const { return m_caretFileDialog->selectedFiles(); }
        
        QString selectedNameFilter() const { return m_caretFileDialog->selectedNameFilter(); }
        
        void setAcceptMode(const QFileDialog::AcceptMode mode) { m_caretFileDialog->setAcceptMode(mode); }
        
        void setConfirmOverwrite(const bool enabled) {
            //m_caretFileDialog->setConfirmOverwrite(enabled);
            m_caretFileDialog->setOption(QFileDialog::DontConfirmOverwrite,
                                         ( ! enabled));
        }
        
        void setDefaultSuffix(const QString& suffix) { m_caretFileDialog->setDefaultSuffix(suffix); }
        
        void setDirectory(const QString& dir) { m_caretFileDialog->setDirectory(dir); }
        
        void setDirectory(const QDir& dir) { m_caretFileDialog->setDirectory(dir); }
        
        void setFileMode(const QFileDialog::FileMode mode) { m_caretFileDialog->setFileMode(mode); }
        
//        void setFilter(const QString& filter) { m_caretFileDialog->setNameFilter(filter); }
        
        void setHistory(const QStringList& paths) { m_caretFileDialog->setHistory(paths); }
        
        void setIconProvider(QFileIconProvider* provider) { m_caretFileDialog->setIconProvider(provider); }
        
        void setItemDelegate(QAbstractItemDelegate* delegate) { m_caretFileDialog->setItemDelegate(delegate); }
        
        void setLabelText(const QFileDialog::DialogLabel label, const QString& text) { m_caretFileDialog->setLabelText(label, text); }
        
        void setNameFilter(const QString& filter) { m_caretFileDialog->setNameFilter(filter); }
        
        void setNameFilterDetailsVisible(bool enabled) {
            //m_caretFileDialog->setNameFilterDetailsVisible(enabled);
            m_caretFileDialog->setOption(QFileDialog::HideNameFilterDetails,
                                         ( ! enabled));
        }
        
        void setNameFilters(const QStringList& filters) { m_caretFileDialog->setNameFilters(filters); }
        
        void setOption(QFileDialog::Option option, bool on = true) { m_caretFileDialog->setOption(option, on); }
        
        void setOptions(QFileDialog::Options options) { m_caretFileDialog->setOptions(options); }
        
        void setProxyModel(QAbstractProxyModel* proxyModel) { m_caretFileDialog->setProxyModel(proxyModel); }
        
        void setReadOnly(const bool enabled) {
            //m_caretFileDialog->setReadOnly(enabled);
            m_caretFileDialog->setOption(QFileDialog::ReadOnly,
                                         enabled);
        }
        
        void setResolveSymlinks(bool enabled) {
            //m_caretFileDialog->setResolveSymlinks(enabled);
            m_caretFileDialog->setOption(QFileDialog::DontResolveSymlinks,
                                         ( ! enabled));
        }
        
        void setSidebarUrls(const QList<QUrl>& urls) { m_caretFileDialog->setSidebarUrls(urls); }
        
        void setViewMode(const QFileDialog::ViewMode viewMode) { m_caretFileDialog->setViewMode(viewMode); }
        
        QList<QUrl> sidebarUrls() const { return m_caretFileDialog->sidebarUrls(); }
        
        bool testOption(QFileDialog::Option option) const { return m_caretFileDialog->testOption(option); }
        
        QFileDialog::ViewMode viewMode() const { return m_caretFileDialog->viewMode(); }
        
    signals:
        void currentChanged(const QString& path);
        
        void directoryEntered(const QString& directory);
        
        void fileSelected(const QString& file);
        
        void filesSelected(const QStringList& selected);
        
        void filterSelected(const QString& filter);
        
    private slots:
        void fileDialogFinished(int);
        
    private:
        CaretFileDialogExtendable(const CaretFileDialogExtendable&);

        CaretFileDialogExtendable& operator=(const CaretFileDialogExtendable&);

        void createDialog();
        
        QVBoxLayout* m_dialogLayout;
        
        CaretFileDialogPrivate* m_caretFileDialog;
    };
    
#ifdef __CARET_FILE_DIALOG_EXTENDABLE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_FILE_DIALOG_EXTENDABLE_DECLARE__

} // namespace
#endif  //__CARET_FILE_DIALOG_EXTENDABLE__H_
