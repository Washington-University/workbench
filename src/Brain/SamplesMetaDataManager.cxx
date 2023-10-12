
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

#define __SAMPLES_META_DATA_MANAGER_DECLARE__
#include "SamplesMetaDataManager.h"
#undef __SAMPLES_META_DATA_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "DingOntologyTermsFile.h"

using namespace caret;


    
/**
 * \class caret::SamplesMetaDataManager 
 * \brief Manages and provides access to metadata for samples
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SamplesMetaDataManager::SamplesMetaDataManager()
: CaretObject()
{
}

/**
 * Destructor.
 */
SamplesMetaDataManager::~SamplesMetaDataManager()
{
}

/**
 * @return The ding ontology terms file (may be NULL if read failure)
 */
const DingOntologyTermsFile*
SamplesMetaDataManager::getDingOntologyTermsFile() const
{
    if ( ! m_attemptedToReadDingOntologyTermsFileFlag) {
        m_attemptedToReadDingOntologyTermsFileFlag = true;
        readDingOntologyTermsFile();
    }
    return m_dingOntologyTermsFile.get();
}


/**
 * Read the Ding Ontology Terms
 */
void
SamplesMetaDataManager::readDingOntologyTermsFile() const
{
    try {
        m_dingOntologyTermsFile.reset(new DingOntologyTermsFile());
        m_dingOntologyTermsFile->readFile(DingOntologyTermsFile::getDingOntologyResourcesFileName());
    }
    catch (const DataFileException& dfe) {
        m_dingOntologyTermsFile.reset();
        CaretLogSevere("DING ONTOLOGY TERMS: "
                       + dfe.whatString());
    }
}
