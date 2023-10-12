#ifndef __SAMPLES_META_DATA_MANAGER_H__
#define __SAMPLES_META_DATA_MANAGER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include "CaretObject.h"



namespace caret {
    class DingOntologyTermsFile;

    class SamplesMetaDataManager : public CaretObject {
        
    public:
        SamplesMetaDataManager();
        
        virtual ~SamplesMetaDataManager();
        
        SamplesMetaDataManager(const SamplesMetaDataManager&) = delete;

        SamplesMetaDataManager& operator=(const SamplesMetaDataManager&) = delete;

        const DingOntologyTermsFile* getDingOntologyTermsFile() const;

        // ADD_NEW_METHODS_HERE

    private:
        void readDingOntologyTermsFile() const;
        
        mutable std::unique_ptr<DingOntologyTermsFile> m_dingOntologyTermsFile;
        
        mutable bool m_attemptedToReadDingOntologyTermsFileFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SAMPLES_META_DATA_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SAMPLES_META_DATA_MANAGER_DECLARE__

} // namespace
#endif  //__SAMPLES_META_DATA_MANAGER_H__
