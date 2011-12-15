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
class QToolBar;
class QToolButton;

namespace caret {
    class GuiSpecDataFileInfo;
    class SpecFileDataFile;
    class SpecFileDataFileTypeGroup;
    class SpecFile;
    class StructureSelectionControl;
    class WuQWidgetObjectGroup;
    
    class SpecFileDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        SpecFileDialog(SpecFile* specFile,
                       QWidget* parent);
        
        virtual ~SpecFileDialog();

    protected:
        virtual void okButtonPressed();
        
        virtual void cancelButtonPressed();
        
    private slots:
        void toolBarButtonTriggered(QAction*);
        void selectToolButtonTriggered(QAction*);
        
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
        SpecFile* specFile;
        
        std::vector<GuiSpecGroup*> dataTypeGroups;
        
        QAction* selectAllFilesToolButtonAction;
        QAction* selectNoneFilesToolButtonAction;
    };
 
    /// Info about data file
    class GuiSpecDataFileInfo : public QObject {
        Q_OBJECT
    public:
        GuiSpecDataFileInfo(QObject* parent,
                         SpecFileDataFile* dataFileInfo,
                         const bool isStructureFile);

        ~GuiSpecDataFileInfo();

    private slots:
        void structureSelectionChanged(const StructureEnum::Enum);
        void metadataActionTriggered();
        void removeActionTriggered(bool);
        
    private:
        GuiSpecDataFileInfo(const GuiSpecDataFileInfo&);
        GuiSpecDataFileInfo& operator=(const GuiSpecDataFileInfo&);
        
        SpecFileDataFile* dataFileInfo;
        QCheckBox*        selectionCheckBox;
        QAction*          metadataAction;
        QToolButton*      metadataToolButton;
        QAction*          removeAction;
        QToolButton*      removeToolButton;
        StructureSelectionControl* structureSelectionControl;
        QLabel*           nameLabel;
        WuQWidgetObjectGroup* widgetGroup;
        
        friend class SpecFileDialog;
    };
    

#ifdef __SPEC_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPEC_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__SPEC_FILE_DIALOG__H_
