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

#include "OperationVolumePalette.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationVolumePalette::getCommandSwitch()
{
    return "-volume-palette";
}

AString OperationVolumePalette::getShortDescription()
{
    return "SET THE PALETTE OF A VOLUME FILE";
}

OperationParameters* OperationVolumePalette::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "volume", "the volume file to modify");
    ret->addStringParameter(2, "mode", "the mapping mode");
    OptionalParameter* subvolumeSelect = ret->createOptionalParameter(3, "-subvolume", "select a single subvolume");
    subvolumeSelect->addStringParameter(1, "subvolume", "the subvolume number or name");
    
    OptionalParameter* posMinMaxPercent = ret->createOptionalParameter(4, "-pos-percent", "percentage min/max for positive data coloring");
    posMinMaxPercent->addDoubleParameter(1, "pos-min-%", "the percentile for the least positive data");
    posMinMaxPercent->addDoubleParameter(2, "pos-max-%", "the percentile for the most positive data");
    
    OptionalParameter* negMinMaxPercent = ret->createOptionalParameter(5, "-neg-percent", "percentage min/max for negative data coloring");
    negMinMaxPercent->addDoubleParameter(1, "neg-min-%", "the percentile for the least negative data");
    negMinMaxPercent->addDoubleParameter(2, "neg-max-%", "the percentile for the most negative data");
    
    OptionalParameter* posMinMaxValue = ret->createOptionalParameter(11, "-pos-user", "user min/max values for positive data coloring");
    posMinMaxValue->addDoubleParameter(1, "pos-min-user", "the value for the least positive data");
    posMinMaxValue->addDoubleParameter(2, "pos-max-user", "the value for the most positive data");
    
    OptionalParameter* negMinMaxValue = ret->createOptionalParameter(12, "-neg-user", "user min/max values for negative data coloring");
    negMinMaxValue->addDoubleParameter(1, "neg-min-user", "the value for the least negative data");
    negMinMaxValue->addDoubleParameter(2, "neg-max-user", "the value for the most negative data");
    
    OptionalParameter* interpolate = ret->createOptionalParameter(9, "-interpolate", "interpolate colors");
    interpolate->addBooleanParameter(1, "interpolate", "boolean, whether to interpolate");
    
    OptionalParameter* displayPositive = ret->createOptionalParameter(6, "-disp-pos", "display positive data");
    displayPositive->addBooleanParameter(1, "display", "boolean, whether to display");
    
    OptionalParameter* displayNegative = ret->createOptionalParameter(7, "-disp-neg", "display positive data");
    displayNegative->addBooleanParameter(1, "display", "boolean, whether to display");
    
    OptionalParameter* displayZero = ret->createOptionalParameter(8, "-disp-zero", "display data closer to zero than the min cutoff");
    displayZero->addBooleanParameter(1, "display", "boolean, whether to display");
    
    OptionalParameter* paletteName = ret->createOptionalParameter(10, "-palette-name", "set the palette used");
    paletteName->addStringParameter(1, "name", "the name of the palette");
    
    OptionalParameter* thresholdOpt = ret->createOptionalParameter(13, "-thresholding", "set the thresholding");
    thresholdOpt->addStringParameter(1, "type", "thresholding setting");
    thresholdOpt->addStringParameter(2, "test", "show values inside or outside thresholds");
    thresholdOpt->addDoubleParameter(3, "min", "lower threshold");
    thresholdOpt->addDoubleParameter(4, "max", "upper threshold");
    
    OptionalParameter* inversionOpt = ret->createOptionalParameter(15, "-inversion", "specify palette inversion");
    inversionOpt->addStringParameter(1, "type", "the type of inversion");
    
    AString myText = AString("The original volume file is overwritten with the modified version.  By default, all columns of the volume file are adjusted ") +
            "to the new settings, use the -subvolume option to change only one subvolume.  Mapping settings not specified in options will be taken from the first subvolume.  " +
            "The <mode> argument must be one of the following:\n\n";
    vector<PaletteScaleModeEnum::Enum> myEnums;
    PaletteScaleModeEnum::getAllEnums(myEnums);
    for (int i = 0; i < (int)myEnums.size(); ++i)
    {
        myText += PaletteScaleModeEnum::toName(myEnums[i]) + "\n";
    }
    myText += "\nThe <name> argument to -palette-name must be one of the following:\n\n";
    PaletteFile myPF;
    int32_t numPalettes = myPF.getNumberOfPalettes();
    for (int i = 0; i < numPalettes; ++i)
    {
        myText += myPF.getPalette(i)->getName() + "\n";
    }
    myText += "\nThe <type> argument to -thresholding must be one of the following:\n\n";
    vector<PaletteThresholdTypeEnum::Enum> myEnums2;
    PaletteThresholdTypeEnum::getAllEnums(myEnums2);
    for (int i = 0; i < (int)myEnums2.size(); ++i)
    {
        myText += PaletteThresholdTypeEnum::toName(myEnums2[i]) + "\n";
    }
    myText += "\nThe <test> argument to -thresholding must be one of the following:\n\n";
    vector<PaletteThresholdTestEnum::Enum> myEnums3;
    PaletteThresholdTestEnum::getAllEnums(myEnums3);
    for (int i = 0; i < (int)myEnums3.size(); ++i)
    {
        myText += PaletteThresholdTestEnum::toName(myEnums3[i]) + "\n";
    }
    myText += "\nThe <type> argument to -inversion must be one of the following:\n\n";
    vector<PaletteInvertModeEnum::Enum> myEnums4;
    PaletteInvertModeEnum::getAllEnums(myEnums4);
    for (int i = 0; i < (int)myEnums4.size(); ++i)
    {
        myText += PaletteInvertModeEnum::toName(myEnums4[i]) + "\n";
    }
    ret->setHelpText(myText);
    return ret;
}

void OperationVolumePalette::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString myVolumeName = myParams->getString(1);
    AString myModeName = myParams->getString(2);
    bool ok = false;
    PaletteScaleModeEnum::Enum myMode = PaletteScaleModeEnum::fromName(myModeName, &ok);
    if (!ok)
    {
        throw OperationException("unknown mapping mode");
    }
    VolumeFile myVolume;
    myVolume.readFile(myVolumeName);
    int mySubvolume = -1;
    OptionalParameter* subvolumeSelect = myParams->getOptionalParameter(3);
    if (subvolumeSelect->m_present)
    {
        mySubvolume = (int)myVolume.getMapIndexFromNameOrNumber(subvolumeSelect->getString(1));
        if (mySubvolume < 0 || mySubvolume >= myVolume.getNumberOfMaps())
        {
            throw OperationException("invalid column specified");
        }
    }
    if (myVolume.getType() == SubvolumeAttributes::LABEL)
    {
        //label-type volumes have a null pointer for palette
        //so reset to anatomy, because that is the command's existing behavior when there is no caret extension
        CaretLogWarning("-volume-palette run on label-type volume '" + myVolumeName + "', resetting to anatomy type");
        myVolume.setType(SubvolumeAttributes::ANATOMY);
    }
    PaletteColorMapping myMapping = *(myVolume.getMapPaletteColorMapping(0));//create the mapping, then use operator= to set for all requested columns, take defaults from first map
    myMapping.setScaleMode(myMode);
    OptionalParameter* posMinMaxPercent = myParams->getOptionalParameter(4);
    if (posMinMaxPercent->m_present)
    {
        myMapping.setAutoScalePercentagePositiveMinimum(posMinMaxPercent->getDouble(1));
        myMapping.setAutoScalePercentagePositiveMaximum(posMinMaxPercent->getDouble(2));
    }
    OptionalParameter* negMinMaxPercent = myParams->getOptionalParameter(5);
    if (negMinMaxPercent->m_present)
    {
        myMapping.setAutoScalePercentageNegativeMinimum(negMinMaxPercent->getDouble(1));
        myMapping.setAutoScalePercentageNegativeMaximum(negMinMaxPercent->getDouble(2));
    }
    OptionalParameter* posMinMaxValue = myParams->getOptionalParameter(11);
    if (posMinMaxValue->m_present)
    {
        myMapping.setUserScalePositiveMinimum(posMinMaxValue->getDouble(1));
        myMapping.setUserScalePositiveMaximum(posMinMaxValue->getDouble(2));
    }
    OptionalParameter* negMinMaxValue = myParams->getOptionalParameter(12);
    if (negMinMaxValue->m_present)
    {
        myMapping.setUserScaleNegativeMinimum(negMinMaxValue->getDouble(1));
        myMapping.setUserScaleNegativeMaximum(negMinMaxValue->getDouble(2));
    }
    OptionalParameter* displayPositive = myParams->getOptionalParameter(6);
    if (displayPositive->m_present)
    {
        myMapping.setDisplayPositiveDataFlag(displayPositive->getBoolean(1));
    }
    OptionalParameter* displayNegative = myParams->getOptionalParameter(7);
    if (displayNegative->m_present)
    {
        myMapping.setDisplayNegativeDataFlag(displayNegative->getBoolean(1));
    }
    OptionalParameter* displayZero = myParams->getOptionalParameter(8);
    if (displayZero->m_present)
    {
        myMapping.setDisplayZeroDataFlag(displayZero->getBoolean(1));
    }
    OptionalParameter* interpolate = myParams->getOptionalParameter(9);
    if (interpolate->m_present)
    {
        myMapping.setInterpolatePaletteFlag(interpolate->getBoolean(1));
    }
    OptionalParameter* paletteName = myParams->getOptionalParameter(10);
    if (paletteName->m_present)
    {
        myMapping.setSelectedPaletteName(paletteName->getString(1));
    }
    OptionalParameter* thresholdOpt = myParams->getOptionalParameter(13);
    if (thresholdOpt->m_present)
    {
        bool ok = false;
        PaletteThresholdTypeEnum::Enum mytype = PaletteThresholdTypeEnum::fromName(thresholdOpt->getString(1), &ok);
        if (!ok) throw OperationException("unrecognized threshold type string: " + thresholdOpt->getString(1));
        PaletteThresholdTestEnum::Enum mytest = PaletteThresholdTestEnum::fromName(thresholdOpt->getString(2), &ok);
        if (!ok) throw OperationException("unrecognized threshold test string: " + thresholdOpt->getString(2));
        myMapping.setThresholdType(mytype);
        myMapping.setThresholdTest(mytest);
        myMapping.setThresholdMinimum(mytype, thresholdOpt->getDouble(3));
        myMapping.setThresholdMaximum(mytype, thresholdOpt->getDouble(4));
    }
    OptionalParameter* inversionOpt = myParams->getOptionalParameter(15);
    if (inversionOpt->m_present)
    {
        bool ok = false;
        PaletteInvertModeEnum::Enum inversionType = PaletteInvertModeEnum::fromName(inversionOpt->getString(1), &ok);
        if (!ok) throw OperationException("unrecognized palette inversion type: " + inversionOpt->getString(1));
        myMapping.setInvertedMode(inversionType);
    }
    if (mySubvolume == -1)
    {
        for (int i = 0; i < myVolume.getNumberOfMaps(); ++i)
        {
            *(myVolume.getMapPaletteColorMapping(i)) = myMapping;
        }
    } else {
        *(myVolume.getMapPaletteColorMapping(mySubvolume)) = myMapping;
    }
    myVolume.writeFile(myVolumeName);
}
