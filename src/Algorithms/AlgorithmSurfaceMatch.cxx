
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

#include "AlgorithmSurfaceMatch.h"
#include "AlgorithmException.h"
#include "DataFileException.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::AlgorithmSurfaceMatch 
 * \brief surface match
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * @return Command line switch
 */
AString
AlgorithmSurfaceMatch::getCommandSwitch()
{
    return "-surface-match";
}

/**
 * @return Short description of algorithm
 */
AString
AlgorithmSurfaceMatch::getShortDescription()
{
    return "SURFACE MATCH";
}

/**
 * @return Parameters for algorithm
 */
OperationParameters*
AlgorithmSurfaceMatch::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "Match Surface File", "Match (Reference) Surface");
    ret->addSurfaceParameter(2, "Input Surface File", "File containing surface that will be transformed");
    ret->addStringParameter(3, "Output Surface Name", "Surface File after transformation");
    
    AString helpText = ("The Input Surface File will be transformed so that its coordinate "
                        "ranges (bounding box) match that of the Match Surface File");
    
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
AlgorithmSurfaceMatch::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    try {
        SurfaceFile* matchSurfaceFile = myParams->getSurface(1);
        SurfaceFile* surfaceFile = myParams->getSurface(2);
        const AString outputSurfaceFileName = myParams->getString(3);
        
        /*
         * Example parameter processing:
         *
         * Gets the surface with key 1
         * SurfaceFile* mySurf = myParams->getSurface(1);
         *
         * Gets the output metric with key 2
         * MetricFile* myMetricOut = myParams->getOutputMetric(2);
         *
         * Gets optional parameter with key 3
         * OptionalParameter* columnSelect = myParams->getOptionalParameter(3);
         * int columnNum = -1;
         * if (columnSelect->m_present) {
         *     columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
         *     if (columnNum < 0) {
         *          throw AlgorithmException("invalid column specified");
         *     }
         * }
         */
        
        /*
         * Constructs and executes the algorithm
         */
        AlgorithmSurfaceMatch(myProgObj,
                              matchSurfaceFile,
                              surfaceFile);
        
        surfaceFile->writeFile(outputSurfaceFileName);
    }
    catch (const DataFileException& dfe) {
        throw AlgorithmException(dfe);
    }
}

/**
 * Constructor
 *
 * Calling the constructor will execute the algorithm
 *
 * @param myProgObj
 *     Parameters for algorithm
 */
AlgorithmSurfaceMatch::AlgorithmSurfaceMatch(ProgressObject* myProgObj,
                                             const SurfaceFile* matchSurfaceFile,
                                             SurfaceFile* surfaceFile)
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
    
    surfaceFile->matchSurfaceBoundingBox(matchSurfaceFile);
    
    /*
     * How you say you are halfway done with the INTERNAL work of the algorithm
     * will report finished automatically when this function ends (myProgress goes out 
     of scope, destructor triggers finish
     */
    //myProgress.reportProgress(0.5f);
    
}

/**
 * @return Algorithm internal weight
 */
float
AlgorithmSurfaceMatch::getAlgorithmInternalWeight()
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
AlgorithmSurfaceMatch::getSubAlgorithmWeight()
{
    /*
     * If you use a subalgorithm
     */
    //return AlgorithmInsertNameHere::getAlgorithmWeight()
    return 0.0f;
}

