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

#include "OperationVolumeCapturePlane.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "ImageFile.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <cmath>

using namespace caret;
using namespace std;

AString OperationVolumeCapturePlane::getCommandSwitch()
{
    return "-volume-capture-plane";
}

AString OperationVolumeCapturePlane::getShortDescription()
{
    return "INTERPOLATE IMAGE FROM PLANE THROUGH VOLUME";
}

OperationParameters* OperationVolumeCapturePlane::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume", "the volume file to interpolate from");
    
    ret->addStringParameter(2, "subvolume", "the name or number of the subvolume to use");
    
    ret->addStringParameter(3, "interp", "interpolation type");
    
    ret->addIntegerParameter(4, "h-dim", "width of output image, in pixels");
    
    ret->addIntegerParameter(5, "v-dim", "height of output image, in pixels");
    
    ret->addDoubleParameter(6, "scale-min", "value to render as black");
    
    ret->addDoubleParameter(7, "scale-max", "value to render as white");
    
    int key = 8;
    char xyz[] = "xyz";
    for (int i = 0; i < 3; ++i)
    {
        ret->addDoubleParameter(key, AString("bottom-left-") + xyz[i], xyz[i] + AString("-coordinate of the bottom left of the output image"));
        ++key;
    }//8 9 10
    for (int i = 0; i < 3; ++i)
    {
        ret->addDoubleParameter(key, AString("bottom-right-") + xyz[i], xyz[i] + AString("-coordinate of the bottom right of the output image"));
        ++key;
    }//11 12 13
    for (int i = 0; i < 3; ++i)
    {
        ret->addDoubleParameter(key, AString("top-left-") + xyz[i], xyz[i] + AString("-coordinate of the top left of the output image"));
        ++key;
    }//14 15 16
    
    ret->addStringParameter(17, "image", "output - the output image");//fake the output formatting
    
    ret->setHelpText(
        AString("NOTE: If you want to generate an image with all of the capabilities of the GUI rendering, see -show-scene.\n\n") +
        "Renders an image of an arbitrary plane through the volume file, with a simple linear grayscale palette.  " +
        "The parameter <interp> must be one of:\n\n" +
        "CUBIC\nENCLOSING_VOXEL\nTRILINEAR"
    );
    return ret;
}

void OperationVolumeCapturePlane::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* myVol = myParams->getVolume(1);
    AString subvolName = myParams->getString(2);
    int subvol = myVol->getMapIndexFromNameOrNumber(subvolName);
    if (subvol < 0 || subvol >= myVol->getNumberOfMaps())
    {
        throw OperationException("invalid subvolume");
    }
    AString interp = myParams->getString(3);
    VolumeFile::InterpType myMethod = VolumeFile::CUBIC;
    if (interp == "CUBIC")
    {
        myMethod = VolumeFile::CUBIC;
    } else if (interp == "TRILINEAR") {
        myMethod = VolumeFile::TRILINEAR;
    } else if (interp == "ENCLOSING_VOXEL") {
        myMethod = VolumeFile::ENCLOSING_VOXEL;
    } else {
        throw OperationException("unrecognized interpolation method");
    }
    int width = (int)myParams->getInteger(4);
    int height = (int)myParams->getInteger(5);
    if (width < 2 || height < 2)
    {
        throw OperationException("output image dimensions must be 2 or greater");//to avoid divide by zero
    }
    float scalemin = (float)myParams->getDouble(6);
    float scalemax = (float)myParams->getDouble(7);
    Vector3D blvec, brvec, tlvec;
    int key = 8;
    for (int i = 0; i < 3; ++i)
    {
        blvec[i] = (float)myParams->getDouble(key);
        ++key;
    }
    for (int i = 0; i < 3; ++i)
    {
        brvec[i] = (float)myParams->getDouble(key);
        ++key;
    }
    for (int i = 0; i < 3; ++i)
    {
        tlvec[i] = (float)myParams->getDouble(key);
        ++key;
    }
    AString outName = myParams->getString(17);
    Vector3D rightTraverse = brvec - blvec, upTraverse = tlvec - blvec;
    if (abs(rightTraverse.normal().dot(upTraverse.normal())) > 0.001f)
    {
        CaretLogWarning("corner points describe non-orthogonal directions, image will be skewed");
    }
    float aspectRatio = (rightTraverse.length() / width) / (upTraverse.length() / height);
    if (aspectRatio > 1.001f || aspectRatio < 0.999f)
    {
        CaretLogWarning("corner points and image dimensions are different aspect ratios, image will be stretched");
    }
    vector<uint8_t> imageData(width * height * 4);
    for (int h = 0; h < height; ++h)
    {
        Vector3D rowStart = blvec + ((float)h) / (height - 1) * upTraverse;
        for (int w = 0; w < width; ++w)
        {
            Vector3D sample = rowStart + ((float)w) / (width - 1) * rightTraverse;
            bool valid = false;
            float value = myVol->interpolateValue(sample, myMethod, &valid, subvol);
            float normalized;
            if (valid)
            {
                if (value <= scalemin)
                {
                    normalized = 0.0f;
                } else {
                    if (value >= scalemax)
                    {
                        normalized = 1.0f;
                    } else {
                        normalized = (value - scalemin) / (scalemax - scalemin);
                    }
                }
            } else {
                normalized = 0.0f;
            }
            uint8_t intensity = (uint8_t)(normalized * 255 + 0.5f);
            uint8_t* pixel = imageData.data() + (w + h * width) * 4;
            for (int i = 0; i < 4; ++i)
            {
                pixel[i] = intensity;
            }
        }
    }
    ImageFile outFile(imageData.data(), width, height, ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM);
    outFile.writeFile(outName);
}
