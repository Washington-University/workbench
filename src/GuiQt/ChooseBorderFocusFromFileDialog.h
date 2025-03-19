#ifndef __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_H__
#define __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#include <memory>

#include "DataFileTypeEnum.h"
#include "FunctionResult.h"

class QTreeView;

namespace caret {

    class BorderFile;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class DataFileEditorItem;
    class FociFile;
    
    class ChooseBorderFocusFromFileDialog : public QDialog {
        
        Q_OBJECT

    public:
        enum class FileMode {
            BORDER,
            FOCUS
        };
        
        class FilesSelections {
        public:
            FilesSelections(const AString& filename,
                            const AString& name,
                            const AString& className,
                            const AString& identifier)
        :   m_filename(filename),
            m_name(name),
            m_className(className),
            m_identifier(identifier)
            { }
            
            AString getFileName()  const { return m_filename; }
            AString getName()      const { return m_name; }
            AString getClassName() const { return m_className; }
            AString getIdentifer() const { return m_identifier; }

        private:
            AString m_filename;
            AString m_name;
            AString m_className;
            AString m_identifier;
        };
        
        ChooseBorderFocusFromFileDialog(const FileMode fileMode,
                                        QWidget* parent = 0);
        
        virtual ~ChooseBorderFocusFromFileDialog();
        
        ChooseBorderFocusFromFileDialog(const ChooseBorderFocusFromFileDialog&) = delete;

        ChooseBorderFocusFromFileDialog& operator=(const ChooseBorderFocusFromFileDialog&) = delete;
        
        void setSelections(const AString& filename,
                           const AString& className,
                           const AString& name);
        
        FilesSelections getFileSelections() const;
        
        virtual void done(int result) override;
        
        virtual QSize sizeHint() const override;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void dataFileSelected(CaretDataFile* caretDataFile);
        
    private:
        const DataFileEditorItem* getSelectedItem() const;
        
        const FileMode m_fileMode;
        
        DataFileTypeEnum::Enum m_dataFileType = DataFileTypeEnum::UNKNOWN;
        
        CaretDataFileSelectionModel* m_fileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_fileSelectionComboBox;
        
        QTreeView* m_treeView;
        
        BorderFile* m_borderFile = NULL;
        
        FociFile* m_fociFile = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_H__
