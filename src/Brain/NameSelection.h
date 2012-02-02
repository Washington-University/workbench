#ifndef __NAME_SELECTION__H_
#define __NAME_SELECTION__H_

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


#include "CaretObject.h"

namespace caret {

    class NameSelection : public CaretObject {
        
    public:
        NameSelection(const AString& name);
        
        virtual ~NameSelection();
        
    private:
        NameSelection(const NameSelection&);

        NameSelection& operator=(const NameSelection&);
        
        /** 
         * @return Name of item 
         */
        inline AString getName() const { return this->name; }
        
        /** 
         * @return selection status.
         */
        inline bool isSelected() const { return this->selected; }
        
        /** 
         * Set selection status.
         * @param selected 
         *    New selection status.
         */
        inline void setSelected(const bool selected) { this->selected = selected; }
        
    public:
        virtual AString toString() const;
        
    private:
        /** 
         * Set name of item
         * @param name 
         *    New name.
         */
        inline void setName(const AString& name) { this->name = name; }
        
        /** 
         * @return Count associated with name. 
         */
        inline int32_t getCount() const { return this->count; }
        
        /** 
         * Set count associated with name
         * @param count 
         *    New value.
         */
        inline void setCount(const int32_t count) { this->count = count; }
        
        /**
         * Increment the count associated with the name.
         */
        inline void incrementCount() { this->count++; }
        
        AString name;
        
        bool selected;
        
        int32_t count;
    };
    
#ifdef __NAME_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NAME_SELECTION_DECLARE__

} // namespace
#endif  //__NAME_SELECTION__H_
