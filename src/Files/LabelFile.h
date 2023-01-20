
#ifndef __LABEL_FILE_H__
#define __LABEL_FILE_H__

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

#include <vector>
#include <stdint.h>

#include "GiftiTypeFile.h"
#include "GroupAndNameHierarchyUserInterface.h"

namespace caret {

    class GroupAndNameHierarchyModel;
    class GiftiDataArray;
    class GiftiLabelTable;
    
    /**
     * \brief A Label data file.
     */
    class LabelFile : public GiftiTypeFile, public GroupAndNameHierarchyUserInterface {
        
    public:
        LabelFile();
        
        LabelFile(const LabelFile& sf);
        
        LabelFile& operator=(const LabelFile& sf);
        
        virtual ~LabelFile();
        
        void clear();
        
        int32_t getNumberOfNodes() const;
        
        int32_t getNumberOfColumns() const;
        
        void setNumberOfNodesAndColumns(int32_t nodes, int32_t columns);

        virtual void addMaps(const int32_t numberOfNodes,
                             const int32_t numberOfMaps);
        
        GiftiLabelTable* getLabelTable();
        
        const GiftiLabelTable* getLabelTable() const;
        
        int32_t getLabelKey(const int32_t nodeIndex,
                            const int32_t columnIndex) const;
        
        AString getLabelName(const int32_t nodeIndex,
                             const int32_t columnIndex) const;
        
        void setLabelKey(const int32_t nodeIndex,
                         const int32_t columnIndex,
                         const int32_t labelIndex);
        
        void getNodeIndicesWithLabelKey(const int32_t columnIndex,
                                        const int32_t labelKey,
                                        std::vector<int32_t>& nodeIndicesOut) const;
        
        const int32_t* getLabelKeyPointerForColumn(const int32_t columnIndex) const;
        
        void setLabelKeysForColumn(const int32_t columnIndex, const int32_t* keysIn);
        
        std::vector<int32_t> getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const;
        
        GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel();
        
        const GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel() const;
        
        //override writeFile in order to check filename against type of file
        virtual void writeFile(const AString& filename);
        
        virtual void groupAndNameHierarchyItemStatusChanged() override;

    protected:
        /**
         * Validate the contents of the file after it
         * has been read such as correct number of 
         * data arrays and proper data types/dimensions.
         */
        virtual void validateDataArraysAfterReading();
        
        void copyHelperLabelFile(const LabelFile& sf);
        
        void initializeMembersLabelFile();
        
    private:
        void validateKeysAndLabels() const;
        
        /** Points to actual data in each Gifti Data Array */
        std::vector<int32_t*> columnDataPointers;

        /** Holds class and name hierarchy used for display selection */
        mutable GroupAndNameHierarchyModel* m_classNameHierarchy;
        
        /** force an update of the class and name hierarchy */
        mutable bool m_forceUpdateOfGroupAndNameHierarchy;
        
    };

} // namespace

#endif // __LABEL_FILE_H__
