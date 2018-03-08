#ifndef __COPY_PALETTE_COLOR_MAPPING_TO_FILES_DIALOG_H__
#define __COPY_PALETTE_COLOR_MAPPING_TO_FILES_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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



#include <memory>
#include <set>

#include "WuQDialogModal.h"

class QCheckBox;

namespace caret {

    class CaretMappableDataFile;
    class PaletteColorMapping;
    
    class CopyPaletteColorMappingToFilesDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static bool run(CaretMappableDataFile* selectedMapFile,
                        const PaletteColorMapping* selectedPaletteColorMapping,
                        QWidget* parent,
                        AString& errorMessageOut);
        
        virtual ~CopyPaletteColorMappingToFilesDialog();

    protected:
        virtual void okButtonClicked() override;

        // ADD_NEW_METHODS_HERE

    private slots:
        void allCheckBoxesOnClicked();
        
        void allCheckBoxesOffClicked();
        
    private:
        CopyPaletteColorMappingToFilesDialog(const std::vector<CaretMappableDataFile*>& allPaletteMapFiles,
                                             CaretMappableDataFile* selectedMapFile,
                                             const PaletteColorMapping* selectedPaletteColorMapping,
                                             QWidget* parent);
        
        CopyPaletteColorMappingToFilesDialog(const CopyPaletteColorMappingToFilesDialog&);

        CopyPaletteColorMappingToFilesDialog& operator=(const CopyPaletteColorMappingToFilesDialog&);
        
        const CaretMappableDataFile* m_selectedMapFile;
        
        const PaletteColorMapping* m_selectedPaletteColorMapping;
        
        std::vector<std::pair<QCheckBox*, CaretMappableDataFile*>> m_fileCheckBoxes;
        
        static std::set<CaretMappableDataFile*> s_previousCheckedFiles;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __COPY_PALETTE_COLOR_MAPPING_TO_FILES_DIALOG_DECLARE__
    std::set<CaretMappableDataFile*> CopyPaletteColorMappingToFilesDialog::s_previousCheckedFiles;
#endif // __COPY_PALETTE_COLOR_MAPPING_TO_FILES_DIALOG_DECLARE__

} // namespace
#endif  //__COPY_PALETTE_COLOR_MAPPING_TO_FILES_DIALOG_H__
