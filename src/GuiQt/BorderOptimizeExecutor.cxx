
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __BORDER_OPTIMIZE_EXECUTOR_DECLARE__
#include "BorderOptimizeExecutor.h"
#undef __BORDER_OPTIMIZE_EXECUTOR_DECLARE__

#include "Border.h"
#include "CaretAssert.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class caret::BorderOptimizeExecutor 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup GuiQt
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
BorderOptimizeExecutor::BorderOptimizeExecutor()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BorderOptimizeExecutor::~BorderOptimizeExecutor()
{
}

/**
 * Run the border optimization algorithm.
 *
 * @param inputData
 *     Input data for the algorithm.
 * @param statisticsInformationOut
 *     Output containing statistics information.
 * @param errorMessageOut
 *     Output containing error information when false returned.
 * @return 
 *     True if successful, else false.
 */
bool
BorderOptimizeExecutor::run(const InputData& inputData,
                            AString& statisticsInformationOut,
                            AString& errorMessageOut)
{
    statisticsInformationOut.clear();
    errorMessageOut.clear();
    
    printInputs(inputData);
    
    /*
     * Inputs for algorithm
     *
     * Pointers to borders selected by user for optimization.
     *     std:vector<Border*> userSelections.m_borders;
     * Pointer to border enclosing ROI drawn by user
     *     Border* userSelectionsOut.m_borderEnclosingROI;
     * Ponters to data files used for border optimization.
     *     std::vector<CaretMappableDataFile*> userSelections.m_optimizeDataFiles;
     * Contains nodes found inside border drawn by the user.
     *     std::vector<int32_t> userSelections.m_nodesInsideROI;
     * Smoothing level ranging [0.0, 1.0]
     *     float userSelections.m_smoothingLevel;
     * Flag for inverted gradient.
     *     bool  userSelections.m_invertedGradientFlag;
     */
    
    
    /*
     * Modifying a border:
     *
     * (1) Make a copy of the border
     *
     *     Border* borderCopy = new Border(*border)
     *
     * (2) Modify the 'copied' border
     *
     * (3) When modification is complete, calling this method
     *     will first make an 'undo' copy of 'border' that is stored
     *     inside of border and then replace the points in 'border'
     *     with those from 'borderCopy'.   This will allow the
     *     user to press the Border ToolBar's 'Undo Finish' button
     *     if the changes are not acceptable.
     *
     *     border->replacePointsWithUndoSaving(borderCopy)
     */
    
    /*
     * Returning false implies errors.
     */
    errorMessageOut = "Implementation is missing.";
    return false;
}

/**
 * Run the border optimization algorithm.
 *
 * @param inputData
 *     Input data for the algorithm.
 */
void
BorderOptimizeExecutor::printInputs(const InputData& inputData)
{
    std::cout << "Optimizing borders: " << std::endl;
    for (std::vector<Border*>::const_iterator bi = inputData.m_borders.begin();
         bi != inputData.m_borders.end();
         bi++) {
        std::cout << "    " << qPrintable((*bi)->getName()) << std::endl;
    }
    
    std::cout << "Optimizing Surface: " << qPrintable(inputData.m_surface->getFileNameNoPath()) << std::endl;
    std::cout << "Number of nodes in ROI: " << qPrintable(AString::number(inputData.m_nodesInsideROI.size())) << std::endl;
    std::cout << "Optimizing Data Files: " << std::endl;
    for (std::vector<CaretPointer<DataFileInfo> >::const_iterator fi = inputData.m_dataFileInfo.begin();
         fi != inputData.m_dataFileInfo.end();
         fi++) {
        CaretPointer<DataFileInfo> dfi = *fi;
        std::cout << "    Name: " << qPrintable(dfi->m_mapFile->getFileNameNoPath()) << std::endl;
        if (dfi->m_allMapsFlag) {
            std::cout << "     Map: All Maps" << std::endl;
        }
        else {
            std::cout << "     Map: " << qPrintable(AString::number(dfi->m_mapIndex))
                  << "     " << qPrintable(dfi->m_mapFile->getMapName(dfi->m_mapIndex)) << std::endl;
        }
        
        std::cout << "    Strength: " << dfi->m_weight << std::endl;
        std::cout << "    Smoothing: " << dfi->m_smoothing << std::endl;
        std::cout << "    Invert Gradient: " << AString::fromBool(dfi->m_invertGradientFlag) << std::endl;
    }
}
