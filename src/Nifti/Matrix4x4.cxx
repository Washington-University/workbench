/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkBase64Utilities.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#include <cmath>

#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "XmlWriter.h"

using namespace caret;

static const bool INFO = false;

static const bool DEBUG = false;

static const float iDF = 1.0f;

static  const double SMALL_POSITIVE_NUMBER = 0.000001;

static  const double SMALL_NEGATIVE_NUMBER = -0.000001;

/**
 *
 * constructor that creates an identity matrix.
 *
 */
Matrix4x4::Matrix4x4()
    : CaretObject()
{
    this->initializeMembersMatrix4x4();
}

/**
 * Destructor
 */
Matrix4x4::~Matrix4x4()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
Matrix4x4::Matrix4x4(const Matrix4x4& o)
    : CaretObject(o)
{
    this->initializeMembersMatrix4x4();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
Matrix4x4&
Matrix4x4::operator=(const Matrix4x4& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
Matrix4x4::copyHelper(const Matrix4x4& o)
{
    this->setMatrix(o);
    this->dataSpaceName = o.dataSpaceName;
    this->transformedSpaceName = o.transformedSpaceName;
}

void
Matrix4x4::initializeMembersMatrix4x4()
{
    this->identity();
    this->dataSpaceName ="";
    this->transformedSpaceName ="";
    this->clearModified();
}
/**
 * Set the matrix to the identity matrix.
 *
 */
void
Matrix4x4::identity()
{
    matrix[0][0] = 1.0;
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[0][3] = 0.0;
    
    matrix[1][0] = 0.0;
    matrix[1][1] = 1.0;
    matrix[1][2] = 0.0;
    matrix[1][3] = 0.0;
    
    matrix[2][0] = 0.0;
    matrix[2][1] = 0.0;
    matrix[2][2] = 1.0;
    matrix[2][3] = 0.0;
    
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;
    
    this->setModified();
}

/**
 * Get the translation from the matrix.
 *
 * param  The translation as an array of three floats.
 *
 */
void
Matrix4x4::getTranslation(float translatationOut[3]) const
{
    translatationOut[0] = matrix[0][3];
    translatationOut[1] = matrix[1][3];
    translatationOut[2] = matrix[2][3];
}

/**
 * Set (replace) the matrix's translation.
 *
 * @param t  An array of three float containing the translation.
 *
 */
void
Matrix4x4::setTranslation(const float t[3])
{
    matrix[0][3] = t[0];
    matrix[1][3] = t[1];
    matrix[2][3] = t[2];
    this->setModified();
}

/**
 * Set (replace) the matrix's translation.
 *
 * @param tx  The translation along the X-Axis.
 * @param ty  The translation along the Y-Axis.
 * @param tz  The translation along the Z-Axis.
 *
 */
void
Matrix4x4::setTranslation(
                   const double tx,
                   const double ty,
                   const double tz)
{
    matrix[0][3] = tx;
    matrix[1][3] = ty;
    matrix[2][3] = tz;
    this->setModified();
}

/**
 *
 * Apply a translation by multiplying the matrix by a matrix
 * containing the specified translation.  Translates in the
 * screen' coordinate system.
 *
 * @param tx  The translation along the X-Axis.
 * @param ty  The translation along the Y-Axis.
 * @param tz  The translation along the Z-Axis.
 *
 */
void
Matrix4x4::translate(
                   const double tx,
                   const double ty,
                   const double tz)
{
    Matrix4x4 cm;
    cm.setTranslation(tx, ty, tz);
    postmultiply(cm);
    this->setModified();
}

/**
 * Apply scaling by multiplying the matrix by a matrix
 * containing the specified scaling.  Translates in the
 * screen' coordinate system.
 *
 * @param sx  The scaling along the X-Axis.
 * @param sy  The scaling along the Y-Axis.
 * @param sz  The scaling along the Z-Axis.
 *
 */
void
Matrix4x4::scale(
                   const double sx,
                   const double sy,
                   const double sz)
{
    Matrix4x4 cm;
    cm.matrix[0][0] = sx;
    cm.matrix[1][1] = sy;
    cm.matrix[2][2] = sz;
    postmultiply(cm);
    this->setModified();
}

/**
 * Apply a rotation about the X-axis.  Rotates in the
 * screen's coordinate system.
 * 
 * @param  degrees  Amount to rotate in degrees.
 *
 */
void
Matrix4x4::rotateX(const double degrees)
{
    rotate(degrees, 1.0, 0.0, 0.0);
}

/**
 * Apply a rotation about the Y-axis.  Rotates in the
 * screen's coordinate system.
 * 
 * @param  degrees  Amount to rotate in degrees.
 *
 */
void
Matrix4x4::rotateY(const double degrees)
{
    rotate(degrees, 0.0, 1.0, 0.0);
}

/**
 * Apply a rotation about the Z-axis.  Rotates in the
 * screen's coordinate system.
 * 
 * @param  degrees  Amount to rotate in degrees.
 *
 */
void
Matrix4x4::rotateZ(const double degrees)
{
    rotate(degrees, 0.0, 0.0, 1.0);
}

/**
 * Rotate angle degrees about the vector.
 * @param angle - Angle of rotation.
 * @param vector - Vector about which rotation occurs.
 *
 */
void
Matrix4x4::rotate(
                   const double angle,
                   const double vector[4])
{
    this->rotate(angle, vector[0], vector[1], vector[2]);
    this->setModified();
}

/**
 * Rotate <I>angle</I> degrees about the vector <I>(x,y,z)</I>.
 * 
 * @param  angle  Amount to rotate in degrees.
 * @param  xin      X-component of the vector.
 * @param  yin      Y-component of the vector.
 * @param  zin      Z-component of the vector.
 *
 */
void
Matrix4x4::rotate(
                   const double angleIn,
                   const double xin,
                   const double yin,
                   const double zin)
{
    /// from vtkTransformConcatenation::Rotate()
    float angle = angleIn;
    if (angle == 0.0 || (xin == 0.0 && yin == 0.0 && zin == 0.0)) {
        return;
    }
    
    // convert to radians
    angle = MathFunctions::toRadians(angle);
    
    // make a normalized quaternion
    double w = std::cos(0.5*angle);
    double f = std::sin(0.5*angle) / std::sqrt(xin*xin+yin*yin+zin*zin);
    double x = xin * f;
    double y = yin * f;
    double z = zin * f;
    
    
    double ww = w*w;
    double wx = w*x;
    double wy = w*y;
    double wz = w*z;
    
    double xx = x*x;
    double yy = y*y;
    double zz = z*z;
    
    double xy = x*y;
    double xz = x*z;
    double yz = y*z;
    
    double s = ww - xx - yy - zz;
    
    // convert the quaternion to a matrix
    Matrix4x4 m;
    m.matrix[0][0] = xx*2.0 + s;
    m.matrix[1][0] = (xy + wz)*2;
    m.matrix[2][0] = (xz - wy)*2;
    m.matrix[0][1] = (xy - wz)*2;
    m.matrix[1][1] = yy*2.0 + s;
    m.matrix[2][1] = (yz + wx)*2;
    m.matrix[0][2] = (xz + wy)*2;
    m.matrix[1][2] = (yz - wx)*2;
    m.matrix[2][2] = zz*2.0 + s;
    m.fixNumericalError();
    
    postmultiply(m);
    
    this->fixNumericalError();
    this->setModified();
}

/**
 * Premultiply by a matrix.
 *
 * @param  tm  Matrix that is used for pre-multiplication.
 *
 */
void
Matrix4x4::premultiply(const Matrix4x4& tm)
{
    double matrixOut[4][4];
    for (int row = 0; row < 4; row++) {
        matrixOut[row][0] = matrix[row][0] * tm.matrix[0][0]
        + matrix[row][1] * tm.matrix[1][0]
        + matrix[row][2] * tm.matrix[2][0]
        + matrix[row][3] * tm.matrix[3][0];
        matrixOut[row][1] = matrix[row][0] * tm.matrix[0][1]
        + matrix[row][1] * tm.matrix[1][1]
        + matrix[row][2] * tm.matrix[2][1]
        + matrix[row][3] * tm.matrix[3][1];
        matrixOut[row][2] = matrix[row][0] * tm.matrix[0][2]
        + matrix[row][1] * tm.matrix[1][2]
        + matrix[row][2] * tm.matrix[2][2]
        + matrix[row][3] * tm.matrix[3][2];
        matrixOut[row][3] = matrix[row][0] * tm.matrix[0][3]
        + matrix[row][1] * tm.matrix[1][3]
        + matrix[row][2] * tm.matrix[2][3]
        + matrix[row][3] * tm.matrix[3][3];
    }
    setMatrix(matrixOut);
    this->fixNumericalError();
    this->setModified();
}

/**
 * Postmultiply by a matrix.
 *
 * @param  tm  Matrix that is used for post-multiplication.
 *
 */
void
Matrix4x4::postmultiply(const Matrix4x4& tm)
{
    double matrixOut[4][4];
    for (int row = 0; row < 4; row++) {
        matrixOut[row][0] = tm.matrix[row][0] * matrix[0][0]
        + tm.matrix[row][1] * matrix[1][0]
        + tm.matrix[row][2] * matrix[2][0]
        + tm.matrix[row][3] * matrix[3][0];
        matrixOut[row][1] = tm.matrix[row][0] * matrix[0][1]
        + tm.matrix[row][1] * matrix[1][1]
        + tm.matrix[row][2] * matrix[2][1]
        + tm.matrix[row][3] * matrix[3][1];
        matrixOut[row][2] = tm.matrix[row][0] * matrix[0][2]
        + tm.matrix[row][1] * matrix[1][2]
        + tm.matrix[row][2] * matrix[2][2]
        + tm.matrix[row][3] * matrix[3][2];
        matrixOut[row][3] = tm.matrix[row][0] * matrix[0][3]
        + tm.matrix[row][1] * matrix[1][3]
        + tm.matrix[row][2] * matrix[2][3]
        + tm.matrix[row][3] * matrix[3][3];
    }
    setMatrix(matrixOut);
    this->fixNumericalError();
    this->setModified();
}

/**
 * Get the matrix as 16 element one-dimensional array for use by OpenGL.
 *
 * @param  m  A 16-element array of double.
 *
 */
void
Matrix4x4::getMatrixForOpenGL(double m[16]) const
{
    m[0]  = this->matrix[0][0];
    m[1]  = this->matrix[1][0];
    m[2]  = this->matrix[2][0];
    m[3]  = this->matrix[3][0];
    
    m[4]  = this->matrix[0][1];
    m[5]  = this->matrix[1][1];
    m[6]  = this->matrix[2][1];
    m[7] = this->matrix[3][1];
    
    m[8]  = this->matrix[0][2];
    m[9]  = this->matrix[1][2];
    m[10] = this->matrix[2][2];
    m[11] = this->matrix[3][2];
    
    m[12]  = this->matrix[0][3];
    m[13]  = this->matrix[1][3];
    m[14] = this->matrix[2][3];
    m[15] = this->matrix[3][3];
}

/**
 * Set the matrix from a one-dimensional OpenGL Matrix as 16 elements.
 *
 * @param  m  A 16-element array of double containing OpenGL Matrix.
 *
 */
void
Matrix4x4::setMatrixFromOpenGL(const double m[16])
{
    this->matrix[0][0] = m[0];
    this->matrix[1][0] = m[1];
    this->matrix[2][0] = m[2];
    this->matrix[3][0] = m[3];
    
    this->matrix[0][1] = m[4];
    this->matrix[1][1] = m[5];
    this->matrix[2][1] = m[6];
    this->matrix[3][1] = m[7];
    
    this->matrix[0][2] = m[8];
    this->matrix[1][2] = m[9];
    this->matrix[2][2] = m[10];
    this->matrix[3][2] = m[11];
    
    this->matrix[0][3] = m[12];
    this->matrix[1][3] = m[13];
    this->matrix[2][3] = m[14];
    this->matrix[3][3] = m[15];
    this->setModified();
}

/**
 * Transpose the matrix.
 *
 */
void
Matrix4x4::transpose()
{
    double m[4][4];
    m[0][0] = matrix[0][0];
    m[0][1] = matrix[1][0];
    m[0][2] = matrix[2][0];
    m[0][3] = matrix[3][0];
    
    m[1][0] = matrix[0][1];
    m[1][1] = matrix[1][1];
    m[1][2] = matrix[2][1];
    m[1][3] = matrix[3][1];
    
    m[2][0] = matrix[0][2];
    m[2][1] = matrix[1][2];
    m[2][2] = matrix[2][2];
    m[2][3] = matrix[3][2];
    
    m[3][0] = matrix[0][3];
    m[3][1] = matrix[1][3];
    m[3][2] = matrix[2][3];
    m[3][3] = matrix[3][3];
    setMatrix(m);
    this->setModified();
}

/**
 * Set the matrix.
 *
 * @param  m  A 4x4 array of doubles.
 *
 */
void
Matrix4x4::setMatrix(const double m[4][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = m[i][j];
        }
    }
    this->setModified();
}

/**
 * Get the matrix.
 *
 * @param  m  A 4x4 array of doubles.
 *
 */
void
Matrix4x4::getMatrix(double m[4][4]) const
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = matrix[i][j];
        }
    }
}

/**
 * Set the matrix.
 *
 * @param  cm  A Matrix.
 *
 */
void
Matrix4x4::setMatrix(const Matrix4x4& cm)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = cm.matrix[i][j];
        }
    }
    this->setModified();
}

void
Matrix4x4::multiplyPoint3(float p[3]) const
{
        float pout[3] = { 0.0f, 0.0f, 0.0f };
        for (int row = 0; row < 3; row++) {
            pout[row] = (float)(this->matrix[row][0] * p[0]
                                + this->matrix[row][1] * p[1]
                                + this->matrix[row][2] * p[2]
                                + this->matrix[row][3]);
        }
        p[0] = pout[0];
        p[1] = pout[1];
        p[2] = pout[2];
}

void
Matrix4x4::multiplyPoint4(float p[4]) const
{
        float pout[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        for (int row = 0; row < 4; row++) {
            pout[row] = (float)(this->matrix[row][0] * p[0]
                                + this->matrix[row][1] * p[1]
                                + this->matrix[row][2] * p[2]
                                + this->matrix[row][3] * p[3]);
        }
        p[0] = pout[0];
        p[1] = pout[1];
        p[2] = pout[2];
        p[3] = pout[3];
}

void
Matrix4x4::multiplyPoint3X3(float p[3]) const
{
    float pout[3] = { 0.0f, 0.0f, 0.0f };
    for (int row = 0; row < 3; row++) {
        pout[row] = (float)(this->matrix[row][0] * p[0]
                            + this->matrix[row][1] * p[1]
                            + this->matrix[row][2] * p[2]);
    }
    p[0] = pout[0];
    p[1] = pout[1];
    p[2] = pout[2];
}

/**
 * Get the data space name (used by GIFTI).
 *
 * @return  Name of data space.
 *
 */
QString
Matrix4x4::getDataSpaceName() const
{
    return dataSpaceName;
}

/**
 * Set the data space name (used by GIFTI).
 *
 * @param  name  Name of data space.
 *
 */
void
Matrix4x4::setDataSpaceName(const QString& name)
{
    dataSpaceName = name;
    this->setModified();
}

/**
 * Get the transformed space name (used by GIFTI).
 *
 * @return  Name of transformed space.
 *
 */
QString
Matrix4x4::getTransformedSpaceName() const
{
    return transformedSpaceName;
}

/**
 * Set the transformed space name (used by GIFTI).
 *
 * @param  name  Name of transformed space.
 *
 */
void
Matrix4x4::setTransformedSpaceName(const QString& name)
{
    transformedSpaceName = name;
    this->setModified();
}

/**
 * Set a matrix element.
 * @param i   Row
 * @param j   Column
 * @return   value at [i][j]
 *
 */
double
Matrix4x4::getMatrixElement(
                   const int32_t i,
                   const int32_t j) const
{
    return this->matrix[i][j];
}

/**
 * Set a matrix element.
 * @param i   Row
 * @param j   Column
 * @param e   New Value
 *
 */
void
Matrix4x4::setMatrixElement(
                   const int32_t i,
                   const int32_t j,
                   const double e)
{
    this->matrix[i][j] = e;
    this->setModified();
}

/**
 * Determine if two matrices are approximately equal.
 * @param m -  matrix to compare.
 * @param error - Maximum difference between a matrix element.
 * @return  true if all corresponding elements differ by less than "error",
 *          else false.
 *
 */
bool
Matrix4x4::compare(
                   const Matrix4x4& m,
                   const float error)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float diff = (float)std::abs(this->matrix[i][j] - m.matrix[i][j]);
            if (diff > error) {
                return false;
            }
        }
    }
    return true;
}

double
Matrix4x4::fixZero(const double f)
{
    if (f > SMALL_POSITIVE_NUMBER) {
        return f;
    }
    else if (f < SMALL_NEGATIVE_NUMBER) {
        return f;
    }
    
    return 0.0;
}

/**
 * Fix numerical error such as very tiny numbers and "negative zeros".
 *
 */
void
Matrix4x4::fixNumericalError()
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            double f = fixZero(this->matrix[i][j]);
            this->matrix[i][j] = f;
        }
    }
    this->setModified();
}

/**
 * Invert a matrix.
 * @return true if inversion of matrix is successful,
 * else false.
 *
 *
bool
Matrix4x4::invert()
{
    bool valid = false;
    double[][] m = Matrix4x4.Inverse(this->matrix, &valid);
    if (m == null) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            this->matrix[i][j] = m[i][j];
        }
    }
    this->setModified();
}
catch (Exception e) {
    NiftiLogger.getLogger().severe(e.getMessage());
}
this->fixNumericalError();

return true;
}
*/
/* ---------------------------------------------------------------------------
 *  Matrix Calculator - Java Application Version
 *  Original Date: October 1997
 *  Updated Swing GUI: June 2002
 *
 *  Version      : $Revision: 1.6 $
 *  Last Modified: $Date: 2005/06/30 06:28:10 $
 *
 *  Author: Marcus Kazmierczak
 *          marcus@mkaz.com
 *          http://www.mkaz.com/math/
 *
 *  Copyright (c) 1997-2002 mkaz.com
 *  Published under a BSD Open Source License
 *  More Info: http://mkaz.com/software/mklicense.html
 *
 *  Edited by Pierre Seguin on june 30, 2005
 *  seguin_pierre@yahoo.ca
 *
 *  ---------------------------------------------------------------------------
 *
void
Matrix4x4::Transpose(double matrix[4][4])
{
    for (int i = 0; i < a.length; i++)
        for (int j = 0; j < a[i].length; j++)
            output[j][i] = input[i][j];
        }
    }
}
*/
bool
Matrix4x4::Inverse(const double a[4][4], double matrixOut[4][4]) const
{
    // Formula used to Calculate Inverse:
    // inv(A) = 1/det(A) * adj(A)
//    if (INFO) {
//        System.out.println("Performing Inverse...");
//    }
    int tms = 4;
    
    double mm[4][4];
    Adjoint(a, mm);
    
    double det = Determinant4(a);
    double dd = 0;
    
    if (det == 0) {
        //if (INFO) {
        //    System.out.println("Determinant Equals 0, Not Invertible.");
        //    return null;
        //}
        return false;
    } else {
        dd = 1 / det;
    }
    
    for (int i = 0; i < tms; i++)
        for (int j = 0; j < tms; j++) {
            matrixOut[i][j] = dd * mm[i][j];
        }
    
    return true;
}

void
Matrix4x4::Adjoint(const double inputMatrix[4][4], double outputMatrix[4][4]) const
{
//    if (INFO) {
//        System.out.println("Performing Adjoint...");
//    }
    const int tms = 4;
    
    double a[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            a[i][j] = inputMatrix[i][j];
        }
    }
    double m[4][4];
    
    int ii, jj, ia, ja;
    double det;
    
    for (int i = 0; i < tms; i++)
        for (int j = 0; j < tms; j++) {
            ia = ja = 0;
            
            double ap[3][3];
            
            for (ii = 0; ii < tms; ii++) {
                for (jj = 0; jj < tms; jj++) {
                    
                    if ((ii != i) && (jj != j)) {
                        ap[ia][ja] = a[ii][jj];
                        ja++;
                    }
                    
                }
                if ((ii != i) && (jj != j)) {
                    ia++;
                }
                ja = 0;
            }
            
            det = Determinant3(ap);
            m[i][j] = std::pow((float)-1, (float)(i + j)) * det;
        }
    
    Matrix4x4 mout;
    mout.setMatrix(m);
    mout.transpose();
    mout.getMatrix(outputMatrix);
}

void
Matrix4x4::UpperTriangle3(const double inputMatrix[3][3], double outputMatrix[3][3]) const
{
    double f1 = 0;
    double temp = 0;
    int tms = 3; //m.length; // get This Matrix Size (could be smaller than
    // global)
    int v = 1;
    
    int iDF = 1;
    
    double m[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m[i][j] = inputMatrix[i][j];
        }
    }
    
    for (int col = 0; col < tms - 1; col++) {
        for (int row = col + 1; row < tms; row++) {
            v = 1;
            
        outahere: while (m[col][col] == 0) // check if 0 in diagonal
        { // if so switch until not
            if (col + v >= tms) // check if switched all rows
            {
                iDF = 0;
                goto outahere;
            } else {
                for (int c = 0; c < tms; c++) {
                    temp = m[col][c];
                    m[col][c] = m[col + v][c]; // switch rows
                    m[col + v][c] = temp;
                }
                v++; // count row switchs
                iDF = iDF * -1; // each switch changes determinant
                // factor
            }
        }
            
            if (m[col][col] != 0) {
                /*
                if (DEBUG) {
                    System.out.println("tms = " + tms + "   col = " + col
                                       + "   row = " + row);
                }
                */
                
                    double bottom = m[col][col];
                    if (bottom != 0.0) {
                        f1 = (-1) * m[row][col] / m[col][col];
                        for (int i = col; i < tms; i++) {
                            m[row][i] = f1 * m[col][i] + m[row][i];
                        }
                    }
                    else {
                       // std::cout << "Still Here!!!" std::endl;
                    }
                
            }
            
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            outputMatrix[i][j] = m[i][j];
        }
    }
}

void
Matrix4x4::UpperTriangle4(const double inputMatrix[4][4], double outputMatrix[4][4]) const
{
    double f1 = 0;
    double temp = 0;
    int tms = 4; //m.length; // get This Matrix Size (could be smaller than
    // global)
    int v = 1;
    
    int iDF = 1;
    
    double m[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = inputMatrix[i][j];
        }
    }
    
    for (int col = 0; col < tms - 1; col++) {
        for (int row = col + 1; row < tms; row++) {
            v = 1;
            
        outahere: while (m[col][col] == 0) // check if 0 in diagonal
        { // if so switch until not
            if (col + v >= tms) // check if switched all rows
            {
                iDF = 0;
                goto outahere;
            } else {
                for (int c = 0; c < tms; c++) {
                    temp = m[col][c];
                    m[col][c] = m[col + v][c]; // switch rows
                    m[col + v][c] = temp;
                }
                v++; // count row switchs
                iDF = iDF * -1; // each switch changes determinant
                // factor
            }
        }
            
            if (m[col][col] != 0) {
                /*
                 if (DEBUG) {
                 System.out.println("tms = " + tms + "   col = " + col
                 + "   row = " + row);
                 }
                 */
                
                double bottom = m[col][col];
                if (bottom != 0.0) {
                    f1 = (-1) * m[row][col] / m[col][col];
                    for (int i = col; i < tms; i++) {
                        m[row][i] = f1 * m[col][i] + m[row][i];
                    }
                }
                else {
                    // std::cout << "Still Here!!!" std::endl;
                }
                
            }
            
        }
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            outputMatrix[i][j] = m[i][j];
        }
    }
}

double
Matrix4x4::Determinant4(const double matrixIn[4][4]) const
{
//    if (INFO) {
//        System.out.println("Getting Determinant...");
//    }
    double matrix[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = matrixIn[i][j];
        }
    }
    int tms = 4; //matrix.length;
    
    double det = 1;
    
    double uppTriMatrix[4][4];
    UpperTriangle4(matrix, uppTriMatrix);
    
    for (int i = 0; i < tms; i++) {
        det = det * uppTriMatrix[i][i];
    } // multiply down diagonal
    
    det = det * iDF; // adjust w/ determinant factor
    
//    if (INFO) {
//        System.out.println("Determinant: " + det);
//    }
    return det;
}
double
Matrix4x4::Determinant3(const double matrixIn[3][3]) const
{
    //    if (INFO) {
    //        System.out.println("Getting Determinant...");
    //    }
    double matrix[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = matrixIn[i][j];
        }
    }
    int tms = 3; //matrix.length;
    
    double det = 1;
    
    double uppTriMatrix[3][3];
    UpperTriangle3(matrix, uppTriMatrix);
    
    for (int i = 0; i < tms; i++) {
        det = det * uppTriMatrix[i][i];
    } // multiply down diagonal
    
    det = det * iDF; // adjust w/ determinant factor
    
    //    if (INFO) {
    //        System.out.println("Determinant: " + det);
    //    }
    return det;
}

/**
 * Write the metadata in GIFTI XML format.
 *
 * @param xmlWriter - output stream
 * @throws XmlException if an error occurs while writing.
 */
void
Matrix4x4::writeAsXML(XmlWriter& xmlWriter) throw (XmlException)
{
}

/**
 * Convert the matrix into a string representation.
 *
 * @return  String representation of the matrix.
 *
 */
QString
Matrix4x4::toString() const
{
    return "Matrix4x4";
}

/**
 * Get a nicely formatted string for printing.
 *
 * @param indentation - use as indentation.
 * @return  String containing label information.
 *
 */
QString
Matrix4x4::toFormattedString(const QString& indentation)
{
    return "Matrix4x4";
}

/**
 * Set this object has been modified.
 *
 */
void
Matrix4x4::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
Matrix4x4::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Get the modification status.  Returns true if this object or
 * any of its children have been modified.
 * @return - The modification status.
 *
 */
bool
Matrix4x4::isModified() const
{
    return this->modifiedFlag;
}

