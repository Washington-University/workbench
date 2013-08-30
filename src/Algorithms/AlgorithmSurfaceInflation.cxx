
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

#include <cmath>

#include "AlgorithmSurfaceInflation.h"
#include "AlgorithmSurfaceSmoothing.h"
#include "AlgorithmException.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::AlgorithmSurfaceInflation 
 * \brief SURFACE INFLATION
 *
 * Perfom surface inflation.
 */

/**
 * @return Command line switch
 */
AString
AlgorithmSurfaceInflation::getCommandSwitch()
{
    return "-surface-inflation";
}

/**
 * @return Short description of algorithm
 */
AString
AlgorithmSurfaceInflation::getShortDescription()
{
    return "SURFACE INFLATION";
}

/**
 * @return Parameters for algorithm
 */
OperationParameters*
AlgorithmSurfaceInflation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "anatomical-surface-in", "the anatomical surface");
    
    ret->addSurfaceParameter(2, "surface-in", "the surface file to inflate");
    
    ret->addIntegerParameter(3, "number-of-smoothing-cycles", "number of smoothing cycles");
    
    ret->addDoubleParameter(4, "smoothing-strength", "smoothing strength (ranges [0.0 - 1.0])");
    
    ret->addIntegerParameter(5, "smoothing-iterations", "smoothing iterations");
    
    ret->addDoubleParameter(6, "inflation-factor", "inflation factor");
    
    ret->addSurfaceOutputParameter(7, "surface-out", "output surface file");
    
    AString helpText = ("Inflate a surface by performing cycles that consist of smoothing "
                        " followed by inflation (to correct shrinkage caused by smoothing).");
    
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
AlgorithmSurfaceInflation::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    SurfaceFile* anatomicalSurfaceIn = myParams->getSurface(1);
    SurfaceFile* surfaceIn = myParams->getSurface(2);
    const int32_t cycles = myParams->getInteger(3);
    const float strength = myParams->getDouble(4);
    const int32_t iterations = myParams->getInteger(5);
    const float inflationFactor = myParams->getDouble(6);
    SurfaceFile* surfaceOut = myParams->getOutputSurface(7);

    /*
     * Constructs and executes the algorithm 
     */
    AlgorithmSurfaceInflation(myProgObj,
                              anatomicalSurfaceIn,
                              surfaceIn,
                              surfaceOut,
                              cycles,
                              strength,
                              iterations,
                              inflationFactor);
}

/**
 * Constructor
 *
 * Calling the constructor will execute the algorithm
 *
 * @param myProgObj
 *     Parameters for algorithm
 */
AlgorithmSurfaceInflation::AlgorithmSurfaceInflation(ProgressObject* myProgObj,
                                                     const SurfaceFile* anatomicalSurfaceFile,
                                                     const SurfaceFile* inputSurfaceFile,
                                                     SurfaceFile* outputSurfaceFile,
                                                     const int32_t cycles,
                                                     const float strength,
                                                     const int32_t iterations,
                                                     const float inflationFactorIn)
   : AbstractAlgorithm(myProgObj)
{
    std::vector<ProgressObject*> subAlgProgress;
    if (myProgObj != NULL) {
        subAlgProgress.resize(cycles);
        for (int32_t i = 0; i < cycles; ++i)
        {
            subAlgProgress[i] = myProgObj->addAlgorithm(AlgorithmSurfaceSmoothing::getAlgorithmWeight());
        }
    }
    
    /*
     * Sets the algorithm up to use the progress object, and will
     * finish the progress object automatically when the algorithm terminates
     */
    LevelProgress myProgress(myProgObj, 1.0f, 0.1f);//lower the internal weight
    
    const float inflationFactor = inflationFactorIn - 1.0;
    
    *outputSurfaceFile = *inputSurfaceFile;
    outputSurfaceFile->translateToCenterOfMass();
    
    const BoundingBox* anatomicalBoundingBox = anatomicalSurfaceFile->getBoundingBox();
    const float anatomicalRangeX = anatomicalBoundingBox->getDifferenceX();
    const float anatomicalRangeY = anatomicalBoundingBox->getDifferenceY();
    const float anatomicalRangeZ = anatomicalBoundingBox->getDifferenceZ();
    
    const int32_t numberOfNodes = outputSurfaceFile->getNumberOfNodes();
    
    for (int iCycle = 0; iCycle < cycles; iCycle++) {
        /*
         * Smooth
         */
        ProgressObject* subProgress = NULL;
        if (myProgObj != NULL)
        {
            subProgress = subAlgProgress[iCycle];
        }
        AlgorithmSurfaceSmoothing(subProgress,
                                  outputSurfaceFile,
                                  outputSurfaceFile,
                                  strength,
                                  iterations);
        
        /*
         * Inflate
         */
        float xyz[3];
        for (int32_t iNode = 0; iNode < numberOfNodes; iNode++) {
            outputSurfaceFile->getCoordinate(iNode,
                                             xyz);
            
            const float x = xyz[0] / anatomicalRangeX;
            const float y = xyz[1] / anatomicalRangeY;
            const float z = xyz[2] / anatomicalRangeZ;
            
            const float radius = std::sqrt(x*x + y*y + z*z);
            const float scale  = 1.0 + inflationFactor * (1.0 - radius);
            
            xyz[0] *= scale;
            xyz[1] *= scale;
            xyz[2] *= scale;
            
            outputSurfaceFile->setCoordinate(iNode,
                                             xyz);
        }
        
        myProgress.reportProgress(static_cast<float>(iCycle +1)
                                  / static_cast<float>(cycles));
    }
    
    outputSurfaceFile->computeNormals();
}

/**
 * @return Algorithm internal weight
 */
float
AlgorithmSurfaceInflation::getAlgorithmInternalWeight()
{
    /*
     * override this if needed, if the progress bar isn't smooth
     */
    return 0.1f;//all this does internally is rescale the coordinates, lower the internal weight
}

/**
 * @return Algorithm sub-algorithm weight
 */
float
AlgorithmSurfaceInflation::getSubAlgorithmWeight()
{
    /*
     * If you use a subalgorithm
     */
    return AlgorithmSurfaceSmoothing::getAlgorithmWeight();
}

