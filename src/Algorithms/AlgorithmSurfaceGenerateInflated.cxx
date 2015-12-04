
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

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "AlgorithmSurfaceGenerateInflated.h"
#include "AlgorithmSurfaceInflation.h"
#include "AlgorithmException.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::AlgorithmSurfaceGenerateInflated 
 * \brief SURFACE GENERATE INFLATED
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * @return Command line switch
 */
AString
AlgorithmSurfaceGenerateInflated::getCommandSwitch()
{
    return "-surface-generate-inflated";
}

/**
 * @return Short description of algorithm
 */
AString
AlgorithmSurfaceGenerateInflated::getShortDescription()
{
    return "SURFACE GENERATE INFLATED";
}

/**
 * @return Parameters for algorithm
 */
OperationParameters*
AlgorithmSurfaceGenerateInflated::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "anatomical-surface-in", "the anatomical surface");
    
    ret->addSurfaceOutputParameter(2, "inflated-surface-out", "the output inflated surface");
    
    ret->addSurfaceOutputParameter(3, "very-inflated-surface-out", "the output very inflated surface");
    
    OptionalParameter* weightOpt = ret->createOptionalParameter(4, "-iterations-scale", "optional iterations scaling");
    weightOpt->addDoubleParameter(1, "iterations-scale-value", "iterations-scale value");
    
    AString helpText = ("Generate inflated and very inflated surfaces. The output surfaces are "
                        "\'matched\' (have same XYZ range) to the anatomcal surface. "
                        "In most cases, an iterations-scale of 1.0 (default) is sufficient.  However, if "
                        "the surface contains a large number of vertices (150,000), try an "
                        "iterations-scale of 2.5.");
    
    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform algorithm
 * @param myParams
 *     Parameters for algorithm
 * @param myProgObj
 *     The progress object
 * @throws
 *     AlgorithmException if errors
 */
void
AlgorithmSurfaceGenerateInflated::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    const SurfaceFile* anatomicalSurfaceIn = myParams->getSurface(1);
    SurfaceFile* inflatedSurfaceFile = myParams->getOutputSurface(2);
    SurfaceFile* veryInflatedSurfaceFile = myParams->getOutputSurface(3);
    
    float iterationsScale = 1.0;
    OptionalParameter* iterationsScaleOpt = myParams->getOptionalParameter(4);
    if (iterationsScaleOpt->m_present) {
        iterationsScale = iterationsScaleOpt->getDouble(1);
    }
    
    /*
     * Constructs and executes the algorithm
     */
    AlgorithmSurfaceGenerateInflated(myProgObj,
                              anatomicalSurfaceIn,
                              inflatedSurfaceFile,
                              veryInflatedSurfaceFile,
                              iterationsScale);
}

/**
 * Constructor
 *
 * Calling the constructor will execute the algorithm
 *
 * @param myProgObj
 *     Parameters for algorithm
 * @param anatomicalSurfaceFile
 *     The anatomical surface.
 * @param inflatedSurfaceFileOut
 *     Inflated surface that is generated.
 * @param veryInflatedSurfaceFileOut
 *     Very inflated surface that is generated.
 * @param iterationsScaleIn
 *     Iterations scale used for surface with large numbers of nodes.
 */
AlgorithmSurfaceGenerateInflated::AlgorithmSurfaceGenerateInflated(ProgressObject* myProgObj,
                                                                   const SurfaceFile* anatomicalSurfaceFile,
                                                                   SurfaceFile* inflatedSurfaceFileOut,
                                                                   SurfaceFile* veryInflatedSurfaceFileOut,
                                                                   const float iterationsScaleIn)
   : AbstractAlgorithm(myProgObj)
{
    ProgressObject* lowProgress = NULL, *inflatedProgress = NULL, *veryInfProgress = NULL;
    if (myProgObj != NULL) {
        lowProgress = myProgObj->addAlgorithm(AlgorithmSurfaceInflation::getAlgorithmWeight());
        inflatedProgress = myProgObj->addAlgorithm(AlgorithmSurfaceInflation::getAlgorithmWeight() * 2);
        veryInfProgress = myProgObj->addAlgorithm(AlgorithmSurfaceInflation::getAlgorithmWeight() * 4);
    }
    
    /*
     * Sets the algorithm up to use the progress object, and will 
     * finish the progress object automatically when the algorithm terminates
     */
    LevelProgress myProgress(myProgObj, 1.0f, 0.1f);//low internal weight because this function doesn't do much non-subalgorithm stuff
    
    const float iterationsScale = ((iterationsScaleIn > 0.0)
                                   ? iterationsScaleIn
                                   : 1.0);
    
    /*
     * Generate low-smooth surface which is an intermediate surface
     * between anatomical and inflated.
     */
    SurfaceFile lowSmoothSurface(*anatomicalSurfaceFile);
    const int32_t lowSmoothCycles = 1;
    const float lowSmoothStrength = 0.2;
    const int32_t lowSmoothIterations = static_cast<int32_t>(50 * iterationsScale);
    const float lowSmoothInflationFactor = 1.0;
    myProgress.setTask("Generating Low-smooth Surface");
    AlgorithmSurfaceInflation(lowProgress,
                              anatomicalSurfaceFile,
                              &lowSmoothSurface,
                              &lowSmoothSurface,
                              lowSmoothCycles,
                              lowSmoothStrength,
                              lowSmoothIterations,
                              lowSmoothInflationFactor);
    

    /*
     * Generation the inflated surface
     */
    *inflatedSurfaceFileOut = lowSmoothSurface;
    const int32_t inflatedSmoothCycles = 2;
    const float inflatedSmoothStrength = 1.0;
    const int32_t inflatedSmoothIterations = static_cast<int32_t>(30 * iterationsScale);
    const float inflatedSmoothInflationFactor = 1.4;
    myProgress.setTask("Generating Inflated Surface");
    AlgorithmSurfaceInflation(inflatedProgress,
                              anatomicalSurfaceFile,
                              inflatedSurfaceFileOut,
                              inflatedSurfaceFileOut,
                              inflatedSmoothCycles,
                              inflatedSmoothStrength,
                              inflatedSmoothIterations,
                              inflatedSmoothInflationFactor);
    inflatedSurfaceFileOut->setSurfaceType(SurfaceTypeEnum::INFLATED);
    
    /*
     * Generation the inflated surface
     */
    *veryInflatedSurfaceFileOut = *inflatedSurfaceFileOut;
    const int32_t veryInflatedSmoothCycles = 4;
    const float veryInflatedSmoothStrength = 1.0;
    const int32_t veryInflatedSmoothIterations = static_cast<int32_t>(30 * iterationsScale);
    const float veryInflatedSmoothInflationFactor = 1.1;
    myProgress.setTask("Generating Very Inflated Surface");
    AlgorithmSurfaceInflation(veryInfProgress,
                              anatomicalSurfaceFile,
                              veryInflatedSurfaceFileOut,
                              veryInflatedSurfaceFileOut,
                              veryInflatedSmoothCycles,
                              veryInflatedSmoothStrength,
                              veryInflatedSmoothIterations,
                              veryInflatedSmoothInflationFactor);
    veryInflatedSurfaceFileOut->setSurfaceType(SurfaceTypeEnum::VERY_INFLATED);
    
    myProgress.setTask("Matching Bounding Boxes");
    inflatedSurfaceFileOut->matchSurfaceBoundingBox(anatomicalSurfaceFile);
    myProgress.reportProgress(0.5);//report progress of non-subalgorithm computation only
    veryInflatedSurfaceFileOut->matchSurfaceBoundingBox(anatomicalSurfaceFile);
    myProgress.reportProgress(1.0);//this isn't really needed, happens automatically when algorithm ends, but for clarity
}

/**
 * @return Algorithm internal weight
 */
float
AlgorithmSurfaceGenerateInflated::getAlgorithmInternalWeight()
{
    /*
     * override this if needed, if the progress bar isn't smooth
     */
    return 0.1f;//we very little inside this algorithm except call other algorithms
}

/**
 * @return Algorithm sub-algorithm weight
 */
float
AlgorithmSurfaceGenerateInflated::getSubAlgorithmWeight()
{
    /*
     * If you use a subalgorithm
     */
    return AlgorithmSurfaceInflation::getAlgorithmWeight() * 7;//7 cycles in total - may deserve factoring in default number of iterations
}

