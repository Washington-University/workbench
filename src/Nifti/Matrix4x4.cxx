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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkBase64Utilities.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

 Program:   Visualization Toolkit
 Module:    $RCSfile: vtkMatrix4x4.cxx,v $
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/



#include <cmath>
#include <iostream>
#include <limits>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ControlPoint3D.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "NiftiEnums.h"
#include "XmlWriter.h"

using namespace caret;

//static const bool INFO = false;

//static const bool DEBUG = false;

//static const float iDF = 1.0f;

static  const double SMALL_POSITIVE_NUMBER = 0.000001;

static  const double SMALL_NEGATIVE_NUMBER = -0.000001;

/**
 *
 * constructor that creates an identity matrix.
 *
 */
Matrix4x4::Matrix4x4()
    : Matrix4x4Interface(),
      CaretObject()
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
    : Matrix4x4Interface(o),
      CaretObject(o)
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
    this->dataSpaceName = NiftiTransformEnum::toName(NiftiTransformEnum::NIFTI_XFORM_TALAIRACH);
    this->transformedSpaceName = NiftiTransformEnum::toName(NiftiTransformEnum::NIFTI_XFORM_TALAIRACH);
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
 *
 * Apply a translation by multiplying the matrix by a matrix
 * containing the specified translation.  Translates in the
 * screen' coordinate system.
 *
 * @param txyz  The translation along the XYZ-Axis.
 */
void
Matrix4x4::translate(const float txyz[3])
{
    translate(txyz[0], txyz[1], txyz[2]);
}

/**
 *
 * Apply a translation by multiplying the matrix by a matrix
 * containing the specified translation.  Translates in the
 * screen' coordinate system.
 *
 * @param txyz  The translation along the XYZ-Axis.
 */
void
Matrix4x4::translate(const double txyz[3])
{
    translate(txyz[0], txyz[1], txyz[2]);
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
 * Get the scaling from the matrix.
 * @param scaleOutX
 *    X scaling output.
 * @param scaleOutY
 *    Y scaling output.
 * @param scaleOutZ
 *    Z scaling output.
 */
void
Matrix4x4::getScale(double& scaleOutX,
                    double& scaleOutY,
                    double& scaleOutZ) const
{
    double U[3][3], VT[3][3];
    
    for (int i = 0; i < 3; i++)
    {
        U[0][i] = matrix[0][i];
        U[1][i] = matrix[1][i];
        U[2][i] = matrix[2][i];
    }
    
    double scale[3];
    Matrix4x4::SingularValueDecomposition3x3(U, U, scale, VT);

    scaleOutX = scale[0];
    scaleOutY = scale[1];
    scaleOutZ = scale[2];
}

//----------------------------------------------------------------------------
// Perform singular value decomposition on the matrix A:
//    A = U * W * VT
// where U and VT are orthogonal W is diagonal (the diagonal elements
// are returned in vector w).
// The matrices U and VT will both have positive determinants.
// The scale factors w are ordered according to how well the
// corresponding eigenvectors (in VT) match the x, y and z axes
// respectively.
//
// The singular value decomposition is used to decompose a linear
// transformation into a rotation, followed by a scale, followed
// by a second rotation.  The scale factors w will be negative if
// the determinant of matrix A is negative.
//
// Contributed by David Gobbi (dgobbi@irus.rri.on.ca)
void
Matrix4x4::SingularValueDecomposition3x3(const double A[3][3],
                                             double U[3][3], double w[3],
                                             double VT[3][3])
{
    int i;
    double B[3][3];
    
    // copy so that A can be used for U or VT without risk
    for (i = 0; i < 3; i++)
    {
        B[0][i] = A[0][i];
        B[1][i] = A[1][i];
        B[2][i] = A[2][i];
    }
    
    // temporarily flip if determinant is negative
    double d = Determinant3x3(B);
    if (d < 0)
    {
        for (i = 0; i < 3; i++)
        {
            B[0][i] = -B[0][i];
            B[1][i] = -B[1][i];
            B[2][i] = -B[2][i];
        }
    }
    
    // orthogonalize, diagonalize, etc.
    Orthogonalize3x3(B, U);
    Transpose3x3(B, B);
    Multiply3x3(B, U, VT);
    Diagonalize3x3(VT, w, VT);
    Multiply3x3(U, VT, U);
    Transpose3x3(VT, VT);
    
    // re-create the flip
    if (d < 0)
    {
        w[0] = -w[0];
        w[1] = -w[1];
        w[2] = -w[2];
    }
    
    /* paranoia check: recombine to ensure that the SVD is correct
     vtkMath::Transpose3x3(B, B);
     
     if (d < 0)
     {
     for (i = 0; i < 3; i++)
     {
     B[0][i] = -B[0][i];
     B[1][i] = -B[1][i];
     B[2][i] = -B[2][i];
     }
     }
     
     int j;
     T2 maxerr = 0;
     T2 tmp;
     T2 M[3][3];
     T2 W[3][3];
     vtkMath::Identity3x3(W);
     W[0][0] = w[0]; W[1][1] = w[1]; W[2][2] = w[2];
     vtkMath::Identity3x3(M);
     vtkMath::Multiply3x3(M, U, M);
     vtkMath::Multiply3x3(M, W, M);
     vtkMath::Multiply3x3(M, VT, M);
     
     for (i = 0; i < 3; i++)
     {
     for (j = 0; j < 3; j++)
     {
     if ((tmp = fabs(B[i][j] - M[i][j])) > maxerr)
     {
     maxerr = tmp;
     }
     }
     }
     
     vtkGenericWarningMacro("SingularValueDecomposition max error = " << maxerr);
     */
}

void
Matrix4x4::Diagonalize3x3(const double A[3][3], double w[3], double V[3][3])
{
    int i,j,k,maxI;
    double tmp, maxVal;
    
    // do the matrix[3][3] to **matrix conversion for Jacobi
    double C[3][3];
    double *ATemp[3],*VTemp[3];
    for (i = 0; i < 3; i++)
    {
        C[i][0] = A[i][0];
        C[i][1] = A[i][1];
        C[i][2] = A[i][2];
        ATemp[i] = C[i];
        VTemp[i] = V[i];
    }
    
    // diagonalize using Jacobi
    Matrix4x4::JacobiN(ATemp,3,w,VTemp);
    
    // if all the eigenvalues are the same, return identity matrix
    if (w[0] == w[1] && w[0] == w[2])
    {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                V[i][j] = 0.0;
            }
        }
        return;
    }
    
    // transpose temporarily, it makes it easier to sort the eigenvectors
    Transpose3x3(V,V);
    
    // if two eigenvalues are the same, re-orthogonalize to optimally line
    // up the eigenvectors with the x, y, and z axes
    for (i = 0; i < 3; i++)
    {
        if (w[(i+1)%3] == w[(i+2)%3]) // two eigenvalues are the same
        {
            // find maximum element of the independant eigenvector
            maxVal = fabs(V[i][0]);
            maxI = 0;
            for (j = 1; j < 3; j++)
            {
                if (maxVal < (tmp = fabs(V[i][j])))
                {
                    maxVal = tmp;
                    maxI = j;
                }
            }
            // swap the eigenvector into its proper position
            if (maxI != i)
            {
                tmp = w[maxI];
                w[maxI] = w[i];
                w[i] = tmp;
                SwapVectors3(V[i],V[maxI]);
            }
            // maximum element of eigenvector should be positive
            if (V[maxI][maxI] < 0)
            {
                V[maxI][0] = -V[maxI][0];
                V[maxI][1] = -V[maxI][1];
                V[maxI][2] = -V[maxI][2];
            }
            
            // re-orthogonalize the other two eigenvectors
            j = (maxI+1)%3;
            k = (maxI+2)%3;
            
            V[j][0] = 0.0;
            V[j][1] = 0.0;
            V[j][2] = 0.0;
            V[j][j] = 1.0;
            MathFunctions::crossProduct(V[maxI],V[j],V[k]);
            MathFunctions::normalizeVector(V[k]);
            MathFunctions::crossProduct(V[k],V[maxI],V[j]);
            
            // transpose vectors back to columns
            Transpose3x3(V,V);
            return;
        }
    }
    
    // the three eigenvalues are different, just sort the eigenvectors
    // to align them with the x, y, and z axes
    
    // find the vector with the largest x element, make that vector
    // the first vector
    maxVal = fabs(V[0][0]);
    maxI = 0;
    for (i = 1; i < 3; i++)
    {
        if (maxVal < (tmp = fabs(V[i][0])))
        {
            maxVal = tmp;
            maxI = i;
        }
    }
    // swap eigenvalue and eigenvector
    if (maxI != 0)
    {
        tmp = w[maxI];
        w[maxI] = w[0];
        w[0] = tmp;
        SwapVectors3(V[maxI],V[0]);
    }
    // do the same for the y element
    if (fabs(V[1][1]) < fabs(V[2][1]))
    {
        tmp = w[2];
        w[2] = w[1];
        w[1] = tmp;
        SwapVectors3(V[2],V[1]);
    }
    
    // ensure that the sign of the eigenvectors is correct
    for (i = 0; i < 2; i++)
    {
        if (V[i][i] < 0)
        {
            V[i][0] = -V[i][0];
            V[i][1] = -V[i][1];
            V[i][2] = -V[i][2];
        }
    }
    // set sign of final eigenvector to ensure that determinant is positive
    if (Determinant3x3(V) < 0)
    {
        V[2][0] = -V[2][0];
        V[2][1] = -V[2][1];
        V[2][2] = -V[2][2];
    }
    
    // transpose the eigenvectors back again
    Transpose3x3(V,V);
}

#define VTK_ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
a[k][l]=h+s*(g-h*tau)

// Jacobi iteration for the solution of eigenvectors/eigenvalues of a nxn
// real symmetric matrix. Square nxn matrix a; size of matrix in n;
// output eigenvalues in w; and output eigenvectors in v. Resulting
// eigenvalues/vectors are sorted in decreasing order; eigenvectors are
// normalized.
int
Matrix4x4::JacobiN(double **a, int n, double *w, double **v)
{
    const int VTK_MAX_ROTATIONS  = 20;
    
    int i, j, k, iq, ip, numPos;
    double tresh, theta, tau, t, sm, s, h, g, c, tmp;
    double bspace[4], zspace[4];
    double *b = bspace;
    double *z = zspace;
    
    // only allocate memory if the matrix is large
    if (n > 4)
    {
        b = new double[n];
        z = new double[n];
    }
    
    // initialize
    for (ip=0; ip<n; ip++)
    {
        for (iq=0; iq<n; iq++)
        {
            v[ip][iq] = 0.0;
        }
        v[ip][ip] = 1.0;
    }
    for (ip=0; ip<n; ip++)
    {
        b[ip] = w[ip] = a[ip][ip];
        z[ip] = 0.0;
    }
    
    // begin rotation sequence
    for (i=0; i<VTK_MAX_ROTATIONS; i++)
    {
        sm = 0.0;
        for (ip=0; ip<n-1; ip++)
        {
            for (iq=ip+1; iq<n; iq++)
            {
                sm += fabs(a[ip][iq]);
            }
        }
        if (sm == 0.0)
        {
            break;
        }
        
        if (i < 3)                                // first 3 sweeps
        {
            tresh = 0.2*sm/(n*n);
        }
        else
        {
            tresh = 0.0;
        }
        
        for (ip=0; ip<n-1; ip++)
        {
            for (iq=ip+1; iq<n; iq++)
            {
                g = 100.0*fabs(a[ip][iq]);
                
                // after 4 sweeps
                if (i > 3 && (fabs(w[ip])+g) == fabs(w[ip])
                    && (fabs(w[iq])+g) == fabs(w[iq]))
                {
                    a[ip][iq] = 0.0;
                }
                else if (fabs(a[ip][iq]) > tresh)
                {
                    h = w[iq] - w[ip];
                    if ( (fabs(h)+g) == fabs(h))
                    {
                        t = (a[ip][iq]) / h;
                    }
                    else
                    {
                        theta = 0.5*h / (a[ip][iq]);
                        t = 1.0 / (fabs(theta)+sqrt(1.0+theta*theta));
                        if (theta < 0.0)
                        {
                            t = -t;
                        }
                    }
                    c = 1.0 / sqrt(1+t*t);
                    s = t*c;
                    tau = s/(1.0+c);
                    h = t*a[ip][iq];
                    z[ip] -= h;
                    z[iq] += h;
                    w[ip] -= h;
                    w[iq] += h;
                    a[ip][iq]=0.0;
                    
                    // ip already shifted left by 1 unit
                    for (j = 0;j <= ip-1;j++)
                    {
                        VTK_ROTATE(a,j,ip,j,iq);
                    }
                    // ip and iq already shifted left by 1 unit
                    for (j = ip+1;j <= iq-1;j++)
                    {
                        VTK_ROTATE(a,ip,j,j,iq);
                    }
                    // iq already shifted left by 1 unit
                    for (j=iq+1; j<n; j++)
                    {
                        VTK_ROTATE(a,ip,j,iq,j);
                    }
                    for (j=0; j<n; j++)
                    {
                        VTK_ROTATE(v,j,ip,j,iq);
                    }
                }
            }
        }
        
        for (ip=0; ip<n; ip++)
        {
            b[ip] += z[ip];
            w[ip] = b[ip];
            z[ip] = 0.0;
        }
    }
    
    //// this is NEVER called
    if ( i >= VTK_MAX_ROTATIONS )
    {
        CaretLogWarning("Matrix4x4::Jacobi: Error extracting eigenfunctions");
        return 0;
    }
    
    // sort eigenfunctions                 these changes do not affect accuracy
    for (j=0; j<n-1; j++)                  // boundary incorrect
    {
        k = j;
        tmp = w[k];
        for (i=j+1; i<n; i++)                // boundary incorrect, shifted already
        {
            if (w[i] >= tmp)                   // why exchage if same?
            {
                k = i;
                tmp = w[k];
            }
        }
        if (k != j)
        {
            w[k] = w[j];
            w[j] = tmp;
            for (i=0; i<n; i++) 
            {
                tmp = v[i][j];
                v[i][j] = v[i][k];
                v[i][k] = tmp;
            }
        }
    }
    // insure eigenvector consistency (i.e., Jacobi can compute vectors that
    // are negative of one another (.707,.707,0) and (-.707,-.707,0). This can
    // reek havoc in hyperstreamline/other stuff. We will select the most
    // positive eigenvector.
    int ceil_half_n = (n >> 1) + (n & 1);
    for (j=0; j<n; j++)
    {
        for (numPos=0, i=0; i<n; i++)
        {
            if ( v[i][j] >= 0.0 )
            {
                numPos++;
            }
        }
        //    if ( numPos < ceil(double(n)/double(2.0)) )
        if ( numPos < ceil_half_n)
        {
            for(i=0; i<n; i++)
            {
                v[i][j] *= -1.0;
            }
        }
    }
    
    if (n > 4)
    {
        delete [] b;
        delete [] z;
    }
    return 1;
}
#undef VTK_ROTATE

void
Matrix4x4::Multiply3x3(const double A[3][3], const double B[3][3],
                       double C[3][3])
{
    double D[3][3];
    
    for (int i = 0; i < 3; i++)
    {
        D[0][i] = A[0][0]*B[0][i] + A[0][1]*B[1][i] + A[0][2]*B[2][i];
        D[1][i] = A[1][0]*B[0][i] + A[1][1]*B[1][i] + A[1][2]*B[2][i];
        D[2][i] = A[2][0]*B[0][i] + A[2][1]*B[1][i] + A[2][2]*B[2][i];
    }
    
    for (int j = 0; j < 3; j++)
    {
        C[j][0] = D[j][0];
        C[j][1] = D[j][1];
        C[j][2] = D[j][2];
    }
}

//----------------------------------------------------------------------------
void
Matrix4x4::Transpose3x3(const double A[3][3], double AT[3][3])
{
    double tmp;
    tmp = A[1][0];
    AT[1][0] = A[0][1];
    AT[0][1] = tmp;
    tmp = A[2][0];
    AT[2][0] = A[0][2];
    AT[0][2] = tmp;
    tmp = A[2][1];
    AT[2][1] = A[1][2];
    AT[1][2] = tmp;
    
    AT[0][0] = A[0][0];
    AT[1][1] = A[1][1];
    AT[2][2] = A[2][2];
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
    
    postmultiply(m);
    
    this->setModified();
}

/*
 * Set the rotation matrix using the given angles.
 * WARNING: Any scaling or translation is will be removed!!!
 *
 * @rotationX
 *    The X-rotation angle.
 * @rotationY
 *    The Y-rotation angle.
 * @rotationZ
 *    The Z-rotation angle.
 */
void
Matrix4x4::setRotation(const double rotationX,
                       const double rotationY,
                       const double rotationZ)
{
    identity();
    rotateY(rotationY);
    rotateX(rotationX);
    rotateZ(rotationZ);
}

/*
 * Get the rotation angles from the matrix.
 *
 * From vktTransform::GetOrientation()
 *
 * @rotationOutX
 *    Output containing X-rotation from matrix.
 * @rotationOutY
 *    Output containing X-rotation from matrix.
 * @rotationOutZ
 *    Output containing X-rotation from matrix.
 */
void
Matrix4x4::getRotation(double& rotationOutX,
                       double& rotationOutY,
                       double& rotationOutZ) const
{
#define VTK_AXIS_EPSILON 0.001
    int i;
    
    // convenient access to matrix
//    double (*matrix)[4] = amatrix->Element;
    double ortho[3][3];
    
    for (i = 0; i < 3; i++)
    {
        ortho[0][i] = matrix[0][i];
        ortho[1][i] = matrix[1][i];
        ortho[2][i] = matrix[2][i];
    }
    if (Determinant3x3(ortho) < 0)
    {
        ortho[0][2] = -ortho[0][2];
        ortho[1][2] = -ortho[1][2];
        ortho[2][2] = -ortho[2][2];
    }
    
    Matrix4x4::Orthogonalize3x3(ortho, ortho);
    
    // first rotate about y axis
    double x2 = ortho[2][0];
    double y2 = ortho[2][1];
    double z2 = ortho[2][2];
    
    double x3 = ortho[1][0];
    double y3 = ortho[1][1];
    double z3 = ortho[1][2];
    
    double d1 = sqrt(x2*x2 + z2*z2);
    
    double cosTheta, sinTheta;
    if (d1 < VTK_AXIS_EPSILON)
    {
        cosTheta = 1.0;
        sinTheta = 0.0;
    }
    else
    {
        cosTheta = z2/d1;
        sinTheta = x2/d1;
    }
    
    double theta = std::atan2(sinTheta, cosTheta);
    rotationOutY = - MathFunctions::toDegrees(theta );
    
    // now rotate about x axis
    double d = std::sqrt(x2*x2 + y2*y2 + z2*z2);
    
    double sinPhi, cosPhi;
    if (d < VTK_AXIS_EPSILON)
    {
        sinPhi = 0.0;
        cosPhi = 1.0;
    }
    else if (d1 < VTK_AXIS_EPSILON)
    {
        sinPhi = y2/d;
        cosPhi = z2/d;
    }
    else
    {
        sinPhi = y2/d;
        cosPhi = (x2*x2 + z2*z2)/(d1*d);
    }
    
    double phi = std::atan2(sinPhi, cosPhi);
    rotationOutX = MathFunctions::toDegrees(phi);
    
    // finally, rotate about z
    double x3p = x3*cosTheta - z3*sinTheta;
    double y3p = - sinPhi*sinTheta*x3 + cosPhi*y3 - sinPhi*cosTheta*z3;
    double d2 = std::sqrt(x3p*x3p + y3p*y3p);
    
    double cosAlpha, sinAlpha;
    if (d2 < VTK_AXIS_EPSILON)
    {
        cosAlpha = 1.0;
        sinAlpha = 0.0;
    }
    else 
    {
        cosAlpha = y3p/d2;
        sinAlpha = x3p/d2;
    }
    
    double alpha = std::atan2(sinAlpha, cosAlpha);
    rotationOutZ = MathFunctions::toDegrees(alpha);
    
    if (MathFunctions::isNaN(rotationOutX)) {
        rotationOutX = 0.0;
    }
    if (MathFunctions::isNaN(rotationOutY)) {
        rotationOutY = 0.0;
    }
    if (MathFunctions::isNaN(rotationOutZ)) {
        rotationOutZ = 0.0;
    }
}

void
Matrix4x4::Orthogonalize3x3(const double A[3][3], double B[3][3])
{
    int i;
    
    // copy the matrix
    for (i = 0; i < 3; i++)
    {
        B[0][i] = A[0][i];
        B[1][i] = A[1][i];
        B[2][i] = A[2][i];
    }
    
    // Pivot the matrix to improve accuracy
    double scale[3];
    int index[3];
    double tmp, largest;
    
    // Loop over rows to get implicit scaling information
    for (i = 0; i < 3; i++)
    {
        largest = fabs(B[i][0]);
        if ((tmp = fabs(B[i][1])) > largest)
        {
            largest = tmp;
        }
        if ((tmp = fabs(B[i][2])) > largest)
        {
            largest = tmp;
        }
        scale[i] = 1.0;
        if (largest != 0)
        {
            scale[i] = double(1.0)/largest;
        }
    }
    
    // first column
    index[0] = 0;
    largest = scale[0]*fabs(B[0][0]);
    if ((tmp = scale[1]*fabs(B[1][0])) >= largest)
    {
        largest = tmp;
        index[0] = 1;
    }
    if ((tmp = scale[2]*fabs(B[2][0])) >= largest)
    {
        index[0] = 2;
    }
    if (index[0] != 0)
    {
        SwapVectors3(B[index[0]],B[0]);
        scale[index[0]] = scale[0];
    }
    
    // second column
    index[1] = 1;
    largest = scale[1]*fabs(B[1][1]);
    if ((tmp = scale[2]*fabs(B[2][1])) >= largest)
    {
        index[1] = 2;
        SwapVectors3(B[2],B[1]);
    }
    
    // third column
    index[2] = 2;
    
    // A quaternian can only describe a pure rotation, not
    // a rotation with a flip, therefore the flip must be
    // removed before the matrix is converted to a quaternion.
    double d = Matrix4x4::Determinant3x3(B);
    if (d < 0)
    {
        for (i = 0; i < 3; i++)
        {
            B[0][i] = -B[0][i];
            B[1][i] = -B[1][i];
            B[2][i] = -B[2][i];
        }
    }
    
    // Do orthogonalization using a quaternion intermediate
    // (this, essentially, does the orthogonalization via
    // diagonalization of an appropriately constructed symmetric
    // 4x4 matrix rather than by doing SVD of the 3x3 matrix)
    double quat[4];
    MathFunctions::matrixToQuatern(B,quat);
    MathFunctions::quaternToMatrix(quat,B);
    
    // Put the flip back into the orthogonalized matrix.
    if (d < 0)
    {
        for (i = 0; i < 3; i++)
        {
            B[0][i] = -B[0][i];
            B[1][i] = -B[1][i];
            B[2][i] = -B[2][i];
        }
    }
    
    // Undo the pivoting
    if (index[1] != 1)
    {
        SwapVectors3(B[index[1]],B[1]);
    }
    if (index[0] != 0)
    {
        SwapVectors3(B[index[0]],B[0]);
    }
}

/**
 * Swap elements in vectors.
 * @param v1
 *    First vector.
 * @param v2
 *    Second vector.
 */
void
Matrix4x4::SwapVectors3(double v1[3],
                         double v2[3])
{
    double x[3] = { v1[0], v1[1], v1[2] };
    
    v1[0] = v2[0];
    v1[1] = v2[1];
    v1[2] = v2[2];
    
    v2[0] = x[0];
    v2[1] = x[1];
    v2[2] = x[2];
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
 * Get the matrix as 16 element one-dimensional array for use by OpenGL.
 *
 * @param  m  A 16-element array of double.
 *
 */
void
Matrix4x4::getMatrixForOpenGL(float m[16]) const
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
Matrix4x4::setMatrixFromOpenGL(const float m[16])
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
 * Convert the given vector to an OpenGL rotation matrix.
 * "This" matrix is set to the identity matrix before createing
 * the rotation matrix.  Use Matrix4x4::getMatrixForOpenGL() 
 * to get the matrix after calling this method and then pass
 * array to glMultMatrixd().
 *
 *  http://lifeofaprogrammergeek.blogspot.com/2008/07/rendering-cylinder-between-two-points.html
 *
 * @param vector
 *    The vector.  MUST be a unit vector.
 */
void
Matrix4x4::setMatrixToOpenGLRotationFromVector(const float vector[3])
{
    float vx = vector[0];
    float vy = vector[1];
    float vz = vector[2];
    
    float z = (float)std::sqrt( vx*vx + vy*vy + vz*vz );
    double ax = 0.0f;
    
    double zero = 1.0e-3;
    
    if (std::abs(vz) < zero) {
        ax = 57.2957795*std::acos( vx/z ); // rotation angle in x-y plane
        if ( vx <= 0.0f ) ax = -ax;
    }
    else {
        ax = 57.2957795*std::acos( vz/z ); // rotation angle
        if ( vz <= 0.0f ) ax = -ax;
    }
    
    float rx = -vy*vz;
    float ry = vx*vz;
    
    if ((std::abs(vx) < zero) && (std::fabs(vz) < zero)) {
        if (vy > 0) {
            ax = 90;
        }
    }

    identity();
    if (std::abs(vz) < zero)  {
        rotateY(90.0);  // Rotate & align with x axis
        rotateX(-ax);   // Rotate to point 2 in x-y plane
    }
    else {
        rotate(ax, rx, ry, 0.0); // Rotate about rotation vector
    }
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
 * @param  m  A 4x4 array of float.
 *
 */
void
Matrix4x4::setMatrix(const float m[4][4])
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
 * @param  m  A 4x4 array of floats.
 *
 */
void
Matrix4x4::getMatrix(float m[4][4]) const
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
Matrix4x4::multiplyPoint3(double p[3]) const
{
    double pout[3] = { 0.0f, 0.0f, 0.0f };
    for (int row = 0; row < 3; row++) {
        pout[row] = (this->matrix[row][0] * p[0]
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
AString
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
Matrix4x4::setDataSpaceName(const AString& name)
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
AString
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
Matrix4x4::setTransformedSpaceName(const AString& name)
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
 */
bool
Matrix4x4::invert()
{
    double m[4][4];
    const bool valid = Matrix4x4::Inverse(this->matrix, m);
    if (valid) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                this->matrix[i][j] = m[i][j];
            }
        }
        this->setModified();
        return true;
    }
    else {
        CaretLogWarning("Matrix inversion failed for " + this->toString());
        return false;
    }
}

/**
 * Inverse from VTK
 */
bool
Matrix4x4::Inverse(const double a[4][4], double matrixOut[4][4]) const
{
    /////SqMatPtr outElem = (SqMatPtr)outElements;
    
    // inverse( original_matrix, inverse_matrix )
    // calculate the inverse of a 4x4 matrix
    //
    //     -1
    //     A  = ___1__ adjoint A
    //         det A
    //
    
    // calculate the 4x4 determinent
    // if the determinent is zero,
    // then the inverse matrix is not unique.
    
    const double det = Determinant4x4(a);
    if ( det == 0.0 ) {
        return false;
    }
    
    // calculate the adjoint matrix
    Adjoint(a, matrixOut);
    //vtkMatrix4x4::Adjoint(inElements, outElements );
    
    // scale the adjoint matrix to get the inverse
    for (int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            matrixOut[i][j] = matrixOut[i][j] / det;
        }
    }
    
    return true;
}

/**
 * Adjoint from vtkMatrix4x4::Adjoint
 */
void
Matrix4x4::Adjoint(const double inputMatrix[4][4], double outputMatrix[4][4]) const
{
    // 
    //   adjoint( original_matrix, inverse_matrix )
    // 
    //     calculate the adjoint of a 4x4 matrix
    //
    //      Let  a   denote the minor determinant of matrix A obtained by
    //           ij
    //
    //      deleting the ith row and jth column from A.
    //
    //                    i+j
    //     Let  b   = (-1)    a
    //          ij            ji
    //
    //    The matrix B = (b  ) is the adjoint of A
    //                     ij
    //
    double a1, a2, a3, a4, b1, b2, b3, b4;
    double c1, c2, c3, c4, d1, d2, d3, d4;
    
    // assign to individual variable names to aid
    // selecting correct values
    
    a1 = inputMatrix[0][0]; b1 = inputMatrix[0][1]; 
    c1 = inputMatrix[0][2]; d1 = inputMatrix[0][3];
    
    a2 = inputMatrix[1][0]; b2 = inputMatrix[1][1]; 
    c2 = inputMatrix[1][2]; d2 = inputMatrix[1][3];
    
    a3 = inputMatrix[2][0]; b3 = inputMatrix[2][1];
    c3 = inputMatrix[2][2]; d3 = inputMatrix[2][3];
    
    a4 = inputMatrix[3][0]; b4 = inputMatrix[3][1]; 
    c4 = inputMatrix[3][2]; d4 = inputMatrix[3][3];
    
    
    // row column labeling reversed since we transpose rows & columns
    
    outputMatrix[0][0]  =   
    Matrix4x4::Determinant3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
    outputMatrix[1][0]  = 
    - Matrix4x4::Determinant3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
    outputMatrix[2][0]  =   
    Matrix4x4::Determinant3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
    outputMatrix[3][0]  = 
    - Matrix4x4::Determinant3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
    
    outputMatrix[0][1]  = 
    - Matrix4x4::Determinant3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
    outputMatrix[1][1]  =   
    Matrix4x4::Determinant3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
    outputMatrix[2][1]  = 
    - Matrix4x4::Determinant3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
    outputMatrix[3][1]  =   
    Matrix4x4::Determinant3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);
    
    outputMatrix[0][2]  =   
    Matrix4x4::Determinant3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
    outputMatrix[1][2]  = 
    - Matrix4x4::Determinant3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
    outputMatrix[2][2]  =   
    Matrix4x4::Determinant3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
    outputMatrix[3][2]  = 
    - Matrix4x4::Determinant3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);
    
    outputMatrix[0][3]  = 
    - Matrix4x4::Determinant3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
    outputMatrix[1][3]  =   
    Matrix4x4::Determinant3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
    outputMatrix[2][3]  = 
    - Matrix4x4::Determinant3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
    outputMatrix[3][3]  =   
    Matrix4x4::Determinant3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

/**
 * From vtkMatrix4x4::Determinant
 */
double
Matrix4x4::Determinant4x4(const double matrixIn[4][4]) const
{
    double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;
    
    // assign to individual variable names to aid selecting
    //  correct elements
    
    a1 = matrixIn[0][0]; b1 = matrixIn[0][1];
    c1 = matrixIn[0][2]; d1 = matrixIn[0][3];
    
    a2 = matrixIn[1][0]; b2 = matrixIn[1][1];
    c2 = matrixIn[1][2]; d2 = matrixIn[1][3];
    
    a3 = matrixIn[2][0]; b3 = matrixIn[2][1];
    c3 = matrixIn[2][2]; d3 = matrixIn[2][3];
    
    a4 = matrixIn[3][0]; b4 = matrixIn[3][1];
    c4 = matrixIn[3][2]; d4 = matrixIn[3][3];
    
    const double result = a1 * Matrix4x4::Determinant3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
    - b1 * Matrix4x4::Determinant3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
    + c1 * Matrix4x4::Determinant3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
    - d1 * Matrix4x4::Determinant3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
    return result;
}

double 
Matrix4x4::Determinant3x3(double a1, double a2, double a3,
                                      double b1, double b2, double b3,
                                      double c1, double c2, double c3)
{
    return ( a1 * Matrix4x4::Determinant2x2( b2, b3, c2, c3 )
            - b1 * Matrix4x4::Determinant2x2( a2, a3, c2, c3 )
            + c1 * Matrix4x4::Determinant2x2( a2, a3, b2, b3 ) );
}

double 
Matrix4x4::Determinant2x2(double a, double b, double c, double d) 
{
    return (a * d - b * c);
};

double
Matrix4x4::Determinant3x3(double A[3][3])
{
    return A[0][0] * A[1][1] * A[2][2] + A[1][0] * A[2][1] * A[0][2] +
    A[2][0] * A[0][1] * A[1][2] - A[0][0] * A[2][1] * A[1][2] -
    A[1][0] * A[0][1] * A[2][2] - A[2][0] * A[1][1] * A[0][2];
}

/**
 * Set this matrix to a landmark transform that maps from the source
 * to the target space as defined by the control points.
 * Replaces the current matrix.
 *
 * @param controlPoints
 *     The control points (pair of source and target coordinates).
 * @param errorMessageOut
 *     Contains error message.
 * @return
 *     True if output matrix is valid, else false.  If false, this
 *     matrix will be the identity matrix.
 */
bool
Matrix4x4::createLandmarkTransformMatrix(const std::vector<ControlPoint3D*>& controlPoints,
                                         AString& errorMessageOut)
{
    const bool debugFlag = false;
    identity();
    errorMessageOut.clear();
    
    if (controlPoints.size() < 3) {
        errorMessageOut = "There must be at least three control points.";
        return false;
    }
    
    float s1[3];
    controlPoints[0]->getSourceXYZ(s1);
    float s2[3];
    controlPoints[1]->getSourceXYZ(s2);
    float s3[3];
    controlPoints[2]->getSourceXYZ(s3);
    float sourceNormalVector[3] = { 0.0, 0.0, 0.0 };
    MathFunctions::normalVector(s1, s2, s3, sourceNormalVector);

    const float tinyValue = 0.00001;
    if ((sourceNormalVector[2] < tinyValue)
        && (sourceNormalVector[2] > -tinyValue)) {
        errorMessageOut = ("First three control points are along a line.  Edit control points to that "
                           "the first three control points form a triangular shape.");
        return false;
    }
    
//    if (sourceNormalVector[2] < 0.0) {
//        CaretLogWarning("Control points are orientated clockwise; unknown if this causes a problem.");
//    }

    
    double leastError = std::numeric_limits<float>::max();
    Matrix4x4 leastErrorMatrix;
    bool leastErrorMatrixValid = false;
    
    const bool testAllTransformTypesFlag = true;
    if (testAllTransformTypesFlag) {
        for (int32_t j = 0; j < 3; j++) {
            LANDMARK_TRANSFORM_MODE mode = LANDMARK_TRANSFORM_AFFINE;
            AString modeName;
            
            switch (j) {
                case 0:
                    mode = LANDMARK_TRANSFORM_AFFINE;
                    modeName = "LANDMARK_TRANSFORM_AFFINE";
                    break;
                case 1:
                    mode = LANDMARK_TRANSFORM_RIGIDBODY;
                    modeName = "LANDMARK_TRANSFORM_RIGIDBODY";
                    break;
                case 2:
                    mode = LANDMARK_TRANSFORM_SIMILARITY;
                    modeName = "LANDMARK_TRANSFORM_SIMILARITY";
                    break;
            }
            Matrix4x4 matrix;
            if (matrix.createLandmarkTransformMatrixPrivate(controlPoints,
                                                        mode,
                                                        errorMessageOut)) {
                
                if (debugFlag) {
                    std::cout << std::endl;
                    std::cout << "Mode:   " << qPrintable(modeName) << std::endl;
                    std::cout << "Matrix: " << qPrintable(matrix.toFormattedString("   ")) << std::endl;
                }
                
                const int32_t numcp = static_cast<int32_t>(controlPoints.size());
                double sum = 0.0;
                for (int32_t i = 0; i < numcp; i++) {
                    double source[3];
                    controlPoints[i]->getSourceXYZ(source);
                    double predicted[3] = { source[0], source[1], source[2] };
                    matrix.multiplyPoint3(predicted);
                    
                    double target[3];
                    controlPoints[i]->getTargetXYZ(target);
                    
                    const double error = MathFunctions::distance3D(predicted, target);
                    sum += error;
                    
                    if (debugFlag) {
                        std::cout << "CP("<< i << ") Source: ("
                        << qPrintable(AString::fromNumbers(source, 3, ",")) << ")  Target: ("
                        << qPrintable(AString::fromNumbers(target, 3, ",")) << ")  Predicted: ("
                        << qPrintable(AString::fromNumbers(predicted, 3, ",")) << ")  Error: "
                        << error << std::endl;
                    }
                }
                
                const double error = (sum /= static_cast<double>(numcp));
                if (debugFlag) {
                    std::cout << "   Average error per control point:  " << error << std::endl;
                }
                
                if (error < leastError) {
                    leastError = error;
                    leastErrorMatrix = matrix;
                    leastErrorMatrixValid = true;
                }
            }
        }
    }

    *this = leastErrorMatrix;
    
    const float averageError = measureTransformError(controlPoints,
                                                     *this);
//    const bool result = createLandmarkTransformMatrixPrivate(controlPoints,
//                                                LANDMARK_TRANSFORM_AFFINE,
//                                                errorMessageOut);
    if (debugFlag) {
        std::cout << std::endl;
        std::cout << "Transform Error:  " << averageError  << std::endl;
    }
    
    return leastErrorMatrixValid;
}

/**
 * Set this matrix to a landmark transform that maps from the source
 * to the target space as defined by the control points.
 * Replaces the current matrix.
 *
 * Code Copied From vtkLandmarkTransform
 * Program:   Visualization Toolkit
 * Module:    vtkLandmarkTransform.cxx
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the above copyright notice for more information.
 *
 * @param controlPoints
 *     The control points (pair of source and target coordinates).
 * @param mode
 *     Mode for transformation.
 * @param errorMessageOut
 *     Contains error message.
 * @return
 *     True if output matrix is valid, else false.  If false, this
 *     matrix will be the identity matrix.
 */
bool
Matrix4x4::createLandmarkTransformMatrixPrivate(const std::vector<ControlPoint3D*>& controlPoints,
                                                const LANDMARK_TRANSFORM_MODE mode,
                                                AString& errorMessageOut) 
{
    identity();
    
    errorMessageOut.clear();
    
    //vtkIdType i;
    int32_t i;
    int32_t j;
    
    //if (this->SourceLandmarks == NULL || this->TargetLandmarks == NULL)
    if (controlPoints.empty())
    {
        identity();
        errorMessageOut = "Control points are empty.";
        return false;
    }
    
    // --- compute the necessary transform to match the two sets of landmarks ---
    
    /*
     The solution is based on
     Berthold K. P. Horn (1987),
     "Closed-form solution of absolute orientation using unit quaternions,"
     Journal of the Optical Society of America A, 4:629-642
     */
    
    // Original python implementation by David G. Gobbi
    
    //const vtkIdType N_PTS = this->SourceLandmarks->GetNumberOfPoints();
    int32_t N_PTS = static_cast<int32_t>(controlPoints.size());
    if (mode == LANDMARK_TRANSFORM_AFFINE) {
        if (N_PTS > 3) {
            N_PTS = 3;
        }
    }
    //    if(N_PTS != this->TargetLandmarks->GetNumberOfPoints())
    //    {
    //        vtkErrorMacro("Update: Source and Target Landmarks contain a different number of points");
    //        return;
    //    }
    
    // -- if no points, stop here
    
    if (N_PTS == 0)
    {
        identity();
        return false;
    }
    
    // -- find the centroid of each set --
    
    double source_centroid[3]={0,0,0};
    double target_centroid[3]={0,0,0};
    double p[3];
    for(i=0;i<N_PTS;i++)
    {
        CaretAssertVectorIndex(controlPoints, i);
        //this->SourceLandmarks->GetPoint(i, p);
        controlPoints[i]->getSourceXYZ(p);
        source_centroid[0] += p[0];
        source_centroid[1] += p[1];
        source_centroid[2] += p[2];
        //this->TargetLandmarks->GetPoint(i, p);
        controlPoints[i]->getTargetXYZ(p);
        target_centroid[0] += p[0];
        target_centroid[1] += p[1];
        target_centroid[2] += p[2];
    }
    source_centroid[0] /= N_PTS;
    source_centroid[1] /= N_PTS;
    source_centroid[2] /= N_PTS;
    target_centroid[0] /= N_PTS;
    target_centroid[1] /= N_PTS;
    target_centroid[2] /= N_PTS;
    
    
    double matrixArray[4][4];
    getMatrix(matrixArray);
    
    // -- if only one point, stop right here
    
    if (N_PTS == 1)
    {
        //this->Matrix->Identity();
        matrixArray[0][3] = target_centroid[0] - source_centroid[0];
        matrixArray[1][3] = target_centroid[1] - source_centroid[1];
        matrixArray[2][3] = target_centroid[2] - source_centroid[2];
        return true;
    }
    
    // -- build the 3x3 matrix M --
    
    double M[3][3];
    double AAT[3][3];
    for(i=0;i<3;i++)
    {
        AAT[i][0] = M[i][0]=0.0F; // fill M with zeros
        AAT[i][1] = M[i][1]=0.0F;
        AAT[i][2] = M[i][2]=0.0F;
    }
    //vtkIdType pt;
    int32_t pt;
    double a[3],b[3];
    double sa=0.0F,sb=0.0F;
    for(pt=0;pt<N_PTS;pt++)
    {
        // get the origin-centred point (a) in the source set
        //this->SourceLandmarks->GetPoint(pt,a);
        CaretAssertVectorIndex(controlPoints, pt);
        controlPoints[pt]->getSourceXYZ(a);
        a[0] -= source_centroid[0];
        a[1] -= source_centroid[1];
        a[2] -= source_centroid[2];
        // get the origin-centred point (b) in the target set
        //this->TargetLandmarks->GetPoint(pt,b);
        controlPoints[pt]->getTargetXYZ(b);
        b[0] -= target_centroid[0];
        b[1] -= target_centroid[1];
        b[2] -= target_centroid[2];
        // accumulate the products a*T(b) into the matrix M
        for(i=0;i<3;i++)
        {
            M[i][0] += a[i]*b[0];
            M[i][1] += a[i]*b[1];
            M[i][2] += a[i]*b[2];
            
            // for the affine transform, compute ((a.a^t)^-1 . a.b^t)^t.
            // a.b^t is already in M.  here we put a.a^t in AAT.
            if (mode == LANDMARK_TRANSFORM_AFFINE)
            {
                AAT[i][0] += a[i]*a[0];
                AAT[i][1] += a[i]*a[1];
                AAT[i][2] += a[i]*a[2];
            }
        }
        // accumulate scale factors (if desired)
        sa += a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
        sb += b[0]*b[0]+b[1]*b[1]+b[2]*b[2];
        
        /*
         * If source points all have same value for 'k',
         * it will cause element [2][2] to be zero
         * and the matrix cannot be inverted.
         */
        if (AAT[2][2] == 0.0) {
            AAT[2][2] = 1.0;
        }
    }
    
    
    if(mode == LANDMARK_TRANSFORM_AFFINE)
    {
        // AAT = (a.a^t)^-1
        MathFunctions::vtkInvert3x3(AAT,AAT);
        
        // M = (a.a^t)^-1 . a.b^t
        MathFunctions::vtkMultiply3x3(AAT,M,M);
        
        // this->Matrix = M^t
        for(i=0;i<3;++i)
        {
            for(j=0;j<3;++j)
            {
                matrixArray[i][j] = M[j][i];
            }
        }
    }
    else
    {
        // compute required scaling factor (if desired)
        double scale = (double)std::sqrt(sb/sa);
        
        // -- build the 4x4 matrix N --
        
        double Ndata[4][4];
        double *N[4];
        for(i=0;i<4;i++)
        {
            N[i] = Ndata[i];
            N[i][0]=0.0F; // fill N with zeros
            N[i][1]=0.0F;
            N[i][2]=0.0F;
            N[i][3]=0.0F;
        }
        // on-diagonal elements
        N[0][0] = M[0][0]+M[1][1]+M[2][2];
        N[1][1] = M[0][0]-M[1][1]-M[2][2];
        N[2][2] = -M[0][0]+M[1][1]-M[2][2];
        N[3][3] = -M[0][0]-M[1][1]+M[2][2];
        // off-diagonal elements
        N[0][1] = N[1][0] = M[1][2]-M[2][1];
        N[0][2] = N[2][0] = M[2][0]-M[0][2];
        N[0][3] = N[3][0] = M[0][1]-M[1][0];
        
        N[1][2] = N[2][1] = M[0][1]+M[1][0];
        N[1][3] = N[3][1] = M[2][0]+M[0][2];
        N[2][3] = N[3][2] = M[1][2]+M[2][1];
        
        // -- eigen-decompose N (is symmetric) --
        
        double eigenvectorData[4][4];
        double *eigenvectors[4],eigenvalues[4];
        
        eigenvectors[0] = eigenvectorData[0];
        eigenvectors[1] = eigenvectorData[1];
        eigenvectors[2] = eigenvectorData[2];
        eigenvectors[3] = eigenvectorData[3];
        
        MathFunctions::vtkJacobiN(N,4,eigenvalues,eigenvectors);
        
        // the eigenvector with the largest eigenvalue is the quaternion we want
        // (they are sorted in decreasing order for us by JacobiN)
        double w,x,y,z;
        
        // first: if points are collinear, choose the quaternion that
        // results in the smallest rotation.
        if (eigenvalues[0] == eigenvalues[1] || N_PTS == 2)
        {
            double s0[3],t0[3],s1[3],t1[3];
            //            this->SourceLandmarks->GetPoint(0,s0);
            //            this->TargetLandmarks->GetPoint(0,t0);
            //            this->SourceLandmarks->GetPoint(1,s1);
            //            this->TargetLandmarks->GetPoint(1,t1);
            CaretAssertVectorIndex(controlPoints, 1);
            controlPoints[0]->getSourceXYZ(s0);
            controlPoints[0]->getTargetXYZ(t0);
            controlPoints[1]->getSourceXYZ(s1);
            controlPoints[1]->getTargetXYZ(t1);
            
            double ds[3],dt[3];
            double rs = 0, rt = 0;
            for (i = 0; i < 3; i++)
            {
                ds[i] = s1[i] - s0[i];      // vector between points
                rs += ds[i]*ds[i];
                dt[i] = t1[i] - t0[i];
                rt += dt[i]*dt[i];
            }
            
            // normalize the two vectors
            rs = sqrt(rs);
            ds[0] /= rs; ds[1] /= rs; ds[2] /= rs;
            rt = sqrt(rt);
            dt[0] /= rt; dt[1] /= rt; dt[2] /= rt;
            
            // take dot & cross product
            w = ds[0]*dt[0] + ds[1]*dt[1] + ds[2]*dt[2];
            x = ds[1]*dt[2] - ds[2]*dt[1];
            y = ds[2]*dt[0] - ds[0]*dt[2];
            z = ds[0]*dt[1] - ds[1]*dt[0];
            
            double r = sqrt(x*x + y*y + z*z);
            double theta = atan2(r,w);
            
            // construct quaternion
            w = cos(theta/2);
            if (r != 0)
            {
                r = sin(theta/2)/r;
                x = x*r;
                y = y*r;
                z = z*r;
            }
            else // rotation by 180 degrees: special case
            {
                // rotate around a vector perpendicular to ds
                MathFunctions::vtkPerpendiculars(ds,dt,0,0);
                r = sin(theta/2);
                x = dt[0]*r;
                y = dt[1]*r;
                z = dt[2]*r;
            }
        }
        else // points are not collinear
        {
            w = eigenvectors[0][0];
            x = eigenvectors[1][0];
            y = eigenvectors[2][0];
            z = eigenvectors[3][0];
        }
        
        // convert quaternion to a rotation matrix
        
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
        
        matrixArray[0][0] = ww + xx - yy - zz;
        matrixArray[1][0] = 2.0*(wz + xy);
        matrixArray[2][0] = 2.0*(-wy + xz);
        
        matrixArray[0][1] = 2.0*(-wz + xy);
        matrixArray[1][1] = ww - xx + yy - zz;
        matrixArray[2][1] = 2.0*(wx + yz);
        
        matrixArray[0][2] = 2.0*(wy + xz);
        matrixArray[1][2] = 2.0*(-wx + yz);
        matrixArray[2][2] = ww - xx - yy + zz;
        
        if (mode != LANDMARK_TRANSFORM_RIGIDBODY)
        { // add in the scale factor (if desired)
            for(i=0;i<3;i++)
            {
                matrixArray[i][0] *= scale;
                matrixArray[i][1] *= scale;
                matrixArray[i][2] *= scale;
            }
        }
    }
    
    // the translation is given by the difference in the transformed source
    // centroid and the target centroid
    double sx, sy, sz;
    
    sx = matrixArray[0][0] * source_centroid[0] +
    matrixArray[0][1] * source_centroid[1] +
    matrixArray[0][2] * source_centroid[2];
    sy = matrixArray[1][0] * source_centroid[0] +
    matrixArray[1][1] * source_centroid[1] +
    matrixArray[1][2] * source_centroid[2];
    sz = matrixArray[2][0] * source_centroid[0] +
    matrixArray[2][1] * source_centroid[1] +
    matrixArray[2][2] * source_centroid[2];
    
    matrixArray[0][3] = target_centroid[0] - sx;
    matrixArray[1][3] = target_centroid[1] - sy;
    matrixArray[2][3] = target_centroid[2] - sz;
    
    // fill the bottom row of the 4x4 matrix
    matrixArray[3][0] = 0.0;
    matrixArray[3][1] = 0.0;
    matrixArray[3][2] = 0.0;
    matrixArray[3][3] = 1.0;
    
    /*
     * Can get zero for a scale factor if planes of the
     * source and target are parallel
     */
    for (int32_t i = 0; i < 3; i++) {
        if (matrixArray[i][i] == 0.0) {
            matrixArray[i][i] = 1.0;
        }
    }
    
    setMatrix(matrixArray);
    //this->Matrix->Modified();
    
    return true;
}

/**
 * Measure the average error for control points using transform matrix.
 * Error is straight line distance between ([source] * [matrix]) and
 * (target).
 *
 * @param controlProints
 *     The control points
 * @param matrix
 *     The transform matrix.
 */
float
Matrix4x4::measureTransformError(const std::vector<ControlPoint3D*>& controlPoints,
                                 const Matrix4x4& matrix) const
{
    const int32_t numcp = static_cast<int32_t>(controlPoints.size());
    if (numcp <= 0) {
        return 0.0;
    }
    
    double sum = 0.0;
    
    for (int32_t i = 0; i < numcp; i++) {
        ControlPoint3D* cp = controlPoints[i];
        
        double pt[3];
        cp->getSourceXYZ(pt);
        matrix.multiplyPoint3(pt);
        
        cp->setTransformedXYZ(pt);
        
        double target[3];
        controlPoints[i]->getTargetXYZ(target);
        
        const double totalError = MathFunctions::distance3D(pt, target);
        sum += totalError;
    }
    
    const double error = (sum /= static_cast<double>(numcp));
    return error;
}

/**
 * Write the matrix as a GIFTI matrix using the given XML tags.
 *
 * @param xmlWriter 
 *    The XML writer
 * @param xmlMatrixTag
 *    XML tag for for the matrix and its components.
 * @param xmlDataSpaceTag
 *    XML tag for for the data space name.
 * @param xmlTransformedSpaceTag
 *    XML tag for for the transformed space name.
 * @param xmlMatrixDataTag
 *    XML tag for the matrix data.
 *
 * @throws XmlException 
 *    If an error occurs while writing.
 */
void 
Matrix4x4::writeAsGiftiXML(XmlWriter& xmlWriter,
                           const AString& xmlMatrixTag,
                           const AString& xmlDataSpaceTag,
                           const AString& xmlTransformedSpaceTag,
                           const AString& xmlMatrixDataTag)
{
    xmlWriter.writeStartElement(xmlMatrixTag);
    
    xmlWriter.writeElementCData(xmlDataSpaceTag, this->dataSpaceName);
    xmlWriter.writeElementCData(xmlTransformedSpaceTag, this->transformedSpaceName);

    /*
     * Note: Matrix4x4 is column major order but GIFTI uses
     * row major order so transpose matrix as values are 
     * written.
     */
    xmlWriter.writeStartElement(xmlMatrixDataTag);
    for (int32_t iRow = 0; iRow < 4; iRow++) {
        for (int32_t jCol = 0; jCol < 4; jCol++) {
            /*
             * Transpose indices
             */
            const int32_t i = jCol;
            const int32_t j = iRow;
            
            const AString txt = (AString::number(this->matrix[i][j]) + " ");
            if (jCol == 0) {
                xmlWriter.writeCharactersWithIndent(txt);
            }
            else {
                xmlWriter.writeCharacters(txt);
            }
            if (jCol == 3) {
                xmlWriter.writeCharacters("\n");
            }
        }
    }
    xmlWriter.writeEndElement();
    
    xmlWriter.writeEndElement();
}

/**
 * Convert the matrix into a string representation.
 *
 * @return  String representation of the matrix.
 *
 */
AString
Matrix4x4::toString() const
{
    return toFormattedString("   ");
}

/**
 * Get a nicely formatted string for printing.
 *
 * @param indentation - use as indentation.
 * @return  String containing label information.
 *
 */
AString
Matrix4x4::toFormattedString(const AString& indentation) const
{
    float translation[3];
    getTranslation(translation);
    double rotation[3];
    getRotation(rotation[0], rotation[1], rotation[2]);
    double scale[3];
    getScale(scale[0], scale[1], scale[2]);

    AString elements;
    for (int32_t iRow = 0; iRow < 4; iRow++) {
        elements.append(indentation);
        for (int32_t iCol = 0; iCol < 4; iCol++) {
            elements.append(AString::number(matrix[iRow][iCol]) + " ");
        }
        elements.append("\n");
    }
    const AString s("Matrix4x4: \n"
                    + elements
                    + indentation + "Translation: " + AString::fromNumbers(translation, 3, ", ") + "\n"
                    + indentation + "Rotation:    " + AString::fromNumbers(rotation, 3, ", ") + "\n"
                    + indentation + "Scale:       " + AString::fromNumbers(scale, 3, ", "));
    return s;
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

