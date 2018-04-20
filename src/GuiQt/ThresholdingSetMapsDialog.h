#ifndef __THRESHOLDING_SET_MAPS_DIALOG_H__
#define __THRESHOLDING_SET_MAPS_DIALOG_H__

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

#include "WuQDialogModal.h"

class QRadioButton;

namespace caret {

    class CaretMappableDataFile;
    
    class ThresholdingSetMapsDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        ThresholdingSetMapsDialog(CaretMappableDataFile* dataFile,
                                  CaretMappableDataFile* thresholdFile,
                                  const int32_t thresholdFileMapIndex,
                                  QWidget* parent = 0);
        
        virtual ~ThresholdingSetMapsDialog();
        
        ThresholdingSetMapsDialog(const ThresholdingSetMapsDialog&) = delete;

        ThresholdingSetMapsDialog& operator=(const ThresholdingSetMapsDialog&) = delete;
        

        // ADD_NEW_METHODS_HERE

    public slots:
        virtual void done(int resultCode) override;
        
    private:
        CaretMappableDataFile* m_dataFile;
        
        CaretMappableDataFile* m_thresholdFile;
        
        const int32_t m_thresholdFileMapIndex;
        
        QRadioButton* m_setSameIndexRadioButton;
        
        QRadioButton* m_setOneToEnRadioButton;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __THRESHOLDING_SET_MAPS_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __THRESHOLDING_SET_MAPS_DIALOG_DECLARE__

} // namespace
#endif  //__THRESHOLDING_SET_MAPS_DIALOG_H__
