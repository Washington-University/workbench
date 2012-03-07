#ifndef __CLASS_AND_NAME_HIERARCHY_MODEL_H_
#define __CLASS_AND_NAME_HIERARCHY_MODEL_H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <map>

#include "CaretObject.h"


namespace caret {

    class BorderFile;
    class GiftiLabelTable;
    
    class ClassAndNameHierarchyModel : public CaretObject {
        
    public:
        ClassAndNameHierarchyModel();
        
        virtual ~ClassAndNameHierarchyModel();
        
        void clear();
        
        void removeUnusedNamesAndClasses(BorderFile* borderFile);
        
        GiftiLabelTable* getClassLabelTable();
        
        const GiftiLabelTable* getClassLabelTable() const;
        
        GiftiLabelTable* getNameLabelTableForClass(const int32_t classKey);
        
        const GiftiLabelTable* getNameLabelTableForClass(const int32_t classKey) const;
        
        bool isClassValid(const int32_t classKey) const;
        
        void setAllSelected(const bool status);
        
        void update(BorderFile* borderFile);
        
        AString toString() const;
        
        AString getName() const;
        
        bool isSelected() const;
        
        void setSelected(const bool selectionStatus);
        
    private:
        ClassAndNameHierarchyModel(const ClassAndNameHierarchyModel&);

        ClassAndNameHierarchyModel& operator=(const ClassAndNameHierarchyModel&);
        
        void clearPrivate(const bool isDestruction);
        
        GiftiLabelTable* classLabelTable;
        
        /**
         * Maps a class key to a label table.
         * The label table contains names that are members of the class.
         * Using this map allows an identical name to be a member of multiple classes
         * but keeps the selection status for each name independent. 
         */
        std::map<int32_t, GiftiLabelTable*> classKeyToChildNamesMap;
        
        /** Name of model, does NOT get cleared. */
        AString name;
        
        /* overlay selection status */
        bool selectionStatus;
    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_MODEL_H_
