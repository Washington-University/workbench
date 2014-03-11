
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
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

#define __CHARTABLE_MATRIX_FILE_SELECTION_MODEL_DECLARE__
#include "ChartableMatrixFileSelectionModel.h"
#undef __CHARTABLE_MATRIX_FILE_SELECTION_MODEL_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartableMatrixInterface.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartableMatrixFileSelectionModel 
 * \brief Selection model for matrix chartable files.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param brain
 *   Brain containing the chartable matrix files.
 */
ChartableMatrixFileSelectionModel::ChartableMatrixFileSelectionModel(Brain* brain)
: CaretObject(),
m_brain(brain)
{
    m_sceneAssistant = new SceneClassAssistant();
}

/**
 * Destructor.
 */
ChartableMatrixFileSelectionModel::~ChartableMatrixFileSelectionModel()
{
    delete m_sceneAssistant;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartableMatrixFileSelectionModel::toString() const
{
    return "ChartableMatrixFileSelectionModel";
}

/**
 * @return The selected file or NULL if no file is available.
 */
ChartableMatrixInterface*
ChartableMatrixFileSelectionModel::getSelectedFile()
{
    updateSelection();
    return m_selectedFile;
}

/**
 * @return The selected file or NULL if no file is available.
 */
const ChartableMatrixInterface*
ChartableMatrixFileSelectionModel::getSelectedFile() const
{
    updateSelection();
    return m_selectedFile;
}

/**
 * Set the selected file or NULL if no file is available.
 *
 * @param selectedFile
 *     Set the selected file.
 */
void
ChartableMatrixFileSelectionModel::setSelectedFile(ChartableMatrixInterface* selectedFile)
{
    m_selectedFile = selectedFile;
}

/**
 * @return Files available for selection.
 */
std::vector<ChartableMatrixInterface*>
ChartableMatrixFileSelectionModel::getAvailableFiles() const
{
    std::vector<ChartableMatrixInterface*> allFiles;
    m_brain->getAllChartableMatrixDataFiles(allFiles);
    
    
    std::vector<ChartableMatrixInterface*> chartableFiles;
    for (std::vector<ChartableMatrixInterface*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        ChartableMatrixInterface* chartFile = *iter;
        if (chartFile->isChartingSupported()) {
            chartableFiles.push_back(chartFile);
        }
    }
    return chartableFiles;
}

/**
 * Update the selected file.
 */
void
ChartableMatrixFileSelectionModel::updateSelection() const
{
    std::vector<ChartableMatrixInterface*> chartableFiles = getAvailableFiles();
    
    if (chartableFiles.empty()) {
        m_selectedFile = NULL;
        return;
    }
    
    if (m_selectedFile != NULL) {
        if (std::find(chartableFiles.begin(),
                      chartableFiles.end(),
                      m_selectedFile) == chartableFiles.end()) {
            m_selectedFile = NULL;
        }
    }
    
    if (m_selectedFile == NULL) {
        if (! chartableFiles.empty()) {
            m_selectedFile = chartableFiles[0];
        }
    }
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ChartableMatrixFileSelectionModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartableMatrixFileSelectionModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    ChartableMatrixInterface* chartFile = getSelectedFile();
    if (chartFile != NULL) {
        sceneClass->addString("m_selectedFile",
                              m_selectedFile->getCaretMappableDataFile()->getFileNameNoPath());
    }
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ChartableMatrixFileSelectionModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const AString chartFileName = sceneClass->getStringValue("m_selectedFile",
                                                             "");
    
    if ( ! chartFileName.isEmpty()) {
        std::vector<ChartableMatrixInterface*> chartFiles = getAvailableFiles();
        
        for (std::vector<ChartableMatrixInterface*>::iterator iter = chartFiles.begin();
             iter != chartFiles.end();
             iter++) {
            ChartableMatrixInterface* chartFile = *iter;
            if (chartFile->getCaretMappableDataFile()->getFileNameNoPath() == chartFileName) {
                setSelectedFile(chartFile);
                break;
            }
        }
    }

    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

