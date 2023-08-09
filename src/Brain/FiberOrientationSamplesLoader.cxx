
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

#include <cmath>

#define __FIBER_ORIENTATION_SAMPLES_LOADER_DECLARE__
#include "FiberOrientationSamplesLoader.h"
#undef __FIBER_ORIENTATION_SAMPLES_LOADER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CiftiFiberOrientationFile.h"
#include "DataFileException.h"
#include "FiberOrientation.h"
#include "FileInformation.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventManager.h"
#include "SpecFile.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::FiberOrientationSamplesLoader 
 * \brief Loads Fiber Orientation samples display on a sphere in features toolbox
 * \ingroup Brain
 */

/**
 * Constructor.
 */
FiberOrientationSamplesLoader::FiberOrientationSamplesLoader()
: CaretObject()
{
    m_sampleVolumesLoadAttemptValid = false;
    m_sampleVolumesValid = false;
    for (int32_t i = 0; i < 3; i++) {
        m_sampleMagnitudeVolumes[i] = NULL;
        m_sampleThetaVolumes[i] = NULL;
        m_samplePhiVolumes[i] = NULL;
    }
    
    m_lastIdentificationValid = false;
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    
}

/**
 * Destructor.
 */
FiberOrientationSamplesLoader::~FiberOrientationSamplesLoader()
{
    reset();
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
FiberOrientationSamplesLoader::reset()
{
    m_sampleVolumesLoadAttemptValid = false;
    m_sampleVolumesValid = false;
    for (int32_t i = 0; i < 3; i++) {
        if (m_sampleMagnitudeVolumes[i] != NULL) {
            delete m_sampleMagnitudeVolumes[i];
            m_sampleMagnitudeVolumes[i] = NULL;
        }
        if (m_sampleThetaVolumes[i] != NULL) {
            delete m_sampleThetaVolumes[i];
            m_sampleThetaVolumes[i] = NULL;
        }
        if (m_samplePhiVolumes[i] != NULL) {
            delete m_samplePhiVolumes[i];
            m_samplePhiVolumes[i] = NULL;
        }
    }
    m_lastIdentificationValid = false;
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
FiberOrientationSamplesLoader::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION) {
        EventIdentificationHighlightLocation* idEvent = dynamic_cast<EventIdentificationHighlightLocation*>(event);
        CaretAssert(idEvent);
        
        switch (idEvent->getLoadFiberOrientationSamplesMode()) {
            case EventIdentificationHighlightLocation::LOAD_FIBER_ORIENTATION_SAMPLES_MODE_NO:
                break;
            case EventIdentificationHighlightLocation::LOAD_FIBER_ORIENTATION_SAMPLES_MODE_YES:
            {
                const Vector3D xyz(idEvent->getStereotaxicXYZ());
                m_lastIdentificationValid = true;
                m_lastIdentificationXYZ[0] = xyz[0];
                m_lastIdentificationXYZ[1] = xyz[1];
                m_lastIdentificationXYZ[2] = xyz[2];
            }
                break;
        }
    }
}

/**
 * Get the fiber orientation vectors for display on a sphere.
 *
 * @param brain
 *    Brain for which vectors are loaded.
 * @param xVectors
 *    Vectors for X-orientation.
 * @param yVectors
 *    Vectors for Y-orientation.
 * @param zVectors
 *    Vectors for Z-orientation.
 * @param fiberOrientation
 *    The nearby fiber orientation
 * @param errorMessageOut
 *    Will contain any error messages.
 *    This error message will only be set in some cases when there is an
 *    error.
 * @return
 *    True if data is valid, else false.
 */
bool
FiberOrientationSamplesLoader::getFiberOrientationSphericalSamplesVectors(Brain* brain,
                                                              std::vector<FiberOrientationSamplesVector>& xVectors,
                                                                  std::vector<FiberOrientationSamplesVector>& yVectors,
                                                                  std::vector<FiberOrientationSamplesVector>& zVectors,
                                                                  FiberOrientation* &fiberOrientationOut,
                                                                  AString& errorMessageOut)
{
    CaretAssert(brain);
    
    errorMessageOut = "";
    fiberOrientationOut = NULL;
    
    if (m_lastIdentificationValid) {
        if (loadSphericalOrientationVolumes(brain,
                                            errorMessageOut) == false) {
            return false;
        }
        
        if (brain->getNumberOfConnectivityFiberOrientationFiles() > 0) {
            CiftiFiberOrientationFile* cfof = brain->getConnectivityFiberOrientationFile(0);
            FiberOrientation* nearestFiberOrientation =
            cfof->getFiberOrientationNearestCoordinate(m_lastIdentificationXYZ, 3.0);
            if (nearestFiberOrientation != NULL) {
                fiberOrientationOut = nearestFiberOrientation;
            }
        }
        
        int64_t ijk[3];
        m_sampleThetaVolumes[0]->enclosingVoxel(m_lastIdentificationXYZ,
                                                ijk);
        if (m_sampleThetaVolumes[0]->indexValid(ijk)) {
            std::vector<int64_t> dims;
            m_sampleThetaVolumes[0]->getDimensions(dims);
            
            const int64_t numberOfOrientations = dims[3];
            xVectors.resize(numberOfOrientations);
            yVectors.resize(numberOfOrientations);
            zVectors.resize(numberOfOrientations);
            
            for (int32_t iAxis = 0; iAxis < 3; iAxis++) {
                for (int64_t iOrient = 0; iOrient < numberOfOrientations; iOrient++) {
                    const float theta = m_sampleThetaVolumes[iAxis]->getValue(ijk[0],
                                                                              ijk[1],
                                                                              ijk[2],
                                                                              iOrient,
                                                                              0);
                    const float phi = m_samplePhiVolumes[iAxis]->getValue(ijk[0],
                                                                          ijk[1],
                                                                          ijk[2],
                                                                          iOrient,
                                                                          0);
                    
                    const float magnitude = m_sampleMagnitudeVolumes[iAxis]->getValue(ijk[0],
                                                                                      ijk[1],
                                                                                      ijk[2],
                                                                                      iOrient,
                                                                                      0);
                    
                    switch (iAxis) {
                        case 0:
                        {
                            FiberOrientationSamplesVector& ov = xVectors[iOrient];
                            ov.direction[0] = -std::sin(theta) * std::cos(phi);
                            ov.direction[1] =  std::sin(theta) * std::sin(phi);
                            ov.direction[2] =  std::cos(theta);
                            ov.magnitude = magnitude;
                            ov.setColor();
                        }
                            break;
                        case 1:
                        {
                            FiberOrientationSamplesVector& ov = yVectors[iOrient];
                            ov.direction[0] = -std::sin(theta) * std::cos(phi);
                            ov.direction[1] =  std::sin(theta) * std::sin(phi);
                            ov.direction[2] =  std::cos(theta);
                            ov.magnitude = magnitude;
                            ov.setColor();
                        }
                            break;
                        case 2:
                        {
                            FiberOrientationSamplesVector& ov = zVectors[iOrient];
                            ov.direction[0] = -std::sin(theta) * std::cos(phi);
                            ov.direction[1] =  std::sin(theta) * std::sin(phi);
                            ov.direction[2] =  std::cos(theta);
                            ov.magnitude = magnitude;
                            ov.setColor();
                        }
                            break;
                    }
                }
            }
            
            return true;
        }
    }
    
    return false;
}

/**
 * Get the volumes containing the spherical orienations.
 *
 * @param brain
 *    Brain for which vectors are loaded.
 * @param magnitudeVolumesOut
 *    The volumes containing the magnitudes.
 * @param phiAngleVolumesOut
 *    The volumes containing the phi angles.
 * @param thetaAngleVolumesOut
 *    The volumes containing the theta angles.
 *
 */
bool
FiberOrientationSamplesLoader::loadSphericalOrientationVolumes(Brain* brain,
                                                               AString& errorMessageOut)
{
    errorMessageOut = "";
    
    FileInformation specFileInfo(brain->getSpecFile()->getFileName());
    const AString directoryName = specFileInfo.getPathName();
    
    if (m_sampleVolumesValid == false) {
        if (m_sampleVolumesLoadAttemptValid == false) {
            const AString filePrefix = "merged_";
            const AString fileSuffix = "samples.nii.gz";
            
            std::vector<VolumeFile*> allVolumes;
            
            for (int32_t i = 0; i < 3; i++) {
                m_sampleMagnitudeVolumes[i] = new VolumeFile();
                m_samplePhiVolumes[i]       = new VolumeFile();
                m_sampleThetaVolumes[i]     = new VolumeFile();
                
                const AString fileNumber = AString::number(i + 1);
                
                try {
                    const AString magFileName = (filePrefix
                                                 + "f"
                                                 + fileNumber
                                                 + fileSuffix);
                    FileInformation magFileInfo(directoryName,
                                                magFileName);
                    const AString magFilePath = magFileInfo.getAbsoluteFilePath();
                    m_sampleMagnitudeVolumes[i]->readFile(magFilePath);
                    allVolumes.push_back(m_sampleMagnitudeVolumes[i]);
                }
                catch (const DataFileException& dfe) {
                    if (errorMessageOut.isEmpty() == false) {
                        errorMessageOut += "\n";
                    }
                    errorMessageOut += dfe.whatString();
                }
                
                try {
                    const AString phiFileName = (filePrefix
                                                 + "ph"
                                                 + fileNumber
                                                 + fileSuffix);
                    FileInformation phiFileInfo(directoryName,
                                                phiFileName);
                    const AString phiFilePath = phiFileInfo.getAbsoluteFilePath();
                    m_samplePhiVolumes[i]->readFile(phiFilePath);
                    allVolumes.push_back(m_samplePhiVolumes[i]);
                }
                catch (const DataFileException& dfe) {
                    if (errorMessageOut.isEmpty() == false) {
                        errorMessageOut += "\n";
                    }
                    errorMessageOut += dfe.whatString();
                }
                
                try {
                    const AString thetaFileName = (filePrefix
                                                   + "th"
                                                   + fileNumber
                                                   + fileSuffix);
                    FileInformation thetaFileInfo(directoryName,
                                                  thetaFileName);
                    const AString thetaFilePath = thetaFileInfo.getAbsoluteFilePath();
                    m_sampleThetaVolumes[i]->readFile(thetaFilePath);
                    allVolumes.push_back(m_sampleThetaVolumes[i]);
                }
                catch (const DataFileException& dfe) {
                    if (errorMessageOut.isEmpty() == false) {
                        errorMessageOut += "\n";
                    }
                    errorMessageOut += dfe.whatString();
                }
            }
            
            if (errorMessageOut.isEmpty()) {
                std::vector<int64_t> dims;
                for (std::vector<VolumeFile*>::iterator iter = allVolumes.begin();
                     iter != allVolumes.end();
                     iter++) {
                    VolumeFile* vf = *iter;
                    std::vector<int64_t> volDims;
                    vf->getDimensions(volDims);
                    
                    if (dims.empty()) {
                        dims = volDims;
                    }
                    else if (dims != volDims) {
                        errorMessageOut += "ERROR: Sample volumes have mis-matched dimensions";
                    }
                }
                m_sampleVolumesValid = true;
            }
            
            m_sampleVolumesLoadAttemptValid = true;
        }
    }
    if (m_sampleVolumesValid) {
        return true;
    }
    
    return false;
}

