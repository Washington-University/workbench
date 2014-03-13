
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
#include <iostream>

#define __FIBER_DECLARE__
#include "Fiber.h"
#undef __FIBER_DECLARE__

#include "AString.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"

using namespace caret;


    
/**
 * \class caret::Fiber 
 * \brief Data for a single fiber
 */

/**
 * Constructor.
 */
Fiber::Fiber(const float* pointerToData)
{
    m_opacityForDrawing = 1.0;
    
    /*
     * Retrieve values from the file
     */
    m_meanF = pointerToData[0];
    m_varF  = pointerToData[1];
    m_theta = pointerToData[2];
    m_phi   = pointerToData[3];
    m_k1    = pointerToData[4];
    m_k2    = pointerToData[5];
    m_psi   = pointerToData[6];

    /*
     * Validate inputs
     */
    if (m_meanF != m_meanF) {
        m_invalidMessage += " meanF=NaN";
    }
    if (m_varF != m_varF) {
        m_invalidMessage += " varF=NaN";
    }
    if (m_theta != m_theta) {
        m_invalidMessage += " theta=NaN";
    }
    if (m_phi != m_phi) {
        m_invalidMessage += " phi=NaN";
    }
    if (m_k1 != m_k1) {
        m_invalidMessage += " k1=NaN";
    }
    else if (m_k1 < 0.0) {
        m_invalidMessage += " k1=negative=" + QString::number(m_k1);
        
    }
    if (m_k2 != m_k2) {
        m_invalidMessage += " k2=NaN";
    }
    else if (m_k1 < 0.0) {
        m_invalidMessage += " k2=negative=" + QString::number(m_k2);
        
    }
    if (m_psi != m_psi) {
        m_invalidMessage += " psi=NaN";
    }
    m_valid = m_invalidMessage.isEmpty();

    if (m_valid) {
        /*
         * Set computed values used for visualization
         */
        
        m_fanningMajorAxisAngle = fanningEigenvalueToAngle(m_k1);
        m_fanningMinorAxisAngle = fanningEigenvalueToAngle(m_k2);
        
        /*
         * m_theta is angle from Positive-Z Axis rotated about a line
         * in the XY-Plane
         *
         * m_phi is angle from positive X-Axis rotated about Z-Axis
         * looking to negative Z.
         *
         * NOTE: 'X' is in radiological space (positive X is left)
         * so flip the sign of the X-coordinate.
         */
        m_directionUnitVector[0] = -std::sin(m_theta) * std::cos(m_phi);
        m_directionUnitVector[1] =  std::sin(m_theta) * std::sin(m_phi);
        m_directionUnitVector[2] =  std::cos(m_theta);
        
        /*
         * Use absolute values of directional unit vector as RGB color components.
         */
        m_directionUnitVectorRGB[0] = std::fabs(m_directionUnitVector[0]);
        m_directionUnitVectorRGB[1] = std::fabs(m_directionUnitVector[1]);
        m_directionUnitVectorRGB[2] = std::fabs(m_directionUnitVector[2]);

//        const float nearZero = 0.0001;
//        float oppositePhi = 0.0;
//        if (std::fabs(m_directionUnitVector[0]) > nearZero) {
//            oppositePhi = M_PI - std::atan2(-m_directionUnitVector[1],
//                                              -m_directionUnitVector[0]);
//        }
//        else {
//            oppositePhi = M_PI_2;
//        }
//        
//        float oppositeTheta = std::acos(-m_directionUnitVector[2]);
//        
//        const float radiansToDegrees = 180.0 / M_PI;
//            std::cout << "Vector: " << qPrintable(AString::fromNumbers(m_directionUnitVector, 3, ",")) << std::endl;
//            std::cout << "Theta/OppTheta/Phi/OppPhi: "
//            << (m_theta * radiansToDegrees) << " "
//            << (oppositeTheta * radiansToDegrees) << " "
//            << (m_phi * radiansToDegrees) << " "
//            << (oppositePhi * radiansToDegrees) << std::endl << std::endl;
    }
}

void
vectorToAngles(const float vector[3],
                    float& azimuthOut,
                    float& elevationOut) {
    const float nearZero = 0.001;
    azimuthOut = 0.0;
    if (std::fabs(vector[0]) > nearZero) {
        azimuthOut = std::atan2(vector[1],
                                 vector[0]);
    }
    else if (vector[1] > nearZero) {
        azimuthOut = M_PI_2;
    }
    else if (vector[1] < -nearZero) {
        azimuthOut = -M_PI_2;
    }
    
    if (azimuthOut > M_PI_2) azimuthOut -= M_PI_2;
    else if (azimuthOut < -M_PI_2) azimuthOut += M_PI_2;
    
    elevationOut = std::acos(vector[2]);
}

/**
 * Destructor.
 */
Fiber::~Fiber()
{
}

/**
 * Convert the fanning eigenvalue to an angle.
 * @param eigenvalue
 *    The eigenvalue
 * @return
 *    Angle derived from the eigenvalue.
 */
float
Fiber::fanningEigenvalueToAngle(const float eigenvalue)
{
    float angle = 0.0;
    
    if (eigenvalue > 0.0) {
        float sigma = 1.0 / std::sqrt(2.0 * eigenvalue);
        if (sigma > 1.0) sigma = 1.0;
        angle = std::asin(sigma);
        angle = angle / 2;
        if (angle < 0.0) {
            angle = -angle;
        }
    }
    else {
        CaretLogSevere("Have a negative eigenvalue="
                       + AString::number(eigenvalue)
                       + " for a fiber.");
    }
    
    return angle;
}

