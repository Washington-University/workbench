#ifndef __DATA_FILE_CONTENT_COPY_MOVE_DIALOG_H__
#define __DATA_FILE_CONTENT_COPY_MOVE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


#include "WuQDialogModal.h"

class QButtonGroup;
class QCheckBox;
class QLabel;
class QRadioButton;

namespace caret {

    class DataFileContentCopyMoveInterface;
    
    class DataFileContentCopyMoveDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        /**
         * Options for dialog.
         */
        class Options {
        public:
            Options() { }
            
            /**
             * Set input file is CZI annotations
             */
            void setCziAnnotationFile(const bool status) {
                m_cziAnnotationFileFlag = status;
            }
            
            /**
             * @return True if input file is CZI annotations
             */
            bool isCziAnnotationFile() const {
                return m_cziAnnotationFileFlag;
            }
        private:
            bool m_cziAnnotationFileFlag = false;
        };
        
        DataFileContentCopyMoveDialog(const int32_t windowIndex,
                                      DataFileContentCopyMoveInterface* sourceDataFileInterface,
                                      std::vector<DataFileContentCopyMoveInterface*>& dataFilesInterface,
                                      const Options& options,
                                      QWidget* parent);
        
        virtual ~DataFileContentCopyMoveDialog();
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void newDestinationFileToolButtonClicked();
        
        void closeSourceFileCheckBoxToggled(bool);
        
    private:
        DataFileContentCopyMoveDialog(const DataFileContentCopyMoveDialog&);

        DataFileContentCopyMoveDialog& operator=(const DataFileContentCopyMoveDialog&);
        
        virtual void okButtonClicked();
        
        QWidget* createOptionsWidget();
        
        QWidget* createSourceWidget();
        
        QWidget* createDestinationWidget();
        
        const int32_t m_windowIndex;
        
        DataFileContentCopyMoveInterface* m_sourceDataFileInterface;
        
        std::vector<DataFileContentCopyMoveInterface*> m_destinationDataFileInterfaces;
        
        const Options& m_options;
        
        QButtonGroup* m_destinationButtonGroup;
        
        QLabel* m_newDestinationFileNameLabel;
        
        int m_newDestinatonFileButtonGroupIndex;
        
        QString m_newDestinationFileName;
        
        QRadioButton* m_newDestinationFileRadioButton;
        
        QCheckBox* m_closeSourceFileCheckBox;
        
        QCheckBox* m_copySelectedAnnotationsOnlyCheckBox;
        
        AString m_cziImageFileName;
        
        AString m_cziAnnotationFileName;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_CONTENT_COPY_MOVE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_CONTENT_COPY_MOVE_DIALOG_DECLARE__

} // namespace
#endif  //__DATA_FILE_CONTENT_COPY_MOVE_DIALOG_H__
