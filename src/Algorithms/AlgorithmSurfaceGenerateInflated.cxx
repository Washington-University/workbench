
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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
    
    AString helpText = ("Generate inflated and very inflated surfaces. "
                        "In most cases, an iterations-scale of 1.0 (default) is sufficient.  However, if "
                        "the surface contains a large number of nodes (150,000), try an "
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
    /*
     * Uncomment these if you use another algorithm inside here
     *
     * ProgressObject* subAlgProgress1 = NULL;
     * if (myProgObj != NULL) {
     *    subAlgProgress1 = myProgObj->addAlgorithm(AlgorithmInsertNameHere::getAlgorithmWeight());
     * }
     */
    
    /*
     * Sets the algorithm up to use the progress object, and will 
     * finish the progress object automatically when the algorithm terminates
     */
    LevelProgress myProgress(myProgObj);
    
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
    AlgorithmSurfaceInflation(myProgObj,
                              anatomicalSurfaceFile,
                              &lowSmoothSurface,
                              &lowSmoothSurface,
                              lowSmoothCycles,
                              lowSmoothStrength,
                              lowSmoothIterations,
                              lowSmoothInflationFactor);
    
    myProgress.reportProgress(0.33);

    /*
     * Generation the inflated surface
     */
    *inflatedSurfaceFileOut = lowSmoothSurface;
    const int32_t inflatedSmoothCycles = 2;
    const float inflatedSmoothStrength = 1.0;
    const int32_t inflatedSmoothIterations = static_cast<int32_t>(30 * iterationsScale);
    const float inflatedSmoothInflationFactor = 1.4;
    AlgorithmSurfaceInflation(myProgObj,
                              anatomicalSurfaceFile,
                              inflatedSurfaceFileOut,
                              inflatedSurfaceFileOut,
                              inflatedSmoothCycles,
                              inflatedSmoothStrength,
                              inflatedSmoothIterations,
                              inflatedSmoothInflationFactor);
    inflatedSurfaceFileOut->setSurfaceType(SurfaceTypeEnum::INFLATED);
    myProgress.reportProgress(0.66);
    
    /*
     * Generation the inflated surface
     */
    *veryInflatedSurfaceFileOut = *inflatedSurfaceFileOut;
    const int32_t veryInflatedSmoothCycles = 4;
    const float veryInflatedSmoothStrength = 1.0;
    const int32_t veryInflatedSmoothIterations = static_cast<int32_t>(30 * iterationsScale);
    const float veryInflatedSmoothInflationFactor = 1.1;
    AlgorithmSurfaceInflation(myProgObj,
                              anatomicalSurfaceFile,
                              veryInflatedSurfaceFileOut,
                              veryInflatedSurfaceFileOut,
                              veryInflatedSmoothCycles,
                              veryInflatedSmoothStrength,
                              veryInflatedSmoothIterations,
                              veryInflatedSmoothInflationFactor);
    veryInflatedSurfaceFileOut->setSurfaceType(SurfaceTypeEnum::VERY_INFLATED);
    myProgress.reportProgress(0.98);
    
    inflatedSurfaceFileOut->matchSurfaceBoundingBox(anatomicalSurfaceFile);
    veryInflatedSurfaceFileOut->matchSurfaceBoundingBox(anatomicalSurfaceFile);
    
    myProgress.reportProgress(1.0);
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
    return 1.0f;
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
    //return AlgorithmInsertNameHere::getAlgorithmWeight()
    return 0.0f;
}

