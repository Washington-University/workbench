#ifndef __CHARTABLE_MATRIX_PARCEL_REORDER_INTERFACE_H__
#define __CHARTABLE_MATRIX_PARCEL_REORDER_INTERFACE_H__

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

#include "ChartableMatrixInterface.h"

namespace caret {
    
    class CiftiParcelLabelFile;
    class CiftiParcelReordering;
    
    class ChartableMatrixParcelInterface : public ChartableMatrixInterface {
        
    protected:
        ChartableMatrixParcelInterface() { }
        
        virtual ~ChartableMatrixParcelInterface() { }
        
    public:
        /**
         * Get the selected parcel label file used for reordering of parcels.
         *
         * @param compatibleParcelLabelFilesOut
         *    All Parcel Label files that are compatible with file implementing
         *    this interface.
         * @param selectedParcelLabelFileOut
         *    The selected parcel label file used for reordering the parcels.
         *    May be NULL!
         * @param selectedParcelLabelFileMapIndexOut
         *    Map index in the selected parcel label file.
         * @param enabledStatusOut
         *    Enabled status of reordering.
         */
        virtual void getSelectedParcelLabelFileAndMapForReordering(std::vector<CiftiParcelLabelFile*>& compatibleParcelLabelFilesOut,
                                                                   CiftiParcelLabelFile* &selectedParcelLabelFileOut,
                                                 int32_t& selectedParcelLabelFileMapIndexOut,
                                                 bool& enabledStatusOut) const = 0;
        
        /**
         * Set the selected parcel label file used for reordering of parcels.
         *
         * @param selectedParcelLabelFile
         *    The selected parcel label file used for reordering the parcels.
         *    May be NULL!
         * @param selectedParcelLabelFileMapIndex
         *    Map index in the selected parcel label file.
         * @param enabledStatus
         *    Enabled status of reordering.
         */
        virtual void setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
                                                                   const int32_t selectedParcelLabelFileMapIndex,
                                                                   const bool enabledStatus) = 0;
        
        /**
         * Create the parcel reordering for the given map index using
         * the given parcel label file and its map index.
         *
         * @param parcelLabelFile
         *    The selected parcel label file used for reordering the parcels.
         * @param parcelLabelFileMapIndex
         *    Map index in the selected parcel label file.
         * @param errorMessageOut
         *    Error message output.  Will only be non-empty if NULL is returned.
         * @return
         *    Pointer to parcel reordering or NULL if not found.
         */
        virtual bool createParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                    const int32_t parcelLabelFileMapIndex,
                                    AString& errorMessageOut) = 0;
        
        /**
         * Get the parcel reordering for the given map index that was created using
         * the given parcel label file and its map index.
         *
         * @param parcelLabelFile
         *    The selected parcel label file used for reordering the parcels.
         * @param parcelLabelFileMapIndex
         *    Map index in the selected parcel label file.
         * @return
         *    Pointer to parcel reordering or NULL if not found.
         */
        virtual const CiftiParcelReordering* getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                                         const int32_t parcelLabelFileMapIndex) const = 0;
        
        /**
         * @return Coloring mode for selected parcel.
         */
        virtual CiftiParcelColoringModeEnum::Enum getSelectedParcelColoringMode() const = 0;
        
        /**
         * Set the coloring mode for selected parcel.
         *
         * @param coloringMode
         *    New value for coloring mode.
         */
        virtual void setSelectedParcelColoringMode(const CiftiParcelColoringModeEnum::Enum coloringMode) = 0;
        
        /**
         * @return Color for selected parcel.
         */
        virtual CaretColorEnum::Enum getSelectedParcelColor() const = 0;
        
        /**
         * @return True if loading attributes (column/row, yoking) are
         * supported by this file type.
         */
        virtual bool isSupportsLoadingAttributes() = 0;
        
        /**
         * @return The matrix loading type (by row/column).
         */
        virtual ChartMatrixLoadingDimensionEnum::Enum getMatrixLoadingDimension() const = 0;
        
        /**
         * Set the matrix loading type (by row/column).
         *
         * @param matrixLoadingType
         *    New value for matrix loading type.
         */
        virtual void setMatrixLoadingDimension(const ChartMatrixLoadingDimensionEnum::Enum matrixLoadingType) = 0;
        
        /**
         * Set color for selected parcel.
         *
         * @param color
         *    New color for selected parcel.
         */
        virtual void setSelectedParcelColor(const CaretColorEnum::Enum color) = 0;
        
        /**
         * @return Selected yoking group.
         */
        virtual YokingGroupEnum::Enum getYokingGroup() const = 0;
        
        /**
         * Set the selected yoking group.
         *
         * @param yokingGroup
         *    New value for yoking group.
         */
        virtual void setYokingGroup(const YokingGroupEnum::Enum yokingType) = 0;
        
    private:
        ChartableMatrixParcelInterface(const ChartableMatrixParcelInterface&);
        
        ChartableMatrixParcelInterface& operator=(const ChartableMatrixParcelInterface&);
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __CHARTABLE_MATRIX_INTERFACE_PARCEL_REORDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_MATRIX_INTERFACE_PARCEL_REORDER_DECLARE__
    
} // namespace
#endif  //__CHARTABLE_MATRIX_PARCEL_REORDER_INTERFACE_H__
