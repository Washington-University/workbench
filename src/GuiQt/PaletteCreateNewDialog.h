#ifndef __PALETTE_CREATE_NEW_DIALOG_H__
#define __PALETTE_CREATE_NEW_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#include "PalettePixmapPainter.h"
#include "WuQDialogModal.h"

class QAbstractButton;
class QRadioButton;
class QSpinBox;

namespace caret {

    class PaletteNew;
    class PaletteSelectionWidget;
    
    class PaletteCreateNewDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        PaletteCreateNewDialog(const PalettePixmapPainter::Mode pixmapMode,
                               QWidget* parent = 0);
        
        virtual ~PaletteCreateNewDialog();
        
        PaletteCreateNewDialog(const PaletteCreateNewDialog&) = delete;

        PaletteCreateNewDialog& operator=(const PaletteCreateNewDialog&) = delete;
        
        std::unique_ptr<PaletteNew> getPalette();
        
        static PaletteNew* createPaletteNew(const AString& name,
                                            const int32_t numberOfPositiveControlPoints,
                                            const int32_t numberOfNegativeControlPoints);

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private slots:
        void paletteSelected();
        
        void typeButtonClicked(QAbstractButton* button);
        
    private:
        const PalettePixmapPainter::Mode m_pixmapMode;
        
        PaletteSelectionWidget* m_paletteSelectionWidget;

        QRadioButton* m_copyPaletteRadioButton;
        
        QRadioButton* m_newPaletteRadioButton;
        
        QSpinBox* m_newPalettePositiveSpinBox;
        
        QSpinBox* m_newPaletteNegativeSpinBox;
        
        std::unique_ptr<PaletteNew> m_palette;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_CREATE_NEW_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_CREATE_NEW_DIALOG_DECLARE__

} // namespace
#endif  //__PALETTE_CREATE_NEW_DIALOG_H__
