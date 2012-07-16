#ifndef __SPEC_FILE_DIALOG__H_
#define __SPEC_FILE_DIALOG__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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


#include "DataFileTypeEnum.h"
#include "SpecFileDialog.h"
#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QAction;
class QCheckBox;
class QLabel;
class QPushButton;
class QToolBar;
class QToolButton;

namespace caret {
    class GuiSpecDataFileInfo;
    class SpecFileDataFile;
    class SpecFileDataFileTypeGroup;
    class SpecFile;
    class StructureEnumComboBox;
    class WuQEventBlockingFilter;
    class WuQWidgetObjectGroup;
    
    class SpecFileDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        static SpecFileDialog* createForLoadingSpecFile(SpecFile* specFile,
                                                QWidget* parent);
        
        static void displayFastOpenDataFile(SpecFile* specFile,
                                                    QWidget* parent);
    private:
        enum Mode {
            MODE_FAST_OPEN,
            MODE_LOAD_SPEC
        };
        
        SpecFileDialog(const Mode mode,
                       SpecFile* specFile,
                       QWidget* parent);
    public:
        virtual ~SpecFileDialog();

    protected:
        virtual void okButtonClicked();
        
        virtual void cancelButtonClicked();
        
    private slots:
        void toolBarButtonTriggered(QAction*);
        void selectToolButtonTriggered(QAction*);
        void fastOpenDataFileWasLoaded();
        
    private:
        class GuiSpecGroup {
        public:
            QWidget* widget;
            std::vector<GuiSpecDataFileInfo*> dataFiles;
        };
        SpecFileDialog(const SpecFileDialog&);

        SpecFileDialog& operator=(const SpecFileDialog&);
        
        GuiSpecGroup* createDataTypeGroup(const DataFileTypeEnum::Enum dataFileType,
                                          std::vector<SpecFileDataFile*>& dataFileInfoVector,
                                          const AString& groupName);
        
        void writeUpdatedSpecFile(const bool confirmIt);
        
        QToolBar* createSelectToolBar();
        
    public:
        virtual AString toString() const;
        
    private:
        SpecFile* m_specFile;
        
        QCheckBox* m_autoCloseFastOpenCheckBox;
        
        Mode m_mode;
        
        std::vector<GuiSpecGroup*> m_dataTypeGroups;
        
        QAction* m_selectAllFilesToolButtonAction;
        QAction* m_selectNoneFilesToolButtonAction;
        
        WuQEventBlockingFilter* m_comboBoxWheelEventBlockingFilter;
        
        friend class GuiSpecDataFileInfo;
    };
 
    /// Info about data file
    class GuiSpecDataFileInfo : public QObject {
        Q_OBJECT
    public:
        GuiSpecDataFileInfo(QObject* parent,
                            const AString& specFilePath,
                            SpecFileDataFile* dataFileInfo,
                            const bool isStructureFile);

        ~GuiSpecDataFileInfo();

    signals:
        void signalFileWasLoaded();
        
    private slots:
        void structureSelectionChanged(const StructureEnum::Enum);
        void metadataActionTriggered();
        void removeActionTriggered(bool);
        void openFilePushButtonClicked();
        
    private:
        GuiSpecDataFileInfo(const GuiSpecDataFileInfo&);
        GuiSpecDataFileInfo& operator=(const GuiSpecDataFileInfo&);
        
        SpecFileDataFile* m_dataFileInfo;
        QPushButton*      m_openFilePushButton;
        QCheckBox*        m_selectionCheckBox;
        QAction*          m_metadataAction;
        QToolButton*      m_metadataToolButton;
        QAction*          m_removeAction;
        QToolButton*      m_removeToolButton;
        StructureEnumComboBox* m_structureEnumComboBox;
        QLabel*           m_nameLabel;
        WuQWidgetObjectGroup* m_widgetGroup;
        
        friend class SpecFileDialog;
    };
    

#ifdef __SPEC_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPEC_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__SPEC_FILE_DIALOG__H_
