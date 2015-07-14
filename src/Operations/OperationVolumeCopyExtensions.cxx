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

#include "OperationVolumeCopyExtensions.h"
#include "OperationException.h"

#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "NiftiHeader.h" //for NiftiExtension
#include "PaletteColorMapping.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationVolumeCopyExtensions::getCommandSwitch()
{
    return "-volume-copy-extensions";
}

AString OperationVolumeCopyExtensions::getShortDescription()
{
    return "COPY EXTENDED DATA TO ANOTHER VOLUME FILE";
}

OperationParameters* OperationVolumeCopyExtensions::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "data-volume", "the volume file containing the voxel data to use");
    
    ret->addVolumeParameter(2, "extension-volume", "the volume file containing the extensions to use");
    
    ret->addVolumeOutputParameter(3, "volume-out", "the output volume");
    
    ret->createOptionalParameter(4, "-drop-unknown", "don't copy extensions that workbench doesn't understand");
    
    ret->setHelpText(
        AString("This command copies the information in a volume file that isn't a critical part of the standard header or data matrix, ") +
        "e.g. map names, palette settings, label tables.  " +
        "If -drop-unknown is not specified, it also copies similar kinds of information set by other software."
    );
    return ret;
}

void OperationVolumeCopyExtensions::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* dataVol = myParams->getVolume(1);
    VolumeFile* extVol = myParams->getVolume(2);
    VolumeFile* outVol = myParams->getOutputVolume(3);
    bool dropUnknown = myParams->getOptionalParameter(4)->m_present;
    if (!dataVol->getVolumeSpace().matches(extVol->getVolumeSpace())) throw OperationException("volume spaces do not match");
    vector<int64_t> dataDims = dataVol->getDimensions();
    vector<int64_t> extDims = extVol->getDimensions();
    if (dataDims[4] != extDims[4]) throw OperationException("number of components (rgb or complex datatypes) does not match");
    if (dataVol->getOriginalDimensions() != extVol->getOriginalDimensions()) throw OperationException("non-spatial dimensions do not match");
    outVol->reinitialize(dataVol->getOriginalDimensions(), dataVol->getSform(), dataDims[4], extVol->getType());
    outVol->m_header.grabNew(extVol->m_header->clone());//copy all standard header fields, too
    if (dropUnknown)
    {
        switch (outVol->m_header->getType())
        {
            case AbstractHeader::NIFTI:
                ((NiftiHeader*)outVol->m_header.getPointer())->m_extensions.clear();
                break;
        }
    }
    if (outVol->getFileMetaData() != NULL)
    {
        *(outVol->getFileMetaData()) = *(extVol->getFileMetaData());
    }
    for (int64_t c = 0; c < dataDims[4]; ++c)
    {
        for (int64_t b = 0; b < dataDims[3]; ++b)
        {
            if (c == 0)//map names, etc
            {
                outVol->setMapName(b, extVol->getMapName(b));
                *(outVol->getMapMetaData(b)) = *(extVol->getMapMetaData(b));
                if (extVol->getType() == SubvolumeAttributes::LABEL)
                {
                    *(outVol->getMapLabelTable(b)) = *(extVol->getMapLabelTable(b));
                } else {
                    *(outVol->getMapPaletteColorMapping(b)) = *(extVol->getMapPaletteColorMapping(b));
                }
            }
            outVol->setFrame(dataVol->getFrame(b, c), b, c);
        }
    }
}
